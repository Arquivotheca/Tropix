/*
 ****************************************************************
 *								*
 *			ohci.c					*
 *								*
 *	"Driver" para controladoras USB OHCI			*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.8.0, de 24.08.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.2				*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/region.h"
#include "../h/map.h"
#include "../h/scb.h"

#include "../h/pci.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/usb.h"

#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****** Tabelas de Métodos **************************************
 */
int			ohci_open (struct usbd_pipe *pipe_ptr);
void			ohci_softintr (void *v);
void			ohci_poll (struct usbd_bus *bus);
void			ohci_freex (struct usbd_bus *bus, struct usbd_xfer *xfer);
struct usbd_xfer	*ohci_allocx (struct usbd_bus *bus);

const struct usbd_bus_methods ohci_bus_methods =
{
	ohci_open,
	ohci_softintr,
	ohci_poll,
	NULL,		/* ohci_allocm (NÃO PRECISA) */
	NULL,		/* ohci_freem  (NÃO PRECISA) */
	ohci_allocx,
	ohci_freex
};

int			ohci_root_ctrl_transfer (struct usbd_xfer *xfer);
int			ohci_root_ctrl_start (struct usbd_xfer *xfer);
void			ohci_root_ctrl_abort (struct usbd_xfer *xfer);
void			ohci_root_ctrl_close (struct usbd_pipe *pipe_ptr);
void			ohci_noop (struct usbd_pipe *pipe_ptr);
void			ohci_root_ctrl_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods ohci_root_ctrl_methods =
{
	ohci_root_ctrl_transfer,
	ohci_root_ctrl_start,
	ohci_root_ctrl_abort,
	ohci_root_ctrl_close,
	ohci_noop,
	ohci_root_ctrl_done,
};

int			ohci_root_intr_transfer (struct usbd_xfer *xfer);
int			ohci_root_intr_start (struct usbd_xfer *xfer);
void			ohci_root_intr_abort (struct usbd_xfer *xfer);
void			ohci_root_intr_close (struct usbd_pipe *pipe_ptr);
void			ohci_root_intr_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods ohci_root_intr_methods =
{
	ohci_root_intr_transfer,
	ohci_root_intr_start,
	ohci_root_intr_abort,
	ohci_root_intr_close,
	ohci_noop,
	ohci_root_intr_done,
};

int			ohci_device_ctrl_transfer (struct usbd_xfer *xfer);
int			ohci_device_ctrl_start (struct usbd_xfer *xfer);
void			ohci_device_ctrl_abort (struct usbd_xfer *xfer);
void			ohci_device_ctrl_close (struct usbd_pipe *pipe_ptr);
void			ohci_device_ctrl_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods ohci_device_ctrl_methods =
{
	ohci_device_ctrl_transfer,
	ohci_device_ctrl_start,
	ohci_device_ctrl_abort,
	ohci_device_ctrl_close,
	ohci_noop,
	ohci_device_ctrl_done,
};

int			ohci_device_intr_transfer (struct usbd_xfer *xfer);
int			ohci_device_intr_start (struct usbd_xfer *xfer);
void			ohci_device_intr_abort (struct usbd_xfer *xfer);
void			ohci_device_intr_close (struct usbd_pipe *pipe_ptr);
void			ohci_device_clear_toggle (struct usbd_pipe *pipe_ptr);
void			ohci_device_intr_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods ohci_device_intr_methods =
{
	ohci_device_intr_transfer,
	ohci_device_intr_start,
	ohci_device_intr_abort,
	ohci_device_intr_close,
	ohci_device_clear_toggle,
	ohci_device_intr_done,
};

int			ohci_device_bulk_start (struct usbd_xfer *xfer);
int			ohci_device_bulk_transfer (struct usbd_xfer *xfer);
void			ohci_device_bulk_abort (struct usbd_xfer *xfer);
void			ohci_device_bulk_close (struct usbd_pipe *pipe_ptr);
void			ohci_device_bulk_done (struct usbd_xfer *xfer);


const struct usbd_pipe_methods ohci_device_bulk_methods =
{
	ohci_device_bulk_transfer,
	ohci_device_bulk_start,
	ohci_device_bulk_abort,
	ohci_device_bulk_close,
	ohci_device_clear_toggle,
	ohci_device_bulk_done,
};

int			ohci_device_isoc_transfer (struct usbd_xfer *xfer);
int			ohci_device_isoc_start (struct usbd_xfer *xfer);
void			ohci_device_isoc_abort (struct usbd_xfer *xfer);
void			ohci_device_isoc_close (struct usbd_pipe *pipe_ptr);
void			ohci_device_isoc_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods ohci_device_isoc_methods =
{
	ohci_device_isoc_transfer,
	ohci_device_isoc_start,
	ohci_device_isoc_abort,
	ohci_device_isoc_close,
	ohci_noop,
	ohci_device_isoc_done,
};

/*
 ****** Simulação do HUB RAIZ ***********************************
 */
const struct usb_device_descriptor ohci_devd =
{
	USB_DEVICE_DESCRIPTOR_SIZE,
	UDESC_DEVICE,		/* type */
	{0x00, 0x01},		/* USB version */
	UDCLASS_HUB,		/* class */
	UDSUBCLASS_HUB,		/* subclass */
	UDPROTO_FSHUB,		/* protocol */
	64,			/* max packet */
	{0},{0},{0x00,0x01},	/* device id */
	1,2,0,			/* string indicies */
	1			/* # of configurations */
};

const struct usb_config_descriptor ohci_confd =
{
	USB_CONFIG_DESCRIPTOR_SIZE,
	UDESC_CONFIG,
	{USB_CONFIG_DESCRIPTOR_SIZE + USB_INTERFACE_DESCRIPTOR_SIZE + USB_ENDPOINT_DESCRIPTOR_SIZE},
	1, 1, 0,
	UC_SELF_POWERED,
	0			/* max power */
};

const struct usb_interface_descriptor ohci_ifcd =
{
	USB_INTERFACE_DESCRIPTOR_SIZE,
	UDESC_INTERFACE,
	0, 0, 1,
	UICLASS_HUB,
	UISUBCLASS_HUB,
	UIPROTO_FSHUB,
	0
};

#define OHCI_INTR_ENDPT	1

const struct usb_endpoint_descriptor ohci_endpd =
{
	USB_ENDPOINT_DESCRIPTOR_SIZE,
	UDESC_ENDPOINT,
	UE_DIR_IN|OHCI_INTR_ENDPT,
	UE_INTERRUPT,
	{8, 0}, 255		/* max packet */
};

const struct usb_hub_descriptor ohci_hubd =
{
	USB_HUB_DESCRIPTOR_SIZE,
	UDESC_HUB,
	0, {0,0}, 0, 0, {0}
};

/*
 ******	Registros do OHCI ***************************************
 */
#define OHCI_REVISION		0x00		/* OHCI revision # */
#define  OHCI_REV_LO(rev)	((rev)        & 0x00F)
#define  OHCI_REV_HI(rev)	(((rev) >> 4) & 0x00F)
#define  OHCI_REV_LEGACY(rev)	((rev)        & 0x100)

#define OHCI_CONTROL		0x04
#define  OHCI_CBSR_MASK		0x00000003	/* Control/Bulk Service Ratio */
#define  OHCI_RATIO_1_1		0x00000000
#define  OHCI_RATIO_1_2		0x00000001
#define  OHCI_RATIO_1_3		0x00000002
#define  OHCI_RATIO_1_4		0x00000003
#define  OHCI_PLE		0x00000004	/* Periodic List Enable */
#define  OHCI_IE		0x00000008	/* Isochronous Enable */
#define  OHCI_CLE		0x00000010	/* Control List Enable */
#define  OHCI_BLE		0x00000020	/* Bulk List Enable */
#define  OHCI_HCFS_MASK		0x000000C0 	/* HostControllerFunctionalState */
#define  OHCI_HCFS_RESET	0x00000000
#define  OHCI_HCFS_RESUME	0x00000040
#define  OHCI_HCFS_OPERATIONAL	0x00000080
#define  OHCI_HCFS_SUSPEND	0x000000C0
#define  OHCI_IR		0x00000100	/* Interrupt Routing */
#define  OHCI_RWC		0x00000200	/* Remote Wakeup Connected */
#define  OHCI_RWE		0x00000400	/* Remote Wakeup Enabled */
#define OHCI_COMMAND_STATUS	0x08
#define  OHCI_HCR		0x00000001	/* Host Controller Reset */
#define  OHCI_CLF		0x00000002	/* Control List Filled */
#define  OHCI_BLF		0x00000004	/* Bulk List Filled */
#define  OHCI_OCR		0x00000008 	/* Ownership Change Request */
#define  OHCI_SOC_MASK		0x00030000	/* Scheduling Overrun Count */
#define OHCI_INTERRUPT_STATUS	0x0c
#define  OHCI_SO		0x00000001	/* Scheduling Overrun */
#define  OHCI_WDH		0x00000002	/* Writeback Done Head */
#define  OHCI_SF		0x00000004	/* Start of Frame */
#define  OHCI_RD		0x00000008	/* Resume Detected */
#define  OHCI_UE		0x00000010	/* Unrecoverable Error */
#define  OHCI_FNO		0x00000020	/* Frame Number Overflow */
#define  OHCI_RHSC		0x00000040	/* Root Hub Status Change */
#define  OHCI_OC		0x40000000	/* Ownership Change */
#define  OHCI_MIE		0x80000000	/* Master Interrupt Enable */
#define OHCI_INTERRUPT_ENABLE	0x10
#define OHCI_INTERRUPT_DISABLE	0x14
#define OHCI_HCCA		0x18
#define OHCI_PERIOD_CURRENT_ED	0x1c
#define OHCI_CONTROL_HEAD_ED	0x20
#define OHCI_CONTROL_CURRENT_ED	0x24
#define OHCI_BULK_HEAD_ED	0x28
#define OHCI_BULK_CURRENT_ED	0x2c
#define OHCI_DONE_HEAD		0x30
#define OHCI_FM_INTERVAL	0x34
#define  OHCI_GET_IVAL(s)	((s) & 0x3FFF)
#define  OHCI_GET_FSMPS(s)	(((s) >> 16) & 0x7FFF)
#define  OHCI_FIT		0x80000000
#define OHCI_FM_REMAINING	0x38
#define OHCI_FM_NUMBER		0x3C
#define OHCI_PERIODIC_START	0x40
#define OHCI_LS_THRESHOLD	0x44
#define OHCI_RH_DESCRIPTOR_A	0x48
#define  OHCI_GET_NDP(s)	((s) & 0xFF)
#define  OHCI_PSM		0x0100		/* Power Switching Mode */
#define  OHCI_NPS		0x0200		/* No Power Switching */
#define  OHCI_DT		0x0400		/* Device Type */
#define  OHCI_OCPM		0x0800		/* Overcurrent Protection Mode */
#define  OHCI_NOCP		0x1000		/* No Overcurrent Protection */
#define  OHCI_GET_POTPGT(s)	((s) >> 24)
#define OHCI_RH_DESCRIPTOR_B	0x4c
#define OHCI_RH_STATUS		0x50
#define  OHCI_LPS		0x00000001	/* Local Power Status */
#define  OHCI_OCI		0x00000002	/* OverCurrent Indicator */
#define  OHCI_DRWE		0x00008000	/* Device Remote Wakeup Enable */
#define  OHCI_LPSC		0x00010000	/* Local Power Status Change */
#define  OHCI_CCIC		0x00020000	/* OverCurrent Indicator Change */
#define  OHCI_CRWE		0x80000000	/* Clear Remote Wakeup Enable */
#define OHCI_RH_PORT_STATUS(n)	(0x50 + (n)*4)	/* 1 based indexing */

#define OHCI_LES		(OHCI_PLE|OHCI_IE|OHCI_CLE|OHCI_BLE)
#define OHCI_ALL_INTRS		(OHCI_SO|OHCI_WDH|OHCI_SF|OHCI_RD|OHCI_UE|OHCI_FNO|OHCI_RHSC|OHCI_OC)
#define OHCI_NORMAL_INTRS	(OHCI_SO|OHCI_WDH|OHCI_RD|OHCI_UE|OHCI_RHSC)

#define OHCI_FSMPS(i)		(((i - 210) * 6 / 7) << 16)
#define OHCI_PERIODIC(i)	((i) * 9 / 10)

#define OHCI_PAGE_SIZE		0x1000
#define OHCI_PAGE(x)		((x) &~ 0xFFF)
#define OHCI_PAGE_OFFSET(x)	((x) & 0xFFF)
#define OHCI_PAGE_MASK(x)	((x) & 0xFFF)

/*
 ****** Estrutura "ohci_ed" *************************************
 */
#define OHCI_ED_ALIGN		16

#define OHCI_ED_GET_FA(s)	((s) & 0x7F)
#define OHCI_ED_ADDRMASK	0x0000007F
#define OHCI_ED_SET_FA(s)	(s)
#define OHCI_ED_GET_EN(s)	(((s) >> 7) & 0x0F)
#define OHCI_ED_SET_EN(s)	((s) << 7)
#define OHCI_ED_DIR_MASK	0x00001800
#define  OHCI_ED_DIR_TD		0x00000000
#define  OHCI_ED_DIR_OUT	0x00000800
#define  OHCI_ED_DIR_IN		0x00001000
#define OHCI_ED_SPEED		0x00002000
#define OHCI_ED_SKIP		0x00004000
#define OHCI_ED_FORMAT_GEN	0x00000000
#define OHCI_ED_FORMAT_ISO	0x00008000
#define OHCI_ED_GET_MAXP(s)	(((s) >> 16) & 0x07FF)
#define OHCI_ED_SET_MAXP(s)	((s) << 16)
#define OHCI_ED_MAXPMASK	(0x7FF << 16)

#define OHCI_HALTED		0x00000001
#define OHCI_TOGGLECARRY	0x00000002
#define OHCI_HEADMASK		0xFFFFFFFC

struct ohci_ed
{
	ulong			ed_flags;
	ulong			ed_tailp;
	ulong			ed_headp;
	ulong			ed_nexted;
};

/*
 ****** Estrutura "ohci_td" *************************************
 */
#define OHCI_TD_ALIGN		16

#define OHCI_TD_R		0x00040000		/* Buffer Rounding  */
#define OHCI_TD_DP_MASK		0x00180000		/* Direction / PID */
#define  OHCI_TD_SETUP		0x00000000
#define  OHCI_TD_OUT		0x00080000
#define  OHCI_TD_IN		0x00100000
#define OHCI_TD_GET_DI(x)	(((x) >> 21) & 7)	/* Delay Interrupt */
#define OHCI_TD_SET_DI(x)	((x) << 21)
#define  OHCI_TD_NOINTR		0x00E00000
#define  OHCI_TD_INTR_MASK	0x00E00000 
#define OHCI_TD_TOGGLE_CARRY	0x00000000
#define OHCI_TD_TOGGLE_0	0x02000000
#define OHCI_TD_TOGGLE_1	0x03000000
#define OHCI_TD_TOGGLE_MASK	0x03000000
#define OHCI_TD_GET_EC(x)	(((x) >> 26) & 3)	/* Error Count */
#define OHCI_TD_GET_CC(x)	((x) >> 28)		/* Condition Code */
#define  OHCI_TD_NOCC		0xF0000000

struct ohci_td
{
	ulong			td_flags;
	ulong			td_cbp;		/* Current Buffer Pointer */
	ulong			td_nexttd;	/* Next TD */
	ulong			td_be;		/* Buffer End */
};

/*
 ****** Estrutura "ohci_itd" ************************************
 */
#define OHCI_ITD_NOFFSET	8
#define OHCI_ITD_ALIGN		32

#define OHCI_ITD_GET_SF(x)	((x) & 0x0000FFFF)
#define OHCI_ITD_SET_SF(x)	((x) & 0xFFFF)
#define OHCI_ITD_GET_DI(x)	(((x) >> 21) & 7)	/* Delay Interrupt */
#define OHCI_ITD_SET_DI(x)	((x) << 21)
#define  OHCI_ITD_NOINTR	0x00E00000
#define OHCI_ITD_GET_FC(x)	((((x) >> 24) & 7)+1)	/* Frame Count */
#define OHCI_ITD_SET_FC(x)	(((x)-1) << 24)
#define OHCI_ITD_GET_CC(x)	((x) >> 28)		/* Condition Code */
#define  OHCI_ITD_NOCC		0xF0000000

#define itd_pswn itd_offset				/* Packet Status Word*/
#define OHCI_ITD_PAGE_SELECT	0x00001000
#define OHCI_ITD_MK_OFFS(len)	(0xE000 | ((len) & 0x1FFF))
#define OHCI_ITD_PSW_LENGTH(x)	((x) & 0xFFF)		/* Transfer length */
#define OHCI_ITD_PSW_GET_CC(x)	((x) >> 12)		/* Condition Code */

struct ohci_itd
{
	ulong			itd_flags;
	ulong			itd_bp0;			/* Buffer Page 0 */
	ulong			itd_nextitd;			/* Next ITD */
	ulong			itd_be;				/* Buffer End */
	ushort			itd_offset[OHCI_ITD_NOFFSET];	/* Buffer offsets */
};

#define OHCI_CC_NO_ERROR		0
#define OHCI_CC_CRC			1
#define OHCI_CC_BIT_STUFFING		2
#define OHCI_CC_DATA_TOGGLE_MISMATCH	3
#define OHCI_CC_STALL			4
#define OHCI_CC_DEVICE_NOT_RESPONDING	5
#define OHCI_CC_PID_CHECK_FAILURE	6
#define OHCI_CC_UNEXPECTED_PID		7
#define OHCI_CC_DATA_OVERRUN		8
#define OHCI_CC_DATA_UNDERRUN		9
#define OHCI_CC_BUFFER_OVERRUN		12
#define OHCI_CC_BUFFER_UNDERRUN		13
#define OHCI_CC_NOT_ACCESSED		15

/* Some delay needed when changing certain registers. */

#define OHCI_ENABLE_POWER_DELAY		5
#define OHCI_READ_DESC_DELAY		5

/*
 ****** Estrutura "ohci_soft_ed" ********************************
 */
#define OHCI_SED_SIZE		((sizeof (struct ohci_soft_ed) + OHCI_ED_ALIGN - 1) / OHCI_ED_ALIGN * OHCI_ED_ALIGN)
#define OHCI_SED_CHUNK		128

struct ohci_soft_ed
{
	struct ohci_ed		ed;
	struct ohci_soft_ed	*next;
	ulong			physaddr;
};

#define OHCI_CALL_DONE	0x0001
#define OHCI_ADD_LEN	0x0002
#define OHCI_TD_HANDLED	0x0004		/* signal process_done has seen it */

/*
 ****** Estrutura "ohci_soft_td" ********************************
 */
#define OHCI_STD_SIZE		((sizeof (struct ohci_soft_td) + OHCI_TD_ALIGN - 1) / OHCI_TD_ALIGN * OHCI_TD_ALIGN)
#define OHCI_STD_CHUNK		128

struct ohci_soft_td
{
	struct ohci_td		td;

	struct ohci_soft_td	*nexttd;	/* mirrors nexttd in TD */
	struct ohci_soft_td	*dnext;		/* next in done list */

	ulong			physaddr;

	struct usbd_xfer	*xfer;

	ushort			len;
	ushort			flags;

	struct ohci_soft_td	*next, **prev;
};

/*
 ****** Estrutura "ohci_soft_itd" *******************************
 */
#define OHCI_SITD_SIZE		((sizeof (struct ohci_soft_itd) + OHCI_ITD_ALIGN - 1) / OHCI_ITD_ALIGN * OHCI_ITD_ALIGN)
#define OHCI_SITD_CHUNK		64

#define	OHCI_ITD_ACTIVE		0x0010	/* Hardware op in progress */
#define	OHCI_ITD_INTFIN		0x0020	/* Hw completion interrupt seen */

struct ohci_soft_itd
{
	struct ohci_itd		itd;

	struct ohci_soft_itd	*nextitd;	/* mirrors nexttd in ITD */
	struct ohci_soft_itd	*dnext;		/* next in done list */

	ulong			physaddr;

	struct usbd_xfer	*xfer;
	ushort			flags;

	struct ohci_soft_itd	*next, **prev;
};

/*
 ****** Estrutura "ohci_hcca" ***********************************
 */
enum { OHCI_NO_INTRS = 32 };

#define OHCI_HCCA_SIZE		256
#define OHCI_HCCA_ALIGN		256
#define OHCI_DONE_INTRS		1

struct ohci_hcca
{
	ulong			hcca_interrupt_table[OHCI_NO_INTRS];
	ulong			hcca_frame_number;
	ulong			hcca_done_head;
};

/*
 ****** Estrutura "ohci_softc" **********************************
 */
enum { OHCI_HASH_SIZE = 128 };

#define HASH(a)			(((a) >> 4) % OHCI_HASH_SIZE)

#define OHCI_NO_EDS		(2 * OHCI_NO_INTRS - 1)

struct ohci_softc
{
	struct usbd_bus		sc_bus;			/* base device */

	ulong			sc_hccadma;		/* Endereço físico */
	struct ohci_hcca	*sc_hcca;

	struct ohci_soft_ed	*sc_eds[OHCI_NO_EDS];
	ulong			sc_bws[OHCI_NO_INTRS];

	ulong			sc_eintrs;		/* enabled interrupts */

	struct ohci_soft_ed	*sc_isoc_head;
	struct ohci_soft_ed	*sc_ctrl_head;
	struct ohci_soft_ed	*sc_bulk_head;

	struct ohci_soft_td	*sc_hash_tds[OHCI_HASH_SIZE];
	struct ohci_soft_itd	*sc_hash_itds[OHCI_HASH_SIZE];

	int			sc_noport;

	uchar			sc_addr;		/* device address */
	uchar			sc_conf;		/* device configuration */

	struct ohci_soft_ed	*sc_freeeds;
	struct ohci_soft_td	*sc_freetds;
	struct ohci_soft_itd	*sc_freeitds;

	struct usbd_xfer	*sc_free_xfers_first,	/* Lista de descritores livres */
				**sc_free_xfers_last;

	struct usbd_xfer	*sc_intrxfer;

	struct ohci_soft_itd	*sc_sidone;
	struct ohci_soft_td	*sc_sdone;

	char			sc_vendor[16];
	int			sc_id_vendor;

	ulong			sc_control;		/* Preserved during suspend/standby */
	ulong			sc_intre;

	ulong			sc_overrun_cnt;

	struct device		*sc_child;
	char			sc_dying;

#if (0)	/*******************************************************/
	struct timeval		sc_overrun_ntc;
	usb_callout_t		sc_tmo_rhsc;
#endif	/*******************************************************/
};

struct ohci_xfer
{
	struct usbd_xfer	xfer;
	struct usb_task		abort_task;
	ulong			ohci_xfer_flags;
};

#define OHCI_ISOC_DIRTY		0x01

/*
 ****** Estrutura "ohci_pipe" ***********************************
 */
struct ohci_pipe
{
	struct usbd_pipe		pipe;

	struct ohci_soft_ed		*sed;

	ulong				aborting;

	union
	{
		struct ohci_soft_td	*td;
		struct ohci_soft_itd	*itd;

	}	tail;

	/* Info needed for different pipe kinds */

	union
	{
		struct					/* Control pipe */
		{
			void			*reqdma;
			ulong			length;
			struct ohci_soft_td	*setup, *data, *stat;

		}	ctl;

		struct					/* Interrupt pipe */
		{
			int			nslots;
			int			pos;

		}	intr;

		struct					/* Bulk pipe */
		{
			ulong			length;
			int			isread;

		}	bulk;

		struct isoc				/* Iso pipe */
		{
			int			next,
						inuse;

		}	iso;

	}	u;
};

/*
 ****** Tabelas auxiliares **************************************
 */
const uchar		revbits[OHCI_NO_INTRS] =
{
	0x00, 0x10, 0x08, 0x18, 0x04, 0x14, 0x0C, 0x1C,
	0x02, 0x12, 0x0A, 0x1A, 0x06, 0x16, 0x0E, 0x1E,
	0x01, 0x11, 0x09, 0x19, 0x05, 0x15, 0x0D, 0x1D,
	0x03, 0x13, 0x0B, 0x1B, 0x07, 0x17, 0x0F, 0x1F
};

#ifdef	USB_MSG
const char *ohci_cc_strs[] =
{
	"NO_ERROR", 	"CRC",			 	"BIT_STUFFING", 	"DATA_TOGGLE_MISMATCH",
	"STALL", 	"DEVICE_NOT_RESPONDING", 	"PID_CHECK_FAILURE", 	"UNEXPECTED_PID",
	"DATA_OVERRUN",	"DATA_UNDERRUN",	 	"BUFFER_OVERRUN", 	"BUFFER_UNDERRUN",
	"reserved", 	"reserved",		 	"NOT_ACCESSED", 	"NOT_ACCESSED"
};
#endif	USB_MSG

/*
 ******	Protótipos de funções ***********************************
 */
int			ohci_init (struct ohci_softc *sc);
void			ohci_intr (struct intr_frame frame);
int			ohci_intr1 (struct ohci_softc *sc);
void			ohci_waitintr (struct ohci_softc *sc, struct usbd_xfer *xfer);

struct ohci_soft_ed	*ohci_alloc_sed (struct ohci_softc *);
void			ohci_free_sed (struct ohci_softc *, struct ohci_soft_ed *);

struct ohci_soft_td 	 *ohci_alloc_std (struct ohci_softc *);
void			ohci_free_std (struct ohci_softc *, struct ohci_soft_td *);

struct ohci_soft_itd	*ohci_alloc_sitd (struct ohci_softc *);
void			ohci_free_sitd (struct ohci_softc *sc, struct ohci_soft_itd *sitd);

struct ohci_soft_itd	*ohci_alloc_sitd (struct ohci_softc *sc);

int			ohci_alloc_std_chain
			(	struct ohci_pipe *opipe, struct ohci_softc *sc,
				int alen, int rd, struct usbd_xfer *xfer,
				struct ohci_soft_td *sp, struct ohci_soft_td **ep
			);

void			ohci_add_ed (struct ohci_soft_ed *sed, struct ohci_soft_ed *head);
void			ohci_rem_ed (struct ohci_soft_ed *sed, struct ohci_soft_ed *head);

struct ohci_soft_itd	*ohci_hash_find_itd (struct ohci_softc *sc, ulong a);
void			ohci_hash_add_itd (struct ohci_softc *sc, struct ohci_soft_itd *sitd);
void			ohci_hash_rem_itd (struct ohci_softc *sc, struct ohci_soft_itd *sitd);

struct ohci_soft_td	*ohci_hash_find_td (struct ohci_softc *sc, ulong a);
void			ohci_hash_add_td (struct ohci_softc *sc, struct ohci_soft_td *std);
void			ohci_hash_rem_td (struct ohci_softc *sc, struct ohci_soft_td *std);

int			ohci_device_setintr (struct ohci_softc *sc, struct ohci_pipe *opipe, int ival);
int			ohci_setup_isoc (struct usbd_pipe *pipe_ptr);

void			ohci_rhsc_able (struct ohci_softc *sc, int on);

void			ohci_add_done (struct ohci_softc *sc, ulong done);
void			ohci_rhsc (struct ohci_softc *sc, struct usbd_xfer *xfer);

int			ohci_device_request (struct usbd_xfer *xfer);
void			ohci_abort_xfer (struct usbd_xfer *xfer, int status);
void			ohci_close_pipe (struct usbd_pipe *pipe_ptr, struct ohci_soft_ed *head);

void			ohci_device_isoc_enter (struct usbd_xfer *xfer);

int			ohci_read_port_or_mem_char (const struct ohci_softc *up, int reg);
void			ohci_write_port_or_mem_char (const struct ohci_softc *up, int value, int reg);
int			ohci_read_port_or_mem_short (const struct ohci_softc *up, int reg);
void			ohci_write_port_or_mem_short (const struct ohci_softc *up, int value, int reg);
int			ohci_read_port_or_mem_long (const struct ohci_softc *up, int reg);
void			ohci_write_port_or_mem_long (const struct ohci_softc *up, int value, int reg);

/*
 ****************************************************************
 *	Função de "probe" dos controladores USB OHCI		*
 ****************************************************************
 */
char *
ohci_probe (PCIDATA *pci, ulong dev_vendor)
{
	if
	(	pci->pci_baseclass != PCIC_SERIALBUS     ||
		pci->pci_subclass  != PCIS_SERIALBUS_USB ||
		pci->pci_progif    != PCI_INTERFACE_OHCI
	)
		return (NOSTR);

	switch (dev_vendor)
	{
	    case 0x523710B9:
		return ("AcerLabs M5237 (Aladdin-V) USB controller");

	    case 0x740C1022:
		return ("AMD-756 USB Controller");

	    case 0x74141022:
		return ("AMD-766 USB Controller");

	    case 0xc8611045:
		return ("OPTi 82C861 (FireLink) USB controller");

	    case 0x00351033:
		return ("NEC uPD 9210 USB controller");

	    case 0x00D710DE:
		return ("nVidia nForce3 USB Controller");

	    case 0x06701095:
		return ("CMD Tech 670 (USB0670) USB controller");

	    case 0x06731095:
		return ("CMD Tech 673 (USB0673) USB controller");

	    case 0x70011039:
		return ("SiS 5571 USB controller");

	    case 0x0019106B:
		return ("Apple KeyLargo USB controller");

	    default:
		return ("OHCI (generic) USB controller");
	}

}	/* end ohci_probe */

/*
 ****************************************************************
 *	Obtém, do PCI, o "base_addr"				*
 ****************************************************************
 */
void
ohci_get_pci_base_addr (PCIDATA *pci, struct ohci_softc *sc)
{
	ulong	pos, addr;

   /***	sc->sc_bus.base_addr = 0; ***/

	for (pos = 0; pos < MAX_PCI_ADDR; pos++)
	{
		if ((addr = pci->pci_addr[pos]) == 0)
			continue;

		sc->sc_bus.base_addr   = addr;
		sc->sc_bus.port_mapped = pci->pci_map_mask & (1 << pos);

		return;
	}

}	/* end ohci_get_pci_base_addr */

/*
 ****************************************************************
 *	Função de "attach" dos controladores USB OHCI		*
 ****************************************************************
 */
void
ohci_attach (PCIDATA *pci, ulong dev_vendor)
{
	int			rev;
	struct ohci_softc	*sc;
	struct device		*ohci, *usb;

	/*
	 *	Verifica se este controlador deve ser habilitado
	 */
	if (++usb_unit >= NUSB)
		{ printf ("NÃO há mais espaço para controladores USB\n"); return; }

	if (scb.y_usb_enable[usb_unit] == 0)
		return;

	/*
	 *	Aloca e zera a estrutura "ohci_softc"
	 */
	if ((sc = malloc_byte (sizeof (struct ohci_softc))) == NULL)
		{ printf ("ohci_attach: memória esgotada\n"); return; }

	memclr (sc, sizeof (struct ohci_softc));

	usb_data[usb_unit].usb_data_ptr    = sc;
	usb_data[usb_unit].usb_data_type   = 'O';
	usb_data[usb_unit].usb_data_stolen = 0;

	/*
	 *	Obtém o "base_addr"
	 */
	ohci_get_pci_base_addr (pci, sc);

	if (sc->sc_bus.base_addr == 0)
		goto bad;

	sc->sc_bus.unit	= usb_unit;
	sc->sc_bus.irq	= pci->pci_intline;

	/*
	 *	Habilita o "BUS mastering"
	 */
	pci_write (pci, PCIR_COMMAND, pci_read (pci, PCIR_COMMAND, 2) | PCIM_CMD_BUSMASTEREN, 2);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (sc->sc_bus.irq, USB_PL, usb_unit, ohci_intr) < 0)
		goto bad;

	/*
	 *	Imprime o que encontrou
	 */
	rev = ohci_read_port_or_mem_long (sc, OHCI_REVISION);

	/*
	 *	A revisão deve ser 1.0 obrigatoriamente (Por quê ?)
	 */
	if (OHCI_REV_HI (rev) != 1 || OHCI_REV_LO (rev) != 0)
	{
		printf
		(	"usb%d: revisão %d.%d NÃO SUPORTADA para OHCI\n",
			usb_unit, OHCI_REV_HI (rev), OHCI_REV_LO (rev)
		);

		sc->sc_bus.usbrev = USBREV_UNKNOWN;
		goto bad;
	}

	sc->sc_bus.usbrev = USBREV_1_0;

	/*
	 *	Imprime o que encontrou
	 */
	printf
	(	"usb: [%d: 0x%X, %d, OHCI rev = 1.0]\n",
		usb_unit, sc->sc_bus.base_addr,	sc->sc_bus.irq
	);

	/*
	 *	Cria o dispositivo pai na mão
	 */
	if (usb_class_list == NULL && usb_create_classes () < 0)
		goto bad;

	ohci = make_device (NULL, "usb_controller", -1);

	if ((usb = device_add_child (ohci, "usb", -1)) == NULL)
		{ printf ("ohci_attach: memória esgotada\n"); goto bad; }

	usb->ivars = sc; sc->sc_bus.bdev = usb;

	/*
	 *	Inicialização
	 */
	if (ohci_init (sc) != USBD_NORMAL_COMPLETION)
		{ printf ("ohci_attach: Erro na inicialização de usb%d\n", usb_unit); goto bad; }

	/*
	 *	Somente agora deixa interromper
	 */
	sc->sc_eintrs = OHCI_NORMAL_INTRS;

	ohci_write_port_or_mem_long (sc, sc->sc_eintrs|OHCI_MIE, OHCI_INTERRUPT_ENABLE);

	/*
	 *	Procura dispsitivos USB
	 */
	device_probe_and_attach (usb);

	return;

    bad:
	free_byte (sc); 	usb_data[usb_unit].usb_data_ptr = NULL;

}	/* end ohci_attach */

/*
 ****************************************************************
 *	Inicializa o controlador USB OHCI			*
 ****************************************************************
 */
int
ohci_init (struct ohci_softc *sc)
{
	struct ohci_soft_ed	*sed, *psed;
	int			err, i;
	ulong			s, ctl, ival, hcr, fm, per, desca;

	for (i = 0; i < OHCI_HASH_SIZE; i++)
		sc->sc_hash_itds[i] = NULL;

	/* Inicializa a lista de descritores livres */

	sc->sc_free_xfers_first = NULL;
	sc->sc_free_xfers_last  = &sc->sc_free_xfers_first;

	/* XXX determine alignment by R/W */

	/* Allocate the HCCA area: deve estar alinhado em 256 */

	if ((sc->sc_hcca = (void *)PGTOBY (malloce (M_CORE, BYUPPG (OHCI_HCCA_SIZE)))) == NULL)
		return (USBD_NOMEM);

	sc->sc_hccadma = VIRT_TO_PHYS_ADDR (sc->sc_hcca);

	memclr (sc->sc_hcca, OHCI_HCCA_SIZE);

	sc->sc_eintrs = OHCI_NORMAL_INTRS;

	/* Allocate dummy ED that starts the control list */

	if ((sc->sc_ctrl_head = ohci_alloc_sed (sc)) == NULL)
		{ err = USBD_NOMEM; goto bad1; }

	sc->sc_ctrl_head->ed.ed_flags |= OHCI_ED_SKIP;

	/* Allocate dummy ED that starts the bulk list */

	if ((sc->sc_bulk_head = ohci_alloc_sed (sc)) == NULL)
		{ err = USBD_NOMEM; goto bad2; }

	sc->sc_bulk_head->ed.ed_flags |= OHCI_ED_SKIP;

	/* Allocate dummy ED that starts the isochronous list */

	if ((sc->sc_isoc_head = ohci_alloc_sed (sc)) == NULL)
		{ err = USBD_NOMEM; goto bad3; }

	sc->sc_isoc_head->ed.ed_flags |= OHCI_ED_SKIP;

	/* Allocate all the dummy EDs that make up the interrupt tree */

	for (i = 0; i < OHCI_NO_EDS; i++)
	{
		if ((sed = ohci_alloc_sed (sc)) == NULL)
		{
			while (--i >= 0)
				ohci_free_sed (sc, sc->sc_eds[i]);

			err = USBD_NOMEM; goto bad4;
		}

		/* All ED fields are set to 0 */

		sc->sc_eds[i]     = sed;
		sed->ed.ed_flags |= OHCI_ED_SKIP;

		if (i != 0)
			psed = sc->sc_eds[(i - 1) / 2];
		else
			psed = sc->sc_isoc_head;

		sed->next	  = psed;
		sed->ed.ed_nexted = psed->physaddr;
	}

	/*
	 * Fill HCCA interrupt table.  The bit reversal is to get
	 * the tree set up properly to spread the interrupts.
	 */
	for (i = 0; i < OHCI_NO_INTRS; i++)
	{
		sc->sc_hcca->hcca_interrupt_table[revbits[i]] =
		    sc->sc_eds[OHCI_NO_EDS - OHCI_NO_INTRS + i]->physaddr;
	}

	/* Determine in what context we are running */

	ctl = ohci_read_port_or_mem_long (sc, OHCI_CONTROL);

	if (ctl & OHCI_IR)
	{
		/* SMM active, request change */
#ifdef	USB_MSG
		printf ("ohci_init: SMM active, request owner change\n");
#endif	USB_MSG

		s = ohci_read_port_or_mem_long (sc, OHCI_COMMAND_STATUS);

		ohci_write_port_or_mem_long (sc, s|OHCI_OCR, OHCI_COMMAND_STATUS);

		for (i = 0; i < 100 && (ctl & OHCI_IR); i++)
		{
			usb_delay_ms (&sc->sc_bus, 1);

			ctl = ohci_read_port_or_mem_long (sc, OHCI_CONTROL);
		}

		if ((ctl & OHCI_IR) == 0)
		{
#ifdef	USB_MSG
			printf ("%s: SMM NÃO responde, reinicializando\n", sc->sc_bus.bdev->nameunit);
#endif	USB_MSG

			ohci_write_port_or_mem_long (sc, OHCI_HCFS_RESET, OHCI_CONTROL);

			goto reset;
		}
	}
#if (0)	/*******************************************************/
	elif ((ctl & OHCI_HCFS_MASK) != OHCI_HCFS_RESET)
	{
		/* Don't bother trying to reuse the BIOS init, we'll reset it anyway */

		/* BIOS started controller */

		printf ("ohci_init: BIOS active\n");

		if ((ctl & OHCI_HCFS_MASK) != OHCI_HCFS_OPERATIONAL)
		{
			ohci_write_port_or_mem_long (sc, OHCI_HCFS_OPERATIONAL, OHCI_CONTROL);
			usb_delay_ms (&sc->sc_bus, USB_RESUME_DELAY);
		}
	}
#endif	/*******************************************************/
	else
	{
#ifdef	USB_MSG
		printf ("ohci_init: cold started\n");
#endif	USB_MSG

	    reset:
		/* Controller was cold started */
		usb_delay_ms (&sc->sc_bus, USB_BUS_RESET_DELAY);
	}

	/*
	 *	This reset should not be necessary according to the OHCI spec, but
	 *	without it some controllers do not start.
	 */
#ifdef	USB_MSG
	printf ("%s: resetting\n", sc->sc_bus.bdev->nameunit);
#endif	USB_MSG

	ohci_write_port_or_mem_long (sc, OHCI_HCFS_RESET, OHCI_CONTROL);

	usb_delay_ms (&sc->sc_bus, USB_BUS_RESET_DELAY);

	/* We now own the host controller and the bus has been reset */

	ival = OHCI_GET_IVAL (ohci_read_port_or_mem_long (sc, OHCI_FM_INTERVAL));

	ohci_write_port_or_mem_long (sc, OHCI_HCR, OHCI_COMMAND_STATUS); /* Reset HC */

	/* Nominal time for a reset is 10 us */

	for (i = 0; /* abaixo */; i++)
	{
		if (i >= 10)
		{
			printf ("%s: tempo expirado para \"reset\"\n", sc->sc_bus.bdev->nameunit);

			err = USBD_IOERROR; goto bad5;
		}

		DELAY (10);

		if ((hcr = ohci_read_port_or_mem_long (sc, OHCI_COMMAND_STATUS) & OHCI_HCR) == 0)
			break;
	}

	/* The controller is now in SUSPEND state, we have 2ms to finish */

	/* Set up HC registers */

	ohci_write_port_or_mem_long (sc, sc->sc_hccadma, OHCI_HCCA);
	ohci_write_port_or_mem_long (sc, sc->sc_ctrl_head->physaddr, OHCI_CONTROL_HEAD_ED);
	ohci_write_port_or_mem_long (sc, sc->sc_bulk_head->physaddr, OHCI_BULK_HEAD_ED);

	/* disable all interrupts and then switch on all desired interrupts */

	ohci_write_port_or_mem_long (sc, OHCI_ALL_INTRS, OHCI_INTERRUPT_DISABLE);

	/* switch on desired functional features */

	ctl = ohci_read_port_or_mem_long (sc, OHCI_CONTROL);

	ctl &= ~ (OHCI_CBSR_MASK|OHCI_LES|OHCI_HCFS_MASK|OHCI_IR);
	ctl |= OHCI_PLE|OHCI_IE|OHCI_CLE|OHCI_BLE|OHCI_RATIO_1_4|OHCI_HCFS_OPERATIONAL;

	/* And finally start it! */

	ohci_write_port_or_mem_long (sc, ctl, OHCI_CONTROL);

	/*
	 * The controller is now OPERATIONAL.  Set a some final
	 * registers that should be set earlier, but that the
	 * controller ignores when in the SUSPEND state.
	 */
	fm = (ohci_read_port_or_mem_long (sc, OHCI_FM_INTERVAL) & OHCI_FIT) ^ OHCI_FIT;
	fm |= OHCI_FSMPS (ival) | ival;

	ohci_write_port_or_mem_long (sc, fm, OHCI_FM_INTERVAL);

	per = OHCI_PERIODIC (ival); /* 90% periodic */

	ohci_write_port_or_mem_long (sc, per, OHCI_PERIODIC_START);

	/* Fiddle the No OverCurrent Protection bit to avoid chip bug */

	desca = ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_A);

	ohci_write_port_or_mem_long (sc, desca | OHCI_NOCP, OHCI_RH_DESCRIPTOR_A);
	ohci_write_port_or_mem_long (sc, OHCI_LPSC, OHCI_RH_STATUS); /* Enable port power */

	usb_delay_ms (&sc->sc_bus, OHCI_ENABLE_POWER_DELAY);

	ohci_write_port_or_mem_long (sc, desca, OHCI_RH_DESCRIPTOR_A);

	/*
	 * The AMD756 requires a delay before re-reading the register,
	 * otherwise it will occasionally report 0 ports.
	 */
	usb_delay_ms (&sc->sc_bus, OHCI_READ_DESC_DELAY);

	sc->sc_noport = OHCI_GET_NDP (ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_A));

	/* Set up the bus struct */

	sc->sc_bus.methods	= &ohci_bus_methods;
	sc->sc_bus.pipe_size	= sizeof (struct ohci_pipe);

#if (0)	/*******************************************************/
	usb_callout_init (sc->sc_tmo_rhsc);
#endif	/*******************************************************/

	return (USBD_NORMAL_COMPLETION);

	/*
	 *	Vários erros
	 */
 bad5:
	for (i = 0; i < OHCI_NO_EDS; i++)
		ohci_free_sed (sc, sc->sc_eds[i]);
 bad4:
	ohci_free_sed (sc, sc->sc_isoc_head);
 bad3:
	ohci_free_sed (sc, sc->sc_ctrl_head);
 bad2:
	ohci_free_sed (sc, sc->sc_bulk_head);
 bad1:
	mrelease (M_CORE, BYUPPG (OHCI_HCCA_SIZE), BYUPPG (sc->sc_hcca));

	return (err);

}	/* end ohci_init */

/*
 ****************************************************************
 *	Rotina de Interrupção					*
 ****************************************************************
 */
void
ohci_intr (struct intr_frame frame)
{
	struct ohci_softc	*sc = usb_data[frame.if_unit].usb_data_ptr;

	if (sc == NULL || sc->sc_dying)
		return;

#ifdef	USB_MSG
	printf ("ohci_intr: real interrupt\n");
#endif	USB_MSG

#ifdef	USB_POLLING
	if (sc->sc_bus.use_polling)
	{
		printf ("ohci_intr: ignorando interrupção durante o \"polling\"\n");
		return;
	}
#endif	USB_POLLING

	ohci_intr1 (sc);

}	/* end ohci_intr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct ohci_soft_ed *
ohci_alloc_sed (struct ohci_softc *sc)
{
	struct ohci_soft_ed	*sed;
	int			i, offs, byte_size;
	void			*virt_addr;

	if (sc->sc_freeeds == NULL)
	{
		byte_size = OHCI_SED_SIZE * OHCI_SED_CHUNK;

		if ((virt_addr = (void *)PGTOBY (malloce (M_CORE, BYUPPG (byte_size)))) == NULL)
			return (NULL);

		for (i = 0; i < OHCI_SED_CHUNK; i++)
		{
			offs		= i * OHCI_SED_SIZE;
			sed		= (struct ohci_soft_ed *)(virt_addr + offs);
			sed->physaddr	= VIRT_TO_PHYS_ADDR (sed);
			sed->next	= sc->sc_freeeds;
			sc->sc_freeeds	= sed;
		}
	}

	sed		= sc->sc_freeeds;
	sc->sc_freeeds	= sed->next;

	memclr (&sed->ed, sizeof (struct ohci_ed));

	sed->next = 0;

	return (sed);

}	/* end ohci_alloc_sed */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_free_sed (struct ohci_softc *sc, struct ohci_soft_ed *sed)
{
	sed->next = sc->sc_freeeds; sc->sc_freeeds = sed;

}	/* end ohci_free_sed */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct ohci_soft_td *
ohci_alloc_std (struct ohci_softc *sc)
{
	struct ohci_soft_td	*std;
	int			byte_size, i, offs, s;
	void			*virt_addr;

	if (sc->sc_freetds == NULL)
	{
		byte_size = OHCI_STD_SIZE * OHCI_STD_CHUNK;

		if ((virt_addr = (void *)PGTOBY (malloce (M_CORE, BYUPPG (byte_size)))) == NULL)
			return (NULL);

		s = splx (irq_pl_vec[sc->sc_bus.irq]);

		for (i = 0; i < OHCI_STD_CHUNK; i++)
		{
			offs		= i * OHCI_STD_SIZE;
			std		= (struct ohci_soft_td *)(virt_addr + offs);
			std->physaddr	= VIRT_TO_PHYS_ADDR (std);
			std->nexttd	= sc->sc_freetds;
			sc->sc_freetds	= std;
		}

		splx (s);
	}

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	std = sc->sc_freetds;
	sc->sc_freetds = std->nexttd;

	memclr (&std->td, sizeof (struct ohci_td));

	std->nexttd = NULL;
	std->xfer = NULL;
	ohci_hash_add_td (sc, std);

	splx (s);

	return (std);

}	/* end ohci_alloc_std */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_free_std (struct ohci_softc *sc, struct ohci_soft_td *std)
{
	int		s;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	ohci_hash_rem_td (sc, std);

	std->nexttd = sc->sc_freetds; sc->sc_freetds = std;

	splx (s);

}	/* end ohci_free_std */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_alloc_std_chain (struct ohci_pipe *opipe, struct ohci_softc *sc,
		     int alen, int rd, struct usbd_xfer *xfer,
		     struct ohci_soft_td *sp, struct ohci_soft_td **ep)
{
	struct ohci_soft_td	*next, *cur;
	ulong			dataphys, tdflags;
	int			offset = 0, len, curlen;
	void			*dma = xfer->dmabuf;
	ushort			flags = xfer->flags;

	len = alen; cur = sp;

	tdflags =	(rd ? OHCI_TD_IN : OHCI_TD_OUT) |
			(flags & USBD_SHORT_XFER_OK ? OHCI_TD_R : 0) |
			OHCI_TD_NOCC | OHCI_TD_TOGGLE_CARRY | OHCI_TD_NOINTR;

	for (EVER)
	{
		if ((next = ohci_alloc_std (sc)) == NULL)
			goto nomem;

		dataphys = VIRT_TO_PHYS_ADDR (dma + offset);

		/*
		 *	The OHCI hardware can handle at most one 4k crossing.
		 *	XXX - currently we only allocate contigous buffers, but
		 *	the OHCI spec says: If during the data transfer the buffer
		 *	address contained in the HC's working copy of
		 *	CurrentBufferPointer crosses a 4K boundary, the upper 20
		 *	bits of Buffer End are copied to the working value of
		 *	CurrentBufferPointer causing the next buffer address to
		 *	be the 0th byte in the same 4K page that contains the
		 *	last byte of the buffer (the 4K boundary crossing may
		 *	occur within a data packet transfer.)
		 *
		 *	If/when dma has multiple segments, this will need to
		 *	properly handle fragmenting TD's.
		 *
		 *	We can describe the above using maxsegsz = 4k and nsegs = 2
		 *	in the future.
		 */
		if
		(	OHCI_PAGE (dataphys) == OHCI_PAGE (VIRT_TO_PHYS_ADDR (dma + offset + len - 1)) ||
			len - (OHCI_PAGE_SIZE - OHCI_PAGE_OFFSET (dataphys)) <= OHCI_PAGE_SIZE
		)
		{
			/* we can handle it in this TD */
			curlen = len;
		}
		else
		{
			/*	XXX The calculation below is wrong and could
			 *	result in a packet that is not a multiple of the
			 *	MaxPacketSize in the case where the buffer does not
			 *	start on an appropriate address (like for example in
			 *	the case of an mbuf cluster). You'll get an early
			 *	short packet.
			 */
			/* must use multiple TDs, fill as much as possible. */

			curlen = 2 * OHCI_PAGE_SIZE - OHCI_PAGE_OFFSET (dataphys);

			/* the length must be a multiple of the max size */

			curlen -= curlen % UGETW (opipe->pipe.endpoint->edesc->wMaxPacketSize);
#ifdef	USB_MSG
			if (curlen == 0)
				printf ("ohci_alloc_std: curlen == 0");
#endif	USB_MSG
		}

		len -= curlen;

		cur->td.td_flags  = tdflags;
		cur->td.td_cbp	  = dataphys;
		cur->nexttd	  = next;
		cur->td.td_nexttd = next->physaddr;
		cur->td.td_be	  = VIRT_TO_PHYS_ADDR (dma + offset + curlen - 1);
		cur->len	  = curlen;
		cur->flags	  = OHCI_ADD_LEN;
		cur->xfer	  = xfer;

		if (len == 0)
			break;

		if (len < 0)
			printf ("Length went negative: %d curlen %d dma %p offset %08x", len, curlen, dma, (int)0);

		offset += curlen; cur = next;
	}

	if
	(	(flags & USBD_FORCE_SHORT_XFER) &&
		(alen % UGETW (opipe->pipe.endpoint->edesc->wMaxPacketSize)) == 0
	)
	{
		/* Force a 0 length transfer at the end. */

		cur = next;

		if ((next = ohci_alloc_std (sc)) == NULL)
			goto nomem;

		cur->td.td_flags  = tdflags;
		cur->td.td_cbp	  = 0;			/* indicate 0 length packet */
		cur->nexttd	  = next;
		cur->td.td_nexttd = next->physaddr;
		cur->td.td_be	  = ~0;
		cur->len	  = 0;
		cur->flags	  = 0;
		cur->xfer	  = xfer;
	}

	*ep = cur;

	return (USBD_NORMAL_COMPLETION);

 nomem:
	/* XXX free chain */
	return (USBD_NOMEM);

}	/* end ohci_alloc_std_chain */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_free_std_chain (struct ohci_softc *sc, struct ohci_soft_td *std, struct ohci_soft_td *stdend)
{
	struct ohci_soft_td	*p;

	for (/* vazio */; std != stdend; std = p)
	{
		p = std->nexttd; ohci_free_std (sc, std);
	}
}
#endif	/*******************************************************/

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct ohci_soft_itd *
ohci_alloc_sitd (struct ohci_softc *sc)
{
	struct ohci_soft_itd	*sitd;
	int			byte_size, i, s, offs;
	void			*virt_addr;

	if (sc->sc_freeitds == NULL)
	{
		byte_size = OHCI_SITD_SIZE * OHCI_SITD_CHUNK;

		if ((virt_addr = (void *)PGTOBY (malloce (M_CORE, BYUPPG (byte_size)))) == NULL)
			return (NULL);

		for (i = 0; i < OHCI_SITD_CHUNK; i++)
		{
			offs		= i * OHCI_SITD_SIZE;
			sitd		= (struct ohci_soft_itd *)(virt_addr + offs);
			sitd->physaddr	= VIRT_TO_PHYS_ADDR (sitd);
			sitd->nextitd	= sc->sc_freeitds;
			sc->sc_freeitds	= sitd;
		}
	}

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sitd		= sc->sc_freeitds;
	sc->sc_freeitds = sitd->nextitd;

	memclr (&sitd->itd, sizeof (struct ohci_itd));

	sitd->nextitd	= NULL;
	sitd->xfer	= NULL;

	ohci_hash_add_itd (sc, sitd);

	splx (s);

	return (sitd);

}	/* end ohci_alloc_sitd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_free_sitd (struct ohci_softc *sc, struct ohci_soft_itd *sitd)
{
	int		s;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	ohci_hash_rem_itd (sc, sitd);

	sitd->nextitd	= sc->sc_freeitds;
	sc->sc_freeitds = sitd;

	splx (s);

}	/* end ohci_free_sitd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct usbd_xfer *
ohci_allocx (struct usbd_bus *bus)
{
	struct ohci_softc	*sc = (struct ohci_softc *)bus;
	struct usbd_xfer	*xfer;

	if ((xfer = sc->sc_free_xfers_first) != NULL)
	{
		/* Retira o primeiro da lista de descritores livres */

		if ((sc->sc_free_xfers_first = sc->sc_free_xfers_first->next_xfer) == NULL)
			sc->sc_free_xfers_last = &sc->sc_free_xfers_first;
	}
	else
	{
		/* É necessário alocar */

		xfer = malloc_byte (sizeof (struct ohci_xfer));
	}

	if (xfer != NULL)
		memclr (xfer, sizeof (struct ohci_xfer));

	return (xfer);

}	/* end ohci_allocx */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_freex (struct usbd_bus *bus, struct usbd_xfer *xfer)
{
	struct ohci_softc	*sc = (struct ohci_softc *)bus;

	if ((xfer->next_xfer = sc->sc_free_xfers_first) == NULL)
		sc->sc_free_xfers_last = &xfer->next_xfer;

	sc->sc_free_xfers_first = xfer;

}	/* end ohci_freex */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_intr1 (struct ohci_softc *sc)
{
	ulong		intrs, eintrs, done;

	/* In case the interrupt occurs before initialization has completed. */
	if (sc == NULL || sc->sc_hcca == NULL)
		return (0);

	intrs = 0;
	done = sc->sc_hcca->hcca_done_head;

	/* The LSb of done is used to inform the HC Driver that an interrupt
	 * condition exists for both the Done list and for another event
	 * recorded in HcInterruptStatus. On an interrupt from the HC, the HC
	 * Driver checks the HccaDoneHead Value. If this value is 0, then the
	 * interrupt was caused by other than the HccaDoneHead update and the
	 * HcInterruptStatus register needs to be accessed to determine that
	 * exact interrupt cause. If HccaDoneHead is nonzero, then a Done list
	 * update interrupt is indicated and if the LSb of done is nonzero,
	 * then an additional interrupt event is indicated and
	 * HcInterruptStatus should be checked to determine its cause.
	 */
	if (done != 0)
	{
		if (done & ~OHCI_DONE_INTRS)
			intrs = OHCI_WDH;

		if (done & OHCI_DONE_INTRS)
		{
			intrs |= ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_STATUS);
			done &= ~OHCI_DONE_INTRS;
		}

		sc->sc_hcca->hcca_done_head = 0;
	}
	else
	{
		intrs = ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_STATUS) & ~OHCI_WDH;
	}

	if (intrs == 0)		/* nothing to be done (PCI shared interrupt) */
		return (0);

	intrs &= ~OHCI_MIE;

	ohci_write_port_or_mem_long (sc, intrs, OHCI_INTERRUPT_STATUS); /* Acknowledge */

	eintrs = intrs & sc->sc_eintrs;

	if (!eintrs)
		return (0);

	sc->sc_bus.intr_context++;
	sc->sc_bus.no_intrs++;

	if (eintrs & OHCI_SO)
	{
		sc->sc_overrun_cnt++;

#if (0)	/*******************************************************/
		if (usbd_ratecheck(&sc->sc_overrun_ntc))
		{
			printf("%s: %u scheduling overruns\n", sc->sc_bus.bdev->nameunit, sc->sc_overrun_cnt);
			sc->sc_overrun_cnt = 0;
		}
#endif	/*******************************************************/
		/* XXX do what */
		eintrs &= ~OHCI_SO;
	}

	if (eintrs & OHCI_WDH)
	{
		ohci_add_done (sc, done &~ OHCI_DONE_INTRS);
		usb_schedsoftintr (&sc->sc_bus);
		eintrs &= ~OHCI_WDH;
	}

	if (eintrs & OHCI_RD)
	{
		printf ("%s: resume detect\n", sc->sc_bus.bdev->nameunit);
		/* XXX process resume detect */
	}

	if (eintrs & OHCI_UE)
	{
		printf ("%s: unrecoverable error, controller halted\n", sc->sc_bus.bdev->nameunit);

		ohci_write_port_or_mem_long (sc, OHCI_HCFS_RESET, OHCI_CONTROL);
		/* XXX what else */
	}

	if (eintrs & OHCI_RHSC)
	{
		ohci_rhsc (sc, sc->sc_intrxfer);

		/*
		 * Disable RHSC interrupt for now, because it will be
		 * on until the port has been reset.
		 */
		ohci_rhsc_able (sc, 0);
#if (0)	/*******************************************************/
		/* Do not allow RHSC interrupts > 1 per second */
		usb_callout (sc->sc_tmo_rhsc, hz, ohci_rhsc_enable, sc);
#endif	/*******************************************************/
		eintrs &= ~OHCI_RHSC;
	}

	sc->sc_bus.intr_context--;

	if (eintrs != 0)
	{
		/* Block unprocessed interrupts. XXX */

		ohci_write_port_or_mem_long (sc, eintrs, OHCI_INTERRUPT_DISABLE);

		sc->sc_eintrs &= ~eintrs;

		printf ("%s: blocking intrs 0x%x\n", sc->sc_bus.bdev->nameunit, eintrs);
	}

	return (1);

}	/* end ohci_intr1 */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_rhsc_able (struct ohci_softc *sc, int on)
{
#ifdef	USB_MSG
	printf ("ohci_rhsc_able: on=%d\n", on);
#endif	USB_MSG

	if (on)
	{
		sc->sc_eintrs |= OHCI_RHSC;
		ohci_write_port_or_mem_long (sc, OHCI_RHSC, OHCI_INTERRUPT_ENABLE);
	}
	else
	{
		sc->sc_eintrs &= ~OHCI_RHSC;
		ohci_write_port_or_mem_long (sc, OHCI_RHSC, OHCI_INTERRUPT_DISABLE);
	}

}	/* end ohci_rhsc_able */

#if (0)	/*******************************************************/
void
ohci_rhsc_enable (void *v_sc)
{
	struct ohci_softc *sc = v_sc;

	ohci_rhsc_able (sc, 1);
}
#endif	/*******************************************************/

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_add_done (struct ohci_softc *sc, ulong done)
{
	struct ohci_soft_itd	*sitd, *sidone, **ip;
	struct ohci_soft_td	*std,  *sdone,  **p;

	/* Reverse the done list */

	for (sdone = NULL, sidone = NULL; done != 0; )
	{
		if ((std = ohci_hash_find_td (sc, done)) != NULL)
		{
			std->dnext	= sdone;
			done		= std->td.td_nexttd;
			sdone		= std;

			continue;
		}

		if ((sitd = ohci_hash_find_itd (sc, done)) != NULL)
		{
			sitd->dnext	= sidone;
			done		= sitd->itd.itd_nextitd;
			sidone		= sitd;

			continue;
		}

		panic ("ohci_add_done: addr %P not found\n", done);
	}

	/* sdone & sidone now hold the done lists */
	/* Put them on the already processed lists */

	for (p = &sc->sc_sdone; *p != NULL; p = &(*p)->dnext)
		/* vazio */;

	*p = sdone;

	for (ip = &sc->sc_sidone; *ip != NULL; ip = &(*ip)->dnext)
		/* vazio */;

	*ip = sidone;

}	/* end ohci_add_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_softintr (void *v)
{
	struct ohci_softc		*sc = v;
	struct ohci_soft_itd		*sitd, *sidone, *sitdnext;
	struct ohci_soft_td		*std, *sdone, *stdnext;
	struct usbd_xfer		*xfer;
	struct ohci_pipe		*opipe;
	int				len, cc, s;

	sc->sc_bus.intr_context++;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sdone = sc->sc_sdone; 	sc->sc_sdone = NULL;
	sidone = sc->sc_sidone; sc->sc_sidone = NULL;

	splx (s);

	for (std = sdone; std; std = stdnext)
	{
		xfer = std->xfer;
		stdnext = std->dnext;

		if (xfer == NULL || (std->flags & OHCI_TD_HANDLED))
		{
			/*
			 * xfer == NULL: There seems to be no xfer associated
			 * with this TD. It is tailp that happened to end up on
			 * the done queue.
			 * flags & OHCI_TD_HANDLED: The TD has already been
			 * handled by process_done and should not be done again.
			 * Shouldn't happen, but some chips are broken(?).
			 */
			continue;
		}

		if (xfer->status == USBD_CANCELLED || xfer->status == USBD_TIMEOUT)
		{
			/* Handled by abort routine. */
			continue;
		}

#if (0)	/*******************************************************/
		usb_uncallout(xfer->timeout_handle, ohci_timeout, xfer);
#endif	/*******************************************************/

		len = std->len;

		if (std->td.td_cbp != 0)
			len -= std->td.td_be - std->td.td_cbp + 1;

		if (std->flags & OHCI_ADD_LEN)
			xfer->actlen += len;

		cc = OHCI_TD_GET_CC (std->td.td_flags);

		if (cc == OHCI_CC_NO_ERROR)
		{
			if (std->flags & OHCI_CALL_DONE)
			{
				xfer->status = USBD_NORMAL_COMPLETION;

				s = splx (irq_pl_vec[sc->sc_bus.irq]);

				usb_transfer_complete (xfer);

				splx(s);
			}

			ohci_free_std (sc, std);
		}
		else
		{
			/*
			 *	Endpoint is halted.  First unlink all the TDs
			 *	belonging to the failed transfer, and then restart
			 *	the endpoint.
			 */
			struct ohci_soft_td	*p, *n;

			opipe = (struct ohci_pipe *)xfer->pipe;

			/* Mark all the TDs in the done queue for the current
			 * xfer as handled
			 */
			for (p = stdnext; p; p = p->dnext)
			{
				if (p->xfer == xfer)
					p->flags |= OHCI_TD_HANDLED;
			}

			/* remove TDs */
			for (p = std; p->xfer == xfer; p = n)
				{ n = p->nexttd; ohci_free_std (sc, p); }

			/* clear halt */
			opipe->sed->ed.ed_headp = p->physaddr;

			ohci_write_port_or_mem_long (sc, OHCI_CLF, OHCI_COMMAND_STATUS);

			if (cc == OHCI_CC_STALL)
				xfer->status = USBD_STALLED;
			else
				xfer->status = USBD_IOERROR;

			s = splx (irq_pl_vec[sc->sc_bus.irq]);

			usb_transfer_complete (xfer);

			splx (s);
		}
	}

	for (sitd = sidone; sitd != NULL; sitd = sitdnext)
	{
		xfer = sitd->xfer;
		sitdnext = sitd->dnext;
		sitd->flags |= OHCI_ITD_INTFIN;

		if (xfer == NULL)
			continue;

		if (xfer->status == USBD_CANCELLED || xfer->status == USBD_TIMEOUT)
			continue;

		if (xfer->pipe)
		{
			if (xfer->pipe->aborting)
				continue;
		}

		opipe = (struct ohci_pipe *)xfer->pipe;

		if (opipe->aborting)
			continue;
 
		cc = OHCI_ITD_GET_CC (sitd->itd.itd_flags);

		if (cc == OHCI_CC_NO_ERROR)
		{
			/* XXX compute length for input */
			if (sitd->flags & OHCI_CALL_DONE)
			{
				opipe->u.iso.inuse -= xfer->nframes;
				/* XXX update frlengths with actual length */
				/* XXX xfer->actlen = actlen; */
				xfer->status = USBD_NORMAL_COMPLETION;

				s = splx (irq_pl_vec[sc->sc_bus.irq]);

				usb_transfer_complete (xfer);

				splx (s);
			}
		}
		else
		{
			/* XXX Do more */
			xfer->status = USBD_IOERROR;

			s = splx (irq_pl_vec[sc->sc_bus.irq]);

			usb_transfer_complete (xfer);

			splx (s);
		}
	}

	sc->sc_bus.intr_context--;

}	/* end ohci_softintr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_ctrl_done (struct usbd_xfer *xfer)
{
	xfer->hcpriv = NULL;

}	/* end ohci_device_ctrl_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_intr_done (struct usbd_xfer *xfer)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct ohci_softc		*sc = (struct ohci_softc *)opipe->pipe.device->bus;
	struct ohci_soft_ed		*sed = opipe->sed;
	struct ohci_soft_td		*data, *tail;

#ifdef	USB_MSG
	printf ("ohci_device_intr_done: xfer=%P, actlen=%d\n", xfer, xfer->actlen);
#endif	USB_MSG

	xfer->hcpriv = NULL;

	if (!xfer->pipe->repeat)
		return;

	data = opipe->tail.td;

	if ((tail = ohci_alloc_std (sc)) == NULL) /* XXX should reuse TD */
		{ xfer->status = USBD_NOMEM; return; }

	tail->xfer = NULL;

	data->td.td_flags = OHCI_TD_IN|OHCI_TD_NOCC|OHCI_TD_SET_DI (1)|OHCI_TD_TOGGLE_CARRY;

	if (xfer->flags & USBD_SHORT_XFER_OK)
		data->td.td_flags |= OHCI_TD_R;

	data->td.td_cbp		= VIRT_TO_PHYS_ADDR (xfer->dmabuf);
	data->nexttd		= tail;
	data->td.td_nexttd	= tail->physaddr;
	data->td.td_be		= data->td.td_cbp + xfer->length - 1;
	data->len		= xfer->length;
	data->xfer		= xfer;
	data->flags		= OHCI_CALL_DONE|OHCI_ADD_LEN;
	xfer->hcpriv		= data;
	xfer->actlen		= 0;

	sed->ed.ed_tailp	= tail->physaddr;
	opipe->tail.td		= tail;

}	/* end ohci_device_intr_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_bulk_done (struct usbd_xfer *xfer)
{
	xfer->hcpriv = NULL;

}	/* end ohci_device_bulk_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_rhsc (struct ohci_softc *sc, struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr;
	struct ohci_pipe	*opipe;
	uchar			*p;
	int			i, m, hstatus;

	hstatus = ohci_read_port_or_mem_long (sc, OHCI_RH_STATUS);

#ifdef	USB_MSG
	printf ("ohci_rhsc: sc=%P xfer=%P hstatus=0x%08x\n", sc, xfer, hstatus);
#endif	USB_MSG

	if (xfer == NULL)
		return; 		/* Just ignore the change */

	pipe_ptr = xfer->pipe; 	opipe = (struct ohci_pipe *)pipe_ptr;

	p = xfer->dmabuf;

	m = MIN (sc->sc_noport, xfer->length * 8 - 1);

	memclr (p, xfer->length);

	for (i = 1; i <= m; i++)
	{
		/* Pick out CHANGE bits from the status reg */

		if (ohci_read_port_or_mem_long (sc, OHCI_RH_PORT_STATUS (i)) >> 16)
			p[i/8] |= 1 << (i%8);
	}

#ifdef	USB_MSG
	printf ("ohci_rhsc: change=0x%02x\n", *p);
#endif	USB_MSG

	xfer->actlen = xfer->length;
	xfer->status = USBD_NORMAL_COMPLETION;

	usb_transfer_complete (xfer);

}	/* end ohci_rhsc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_root_intr_done (struct usbd_xfer *xfer)
{
	xfer->hcpriv = NULL;

}	/* end ohci_root_intr_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_root_ctrl_done (struct usbd_xfer *xfer)
{
	xfer->hcpriv = NULL;

}	/* end ohci_root_ctrl_done */

#ifdef	USB_POLLING
/*
 ****************************************************************
 *	Aguarda uma interrupção					*
 ****************************************************************
 */
void
ohci_waitintr (struct ohci_softc *sc, struct usbd_xfer *xfer)
{
	int		timo = xfer->timeout;
	int		usecs;
	ulong		intrs;

	xfer->status = USBD_IN_PROGRESS;

	for (usecs = timo * 1000000 / hz; usecs > 0; usecs -= 1000)
	{
		usb_delay_ms (&sc->sc_bus, 1);

		if (sc->sc_dying)
			break;

		intrs = ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_STATUS) & sc->sc_eintrs;

#ifdef	USB_MSG
		printf ("ohci_waitintr: 0x%04x\n", intrs);
#endif	USB_MSG

		if (intrs)
		{
			ohci_intr1 (sc);

			if (xfer->status != USBD_IN_PROGRESS)
				return;
		}
	}

	printf ("ohci_waitintr: tempo expirado\n");

	xfer->status = USBD_TIMEOUT;

	usb_transfer_complete (xfer);

	/* XXX should free TD */

}	/* end ohci_waitintr */
#endif	USB_POLLING

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_poll (struct usbd_bus *bus)
{
	struct ohci_softc *sc = (struct ohci_softc *)bus;

	if (ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_STATUS) & sc->sc_eintrs)
		ohci_intr1 (sc);

}	/* end ohci_poll */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_request (struct usbd_xfer *xfer)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct usb_device_request	*req = &xfer->request;
	struct usbd_device		*udev = opipe->pipe.device;
	struct ohci_softc		*sc = (struct ohci_softc *)udev->bus;
	int				addr = udev->address;
	struct ohci_soft_td		*data = 0;
	struct ohci_soft_td		*setup, *stat, *next, *tail;
	struct ohci_soft_ed		*sed;
	int				isread, len, err, s;

	isread = req->bmRequestType & UT_READ;

	len = UGETW (req->wLength);

#ifdef	USB_MSG
	printf
	(	"ohci_device_control type=0x%02x, request=0x%02x, "
		"wValue=0x%04x, wIndex=0x%04x len=%d, addr=%d, endpt=%d\n",
		req->bmRequestType, req->bRequest, UGETW (req->wValue),
		UGETW (req->wIndex), len, addr,
		opipe->pipe.endpoint->edesc->bEndpointAddress
	);
#endif	USB_MSG

	setup = opipe->tail.td;

	if ((stat = ohci_alloc_std (sc)) == NULL)
		{ err = USBD_NOMEM; goto bad1; }

	if ((tail = ohci_alloc_std (sc)) == NULL)
		{ err = USBD_NOMEM; goto bad2; }

	tail->xfer = NULL;

	sed = opipe->sed;
	opipe->u.ctl.length = len;

	/* Update device address and length since they may have changed */
	/* XXX This only needs to be done once, but it's too early in open */
	/* XXXX Should not touch ED here! */

	sed->ed.ed_flags = (sed->ed.ed_flags & ~ (OHCI_ED_ADDRMASK|OHCI_ED_MAXPMASK)) |
				OHCI_ED_SET_FA (addr) |
				OHCI_ED_SET_MAXP (UGETW (opipe->pipe.endpoint->edesc->wMaxPacketSize));

	/* Set up data transaction */

	if (len != 0)
	{
		if ((data = ohci_alloc_std (sc)) == NULL)
			{ err = USBD_NOMEM; goto bad3; }

		data->td.td_flags = (isread ? OHCI_TD_IN : OHCI_TD_OUT)|OHCI_TD_NOCC |
			OHCI_TD_TOGGLE_1|OHCI_TD_NOINTR |
			(xfer->flags & USBD_SHORT_XFER_OK ? OHCI_TD_R : 0);

		data->td.td_cbp		= VIRT_TO_PHYS_ADDR (xfer->dmabuf);
		data->nexttd		= stat;
		data->td.td_nexttd	= stat->physaddr;
		data->td.td_be		= data->td.td_cbp + len - 1;
		data->len		= len;
		data->xfer		= xfer;
		data->flags		= OHCI_ADD_LEN;

		next			= data;
		stat->flags		= OHCI_CALL_DONE;
	}
	else
	{
		next			= stat;

		/* XXX ADD_LEN? */

		stat->flags		= OHCI_CALL_DONE|OHCI_ADD_LEN;
	}

	memmove (opipe->u.ctl.reqdma, req, sizeof (*req));

	setup->td.td_flags = OHCI_TD_SETUP|OHCI_TD_NOCC|OHCI_TD_TOGGLE_0|OHCI_TD_NOINTR;

	setup->td.td_cbp	= VIRT_TO_PHYS_ADDR (opipe->u.ctl.reqdma);
	setup->nexttd		= next;
	setup->td.td_nexttd	= next->physaddr;
	setup->td.td_be		= setup->td.td_cbp + sizeof *req - 1;
	setup->len		= 0;
	setup->xfer		= xfer;
	setup->flags		= 0;
	xfer->hcpriv		= setup;

	stat->td.td_flags	= (isread ? OHCI_TD_OUT : OHCI_TD_IN) |
					OHCI_TD_NOCC|OHCI_TD_TOGGLE_1|OHCI_TD_SET_DI (1);

	stat->td.td_cbp		= 0;
	stat->nexttd		= tail;
	stat->td.td_nexttd	= tail->physaddr;
	stat->td.td_be		= 0;
	stat->len		= 0;
	stat->xfer		= xfer;

	/* Insert ED in schedule */
	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sed->ed.ed_tailp = tail->physaddr;
	opipe->tail.td = tail;

	ohci_write_port_or_mem_long (sc, OHCI_CLF, OHCI_COMMAND_STATUS);

#if (0)	/*******************************************************/
	if (xfer->timeout && !sc->sc_bus.use_polling)
	{
		usb_callout (xfer->timeout_handle, MS_TO_TICKS (xfer->timeout),
			    ohci_timeout, xfer);
	}
#endif	/*******************************************************/

	splx (s);

	return (USBD_NORMAL_COMPLETION);

	/*
	 *	Diversos erros
	 */
 bad3:
	ohci_free_std (sc, tail);
 bad2:
	ohci_free_std (sc, stat);
 bad1:
	return (err);

}	/* end ohci_device_request */

/*
 ****************************************************************
 *	Add an ED to the schedule.  Called at splusb ()		*
 ****************************************************************
 */
void
ohci_add_ed (struct ohci_soft_ed *sed, struct ohci_soft_ed *head)
{
	sed->next	   = head->next;
	sed->ed.ed_nexted  = head->ed.ed_nexted;
	head->next	   = sed;
	head->ed.ed_nexted = sed->physaddr;

}	/* end ohci_add_ed */

/*
 ****************************************************************
 *	Remove an ED from the schedule				*
 ****************************************************************
 */
void
ohci_rem_ed (struct ohci_soft_ed *sed, struct ohci_soft_ed *head)
{
	struct ohci_soft_ed	*p;

	for (p = head; p != NULL && p->next != sed; p = p->next)
		/* vazio */;

	if (p == NULL)
		printf ("ohci_rem_ed: ED not found\n");
	else
		{ p->next = sed->next; p->ed.ed_nexted = sed->ed.ed_nexted; }

}	/* end ohci_rem_ed */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 *
 * When a transfer is completed the TD is added to the done queue by
 * the host controller.  This queue is the processed by software.
 * Unfortunately the queue contains the physical address of the TD
 * and we have no simple way to translate this back to a kernel address.
 * To make the translation possible (and fast) we use a hash table of
 * TDs currently in the schedule.  The physical address is used as the
 * hash value.
 */
void
ohci_hash_add_td (struct ohci_softc *sc, struct ohci_soft_td *std)
{
	int		h = HASH (std->physaddr);

	if ((std->next = sc->sc_hash_tds[h]) != NULL)
		sc->sc_hash_tds[h]->prev = &std->next;

	sc->sc_hash_tds[h] = std;
	std->prev	   = &sc->sc_hash_tds[h];

}	/* end ohci_hash_add_td */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_hash_rem_td (struct ohci_softc *sc, struct ohci_soft_td *std)
{
	if (std->next != NULL)
		std->next->prev = std->prev;

	*std->prev = std->next;

}	/* end ohci_hash_rem_td */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct ohci_soft_td *
ohci_hash_find_td (struct ohci_softc *sc, ulong a)
{
	int			h = HASH (a);
	struct ohci_soft_td	*std;

	/*
	 *	if these are present they should be masked out at an earlier stage.
	 */
	for (std = sc->sc_hash_tds[h]; std != NULL; std = std->next)
	{
		if (std->physaddr == a)
			return (std);
	}

#ifdef	USB_MSG
	printf ("%s: ohci_hash_find_td: addr 0x%08lx not found\n", sc->sc_bus.bdev->nameunit, a);
#endif	USB_MSG

	return (NULL);

}	/* end ohci_hash_find_td */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_hash_add_itd (struct ohci_softc *sc, struct ohci_soft_itd *sitd)
{
	int		h = HASH (sitd->physaddr);

#ifdef	USB_MSG
	printf ("ohci_hash_add_itd: sitd=%P physaddr=%P\n", sitd, sitd->physaddr);
#endif	USB_MSG

	if ((sitd->next = sc->sc_hash_itds[h]) != NULL)
		sc->sc_hash_itds[h]->prev = &sitd->next;

	sc->sc_hash_itds[h] = sitd;
	sitd->prev	    = &sc->sc_hash_itds[h];

}	/* end ohci_hash_add_itd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_hash_rem_itd (struct ohci_softc *sc, struct ohci_soft_itd *sitd)
{
#ifdef	USB_MSG
	printf ("ohci_hash_rem_itd: sitd=%P physaddr=0x%08lx\n", sitd, sitd->physaddr);
#endif	USB_MSG

	if (sitd->next != NULL)
		sitd->next->prev = sitd->prev;

	*sitd->prev = sitd->next;

}	/* end ohci_hash_rem_itd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct ohci_soft_itd *
ohci_hash_find_itd (struct ohci_softc *sc, ulong a)
{
	int			h = HASH (a);
	struct ohci_soft_itd	*sitd;

	for (sitd = sc->sc_hash_itds[h]; sitd != NULL; sitd = sitd->next)
	{
		if (sitd->physaddr == a)
			return (sitd);
	}

	return (NULL);

}	/* end ohci_hash_find_itd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_open (struct usbd_pipe *pipe_ptr)
{
	struct usbd_device		*udev = pipe_ptr->device;
	struct ohci_softc		*sc = (struct ohci_softc *)udev->bus;
	struct usb_endpoint_descriptor	*ed = pipe_ptr->endpoint->edesc;
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	uchar				addr = udev->address;
	uchar				xfertype = ed->bmAttributes & UE_XFERTYPE;
	struct ohci_soft_ed		*sed;
	struct ohci_soft_td		*std;
	struct ohci_soft_itd		*sitd;
	ulong				tdphys, fmt;
	int				s, ival;

#ifdef	USB_MSG
	printf ("ohci_open: pipe=%P, addr=%d, endpt=%d (%d)\n", pipe_ptr, addr, ed->bEndpointAddress, sc->sc_addr);
#endif	USB_MSG

	if (sc->sc_dying)
		return (USBD_IOERROR);

	std = NULL; 	sed = NULL;

	if (addr == sc->sc_addr)
	{
		switch (ed->bEndpointAddress)
		{
		    case USB_CONTROL_ENDPOINT:
			pipe_ptr->methods = &ohci_root_ctrl_methods;
			break;

		    case UE_DIR_IN|OHCI_INTR_ENDPT:
			pipe_ptr->methods = &ohci_root_intr_methods;
			break;

		    default:
			return (USBD_INVAL);
		}
	}
	else
	{
		if ((sed = ohci_alloc_sed (sc)) == NULL)
			goto bad0;

		opipe->sed = sed;

		if (xfertype == UE_ISOCHRONOUS)
		{
			if ((sitd = ohci_alloc_sitd (sc)) == NULL)
				goto bad1;

			opipe->tail.itd = sitd;
			tdphys		= sitd->physaddr;
			fmt		= OHCI_ED_FORMAT_ISO;

			if (UE_GET_DIR (ed->bEndpointAddress) == UE_DIR_IN)
				fmt |= OHCI_ED_DIR_IN;
			else
				fmt |= OHCI_ED_DIR_OUT;
		}
		else
		{
			if ((std = ohci_alloc_std (sc)) == NULL)
				goto bad1;

			opipe->tail.td	= std;
			tdphys		= std->physaddr;
			fmt		= OHCI_ED_FORMAT_GEN|OHCI_ED_DIR_TD;
		}

		sed->ed.ed_flags = OHCI_ED_SET_FA (addr) | OHCI_ED_SET_EN (ed->bEndpointAddress) |
					(udev->speed == USB_SPEED_LOW ? OHCI_ED_SPEED : 0) |
					fmt | OHCI_ED_SET_MAXP (UGETW (ed->wMaxPacketSize));

		sed->ed.ed_headp = tdphys | (pipe_ptr->endpoint->savedtoggle ? OHCI_TOGGLECARRY : 0);
                sed->ed.ed_tailp = tdphys;

		switch (xfertype)
		{
		    case UE_CONTROL:
			pipe_ptr->methods = &ohci_device_ctrl_methods;

			if ((opipe->u.ctl.reqdma = malloc_byte (sizeof (struct usb_device_request))) == NULL)
				goto bad;

			s = splx (irq_pl_vec[sc->sc_bus.irq]);

			ohci_add_ed (sed, sc->sc_ctrl_head);

			splx (s);
			break;

		    case UE_INTERRUPT:
			pipe_ptr->methods = &ohci_device_intr_methods;

			if ((ival = pipe_ptr->interval) == USBD_DEFAULT_INTERVAL)
				ival = ed->bInterval;

			return (ohci_device_setintr (sc, opipe, ival));

		    case UE_ISOCHRONOUS:
			pipe_ptr->methods = &ohci_device_isoc_methods;

			return (ohci_setup_isoc (pipe_ptr));

		    case UE_BULK:
			pipe_ptr->methods = &ohci_device_bulk_methods;

			s = splx (irq_pl_vec[sc->sc_bus.irq]);

			ohci_add_ed (sed, sc->sc_bulk_head);

			splx (s);
			break;
		}
	}

	return (USBD_NORMAL_COMPLETION);

	/*
	 *	Diversos erros
	 */
    bad:
	if (std != NULL)
		ohci_free_std (sc, std);
    bad1:
	if (sed != NULL)
		ohci_free_sed (sc, sed);
    bad0:
	return (USBD_NOMEM);

}	/* end ohci_open */

/*
 ****************************************************************
 *	Close a reqular pipe					*
 ****************************************************************
 *
 *	Assumes that there are no pending transactions.
 */
void
ohci_close_pipe (struct usbd_pipe *pipe_ptr, struct ohci_soft_ed *head)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc		*sc = (struct ohci_softc *)pipe_ptr->device->bus;
	struct ohci_soft_ed		*sed = opipe->sed;
	int				s;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	ohci_rem_ed (sed, head);

	/* Make sure the host controller is not touching this ED */

	usb_delay_ms (&sc->sc_bus, 1);

	splx (s);

	pipe_ptr->endpoint->savedtoggle = (sed->ed.ed_headp & OHCI_TOGGLECARRY) ? 1 : 0;

	ohci_free_sed (sc, opipe->sed);

}	/* end ohci_close_pipe */

/*
 ****************************************************************
 *	Abort a device request					*
 ****************************************************************
 *
 *	If this routine is called at splusb () it guarantees that the request
 *	will be removed from the hardware scheduling and that the callback
 *	for it will be called with USBD_CANCELLED status.
 *	It's impossible to guarantee that the requested transfer will not
 *	have happened since the hardware runs concurrently.
 *	If the transaction has already happened we rely on the ordinary
 *	interrupt processing to process it.
 */
void
ohci_abort_xfer (struct usbd_xfer *xfer, int status)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct ohci_softc		*sc = (struct ohci_softc *)opipe->pipe.device->bus;
	struct ohci_soft_ed		*sed = opipe->sed;
	struct ohci_soft_td		*p, *n;
	ulong				headp;
	int				s, hit;

#ifdef	USB_MSG
	printf ("ohci_abort_xfer: xfer=%P pipe=%P sed=%P\n", xfer, opipe,sed);
#endif	USB_MSG

	if (sc->sc_dying)
	{
		/* If we're dying, just do the software part */

		s = splx (irq_pl_vec[sc->sc_bus.irq]);

		xfer->status = status;	/* make software ignore it */
#if (0)	/*******************************************************/
		usb_uncallout (xfer->timeout_handle, ohci_timeout, xfer);
#endif	/*******************************************************/
		usb_transfer_complete (xfer);

		splx (s);
	}

#if (0)	/*******************************************************/
	if (xfer->device->bus->intr_context || !curproc)
		panic ("ohci_abort_xfer: not in process context\n");
#endif	/*******************************************************/

	/*
	 *	Step 1: Make interrupt routine and hardware ignore xfer.
	 */
	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	xfer->status = status;	/* make software ignore it */

#if (0)	/*******************************************************/
	usb_uncallout (xfer->timeout_handle, ohci_timeout, xfer);
#endif	/*******************************************************/

	splx (s);

#ifdef	USB_MSG
	printf ("ohci_abort_xfer: stop ed=%P\n", sed);
#endif	USB_MSG

	sed->ed.ed_flags |= OHCI_ED_SKIP; /* force hardware skip */

	/*
	 *	Step 2: Wait until we know hardware has finished any possible
	 *	use of the xfer.  Also make sure the soft interrupt routine
	 *	has run.
	 */
	usb_delay_ms (opipe->pipe.device->bus, 20); /* Hardware finishes in 1ms */

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	usb_schedsoftintr (&sc->sc_bus);

	splx (s);

	/*
	 *	Step 3: Remove any vestiges of the xfer from the hardware.
	 *	The complication here is that the hardware may have executed
	 *	beyond the xfer we're trying to abort.  So as we're scanning
	 *	the TDs of this xfer we check if the hardware points to
	 *	any of them.
	 */
	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	p = xfer->hcpriv;

	for (headp = sed->ed.ed_headp & OHCI_HEADMASK, hit = 0; p->xfer == xfer; p = n)
	{
		hit |= (headp == p->physaddr);	n = p->nexttd;

		ohci_free_std (sc, p);
	}

	/* Zap headp register if hardware pointed inside the xfer */

	if (hit)
	{
#ifdef	USB_MSG
		printf ("ohci_abort_xfer: set hd=%P, tl=%P\n", p->physaddr, sed->ed.ed_tailp);
#endif	USB_MSG
		sed->ed.ed_headp = p->physaddr; /* unlink TDs */
	}
	else
	{
#ifdef	USB_MSG
		printf ("ohci_abort_xfer: no hit\n");
#endif	USB_MSG
	}

	/*
	 *	Step 4: Turn on hardware again.
	 */
	sed->ed.ed_flags &= ~OHCI_ED_SKIP; /* remove hardware skip */

	/*
	 *	Step 5: Execute callback
	 */
	usb_transfer_complete (xfer);

	splx (s);

}	/* end ohci_abort_xfer */

/*
 ****************************************************************
 *	Simulate a hardware hub					*
 ****************************************************************
 */
int
ohci_root_ctrl_transfer (struct usbd_xfer *xfer)
{
	int		err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (ohci_root_ctrl_start (xfer->pipe->first_xfer));

}	/* end ohci_root_ctrl_transfer */

/*
 ****************************************************************
 *	Executa as operações no HUB raiz			*
 ****************************************************************
 */
int
ohci_root_ctrl_start (struct usbd_xfer *xfer)
{
	struct ohci_softc		*sc = (struct ohci_softc *)xfer->pipe->device->bus;
	struct usb_device_request	*req;
	void				*buf = NULL;
	int				port, i;
	int				s, len, value, index, l, totlen = 0;
	struct usb_port_status		ps;
	struct usb_hub_descriptor	hubd;
	int				err;
	ulong				v;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	req = &xfer->request;

	len	= UGETW (req->wLength);
	value	= UGETW (req->wValue);
	index	= UGETW (req->wIndex);

#ifdef	USB_MSG
	printf
	(	"ohci_root_ctrl_control type=0x%02X request=%02X wValue=%04X, index=%d, len=%d\n",
		req->bmRequestType, req->bRequest, value, index, len
	);
#endif	USB_MSG

	if (len != 0)
		buf = xfer->dmabuf;

#define C(x,y)	((x) | ((y) << 8))

	switch (C (req->bRequest, req->bmRequestType))
	{
	    case C (UR_CLEAR_FEATURE, UT_WRITE_DEVICE):
	    case C (UR_CLEAR_FEATURE, UT_WRITE_INTERFACE):
	    case C (UR_CLEAR_FEATURE, UT_WRITE_ENDPOINT):
		/*
		 * DEVICE_REMOTE_WAKEUP and ENDPOINT_HALT are no-ops
		 * for the integrated root hub.
		 */
		break;

	    case C (UR_GET_CONFIG, UT_READ_DEVICE):
		if (len > 0)
			{ * (uchar *)buf = sc->sc_conf; totlen = 1; }
		break;

	    case C (UR_GET_DESCRIPTOR, UT_READ_DEVICE):
		switch (value >> 8)
		{
		    case UDESC_DEVICE:
			if ((value & 0xff) != 0)
				{ err = USBD_IOERROR; goto ret;	}

			totlen = l = MIN (len, USB_DEVICE_DESCRIPTOR_SIZE);
			USETW (ohci_devd.idVendor, sc->sc_id_vendor);
			memmove (buf, &ohci_devd, l);
			break;

		    case UDESC_CONFIG:
			if ((value & 0xff) != 0)
				{ err = USBD_IOERROR; goto ret;	}

			totlen = l = MIN (len, USB_CONFIG_DESCRIPTOR_SIZE);
			memmove (buf, &ohci_confd, l);

			buf = (char *)buf + l;	len -= l;
			l = MIN (len, USB_INTERFACE_DESCRIPTOR_SIZE); totlen += l;

			memmove (buf, &ohci_ifcd, l);

			buf = (char *)buf + l;	len -= l;
			l = MIN (len, USB_ENDPOINT_DESCRIPTOR_SIZE);  totlen += l;

			memmove (buf, &ohci_endpd, l);
			break;

		    case UDESC_STRING:
			if (len == 0)
				break;

			*(uchar *)buf = 0; totlen = 1;

			switch (value & 0xff)
			{
			    case 1: /* Vendor */
				totlen = usb_str (buf, len, sc->sc_vendor);
				break;

			    case 2: /* Product */
				totlen = usb_str (buf, len, "OHCI root hub");
				break;
			}
			break;

		    default:
			err = USBD_IOERROR;
			goto ret;
		}
		break;

	    case C (UR_GET_INTERFACE, UT_READ_INTERFACE):
		if (len > 0)
			{ *(uchar *)buf = 0; totlen = 1; }
		break;

	    case C (UR_GET_STATUS, UT_READ_DEVICE):
		if (len > 1)
			{ USETW (( (struct usb_status *)buf)->wStatus,UDS_SELF_POWERED); totlen = 2; }
		break;

	    case C (UR_GET_STATUS, UT_READ_INTERFACE):
	    case C (UR_GET_STATUS, UT_READ_ENDPOINT):
		if (len > 1)
			{ USETW (( (struct usb_status *)buf)->wStatus, 0); totlen = 2; }
		break;

	    case C (UR_SET_ADDRESS, UT_WRITE_DEVICE):
		if (value >= USB_MAX_DEVICES)
			{ err = USBD_IOERROR; goto ret;	}

		sc->sc_addr = value;
		break;

	    case C (UR_SET_CONFIG, UT_WRITE_DEVICE):
		if (value != 0 && value != 1)
			{ err = USBD_IOERROR; goto ret;	}

		sc->sc_conf = value;
		break;

	    case C (UR_SET_DESCRIPTOR, UT_WRITE_DEVICE):
		break;

	    case C (UR_SET_FEATURE, UT_WRITE_DEVICE):
	    case C (UR_SET_FEATURE, UT_WRITE_INTERFACE):
	    case C (UR_SET_FEATURE, UT_WRITE_ENDPOINT):
		err = USBD_IOERROR;
		goto ret;

	    case C (UR_SET_INTERFACE, UT_WRITE_INTERFACE):
		break;

	    case C (UR_SYNCH_FRAME, UT_WRITE_ENDPOINT):
		break;

	/* Hub requests */
	    case C (UR_CLEAR_FEATURE, UT_WRITE_CLASS_DEVICE):
		break;

	    case C (UR_CLEAR_FEATURE, UT_WRITE_CLASS_OTHER):
		if (index < 1 || index > sc->sc_noport)
			{ err = USBD_IOERROR; goto ret; }

		port = OHCI_RH_PORT_STATUS (index);

		switch (value)
		{
		    case UHF_PORT_ENABLE:
			ohci_write_port_or_mem_long (sc, UPS_CURRENT_CONNECT_STATUS, port);
			break;

		    case UHF_PORT_SUSPEND:
			ohci_write_port_or_mem_long (sc, UPS_OVERCURRENT_INDICATOR, port);
			break;

		    case UHF_PORT_POWER:
			/* Yes, writing to the LOW_SPEED bit clears power */
			ohci_write_port_or_mem_long (sc, UPS_LOW_SPEED, port);
			break;

		    case UHF_C_PORT_CONNECTION:
			ohci_write_port_or_mem_long (sc, UPS_C_CONNECT_STATUS << 16, port);
			break;

		    case UHF_C_PORT_ENABLE:
			ohci_write_port_or_mem_long (sc, UPS_C_PORT_ENABLED << 16, port);
			break;

		    case UHF_C_PORT_SUSPEND:
			ohci_write_port_or_mem_long (sc, UPS_C_SUSPEND << 16, port);
			break;

		    case UHF_C_PORT_OVER_CURRENT:
			ohci_write_port_or_mem_long (sc, UPS_C_OVERCURRENT_INDICATOR << 16, port);
			break;

		    case UHF_C_PORT_RESET:
			ohci_write_port_or_mem_long (sc, UPS_C_PORT_RESET << 16, port);
			break;

		    default:
			err = USBD_IOERROR;
			goto ret;
		}

		switch (value)
		{
		    case UHF_C_PORT_CONNECTION:
		    case UHF_C_PORT_ENABLE:
		    case UHF_C_PORT_SUSPEND:
		    case UHF_C_PORT_OVER_CURRENT:
		    case UHF_C_PORT_RESET:
			/* Enable RHSC interrupt if condition is cleared */
			if ((ohci_read_port_or_mem_long (sc, port) >> 16) == 0)
				ohci_rhsc_able (sc, 1);
			break;
		    default:
			break;
		}
		break;

	    case C (UR_GET_DESCRIPTOR, UT_READ_CLASS_DEVICE):
		if (value != 0)
			{ err = USBD_IOERROR; goto ret; }

		v	= ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_A);
		hubd	= ohci_hubd;

		hubd.bNbrPorts = sc->sc_noport;

		USETW (hubd.wHubCharacteristics,
		      (v & OHCI_NPS ? UHD_PWR_NO_SWITCH :
		       v & OHCI_PSM ? UHD_PWR_GANGED : UHD_PWR_INDIVIDUAL)
		      /* XXX overcurrent */
		      );

		hubd.bPwrOn2PwrGood = OHCI_GET_POTPGT (v);

		v = ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_B);

		for (i = 0, l = sc->sc_noport; l > 0; i++, l -= 8, v >>= 8)
			hubd.DeviceRemovable[i++] = (uchar)v;

		hubd.bDescLength = USB_HUB_DESCRIPTOR_SIZE + i;

		l = MIN (len, hubd.bDescLength); totlen = l;

		memmove (buf, &hubd, l);

		break;

	    case C (UR_GET_STATUS, UT_READ_CLASS_DEVICE):
		if (len != 4)
			{ err = USBD_IOERROR; goto ret; }
		memclr (buf, len); /* ? XXX */
		totlen = len;
		break;

	    case C (UR_GET_STATUS, UT_READ_CLASS_OTHER):
		if (index < 1 || index > sc->sc_noport)
			{ err = USBD_IOERROR; goto ret; }

		if (len != 4)
			{ err = USBD_IOERROR; goto ret;	}

		v = ohci_read_port_or_mem_long (sc, OHCI_RH_PORT_STATUS (index));

		USETW (ps.wPortStatus, v);
		USETW (ps.wPortChange, v >> 16);

		l = MIN (len, sizeof ps);

		memmove (buf, &ps, l);

		totlen = l;

		break;


	    case C (UR_SET_DESCRIPTOR, UT_WRITE_CLASS_DEVICE):
		err = USBD_IOERROR;
		goto ret;

	    case C (UR_SET_FEATURE, UT_WRITE_CLASS_DEVICE):
		break;

	    case C (UR_SET_FEATURE, UT_WRITE_CLASS_OTHER):
		if (index < 1 || index > sc->sc_noport)
			{ err = USBD_IOERROR; goto ret;	}
		port = OHCI_RH_PORT_STATUS (index);

		switch (value)
		{
		    case UHF_PORT_ENABLE:
			ohci_write_port_or_mem_long (sc, UPS_PORT_ENABLED, port);
			break;

		    case UHF_PORT_SUSPEND:
			ohci_write_port_or_mem_long (sc, UPS_SUSPEND, port);
			break;

		    case UHF_PORT_RESET:
			ohci_write_port_or_mem_long (sc, UPS_RESET, port);

			for (i = 0; i < 5; i++)
			{
				usb_delay_ms (&sc->sc_bus, USB_PORT_ROOT_RESET_DELAY);

				if (sc->sc_dying)
					{ err = USBD_IOERROR; goto ret;	}

				if ((ohci_read_port_or_mem_long (sc, port) & UPS_RESET) == 0)
					break;
			}

			break;

		    case UHF_PORT_POWER:
			ohci_write_port_or_mem_long (sc, UPS_PORT_POWER, port);
			break;

		    default:
			err = USBD_IOERROR;
			goto ret;
		}
		break;

	    default:
		err = USBD_IOERROR;
		goto ret;
	}

	xfer->actlen = totlen; 	err = USBD_NORMAL_COMPLETION;

   ret:
	xfer->status = err;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	usb_transfer_complete (xfer);

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end ohci_root_ctrl_start */

/*
 ****************************************************************
 *	Abort a root control request				*
 ****************************************************************
 */
void
ohci_root_ctrl_abort (struct usbd_xfer *xfer)
{
}	/* end ohci_root_ctrl_abort */

/*
 ****************************************************************
 *	Close the root pipe					*
 ****************************************************************
 */
void
ohci_root_ctrl_close (struct usbd_pipe *pipe_ptr)
{
}	/* end ohci_root_ctrl_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_root_intr_transfer (struct usbd_xfer *xfer)
{
	int		err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (ohci_root_intr_start (xfer->pipe->first_xfer));

}	/* end ohci_root_intr_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_root_intr_start (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	struct ohci_softc	*sc = (struct ohci_softc *)pipe_ptr->device->bus;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	sc->sc_intrxfer = xfer;

	return (USBD_IN_PROGRESS);

}	/* end ohci_root_intr_start */

/*
 ****************************************************************
 *	Abort a root interrupt request				*
 ****************************************************************
 */
void
ohci_root_intr_abort (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	struct ohci_softc	*sc = (struct ohci_softc *)pipe_ptr->device->bus;
	int			s;

	if (xfer->pipe->intrxfer == xfer)
	{
#ifdef	USB_MSG
		printf ("ohci_root_intr_abort: remove\n");
#endif	USB_MSG
		xfer->pipe->intrxfer = NULL;
	}

	xfer->status = USBD_CANCELLED;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	usb_transfer_complete (xfer);

	splx (s);

}	/* end ohci_root_intr_abort */

/*
 ****************************************************************
 *	Close the root pipe					*
 ****************************************************************
 */
void
ohci_root_intr_close (struct usbd_pipe *pipe_ptr)
{
	struct ohci_softc	*sc = (struct ohci_softc *)pipe_ptr->device->bus;

	sc->sc_intrxfer = NULL;

}	/* end ohci_root_intr_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_ctrl_transfer (struct usbd_xfer *xfer)
{
	int		err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (ohci_device_ctrl_start (xfer->pipe->first_xfer));

}	/* end ohci_device_ctrl_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_ctrl_start (struct usbd_xfer *xfer)
{
	struct ohci_softc	*sc = (struct ohci_softc *)xfer->pipe->device->bus;
	int			err;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	if (err = ohci_device_request (xfer))
		return (err);

#ifdef	USB_POLLING
	if (sc->sc_bus.use_polling)
		ohci_waitintr (sc, xfer);
#endif	USB_POLLING

	return (USBD_IN_PROGRESS);

}	/* end ohci_device_ctrl_start */

/*
 ****************************************************************
 *	Abort a device control request				*
 ****************************************************************
 */
void
ohci_device_ctrl_abort (struct usbd_xfer *xfer)
{
	ohci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end ohci_device_ctrl_abort */

/*
 ****************************************************************
 *	Close a device control pipe				*
 ****************************************************************
 */
void
ohci_device_ctrl_close (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe	*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc	*sc = (struct ohci_softc *)pipe_ptr->device->bus;

	ohci_close_pipe (pipe_ptr, sc->sc_ctrl_head);
	ohci_free_std (sc, opipe->tail.td);

}	/* end ohci_device_ctrl_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_clear_toggle (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe *opipe = (struct ohci_pipe *)pipe_ptr;

	opipe->sed->ed.ed_headp &= ~OHCI_TOGGLECARRY;

}	/* end ohci_device_clear_toggle */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_noop (struct usbd_pipe *pipe_ptr)
{
}	/* end ohci_noop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_bulk_transfer (struct usbd_xfer *xfer)
{
	int		err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (ohci_device_bulk_start (xfer->pipe->first_xfer));

}	/* end ohci_device_bulk_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_bulk_start (struct usbd_xfer *xfer)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct usbd_device		*udev = opipe->pipe.device;
	struct ohci_softc		*sc = (struct ohci_softc *)udev->bus;
	int				addr = udev->address;
	struct ohci_soft_td		*data, *tail, *tdp;
	struct ohci_soft_ed		*sed;
	int				s, len, isread, endpt, err;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	len	= xfer->length;
	endpt	= xfer->pipe->endpoint->edesc->bEndpointAddress;
	isread	= UE_GET_DIR (endpt) == UE_DIR_IN;
	sed	= opipe->sed;

#ifdef	USB_MSG
	printf
	(	"ohci_device_bulk_start: xfer=%P len=%d isread=%d flags=%d endpt=%d\n",
		xfer, len, isread, xfer->flags, endpt
	);
#endif	USB_MSG

	opipe->u.bulk.isread = isread;
	opipe->u.bulk.length = len;

	/* Update device address */
	sed->ed.ed_flags = (sed->ed.ed_flags & ~OHCI_ED_ADDRMASK) | OHCI_ED_SET_FA (addr);

	/* Allocate a chain of new TDs (including a new tail) */
	data = opipe->tail.td;

	err = ohci_alloc_std_chain (opipe, sc, len, isread, xfer, data, &tail);

	/* We want interrupt at the end of the transfer */

	tail->td.td_flags &= ~OHCI_TD_INTR_MASK;
	tail->td.td_flags |= OHCI_TD_SET_DI (1);
	tail->flags       |= OHCI_CALL_DONE;

	tail = tail->nexttd;	/* point at sentinel */

	if (err)
		return (err);

	tail->xfer	= NULL;
	xfer->hcpriv	= data;

#ifdef	USB_MSG
	printf
	(	"ohci_device_bulk_start: ed_flags=0x%08x td_flags=0x%08x td_cbp=0x%08x td_be=0x%08x\n",
		sed->ed.ed_flags, data->td.td_flags, data->td.td_cbp, data->td.td_be
	);
#endif	USB_MSG

#ifdef USB_DEBUG
	if (ohcidebug > 5)
		{ ohci_dump_ed (sed); ohci_dump_tds (data); }
#endif

	/* Insert ED in schedule */
	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	for (tdp = data; tdp != tail; tdp = tdp->nexttd)
		tdp->xfer = xfer;

	sed->ed.ed_tailp = tail->physaddr;
	opipe->tail.td = tail;
	sed->ed.ed_flags &= ~OHCI_ED_SKIP;
	ohci_write_port_or_mem_long (sc, OHCI_BLF, OHCI_COMMAND_STATUS);

#if (0)	/*******************************************************/
	if (xfer->timeout && !sc->sc_bus.use_polling)
                usb_callout (xfer->timeout_handle, MS_TO_TICKS (xfer->timeout), ohci_timeout, xfer);
#endif	/*******************************************************/

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end ohci_device_bulk_start */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_bulk_abort (struct usbd_xfer *xfer)
{
	ohci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end ohci_device_bulk_abort */

/*
 ****************************************************************
 *	Close a device bulk pipe				*
 ****************************************************************
 */
void
ohci_device_bulk_close (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc		*sc = (struct ohci_softc *)pipe_ptr->device->bus;

#ifdef	USB_MSG
	printf ("ohci_device_bulk_close: pipe=%P\n", pipe_ptr);
#endif	USB_MSG

	ohci_close_pipe (pipe_ptr, sc->sc_bulk_head);
	ohci_free_std (sc, opipe->tail.td);

}	/* end ohci_device_bulk_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_intr_transfer (struct usbd_xfer *xfer)
{
	int		err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (ohci_device_intr_start (xfer->pipe->first_xfer));

}	/* end ohci_device_intr_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_intr_start (struct usbd_xfer *xfer)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct usbd_device		*udev = opipe->pipe.device;
	struct ohci_softc		*sc = (struct ohci_softc *)udev->bus;
	struct ohci_soft_ed		*sed = opipe->sed;
	struct ohci_soft_td		*data, *tail;
	int				len, s;

	if (sc->sc_dying)
		return (USBD_IOERROR);

#ifdef	USB_MSG
	printf
	(	"ohci_device_intr_start: xfer=%P len=%d "flags=%d priv=%P\n",
		xfer, xfer->length, xfer->flags, xfer->priv
	);
#endif	USB_MSG

	len = xfer->length;

	data = opipe->tail.td;

	if ((tail = ohci_alloc_std (sc)) == NULL)
		return (USBD_NOMEM);

	tail->xfer = NULL;

	data->td.td_flags = OHCI_TD_IN|OHCI_TD_NOCC|OHCI_TD_SET_DI (1)|OHCI_TD_TOGGLE_CARRY;

	if (xfer->flags & USBD_SHORT_XFER_OK)
		data->td.td_flags |= OHCI_TD_R;

	data->td.td_cbp		= VIRT_TO_PHYS_ADDR (xfer->dmabuf);
	data->nexttd		= tail;
	data->td.td_nexttd	= tail->physaddr;
	data->td.td_be		= data->td.td_cbp + len - 1;
	data->len		= len;
	data->xfer		= xfer;
	data->flags		= OHCI_CALL_DONE|OHCI_ADD_LEN;
	xfer->hcpriv		= data;

#ifdef USB_DEBUG
	if (ohcidebug > 5)
		{ printf ("ohci_device_intr_transfer:\n"); ohci_dump_ed (sed); ohci_dump_tds (data); }
#endif

	/* Insert ED in schedule */

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sed->ed.ed_tailp	= tail->physaddr;
	opipe->tail.td		= tail;
	sed->ed.ed_flags       &= ~OHCI_ED_SKIP;

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end ohci_device_intr_start */

/*
 ****************************************************************
 *	Abort a device control request				*
 ****************************************************************
 */
void
ohci_device_intr_abort (struct usbd_xfer *xfer)
{
	if (xfer->pipe->intrxfer == xfer)
		xfer->pipe->intrxfer = NULL;

	ohci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end ohci_device_intr_abort */

/*
 ****************************************************************
 *	Close a device interrupt pipe				*
 ****************************************************************
 */
void
ohci_device_intr_close (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc		*sc = (struct ohci_softc *)pipe_ptr->device->bus;
	int				nslots = opipe->u.intr.nslots;
	int				pos = opipe->u.intr.pos;
	int				j, s;
	struct ohci_soft_ed		*p, *sed = opipe->sed;

#ifdef	USB_MSG
	printf ("ohci_device_intr_close: pipe=%P nslots=%d pos=%d\n", pipe_ptr, nslots, pos);
#endif	USB_MSG

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sed->ed.ed_flags |= OHCI_ED_SKIP;

	if ((sed->ed.ed_tailp & OHCI_HEADMASK) != (sed->ed.ed_headp & OHCI_HEADMASK))
		usb_delay_ms (&sc->sc_bus, 2);

	for (p = sc->sc_eds[pos]; p != NULL && p->next != sed; p = p->next)
		/* vazio */;

	if (p == NULL)
		panic ("ohci_device_intr_close: p == NULL\n");

	p->next		= sed->next;
	p->ed.ed_nexted = sed->ed.ed_nexted;

	splx (s);

	for (j = 0; j < nslots; j++)
		--sc->sc_bws [(pos * nslots + j) % OHCI_NO_INTRS];

	ohci_free_std (sc, opipe->tail.td);
	ohci_free_sed (sc, opipe->sed);

}	/* end ohci_device_intr_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_setintr (struct ohci_softc *sc, struct ohci_pipe *opipe, int ival)
{
	int			i, j, s, best;
	ulong			npoll, slow, shigh, nslots;
	ulong			bestbw, bw;
	struct ohci_soft_ed	*hsed, *sed = opipe->sed;

#ifdef	USB_MSG
	printf ("ohci_setintr: pipe=%P\n", opipe);
#endif	USB_MSG

	if (ival == 0)
		{ printf ("ohci_setintr: intervalo == 0 ?\n"); return (USBD_INVAL); }

	for (npoll = OHCI_NO_INTRS; npoll > ival; npoll /= 2)
		/* vazio */;

#ifdef	USB_MSG
	printf ("ohci_setintr: ival=%d npoll=%d\n", ival, npoll);
#endif	USB_MSG

	/*
	 * We now know which level in the tree the ED must go into.
	 * Figure out which slot has most bandwidth left over.
	 * Slots to examine:
	 * npoll
	 * 1	0
	 * 2	1 2
	 * 4	3 4 5 6
	 * 8	7 8 9 10 11 12 13 14
	 * N    (N-1) .. (N-1+N-1)
	 */
	slow	= npoll - 1;
	shigh	= slow + npoll;
	nslots	= OHCI_NO_INTRS / npoll;

	for (best = i = slow, bestbw = ~0; i < shigh; i++)
	{
		bw = 0;

		for (j = 0; j < nslots; j++)
			bw += sc->sc_bws[(i * nslots + j) % OHCI_NO_INTRS];

		if (bw < bestbw)
			{ best = i; bestbw = bw; }
	}

#ifdef	USB_MSG
	printf ("ohci_setintr: best=%d (%d..%d) bestbw=%d\n", best, slow, shigh, bestbw);
#endif	USB_MSG

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	hsed			= sc->sc_eds[best];
	sed->next		= hsed->next;
	sed->ed.ed_nexted	= hsed->ed.ed_nexted;
	hsed->next		= sed;
	hsed->ed.ed_nexted	= sed->physaddr;

	splx (s);

	for (j = 0; j < nslots; j++)
		++sc->sc_bws[(best * nslots + j) % OHCI_NO_INTRS];

	opipe->u.intr.nslots	= nslots;
	opipe->u.intr.pos	= best;

#ifdef	USB_MSG
	printf ("ohci_setintr: returns %P\n", opipe);
#endif	USB_MSG

	return (USBD_NORMAL_COMPLETION);

}	/* end ohci_device_setintr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_isoc_transfer (struct usbd_xfer *xfer)
{
	int		err;

	/* Put it on our queue, */

	err = usb_insert_transfer (xfer);

	/* bail out on error, */

	if (err && err != USBD_IN_PROGRESS)
		return (err);

	/* XXX should check inuse here */

	/* insert into schedule, */

	ohci_device_isoc_enter (xfer);

	/* and start if the pipe wasn't running */

	if (!err)
		ohci_device_isoc_start (xfer->pipe->first_xfer);

	return (err);

}	/* end ohci_device_isoc_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_isoc_enter (struct usbd_xfer *xfer)
{
	struct ohci_pipe	*opipe = (struct ohci_pipe *)xfer->pipe;
	struct usbd_device	*dev = opipe->pipe.device;
	struct ohci_softc	*sc = (struct ohci_softc *)dev->bus;
	struct ohci_soft_ed	*sed = opipe->sed;
	struct isoc		*iso = &opipe->u.iso;
	struct ohci_xfer	*oxfer = (struct ohci_xfer *)xfer;
	struct ohci_soft_itd	*sitd, *nsitd;
	ulong			buf, offs, noffs, bp0, tdphys;
	int			i, ncur, nframes, s;

	if (sc->sc_dying)
		return;

	if (iso->next == -1)
	{
		/* Not in use yet, schedule it a few frames ahead. */
		iso->next = sc->sc_hcca->hcca_frame_number + 5;
	}

	if (xfer->hcpriv)
	{
		for (sitd = xfer->hcpriv; sitd != NULL && sitd->xfer == xfer; sitd = sitd->nextitd)
			ohci_free_sitd (sc, sitd); /* Free ITDs in prev xfer*/

		if (sitd == NULL)
		{
			if ((sitd = ohci_alloc_sitd (sc)) == NULL)
				panic("cant alloc isoc");

			opipe->tail.itd	  = sitd;
			tdphys		  = sitd->physaddr;
			sed->ed.ed_flags |= OHCI_ED_SKIP; /* Stop*/
			sed->ed.ed_headp  =
			sed->ed.ed_tailp  = tdphys;
			sed->ed.ed_flags &= ~OHCI_ED_SKIP; /* Start.*/
		}
	}

	sitd	= opipe->tail.itd;
	buf	= VIRT_TO_PHYS_ADDR (xfer->dmabuf);
	bp0	= OHCI_PAGE (buf);
	offs	= OHCI_PAGE_OFFSET (buf);
	nframes = xfer->nframes;

	xfer->hcpriv = sitd;

	for (i = ncur = 0; i < nframes; i++, ncur++)
	{
		noffs = offs + xfer->frlengths[i];

		if
		(	ncur == OHCI_ITD_NOFFSET ||	/* all offsets used */
			OHCI_PAGE (buf + noffs) > bp0 + OHCI_PAGE_SIZE
		)
		{	/* too many page crossings */

			/* Allocate next ITD */
			if ((nsitd = ohci_alloc_sitd(sc)) == NULL)
			{
				/* XXX what now? */
				printf ("%s: isoc TD alloc failed\n", sc->sc_bus.bdev->nameunit);
				return;
			}

			/* Fill current ITD */
			sitd->itd.itd_flags =   OHCI_ITD_NOCC |
						OHCI_ITD_SET_SF(iso->next) |
						OHCI_ITD_SET_DI (6) | /* delay intr a little */
						OHCI_ITD_SET_FC (ncur);

			sitd->itd.itd_bp0	= bp0;
			sitd->nextitd		= nsitd;
			sitd->itd.itd_nextitd	= nsitd->physaddr;
			sitd->itd.itd_be	= bp0 + offs - 1;
			sitd->xfer		= xfer;
			sitd->flags		= OHCI_ITD_ACTIVE;

			sitd = nsitd;
			iso->next = iso->next + ncur;
			bp0 = OHCI_PAGE (buf + offs);
			ncur = 0;
		}

		sitd->itd.itd_offset[ncur] = OHCI_ITD_MK_OFFS (offs);
		offs = noffs;
	}

	if ((nsitd = ohci_alloc_sitd (sc)) == NULL)
	{
		/* XXX what now? */
		printf ("%s: isoc TD alloc failed\n", sc->sc_bus.bdev->nameunit);
		return;
	}

	/* Fixup last used ITD */
	sitd->itd.itd_flags = OHCI_ITD_NOCC |
				OHCI_ITD_SET_SF(iso->next) |
				OHCI_ITD_SET_DI(0) |
				OHCI_ITD_SET_FC(ncur);

	sitd->itd.itd_bp0	= bp0;
	sitd->nextitd		= nsitd;
	sitd->itd.itd_nextitd	= nsitd->physaddr;
	sitd->itd.itd_be	= bp0 + offs - 1;
	sitd->xfer		= xfer;
	sitd->flags		= OHCI_CALL_DONE | OHCI_ITD_ACTIVE;

	iso->next   = iso->next + ncur;
	iso->inuse += nframes;

	xfer->actlen = offs;	/* XXX pretend we did it all */

	xfer->status = USBD_IN_PROGRESS;

	oxfer->ohci_xfer_flags |= OHCI_ISOC_DIRTY;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	opipe->tail.itd = nsitd;
	sed->ed.ed_flags &= ~OHCI_ED_SKIP;
	sed->ed.ed_tailp = nsitd->physaddr;

	splx(s);

}	/* end ohci_device_isoc_enter */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_device_isoc_start (struct usbd_xfer *xfer)
{
	struct ohci_pipe	*opipe = (struct ohci_pipe *)xfer->pipe;
	struct ohci_softc	*sc = (struct ohci_softc *)opipe->pipe.device->bus;
	struct ohci_soft_ed	*sed;
	int			s;

	if (sc->sc_dying)
		return (USBD_IOERROR);

#ifdef DIAGNOSTIC
	if (xfer->status != USBD_IN_PROGRESS)
		printf("ohci_device_isoc_start: not in progress %p\n", xfer);
#endif

	/* XXX anything to do? */

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	sed = opipe->sed;  /*  Turn off ED skip-bit to start processing */
	sed->ed.ed_flags &= ~OHCI_ED_SKIP;    /* ED's ITD list.*/

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end ohci_device_isoc_start */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_isoc_abort (struct usbd_xfer *xfer)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)xfer->pipe;
	struct ohci_softc		*sc = (struct ohci_softc *)opipe->pipe.device->bus;
	struct ohci_soft_ed		*sed;
	struct ohci_soft_itd		*sitd, *tmp_sitd;
	int				s, undone, num_sitds;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	opipe->aborting = 1;

	/* Transfer is already done. */
	if (xfer->status != USBD_NOT_STARTED && xfer->status != USBD_IN_PROGRESS)
	{
		splx (s);
		printf ("ohci_device_isoc_abort: early return\n");
		return;
	}

	/* Give xfer the requested abort code. */
	xfer->status = USBD_CANCELLED;

	sed = opipe->sed;
	sed->ed.ed_flags |= OHCI_ED_SKIP; /* force hardware skip */

	num_sitds = 0;
	sitd = xfer->hcpriv;

	for (/* acima */; sitd != NULL && sitd->xfer == xfer; sitd = sitd->nextitd)
		num_sitds++;

	splx (s);

	/*
	 * Each sitd has up to OHCI_ITD_NOFFSET transfers, each can
	 * take a usb 1ms cycle. Conservatively wait for it to drain.
	 * Even with DMA done, it can take awhile for the "batch"
	 * delivery of completion interrupts to occur thru the controller.
	 */
 
	do
	{
		usb_delay_ms (&sc->sc_bus, 2 * (num_sitds*OHCI_ITD_NOFFSET));

		undone   = 0;
		tmp_sitd = xfer->hcpriv;

		for (; tmp_sitd != NULL && tmp_sitd->xfer == xfer; tmp_sitd = tmp_sitd->nextitd)
		{
			if
			(	OHCI_CC_NO_ERROR == OHCI_ITD_GET_CC (tmp_sitd->itd.itd_flags) &&
				tmp_sitd->flags & OHCI_ITD_ACTIVE &&
				(tmp_sitd->flags & OHCI_ITD_INTFIN) == 0
			)
				undone++;
		}

	}
	while (undone != 0);


	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	/* Run callback. */
	usb_transfer_complete (xfer);

	if (sitd != NULL)
	{
		/*
		 * Only if there is a `next' sitd in next xfer... unlink this xfer's sitds.
		 */
		sed->ed.ed_headp = sitd->physaddr;
	}
	else
	{
		sed->ed.ed_headp = 0;
	}

	sed->ed.ed_flags &= ~OHCI_ED_SKIP; /* remove hardware skip */

	splx (s);

}	/* end ohci_device_isoc_abort */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_isoc_done (struct usbd_xfer *xfer)
{
	/* This null routine corresponds to non-isoc "done()" routines
	 * that free the stds associated with an xfer after a completed
	 * xfer interrupt. However, in the case of isoc transfers, the
	 * sitds associated with the transfer have already been processed
	 * and reallocated for the next iteration by
	 * "ohci_device_isoc_transfer()".
	 *
	 * Routine "usb_transfer_complete()" is called at the end of every
	 * relevant usb interrupt. "usb_transfer_complete()" indirectly
	 * calls 1) "ohci_device_isoc_transfer()" (which keeps pumping the
	 * pipeline by setting up the next transfer iteration) and 2) then 
	 * calls "ohci_device_isoc_done()". Isoc transfers have not been 
	 * working for the ohci usb because this routine was trashing the
	 * xfer set up for the next iteration (thus, only the first 
	 * UGEN_NISOREQS xfers outstanding on an open would work). Perhaps
	 * this could all be re-factored, but that's another pass...
	 */
}

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ohci_setup_isoc (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc		*sc = (struct ohci_softc *)pipe_ptr->device->bus;
	struct isoc			*iso = &opipe->u.iso;
	int				s;

	iso->next = -1;
	iso->inuse = 0;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	ohci_add_ed (opipe->sed, sc->sc_isoc_head);

	splx (s);

	return (USBD_NORMAL_COMPLETION);

}	/* end ohci_setup_isoc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ohci_device_isoc_close (struct usbd_pipe *pipe_ptr)
{
	struct ohci_pipe		*opipe = (struct ohci_pipe *)pipe_ptr;
	struct ohci_softc		*sc = (struct ohci_softc *)pipe_ptr->device->bus;
	struct ohci_soft_ed		*sed;

	sed = opipe->sed;
	sed->ed.ed_flags |= OHCI_ED_SKIP; /* Stop device. */

	ohci_close_pipe (pipe_ptr, sc->sc_isoc_head); /* Stop isoc list, free ED.*/

	/* up to NISOREQs xfers still outstanding. */

	ohci_free_sitd (sc, opipe->tail.itd);	/* Next `avail free' sitd.*/

}	/* end ohci_device_isoc_close */

#ifdef	USB_MSG
/*
 ****************************************************************
 *	Le 1 byte de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
ohci_read_port_or_mem_char (const struct ohci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port (up->sc_bus.base_addr + reg));
	else
		return (* (char *)(up->sc_bus.base_addr + reg));

}	/* end ohci_write_port_or_mem_char */

/*
 ****************************************************************
 *	Le 2 bytes de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
ohci_read_port_or_mem_short (const struct ohci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port_short (up->sc_bus.base_addr + reg));
	else
		return (* (short *)(up->sc_bus.base_addr + reg));

}	/* end ohci_read_port_or_mem_short */

/*
 ****************************************************************
 *	Escreve 2 bytes em um registro (da porta ou memória)	*
 ****************************************************************
 */
void
ohci_write_port_or_mem_short (const struct ohci_softc *up, int value, int reg)
{
	if (up->sc_bus.port_mapped)
		write_port_short (value, up->sc_bus.base_addr + reg);
	else
		* (short *)(up->sc_bus.base_addr + reg) = value;

}	/* end ohci_write_port_or_mem_short */
#endif	USB_MSG

/*
 ****************************************************************
 *	Le 4 bytes de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
ohci_read_port_or_mem_long (const struct ohci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port_long (up->sc_bus.base_addr + reg));
	else
		return (*(long *)(up->sc_bus.base_addr + reg));

}	/* end ohci_read_port_or_mem_long */

/*
 ****************************************************************
 *	Escreve 4 bytes em um registro (da porta ou memória)	*
 ****************************************************************
 */
void
ohci_write_port_or_mem_long (const struct ohci_softc *up, int value, int reg)
{
	if (up->sc_bus.port_mapped)
		write_port_long (value, up->sc_bus.base_addr + reg);
	else
		*(long *)(up->sc_bus.base_addr + reg) = value;

}	/* end ohci_write_port_or_mem_long */

#if (0)	/*******************************************************/
void
ohci_timeout (void *addr)
{
	struct ohci_xfer *oxfer = addr;
	struct ohci_pipe *opipe = (struct ohci_pipe *)oxfer->xfer.pipe;
	struct ohci_softc *sc = (struct ohci_softc *)opipe->pipe.device->bus;

	printf ("ohci_timeout: oxfer=%P\n", oxfer);

	if (sc->sc_dying) {
		ohci_abort_xfer (&oxfer->xfer, USBD_TIMEOUT);
		return;
	}

	/* Execute the abort in a process context */
	usb_init_task (&oxfer->abort_task, ohci_timeout_task, addr);
	usb_add_task (oxfer->xfer.pipe->device, &oxfer->abort_task);
}

void
ohci_timeout_task (void *addr)
{
	struct usbd_xfer *xfer = addr;
	int s;

	printf ("ohci_timeout_task: xfer=%P\n", xfer);

	s = splx (irq_pl_vec[sc->sc_bus.irq]);
	ohci_abort_xfer (xfer, USBD_TIMEOUT);
	splx (s);
}

#ifdef USB_DEBUG
void
ohci_dump_tds (struct ohci_soft_td *std)
{
	for (; std; std = std->nexttd)
		ohci_dump_td (std);
}

void
ohci_dump_td (struct ohci_soft_td *std)
{
	char sbuf[128];

	bitmask_snprintf ((ulong)std->td.td_flags,
			 "\20\23R\24OUT\25IN\31TOG1\32SETTOGGLE",
			 sbuf, sizeof (sbuf));

	printf ("TD (%P) at %08lx: %s delay=%d ec=%d cc=%d\ncbp=0x%08lx "
	       "nexttd=0x%08lx be=0x%08lx\n",
	       std, (ulong)std->physaddr, sbuf,
	       OHCI_TD_GET_DI (std->td.td_flags),
	       OHCI_TD_GET_EC (std->td.td_flags),
	       OHCI_TD_GET_CC (std->td.td_flags),
	       (ulong)std->td.td_cbp,
	       (ulong)std->td.td_nexttd,
	       (ulong)std->td.td_be);
}

void
ohci_dump_itd (struct ohci_soft_itd *sitd)
{
	int i;

	printf ("ITD (%P) at %08lx: sf=%d di=%d fc=%d cc=%d\n"
	       "bp0=0x%08lx next=0x%08lx be=0x%08lx\n",
	       sitd, (ulong)sitd->physaddr,
	       OHCI_ITD_GET_SF (sitd->itd.itd_flags),
	       OHCI_ITD_GET_DI (sitd->itd.itd_flags),
	       OHCI_ITD_GET_FC (sitd->itd.itd_flags),
	       OHCI_ITD_GET_CC (sitd->itd.itd_flags),
	       (ulong)sitd->itd.itd_bp0,
	       (ulong)sitd->itd.itd_nextitd,
	       (ulong)sitd->itd.itd_be);
	for (i = 0; i < OHCI_ITD_NOFFSET; i++)
		printf ("offs[%d]=0x%04x ", i,
		       (ulong)le16toh (sitd->itd.itd_offset[i]));
	printf ("\n");
}

void
ohci_dump_itds (struct ohci_soft_itd *sitd)
{
	for (; sitd; sitd = sitd->nextitd)
		ohci_dump_itd (sitd);
}

void
ohci_dump_ed (struct ohci_soft_ed *sed)
{
	char sbuf[128], sbuf2[128];

	bitmask_snprintf ((ulong)sed->ed.ed_flags,
			 "\20\14OUT\15IN\16LOWSPEED\17SKIP\20ISO",
			 sbuf, sizeof (sbuf));

	bitmask_snprintf ((ulong)sed->ed.ed_headp,
			 "\20\1HALT\2CARRY", sbuf2, sizeof (sbuf2));

	printf
	(	"ED (%P) at 0x%08lx: addr=%d endpt=%d maxp=%d flags=%s\ntailp=0x%08lx "
		"headflags=%s headp=0x%08lx nexted=0x%08lx\n",
		sed, (ulong)sed->physaddr,
		OHCI_ED_GET_FA (sed->ed.ed_flags), OHCI_ED_GET_EN (sed->ed.ed_flags),
		OHCI_ED_GET_MAXP (sed->ed.ed_flags), sbuf,
		sed->ed.ed_tailp, sbuf2,
		sed->ed.ed_headp, sed->ed.ed_nexted
	);
}
#endif

/*
 * Shut down the controller when the system is going down.
 */
#ifdef USB_DEBUG
void
ohci_dumpregs (struct ohci_softc *sc)
{
	printf ("ohci_dumpregs: rev=0x%08x control=0x%08x command=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_REVISION),
		 ohci_read_port_or_mem_long (sc, OHCI_CONTROL),
		 ohci_read_port_or_mem_long (sc, OHCI_COMMAND_STATUS));
	printf ("               intrstat=0x%08x intre=0x%08x intrd=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_STATUS),
		 ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_ENABLE),
		 ohci_read_port_or_mem_long (sc, OHCI_INTERRUPT_DISABLE));
	printf ("               hcca=0x%08x percur=0x%08x ctrlhd=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_HCCA),
		 ohci_read_port_or_mem_long (sc, OHCI_PERIOD_CURRENT_ED),
		 ohci_read_port_or_mem_long (sc, OHCI_CONTROL_HEAD_ED));
	printf ("               ctrlcur=0x%08x bulkhd=0x%08x bulkcur=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_CONTROL_CURRENT_ED),
		 ohci_read_port_or_mem_long (sc, OHCI_BULK_HEAD_ED),
		 ohci_read_port_or_mem_long (sc, OHCI_BULK_CURRENT_ED));
	printf ("               done=0x%08x fmival=0x%08x fmrem=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_DONE_HEAD),
		 ohci_read_port_or_mem_long (sc, OHCI_FM_INTERVAL),
		 ohci_read_port_or_mem_long (sc, OHCI_FM_REMAINING));
	printf ("               fmnum=0x%08x perst=0x%08x lsthrs=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_FM_NUMBER),
		 ohci_read_port_or_mem_long (sc, OHCI_PERIODIC_START),
		 ohci_read_port_or_mem_long (sc, OHCI_LS_THRESHOLD));
	printf ("               desca=0x%08x descb=0x%08x stat=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_A),
		 ohci_read_port_or_mem_long (sc, OHCI_RH_DESCRIPTOR_B),
		 ohci_read_port_or_mem_long (sc, OHCI_RH_STATUS));
	printf (("               port1=0x%08x port2=0x%08x\n",
		 ohci_read_port_or_mem_long (sc, OHCI_RH_PORT_STATUS (1)),
		 ohci_read_port_or_mem_long (sc, OHCI_RH_PORT_STATUS (2)));
	printf ("         HCCA: frame_number=0x%04x done_head=0x%08x\n",
		 sc->sc_hcca->hcca_frame_number,
		 sc->sc_hcca->hcca_done_head);
}
#endif
#endif	/*******************************************************/
