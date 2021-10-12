/*
 ****************************************************************
 *								*
 *			uhci.c					*
 *								*
 *	"Driver" para controladoras USB UHCI			*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.6.0, de 06.10.04				*
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
 ******	Tabelas de métodos **************************************
 */
int			uhci_open (struct usbd_pipe *pipe_ptr);
void			uhci_softintr (void *v);
void			uhci_poll (struct usbd_bus *bus);
#if (0)	/*******************************************************/
int			uhci_allocm (struct usbd_bus *bus, void **dma, ulong size);
void			uhci_freem (struct usbd_bus *bus, void **dma);
#endif	/*******************************************************/
struct usbd_xfer	*uhci_allocx (struct usbd_bus *bus);
void			uhci_freex (struct usbd_bus *bus, struct usbd_xfer *xfer);

const struct usbd_bus_methods uhci_bus_methods =
{
	uhci_open,
	uhci_softintr,
	uhci_poll,
	NULL,		/* uhci_allocm (NÃO PRECISA) */
	NULL,		/* uhci_freem  (NÃO PRECISA) */
	uhci_allocx,
	uhci_freex
};

int		uhci_root_ctrl_transfer (struct usbd_xfer *xfer);
int		uhci_root_ctrl_start (struct usbd_xfer *xfer);
void		uhci_root_ctrl_abort (struct usbd_xfer *xfer);
void		uhci_root_ctrl_close (struct usbd_pipe *pipe_ptr);
void		uhci_noop (struct usbd_pipe *pipe_ptr);
void		uhci_root_ctrl_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods uhci_root_ctrl_methods =
{
	uhci_root_ctrl_transfer,
	uhci_root_ctrl_start,
	uhci_root_ctrl_abort,
	uhci_root_ctrl_close,
	uhci_noop,
	uhci_root_ctrl_done
};

int		uhci_root_intr_transfer (struct usbd_xfer *xfer);
int		uhci_root_intr_start (struct usbd_xfer *xfer);
void		uhci_root_intr_abort (struct usbd_xfer *xfer);
void		uhci_root_intr_close (struct usbd_pipe *pipe_ptr);
void		uhci_root_intr_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods uhci_root_intr_methods =
{
	uhci_root_intr_transfer,
	uhci_root_intr_start,
	uhci_root_intr_abort,
	uhci_root_intr_close,
	uhci_noop,
	uhci_root_intr_done
};

int		uhci_device_ctrl_transfer (struct usbd_xfer *xfer);
int		uhci_device_ctrl_start (struct usbd_xfer *xfer);
void		uhci_device_ctrl_abort (struct usbd_xfer *xfer);
void		uhci_device_ctrl_close (struct usbd_pipe *pipe_ptr);
void		uhci_device_ctrl_done (struct usbd_xfer *xfer);

const struct usbd_pipe_methods uhci_device_ctrl_methods =
{
	uhci_device_ctrl_transfer,
	uhci_device_ctrl_start,
	uhci_device_ctrl_abort,
	uhci_device_ctrl_close,
	uhci_noop,
	uhci_device_ctrl_done,
};

int		uhci_device_intr_transfer (struct usbd_xfer *);
int		uhci_device_intr_start (struct usbd_xfer *);
void		uhci_device_intr_abort (struct usbd_xfer *);
void		uhci_device_intr_close (struct usbd_pipe *);
void		uhci_device_intr_done (struct usbd_xfer *);
void		uhci_device_clear_toggle (struct usbd_pipe *pipe_ptr);

const struct usbd_pipe_methods uhci_device_intr_methods =
{
	uhci_device_intr_transfer,
	uhci_device_intr_start,
	uhci_device_intr_abort,
	uhci_device_intr_close,
	uhci_device_clear_toggle,
	uhci_device_intr_done,
};

int		uhci_device_bulk_transfer (struct usbd_xfer *);
int		uhci_device_bulk_start (struct usbd_xfer *);
void		uhci_device_bulk_abort (struct usbd_xfer *);
void		uhci_device_bulk_close (struct usbd_pipe *);
void		uhci_device_bulk_done (struct usbd_xfer *);

const struct usbd_pipe_methods uhci_device_bulk_methods =
{
	uhci_device_bulk_transfer,
	uhci_device_bulk_start,
	uhci_device_bulk_abort,
	uhci_device_bulk_close,
	uhci_device_clear_toggle,
	uhci_device_bulk_done,
};

const struct usbd_pipe_methods uhci_device_isoc_methods =
{
#if (0)	/*******************************************************/
	uhci_device_isoc_transfer,
	uhci_device_isoc_start,
	uhci_device_isoc_abort,
	uhci_device_isoc_close,
	uhci_noop,
	uhci_device_isoc_done,
#endif	/*******************************************************/
};

/*
 ****** Simulação do HUB Raiz ***********************************
 */
const struct usb_device_descriptor uhci_devd =
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

const struct usb_config_descriptor uhci_confd =
{
	USB_CONFIG_DESCRIPTOR_SIZE,
	UDESC_CONFIG,
	{USB_CONFIG_DESCRIPTOR_SIZE +
	 USB_INTERFACE_DESCRIPTOR_SIZE +
	 USB_ENDPOINT_DESCRIPTOR_SIZE},
	1,
	1,
	0,
	UC_SELF_POWERED,
	0			/* max power */
};

const struct usb_interface_descriptor uhci_ifcd =
{
	USB_INTERFACE_DESCRIPTOR_SIZE,
	UDESC_INTERFACE,
	0,
	0,
	1,
	UICLASS_HUB,
	UISUBCLASS_HUB,
	UIPROTO_FSHUB,
	0
};

#define		UHCI_INTR_ENDPT		1

const struct usb_endpoint_descriptor uhci_endpd =
{
	USB_ENDPOINT_DESCRIPTOR_SIZE,
	UDESC_ENDPOINT,
	UE_DIR_IN | UHCI_INTR_ENDPT,
	UE_INTERRUPT,
	{8},
	255
};

const struct usb_hub_descriptor uhci_hubd_piix =
{
	USB_HUB_DESCRIPTOR_SIZE,
	UDESC_HUB,
	2,
	{ UHD_PWR_NO_SWITCH | UHD_OC_INDIVIDUAL, 0 },
	50,			/* power on to power good */
	0,
	{ 0x00 },		/* both ports are removable */
};

/*
 ******	Registros do UHCI ***************************************
 */
#define	REGSZ			20		/* Os 8 registros ocupam 20 bytes */

#define UHCI_CMD		0x00
#define  UHCI_CMD_RS		0x0001
#define  UHCI_CMD_HCRESET	0x0002
#define  UHCI_CMD_GRESET	0x0004
#define  UHCI_CMD_EGSM		0x0008
#define  UHCI_CMD_FGR		0x0010
#define  UHCI_CMD_SWDBG		0x0020
#define  UHCI_CMD_CF		0x0040
#define  UHCI_CMD_MAXP		0x0080

#define UHCI_STS		0x02
#define  UHCI_STS_USBINT	0x0001
#define  UHCI_STS_USBEI		0x0002
#define  UHCI_STS_RD		0x0004
#define  UHCI_STS_HSE		0x0008
#define  UHCI_STS_HCPE		0x0010
#define  UHCI_STS_HCH		0x0020
#define  UHCI_STS_ALLINTRS	0x003F

#define UHCI_INTR		0x04
#define  UHCI_INTR_TOCRCIE	0x0001
#define  UHCI_INTR_RIE		0x0002
#define  UHCI_INTR_IOCE		0x0004
#define  UHCI_INTR_SPIE		0x0008

#define UHCI_FRNUM		0x06
#define  UHCI_FRNUM_MASK	0x03FF
 
#define UHCI_FLBASEADDR		0x08		/* 4 bytes */

#define UHCI_SOF		0x0C
#define  UHCI_SOF_MASK		0x7F

#define UHCI_PORTSC1      	0x010
#define UHCI_PORTSC2      	0x012
#define UHCI_PORTSC_CCS		0x0001
#define UHCI_PORTSC_CSC		0x0002
#define UHCI_PORTSC_PE		0x0004
#define UHCI_PORTSC_POEDC	0x0008
#define UHCI_PORTSC_LS		0x0030
#define UHCI_PORTSC_LS_SHIFT	4
#define UHCI_PORTSC_RD		0x0040
#define UHCI_PORTSC_LSDA	0x0100
#define UHCI_PORTSC_PR		0x0200
#define UHCI_PORTSC_OCI		0x0400
#define UHCI_PORTSC_OCIC	0x0800
#define UHCI_PORTSC_SUSP	0x1000

#define URWMASK(x)		((x) & (UHCI_PORTSC_SUSP|UHCI_PORTSC_PR|UHCI_PORTSC_RD|UHCI_PORTSC_PE))

#define UHCI_FRAMELIST_COUNT	1024
#define UHCI_FRAMELIST_ALIGN	4096

#define UHCI_TD_ALIGN		16
#define UHCI_QH_ALIGN		16

#define UHCI_PTR_T		0x00000001
#define UHCI_PTR_TD		0x00000000
#define UHCI_PTR_QH		0x00000002
#define UHCI_PTR_VF		0x00000004

/* 
 *	Wait this long after a QH has been removed.  This gives that HC a
 *	chance to stop looking at it before it's recycled.
 */
#define UHCI_QH_REMOVE_DELAY	5

#define PWR_RESUME		0

#ifdef	USB_MSG
/*
 ******	Nomes dos Pedidos ***************************************
 */
const char	*request_str[] =
{
	"GET_STATUS",		"CLEAR_FEATURE", 	"SET_FEATURE",	"SET_ADDRESS",
	"GET_DESCRIPTOR",	"SET_DESCRIPTOR",	"GET_CONFIG",	"SET_CONFIG",
	"GET_INTERFACE",	"SET_INTERFACE",	"SYNCH_FRAME"
};
#endif	USB_MSG

/*
 ****** Descritores de Transferência ****************************
 */
/*
 *	The Queue Heads and Transfer Descriptors are accessed
 *	by both the CPU and the USB controller which run
 *	concurrently.  This means that they have to be accessed
 *	with great care.  As long as the data structures are
 *	not linked into the controller's frame list they cannot
 *	be accessed by it and anything goes.  As soon as a
 *	TD is accessible by the controller it "owns" the td_status
 *	field; it will not be written by the CPU.  Similarly
 *	the controller "owns" the qh_elink field.
 */
struct uhci_td
{
	ulong		td_link;
	ulong		td_status;
	ulong		td_token;
	ulong		td_buffer;
};

struct uhci_qh
{
	ulong		qh_hlink;
	ulong		qh_elink;
};

#define UHCI_TD_GET_ACTLEN(s)	(((s) + 1) & 0x3FF)
#define UHCI_TD_ZERO_ACTLEN(t)	((t) | 0x3FF)
#define UHCI_TD_BITSTUFF	0x00020000
#define UHCI_TD_CRCTO		0x00040000
#define UHCI_TD_NAK		0x00080000
#define UHCI_TD_BABBLE		0x00100000
#define UHCI_TD_DBUFFER		0x00200000
#define UHCI_TD_STALLED		0x00400000
#define UHCI_TD_ACTIVE		0x00800000
#define UHCI_TD_IOC		0x01000000
#define UHCI_TD_IOS		0x02000000
#define UHCI_TD_LS		0x04000000
#define UHCI_TD_GET_ERRCNT(s)	(((s) >> 27) & 3)
#define UHCI_TD_SET_ERRCNT(n)	((n) << 27)
#define UHCI_TD_SPD		0x20000000

#define UHCI_TD_PID_IN		0x00000069
#define UHCI_TD_PID_OUT		0x000000E1
#define UHCI_TD_PID_SETUP	0x0000002D
#define UHCI_TD_GET_PID(s)	((s) & 0xFF)
#define UHCI_TD_SET_DEVADDR(a)	((a) << 8)
#define UHCI_TD_GET_DEVADDR(s)	(((s) >> 8) & 0x7f)
#define UHCI_TD_SET_ENDPT(e)	(((e)&0xf) << 15)
#define UHCI_TD_GET_ENDPT(s)	(((s) >> 15) & 0xf)
#define UHCI_TD_SET_DT(t)	((t) << 19)
#define UHCI_TD_GET_DT(s)	(((s) >> 19) & 1)
#define UHCI_TD_SET_MAXLEN(l)	(((l)-1) << 21)
#define UHCI_TD_GET_MAXLEN(s)	((((s) >> 21) + 1) & 0x7FF)
#define UHCI_TD_MAXLEN_MASK	0xFFE00000

#define UHCI_TD_ERROR				\
	(	UHCI_TD_BITSTUFF |		\
		UHCI_TD_CRCTO    |		\
		UHCI_TD_BABBLE   |		\
		UHCI_TD_DBUFFER  |		\
		UHCI_TD_STALLED			\
	)

#define UHCI_TD_SETUP(len, endp, dev)		\
	(	UHCI_TD_SET_MAXLEN(len)  |	\
		UHCI_TD_SET_ENDPT(endp)  |	\
		UHCI_TD_SET_DEVADDR(dev) |	\
		UHCI_TD_PID_SETUP		\
	)

#define UHCI_TD_OUT(len, endp, dev, dt)		\
	(	UHCI_TD_SET_MAXLEN(len)  |	\
		UHCI_TD_SET_ENDPT(endp)  |	\
		UHCI_TD_SET_DEVADDR(dev) |	\
		UHCI_TD_PID_OUT	|		\
		UHCI_TD_SET_DT(dt)		\
	)

#define UHCI_TD_IN(len, endp, dev, dt)		\
	(	UHCI_TD_SET_MAXLEN(len)  |	\
		UHCI_TD_SET_ENDPT(endp)  |	\
		UHCI_TD_SET_DEVADDR(dev) |	\
		UHCI_TD_PID_IN           |	\
		UHCI_TD_SET_DT(dt)		\
	)

/*
 ******	Estrutura "uhci_softc" **********************************
 */
#define UHCI_VFRAMELIST_COUNT		128

union uhci_soft_td_qh
{
	struct uhci_soft_qh		*sqh;
	struct uhci_soft_td		*std;
};

/*
 *	An interrupt info struct contains the information needed to
 *	execute a requested routine when the controller generates an
 *	interrupt.  Since we cannot know which transfer generated
 *	the interrupt all structs are linked together so they can be
 *	searched at interrupt time.
 */
struct uhci_intr_info
{
	struct uhci_softc		*sc;
	struct usbd_xfer		*xfer;
	struct uhci_soft_td		*stdstart;
	struct uhci_soft_td		*stdend;
	struct uhci_intr_info		*next_intr;
	struct uhci_intr_info		**prev_intr;
};

struct uhci_xfer
{
	struct usbd_xfer		xfer;
	struct uhci_intr_info		iinfo;
	struct usb_task			abort_task;
	int				curframe;
#if (0)	/*******************************************************/
	char				uhci_xfer_flags;
	EVENT				uhci_xfer_event;
#endif	/*******************************************************/
};

enum
{
	UHCI_XFER_ABORTING =  0x0001, 	/* xfer is aborting. */
#if (0)	/*******************************************************/
	UHCI_XFER_ABORTWAIT = 0x0002	/* abort completion is being awaited. */
#endif	/*******************************************************/
};

/*
 *	Extra information that we need for a TD.
 */
struct uhci_soft_td
{
	struct uhci_td		td;		/* The real TD, must be first */
	union uhci_soft_td_qh	link;		/* soft version of the td_link field */
	ulong			physaddr;	/* TD's physical address */
};

/* 
 *	Make the size such that it is a multiple of UHCI_TD_ALIGN.  This way
 *	we can pack a number of soft TD together and have the real TD well
 *	aligned. NOTE: Minimum size is 32 bytes.
 */
#define UHCI_STD_SIZE		((sizeof (struct uhci_soft_td) + UHCI_TD_ALIGN - 1) / UHCI_TD_ALIGN * UHCI_TD_ALIGN)
#define UHCI_STD_CHUNK		128		/* (PAGE_SIZE / UHCI_TD_SIZE) */

#define UHCI_SQH_SIZE		((sizeof (struct uhci_soft_qh) + UHCI_QH_ALIGN - 1) / UHCI_QH_ALIGN * UHCI_QH_ALIGN)
#define UHCI_SQH_CHUNK		128		/* (PAGE_SIZE / UHCI_QH_SIZE) */

/*
 *	Extra information that we need for a QH.
 */
struct uhci_soft_qh
{
	struct uhci_qh		qh;		/* The real QH, must be first */
	struct uhci_soft_qh	*hlink;		/* soft version of qh_hlink */
	struct uhci_soft_td	*elink;		/* soft version of qh_elink */
	ulong			physaddr;	/* QH's physical address */
	int			pos;		/* Timeslot position */
};

/*
 *	Information about an entry in the virtual frame list.
 */
struct uhci_vframe
{
	struct uhci_soft_td	*htd;		/* pointer to dummy TD */
	struct uhci_soft_td	*etd;		/* pointer to last TD */
	struct uhci_soft_qh	*hqh;		/* pointer to dummy QH */
	struct uhci_soft_qh	*eqh;		/* pointer to last QH */
	uint			bandwidth;	/* max bandwidth used by this frame */
};

struct uhci_softc
{
	struct usbd_bus		sc_bus;		/* base device */

	ulong			*sc_pframes;	/* Endereço virtual */
	ulong			sc_dma;		/* Endereço físico */
	struct uhci_vframe	sc_vframes[UHCI_VFRAMELIST_COUNT];

	struct uhci_soft_qh	*sc_lctl_start;	/* dummy QH for low speed control */
	struct uhci_soft_qh	*sc_lctl_end;	/* last control QH */
	struct uhci_soft_qh	*sc_hctl_start;	/* dummy QH for high speed control */
	struct uhci_soft_qh	*sc_hctl_end;	/* last control QH */
	struct uhci_soft_qh	*sc_bulk_start;	/* dummy QH for bulk */
	struct uhci_soft_qh	*sc_bulk_end;	/* last bulk transfer */
	struct uhci_soft_qh	*sc_last_qh;	/* dummy QH at the end */
	ulong			sc_loops;	/* number of QHs that wants looping */

	struct uhci_soft_td	*sc_freetds;	/* TD free list */
	struct uhci_soft_qh	*sc_freeqhs;	/* QH free list */

	struct usbd_xfer	*sc_free_xfers_first,	/* Lista de descritores livres */
				**sc_free_xfers_last;

	char			sc_addr;	/* device address */
	char			sc_conf;	/* device configuration */

	char			sc_saved_sof;
	ushort			sc_saved_frnum;

	char			sc_isreset;
	char			sc_suspend;
	char			sc_dying;

	struct uhci_intr_info	*sc_intrhead;	/* Lista ativa */

	/* Info for the root hub interrupt channel */

	int			sc_ival;	/* time between root hub intrs */
	struct usbd_xfer	*sc_intr_xfer;	/* root hub interrupt transfer */

#if (0)	/*******************************************************/
	TIMEOUT			sc_poll_handle;
	struct device		*sc_child;	/* /dev/usb# device */
#endif	/*******************************************************/

	char			sc_vendor[16];	/* vendor string for root hub */
	int			sc_id_vendor;	/* vendor ID for root hub */
};

/*
 ****** Estrutura "uhci_pipe" ***********************************
 */
struct uhci_pipe
{
	struct usbd_pipe	pipe;
	int			nexttoggle;

	char			aborting;
	struct usbd_xfer	*abortstart, *abortend;

	union
	{
		struct					/* Control pipe */
		{
			struct uhci_soft_qh	*sqh;
			void			*reqdma;
			struct uhci_soft_td	*setup, *stat;
			uint			length;

		}	ctl;

		struct					/* Interrupt pipe */
		{
			int			npoll;
			int			isread;
			struct uhci_soft_qh	**qhs;

		}	intr;

		struct					/* Bulk pipe */
		{
			struct uhci_soft_qh	*sqh;
			uint			length;
			int			isread;

		}	bulk;

		struct iso 				/* Iso pipe */
		{
			struct uhci_soft_td	**stds;
			int			next, inuse;

		}	iso;

	}	u;
};

/*
 ******	Protótipos de funções ******************************
 */
int			uhci_init (struct uhci_softc *sc);
int			uhci_run (struct uhci_softc *sc, int run);
void			uhci_intr (struct intr_frame frame);
void			uhci_intr1 (struct uhci_softc *sc);

int			uhci_device_request (struct usbd_xfer *xfer);
void			uhci_waitintr (struct uhci_softc *sc, struct usbd_xfer *xfer);
void			uhci_abort_xfer (struct usbd_xfer *xfer, int status);

void			uhci_add_hs_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh);
void			uhci_remove_hs_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh);
void			uhci_add_ls_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh);
void			uhci_remove_ls_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh);

void			uhci_add_bulk (struct uhci_softc *sc, struct uhci_soft_qh *sqh);
void			uhci_remove_bulk (struct uhci_softc *sc, struct uhci_soft_qh *sqh);

void			uhci_add_loop (struct uhci_softc *sc);
void			uhci_rem_loop (struct uhci_softc *sc);

int			uhci_alloc_std_chain
			(	struct uhci_pipe *upipe, struct uhci_softc *sc, int len, int rd, ushort flags,
				void **dma, struct uhci_soft_td **sp, struct uhci_soft_td **ep
			);

void			uhci_free_std_chain
			(	struct uhci_softc *sc, struct uhci_soft_td *std,
				struct uhci_soft_td *stdend
			);

struct uhci_soft_qh	*uhci_find_prev_qh (struct uhci_soft_qh *pqh, struct uhci_soft_qh *sqh);
void			usbd_start_next(struct usbd_pipe *pipe_ptr);

void			uhci_add_intr (struct uhci_softc *sc, struct uhci_soft_qh *sqh);

struct uhci_soft_td	*uhci_alloc_std (struct uhci_softc *sc);
void			uhci_free_std (struct uhci_softc *sc, struct uhci_soft_td *std);

struct uhci_soft_qh	*uhci_alloc_sqh (struct uhci_softc *sc);
void			uhci_free_sqh (struct uhci_softc *sc, struct uhci_soft_qh *sqh);

void			uhci_check_intr (struct uhci_softc *sc, struct uhci_intr_info *ii);
void			uhci_idone (struct uhci_intr_info *ii);

int			uhci_setup_isoc (struct usbd_pipe *pipe_ptr);
int			uhci_portreset (struct uhci_softc *sc, int index);

int			uhci_read_port_or_mem_char (const struct uhci_softc *up, int reg);
void			uhci_write_port_or_mem_char (const struct uhci_softc *up, int value, int reg);
int			uhci_read_port_or_mem_short (const struct uhci_softc *up, int reg);
void			uhci_write_port_or_mem_short (const struct uhci_softc *up, int value, int reg);
int			uhci_read_port_or_mem_long (const struct uhci_softc *up, int reg);
void			uhci_write_port_or_mem_long (const struct uhci_softc *up, int value, int reg);

/*
 ****************************************************************
 *	Função de "probe" dos controladores USB UHCI		*
 ****************************************************************
 */
char *
uhci_probe (PCIDATA *pci, ulong dev_vendor)
{
	if
	(	pci->pci_baseclass != PCIC_SERIALBUS     ||
		pci->pci_subclass  != PCIS_SERIALBUS_USB ||
		pci->pci_progif    != PCI_INTERFACE_UHCI
	)
		return (NOSTR);

	switch (dev_vendor)
	{
		/* Intel -- vendor 0x8086 */

	    case 0x70208086:
		return ("Intel 82371SB (PIIX3) USB controller");

	    case 0x71128086:
		return ("Intel 82371AB/EB (PIIX4) USB controller");

	    case 0x24128086:
		return ("Intel 82801AA (ICH) USB controller");

	    case 0x24228086:
		return ("Intel 82801AB (ICH0) USB controller");

	    case 0x24428086:
		return ("Intel 82801BA/BAM (ICH2) USB controller USB-A");

	    case 0x24448086:
		return ("Intel 82801BA/BAM (ICH2) USB controller USB-B");

	    case 0x24828086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-A");

	    case 0x24848086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-B");

	    case 0x24878086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-C");

	    case 0x24C28086:
		return ("Intel 82801DB (ICH4) USB controller USB-A");

	    case 0x24C48086:
		return ("Intel 82801DB (ICH4) USB controller USB-B");

	    case 0x24C78086:
		return ("Intel 82801DB (ICH4) USB controller USB-C");

	    case 0x24D28086:
		return ("Intel 82801EB (ICH5) USB controller USB-A");

	    case 0x24D48086:
		return ("Intel 82801EB (ICH5) USB controller USB-B");

	    case 0x24D78086:
		return ("Intel 82801EB (ICH5) USB controller USB-C");

	    case 0x24DE8086:
		return ("Intel 82801EB (ICH5) USB controller USB-D");

	    case 0x26588086:
		return ("Intel 82801FB/FR/FW/FRW (ICH6) USB controller USB-A");

	    case 0x26598086:
		return ("Intel 82801FB/FR/FW/FRW (ICH6) USB controller USB-B");

	    case 0x265A8086:
		return ("Intel 82801FB/FR/FW/FRW (ICH6) USB controller USB-C");

	    case 0x265B8086:
		return ("Intel 82801FB/FR/FW/FRW (ICH6) USB controller USB-D");

	    case 0x28308086:
		return ("Intel 82801 (ICH8) USB controller USB-A");

	    case 0x28318086:
		return ("Intel 82801 (ICH8) USB controller USB-B");

	    case 0x28328086:
		return ("Intel 82801 (ICH8) USB controller USB-C");

	    case 0x28348086:
		return ("Intel 82801 (ICH8) USB controller USB-D");

	    case 0x28358086:
		return ("Intel 82801 (ICH8) USB controller USB-E");

	    case 0x719A8086:
		return ("Intel 82443MX USB controller");

	    case 0x76028086:
		return ("Intel 82372FB/82468GX USB controller");

		/* VIA Technologies -- vendor 0x1106 (0x1107 on the Apollo Master) */

	    case 0x30381106:
		return ("VIA 83C572 USB controller");

#if (0)	/*******************************************************/
		/* AcerLabs -- vendor 0x10b9 */

	    case 0x523710B9:
		return ("AcerLabs M5237 (Aladdin-V) USB controller");

		/* OPTi -- vendor 0x1045 */

	    case 0xC8611045:
		return ("OPTi 82C861 (FireLink) USB controller");

		/* NEC -- vendor 0x1033 */

	    case 0x00351033:
		return ("NEC uPD 9210 USB controller");

		/* CMD Tech -- vendor 0x1095 */

	    case 0x06701095:
		return ("CMD Tech 670 (USB0670) USB controller");

	    case 0x06731095:
		return ("CMD Tech 673 (USB0673) USB controller");
#endif	/*******************************************************/

	    default:
		return ("UHCI (generic) USB controller");
	}

}	/* uhci_probe */

/*
 ****************************************************************
 *	Função de "attach" dos controladores USB UHCI		*
 ****************************************************************
 */
void
uhci_attach (PCIDATA *pci, ulong dev_vendor)
{
	ulong			base_addr;
	int			index;
	struct uhci_softc	*sc;
	struct device		*uhci, *usb;

	/*
	 *	Verifica se este controlador deve ser habilitado
	 */
	if (++usb_unit >= NUSB)
		{ printf ("uhci_attach: NÃO há mais espaço para controladores USB\n"); return; }

	if (scb.y_usb_enable[usb_unit] == 0)
		return;

	/*
	 *	Aloca e zera a estrutura "uhci_softc"
	 */
	if ((sc = malloc_byte (sizeof (struct uhci_softc))) == NULL)
		{ printf ("uhci_attach: memória esgotada\n"); return; }

	memclr (sc, sizeof (struct uhci_softc));

	usb_data[usb_unit].usb_data_ptr    = sc;
	usb_data[usb_unit].usb_data_type   = 'U';
	usb_data[usb_unit].usb_data_stolen = 0;

	/*
	 *	Obtém o endereço base e o tipo de mapeamento
	 */
	for (index = 0; index < MAX_PCI_ADDR; index++)
	{
		if (index >= MAX_PCI_ADDR)
			{ printf ("%g: Não consegui obter o endereço do controladore USB\n"); goto bad; }

		if ((base_addr = pci->pci_addr[index]) != 0)
			break;
	}

	sc->sc_bus.base_addr	= base_addr;
	sc->sc_bus.port_mapped	= pci->pci_map_mask & (1 << index);

	sc->sc_bus.unit		= usb_unit;
	sc->sc_bus.irq		= pci->pci_intline;

	/*
	 *	Habilita o "BUS mastering"
	 */
	pci_write (pci, PCIR_COMMAND, pci_read (pci, PCIR_COMMAND, 2) | PCIM_CMD_BUSMASTEREN, 2);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (sc->sc_bus.irq, USB_PL, usb_unit, uhci_intr) < 0)
		goto bad;

	/*
	 *	Armazena a Marca do USB
	 */
	switch (pci->pci_vendor)
	{
	    case 0x8086:
		strcpy (sc->sc_vendor, "Intel");
		break;

	    case 0x1106:
		strcpy (sc->sc_vendor, "VIA");
		break;

	    default:
		sprintf (sc->sc_vendor, "(0x%04X)", pci->pci_vendor);
	}

	/*
	 *	Imprime o que encontrou
	 */
	printf ("usb: [%d: 0x%X, %d, UHCI rev = ", usb_unit, base_addr, sc->sc_bus.irq);

	switch (pci_read (pci, PCI_USBREV, 4) & PCI_USBREV_MASK)
	{
	    case PCI_USBREV_PRE_1_0:
		sc->sc_bus.usbrev = USBREV_PRE_1_0;
		printf ("PRE 1.0");
		break;

	    case PCI_USBREV_1_0:
		sc->sc_bus.usbrev = USBREV_1_0;
		printf ("1.0");
		break;

	    default:
		sc->sc_bus.usbrev = USBREV_UNKNOWN;
		printf ("???");
		break;
	}

	printf ("]\n");

	/*
	 *	Cria o dispositivo pai na mão
	 */
	if (usb_class_list == NULL && usb_create_classes () < 0)
		goto bad;

	uhci = make_device (NULL, "usb_controller", -1);

	if ((usb = device_add_child (uhci, "usb", -1)) == NULL)
		{ printf ("uhci_attach: memória esgotada\n"); goto bad; }

	usb->ivars = sc; sc->sc_bus.bdev = usb;

	/*
	 *	Inabilita o suporte a dispositivos legados
	 */
	pci_write (pci, PCI_LEGSUP, PCI_LEGSUP_USBPIRQDEN, 2);

	/*
	 *	Inicialização
	 */
	if (uhci_init (sc) != USBD_NORMAL_COMPLETION)
		{ printf ("uhci_attach: Erro na inicialização de usb%d\n", usb_unit); goto bad; }

	/*
	 *	Somente agora deixa interromper
	 */
	uhci_write_port_or_mem_short (sc, UHCI_INTR_TOCRCIE|UHCI_INTR_RIE|UHCI_INTR_IOCE|UHCI_INTR_SPIE, UHCI_INTR);

	/*
	 *	Procura dispsitivos USB
	 */
	device_probe_and_attach (usb);

	return;

    bad:
	free_byte (sc); 	usb_data[usb_unit].usb_data_ptr = NULL;

}	/* end uhci_attach */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
uhci_init (struct uhci_softc *sc)
{
	int			i, j;
	pg_t			pgno;
	struct uhci_soft_td	*std;
	struct uhci_soft_qh	*lsqh, *bsqh, *chsqh, *clsqh, *sqh;

#ifdef	USB_MSG
	printf
	(	"%s regs: cmd=%04x, sts=%04x, intr=%04x, frnum=%04x, "
		"flbase=%08x, sof=%04x, portsc1=%04x, portsc2=%04x\n",
		sc->sc_bus.bdev->nameunit,
		uhci_read_port_or_mem_short (sc, UHCI_CMD),
		uhci_read_port_or_mem_short (sc, UHCI_STS),
		uhci_read_port_or_mem_short (sc, UHCI_INTR),
		uhci_read_port_or_mem_short (sc, UHCI_FRNUM),
		uhci_read_port_or_mem_long (sc, UHCI_FLBASEADDR),
		uhci_read_port_or_mem_char (sc, UHCI_SOF),
		uhci_read_port_or_mem_short (sc, UHCI_PORTSC1),
		uhci_read_port_or_mem_short (sc, UHCI_PORTSC2)
	);
#endif	USB_MSG

	/*
	 *	Inicialização
	 */
	uhci_write_port_or_mem_short (sc, 0, UHCI_INTR);		/* Desliga as interrupções */

	uhci_write_port_or_mem_short (sc, UHCI_CMD_GRESET, UHCI_CMD);	/* Reset global */

	DELAY (50 * 1000);						/* Linux */

#if (0)	/*******************************************************/
	DELAY (100 * 1000);						/* FreeBSD */
#endif	/*******************************************************/

	uhci_write_port_or_mem_short (sc, 0, UHCI_CMD);

	DELAY (10 * 1000);						/* Linux */

	uhci_write_port_or_mem_short (sc, UHCI_CMD_HCRESET, UHCI_CMD);	/* Reset local */

	for (i = 100; /* abaixo */; i--)
	{
		if (i < 0)
			{ printf ("O controlador USB Não conseguir dar reset\n"); break; }

		if ((uhci_read_port_or_mem_short (sc, UHCI_CMD) & UHCI_CMD_HCRESET) == 0)
			break;

		DELAY (1000);
	}

	/*
	 *	Aloca e inicializa o vetor de ponteiros
	 */
	pgno = BYUPPG (UHCI_FRAMELIST_COUNT * sizeof (void *));

	if ((sc->sc_pframes = (ulong *)PGTOBY (malloce (M_CORE, pgno))) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	sc->sc_dma = VIRT_TO_PHYS_ADDR (sc->sc_pframes);

	uhci_write_port_or_mem_short (sc, 0, UHCI_FRNUM);		/* Segmento = 0 */
	uhci_write_port_or_mem_long  (sc, sc->sc_dma, UHCI_FLBASEADDR);	/* Atribui a lista de Segmentos */

	/*
	 *	Aloca um descritor de transferências
	 */
	if ((std = uhci_alloc_std (sc)) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	std->link.std	  = NULL;
	std->td.td_link   = UHCI_PTR_T;
	std->td.td_status = 0;			/* inactive */
	std->td.td_token  = 0;
	std->td.td_buffer = 0;

	/*
	 *	Aloca um cabeçalho de fila fictício
	 */
	if ((lsqh = uhci_alloc_sqh (sc)) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	lsqh->hlink	  = NULL;
	lsqh->qh.qh_hlink = UHCI_PTR_T;		/* Final da cadeia de QHs */
	lsqh->elink	  = std;
	lsqh->qh.qh_elink = std->physaddr|UHCI_PTR_TD;

	sc->sc_last_qh	  = lsqh;

	/*
	 *	Aloca um cabeçalho de fila fictício para o tráfego regular
	 */
	if ((bsqh = uhci_alloc_sqh (sc)) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	bsqh->hlink	  = lsqh;
	bsqh->qh.qh_hlink = lsqh->physaddr|UHCI_PTR_QH;
	bsqh->elink	  = NULL;
	bsqh->qh.qh_elink = UHCI_PTR_T;

	sc->sc_bulk_start = sc->sc_bulk_end = bsqh;

	/*
	 *	Aloca um cabeçalho de fila fictício para o tráfego maciço
	 */
	if ((chsqh = uhci_alloc_sqh (sc)) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	chsqh->hlink	   = bsqh;
	chsqh->qh.qh_hlink = bsqh->physaddr|UHCI_PTR_QH;
	chsqh->elink	   = NULL;
	chsqh->qh.qh_elink = UHCI_PTR_T;
	sc->sc_hctl_start  = sc->sc_hctl_end = chsqh;

	/*
	 *	Aloca um cabeçalho de fila fictício para o tráfego de controle
	 */
	if ((clsqh = uhci_alloc_sqh (sc)) == NULL)
		{ printf ("uhci_init: memória esgotada\n"); return (-1); }

	clsqh->hlink	   = bsqh;
	clsqh->qh.qh_hlink = chsqh->physaddr|UHCI_PTR_QH;
	clsqh->elink	   = NULL;
	clsqh->qh.qh_elink = UHCI_PTR_T;
	sc->sc_lctl_start  = sc->sc_lctl_end = clsqh;

	/*
	 *	Make all (virtual) frame list pointers point to the interrupt
	 *	queue heads and the interrupt queue heads at the control
	 *	queue head and point the physical frame list to the virtual.
	 */
	for (i = 0; i < UHCI_VFRAMELIST_COUNT; i++)
	{
		if ((std = uhci_alloc_std (sc)) == NULL || (sqh = uhci_alloc_sqh (sc)) == NULL)
			{ printf ("uhci_init: memória esgotada\n"); return (-1); }

		std->link.sqh	  = sqh;
		std->td.td_link	  = sqh->physaddr|UHCI_PTR_QH;
		std->td.td_status = UHCI_TD_IOS;		/* iso, inativo */
		std->td.td_token  = 0;
		std->td.td_buffer = 0;
		sqh->hlink	  = clsqh;
		sqh->qh.qh_hlink  = clsqh->physaddr|UHCI_PTR_QH;
		sqh->elink	  = NULL;
		sqh->qh.qh_elink  = UHCI_PTR_T;

		sc->sc_vframes[i].htd = std;
		sc->sc_vframes[i].etd = std;
		sc->sc_vframes[i].hqh = sqh;
		sc->sc_vframes[i].eqh = sqh;

		for (j = i; j < UHCI_FRAMELIST_COUNT; j += UHCI_VFRAMELIST_COUNT)
			sc->sc_pframes[j] = std->physaddr;
	}

	/* Inicializa a lista ativa */

	sc->sc_intrhead = NULL;	

	/* Inicializa a lista de descritores livres */

	sc->sc_free_xfers_first = NULL;
	sc->sc_free_xfers_last = &sc->sc_free_xfers_first;

#if (0)	/*******************************************************/
	usb_callout_init(sc->sc_poll_handle);
#endif	/*******************************************************/

	/* Set up the bus struct */

	sc->sc_bus.methods   = &uhci_bus_methods;
	sc->sc_bus.pipe_size = sizeof (struct uhci_pipe);

	uhci_write_port_or_mem_short (sc, UHCI_CMD_MAXP, UHCI_CMD);	/* Assumindo pacotes de 64 bytes */

	/*
	 *	Executa o comando
	 */
	return (uhci_run (sc, 1));

}	/* end uhci_init */

/*
 ****************************************************************
 *	Executa um comando					*
 ****************************************************************
 */
int
uhci_run (struct uhci_softc *sc, int run)
{
	int		s, n, running, cmd;

	run = (run != 0);

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	cmd = uhci_read_port_or_mem_short (sc, UHCI_CMD);

	if (run)
		cmd |= UHCI_CMD_RS;
	else
		cmd &= ~UHCI_CMD_RS;

	uhci_write_port_or_mem_short (sc, cmd, UHCI_CMD);

#ifdef	USB_MSG
	printf
	(	"uhci_run: após escrever o valor 0x%04X, cmd = 0x%04X sts=0x%04X\n",
		cmd, uhci_read_port_or_mem_short (sc, UHCI_CMD), uhci_read_port_or_mem_short (sc, UHCI_STS)
	);
#endif	USB_MSG

	for (n = 0; n < 10; n++)
	{
		running = !(uhci_read_port_or_mem_short (sc, UHCI_STS) & UHCI_STS_HCH);

		/* return when we've entered the state we want */

		if (run == running)
		{
			splx (s);

#ifdef	USB_MSG
			printf
			(	"uhci_run: retorno NORMAL: cmd = 0x%04X sts=0x%04X\n",
				uhci_read_port_or_mem_short (sc, UHCI_CMD), uhci_read_port_or_mem_short (sc, UHCI_STS)
			);
#endif	USB_MSG

			return (USBD_NORMAL_COMPLETION);
		}

		usb_delay_ms (&sc->sc_bus, 1);
	}

	printf ("uhci_run: NÃO consegui %s o USB%d\n", run ? "iniciar" : "parar", sc->sc_bus.unit);

	splx (s);

	return (USBD_IOERROR);

}	/* end uhci_run */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_device_setintr (struct uhci_softc *sc, struct uhci_pipe *upipe, int ival)
{
	struct uhci_soft_qh	*sqh;
	int			i, npoll, s;
	unsigned		bestbw, bw, bestoffs, offs;

#ifdef	USB_MSG
	printf ("uhci_setintr: pipe = %P\n", upipe);
#endif	USB_MSG

	if   (ival == 0)
		{ printf ("uhci_setintr: 0 interval\n"); return (USBD_INVAL); }

	if (ival > UHCI_VFRAMELIST_COUNT)
		ival = UHCI_VFRAMELIST_COUNT;

	npoll = (UHCI_VFRAMELIST_COUNT + ival - 1) / ival;

#ifdef	USB_MSG
	printf ("uhci_setintr: ival = %d npoll = %d\n", ival, npoll);
#endif	USB_MSG

	upipe->u.intr.npoll = npoll;
	upipe->u.intr.qhs = malloc_byte (npoll * sizeof (struct uhci_soft_qh *));

	/*
	 *	Figure out which offset in the schedule that has most
	 *	bandwidth left over.
	 */
#define MOD(i) ((i) & (UHCI_VFRAMELIST_COUNT-1))

	for (bestoffs = offs = 0, bestbw = ~0; offs < ival; offs++)
	{
		for (bw = i = 0; i < npoll; i++)
			bw += sc->sc_vframes[MOD(i * ival + offs)].bandwidth;

		if (bw < bestbw)
			{ bestbw = bw; bestoffs = offs; }
	}

#ifdef	USB_MSG
	printf ("uhci_setintr: bw = %d offs = %d\n", bestbw, bestoffs);
#endif	USB_MSG

	for (i = 0; i < npoll; i++)
	{
		upipe->u.intr.qhs[i] = sqh = uhci_alloc_sqh (sc);
		sqh->elink = NULL;
		sqh->qh.qh_elink = UHCI_PTR_T;
		sqh->pos = MOD (i * ival + bestoffs);
	}

#undef MOD

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	/* Enter QHs into the controller data structures */

	for (i = 0; i < npoll; i++)
		uhci_add_intr (sc, upipe->u.intr.qhs[i]);

	splx (s);

#ifdef	USB_MSG
	printf ("uhci_setintr: returns %P\n", upipe);
#endif	USB_MSG

	return (USBD_NORMAL_COMPLETION);

}	/* end uhci_device_setintr */

/*
 ****************************************************************
 *	Add interrupt QH, called with vflock			*
 ****************************************************************
 */
void
uhci_add_intr (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_vframe	*vf = &sc->sc_vframes[sqh->pos];
	struct uhci_soft_qh	*eqh;

#ifdef	USB_MSG
	printf ("uhci_add_intr: n = %d sqh = %P\n", sqh->pos, sqh);
#endif	USB_MSG

	eqh		 = vf->eqh;

	sqh->hlink       = eqh->hlink;
	sqh->qh.qh_hlink = eqh->qh.qh_hlink;
	eqh->hlink       = sqh;
	eqh->qh.qh_hlink = (sqh->physaddr | UHCI_PTR_QH);

	vf->eqh		 = sqh;
	vf->bandwidth++;

}	/* end uhci_add_intr */

/*
 ****************************************************************
 *	Remove interrupt QH					*
 ****************************************************************
 */
void
uhci_remove_intr (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_vframe	*vf = &sc->sc_vframes[sqh->pos];
	struct uhci_soft_qh	*pqh;

#ifdef	USB_MSG
	printf ("uhci_remove_intr: n=%d sqh=%p\n", sqh->pos, sqh);
#endif	USB_MSG

	/* See comment in uhci_remove_ctrl() */

	if (!(sqh->qh.qh_elink & UHCI_PTR_T))
	{
		sqh->qh.qh_elink = UHCI_PTR_T;

		DELAY (UHCI_QH_REMOVE_DELAY);
	}

	pqh = uhci_find_prev_qh (vf->hqh, sqh);

	pqh->hlink       = sqh->hlink;
	pqh->qh.qh_hlink = sqh->qh.qh_hlink;

	DELAY (UHCI_QH_REMOVE_DELAY);

	if (vf->eqh == sqh)
		vf->eqh = pqh;

	vf->bandwidth--;

}	/* end uhci_remove_intr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 *
 *
 *	The USB hub protocol requires that SET_FEATURE(PORT_RESET) also
 *	enables the port, and also states that SET_FEATURE(PORT_ENABLE)
 *	should not be used by the USB subsystem.  As we cannot issue a
 *	SET_FEATURE(PORT_ENABLE) externally, we must ensure that the port
 *	will be enabled as part of the reset.
 *
 *	On the VT83C572, the port cannot be successfully enabled until the
 *	outstanding "port enable change" and "connection status change"
 *	events have been reset.
 */
int
uhci_portreset (struct uhci_softc *sc, int index)
{
	int		lim, port, x;

	if   (index == 1)
		port = UHCI_PORTSC1;
	elif (index == 2)
		port = UHCI_PORTSC2;
	else
		return (USBD_IOERROR);

	x = URWMASK (uhci_read_port_or_mem_short (sc, port));

	uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_PR, port);

	usb_delay_ms (&sc->sc_bus, USB_PORT_ROOT_RESET_DELAY);

#ifdef	USB_MSG
	printf ("uhci port %d reset, status0 = 0x%04X\n", index, uhci_read_port_or_mem_short (sc, port));
#endif	USB_MSG

	x = URWMASK (uhci_read_port_or_mem_short (sc, port));

	uhci_write_port_or_mem_short (sc, x & ~UHCI_PORTSC_PR, port);

	DELAY (100);

#ifdef	USB_MSG
	printf ("uhci port %d reset, status1 = 0x%04X\n", index, uhci_read_port_or_mem_short (sc, port));
#endif	USB_MSG

	x = URWMASK (uhci_read_port_or_mem_short (sc, port));

	uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_PE, port);

	for (lim = 10; lim >= 0; lim--)
	{
		usb_delay_ms (&sc->sc_bus, USB_PORT_RESET_DELAY);

		x = uhci_read_port_or_mem_short (sc, port);

#ifdef	USB_MSG
		printf ("uhci port %d iteration %u, status = 0x%04X\n", index, lim, x);
#endif	USB_MSG

		if ((x & UHCI_PORTSC_CCS) == 0)
		{
			/*
			 *	No device is connected (or was disconnected
			 *	during reset).  Consider the port reset.
			 *	The delay must be long enough to ensure on
			 *	the initial iteration that the device
			 *	connection will have been registered.  50ms
			 *	appears to be sufficient, but 20ms is not.
			 */
			printf ("uhci port %d loop %u, device detached\n", index, lim);
			break;
		}

		if (x & (UHCI_PORTSC_POEDC|UHCI_PORTSC_CSC))
		{
			/*
			 *	Port enabled changed and/or connection
			 *	status changed were set.  Reset either or
			 *	both raised flags (by writing a 1 to that bit),
			 *	and wait again for state to settle.
			 */
			uhci_write_port_or_mem_short
			(	sc,
				URWMASK (x) | (x & (UHCI_PORTSC_POEDC|UHCI_PORTSC_CSC)),
				port
			);
			continue;
		}

		if (x & UHCI_PORTSC_PE) 	/* Port is enabled */
			break;

		uhci_write_port_or_mem_short (sc, URWMASK (x) | UHCI_PORTSC_PE, port);
	}

#ifdef	USB_MSG
	printf ("uhci port %d reset, status2 = 0x%04X\n", index, uhci_read_port_or_mem_short (sc, port));
#endif	USB_MSG

	if (lim <= 0)
		{ printf ("uhci port %d reset timed out\n", index); return (USBD_TIMEOUT); }

	sc->sc_isreset = 1;

	return (USBD_NORMAL_COMPLETION);

}	/* end uhci_portreset */

/*
 ****************************************************************
 *	Open a new pipe						*
 ****************************************************************
 */
int
uhci_open (struct usbd_pipe *pipe_ptr)
{
	struct uhci_softc		*sc = (struct uhci_softc *)pipe_ptr->device->bus;
	struct uhci_pipe		*upipe = (struct uhci_pipe *)pipe_ptr;
	struct usb_endpoint_descriptor	*ed = pipe_ptr->endpoint->edesc;
	int				ival;

#ifdef	USB_MSG
	printf
	(	"uhci_open: pipe = %P, addr = %d, endpt = %d (%d), bmAttributes = %P\n",
		pipe_ptr, pipe_ptr->device->address,
		ed->bEndpointAddress, sc->sc_addr, ed->bmAttributes
	);
#endif	USB_MSG

	upipe->aborting = 0; upipe->nexttoggle = pipe_ptr->endpoint->savedtoggle;

	if (pipe_ptr->device->address == sc->sc_addr)
	{
		switch (ed->bEndpointAddress)
		{
		    case USB_CONTROL_ENDPOINT:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_root_ctrl_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_root_ctrl_methods;
			break;

		    case UE_DIR_IN | UHCI_INTR_ENDPT:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_root_intr_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_root_intr_methods;
			break;

		    default:
			return (USBD_INVAL);
		}
	}
	else
	{
		switch (ed->bmAttributes & UE_XFERTYPE)
		{
		    case UE_CONTROL:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_device_ctrl_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_device_ctrl_methods;
			upipe->u.ctl.sqh = uhci_alloc_sqh (sc);

			if (upipe->u.ctl.sqh == NULL)
				goto bad;

			upipe->u.ctl.setup = uhci_alloc_std(sc);

			if (upipe->u.ctl.setup == NULL)
			{
				uhci_free_sqh (sc, upipe->u.ctl.sqh);
				goto bad;
			}

			upipe->u.ctl.stat = uhci_alloc_std(sc);

			if (upipe->u.ctl.stat == NULL)
			{
				uhci_free_sqh (sc, upipe->u.ctl.sqh);
				uhci_free_std (sc, upipe->u.ctl.setup);
				goto bad;
			}

			if ((upipe->u.ctl.reqdma = malloc_byte (sizeof (struct usb_device_request))) == NULL)
			{
				uhci_free_sqh (sc, upipe->u.ctl.sqh);
				uhci_free_std (sc, upipe->u.ctl.setup);
				uhci_free_std (sc, upipe->u.ctl.stat);
				goto bad;
			}

			break;

		    case UE_INTERRUPT:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_device_intr_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_device_intr_methods;
			ival = pipe_ptr->interval;

			if (ival == USBD_DEFAULT_INTERVAL)
				ival = ed->bInterval;

			return (uhci_device_setintr (sc, upipe, ival));

		    case UE_ISOCHRONOUS:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_device_isoc_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_device_isoc_methods;
			return (uhci_setup_isoc (pipe_ptr));

		    case UE_BULK:
#ifdef	USB_MSG
			printf ("uhci_open: pipe_ptr->methods = &uhci_device_bulk_methods\n");
#endif	USB_MSG
			pipe_ptr->methods = &uhci_device_bulk_methods;
			upipe->u.bulk.sqh = uhci_alloc_sqh(sc);

			if (upipe->u.bulk.sqh == NULL)
				goto bad;
			break;
		}
	}

	return (USBD_NORMAL_COMPLETION);

 bad:
	return (USBD_NOMEM);

}	/* end uhci_open */

/*
 ****************************************************************
 *	Aloca um descritor de transferência			*
 ****************************************************************
 */
struct usbd_xfer *
uhci_allocx (struct usbd_bus *bus)
{
	struct uhci_softc	*sc = (struct uhci_softc *)bus;
	struct usbd_xfer	*xfer;

	/*
	 *	Verifica se há descritor disponível na lista livre
	 */
	if ((xfer = sc->sc_free_xfers_first) != NULL)
	{
		/* Retira o primeiro da lista de descritores livres */

		if ((sc->sc_free_xfers_first = sc->sc_free_xfers_first->next_xfer) == NULL)
			sc->sc_free_xfers_last = &sc->sc_free_xfers_first;
	}
	else
	{
		/* É necessário alocar */

		xfer = malloc_byte (sizeof (struct uhci_xfer));
	}

	if (xfer != NULL)
	{
		struct uhci_xfer *uxfer = (struct uhci_xfer *)xfer;

		memclr (uxfer, sizeof (struct uhci_xfer));

		uxfer->iinfo.sc = sc;

#if (0)	/*******************************************************/
		usb_init_task(&UXFER(xfer)->abort_task, uhci_timeout_task, xfer);
#endif	/*******************************************************/

	   /***	uxfer->uhci_xfer_flags = 0;		***/

	   /***	EVENTCLEAR (&uxfer->uhci_xfer_event);	***/
	}

	return (xfer);

}	/* end uhci_allocx */

/*
 ****************************************************************
 *	Devolve o descritor para a lista livre			*
 ****************************************************************
 */
void
uhci_freex (struct usbd_bus *bus, struct usbd_xfer *xfer)
{
	struct uhci_softc	*sc = (struct uhci_softc *)bus;

	if ((xfer->next_xfer = sc->sc_free_xfers_first) == NULL)
		sc->sc_free_xfers_last = &xfer->next_xfer;

	sc->sc_free_xfers_first = xfer;

}	/* end uhci_freex */

/*
 ****************************************************************
 * Simulate a hardware hub by handling all the necessary requests *
 ****************************************************************
 */
int
uhci_root_ctrl_transfer (struct usbd_xfer *xfer)
{
	int	err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (uhci_root_ctrl_start (xfer->pipe->first_xfer));

}	/* end uhci_root_ctrl_transfer */

/*
 ****************************************************************
 *	Executa os pedidos referentes ao HUB Raiz		*
 ****************************************************************
 */
int
uhci_root_ctrl_start (struct usbd_xfer *xfer)
{
	struct uhci_softc		*sc = (struct uhci_softc *)xfer->pipe->device->bus;
	struct usb_device_request	*req;
	void		 		*buf = NULL;
	int				port, x;
	int				s, len, value, index, status, change, l, totlen = 0;
	struct usb_port_status		ps;
	int				err;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	req = &xfer->request;

#ifdef	USB_MSG
	printf
	(	"uhci_root_ctrl_start: type = 0x%02X request = %s (%d)\n",
		req->bmRequestType, request_str[req->bRequest], req->bRequest
	);
#endif	USB_MSG

	len   = UGETW (req->wLength);
	value = UGETW (req->wValue);
	index = UGETW (req->wIndex);

	if (len != 0)
		buf = xfer->dmabuf;

#define C(x,y) ((x) | ((y) << 8))

	switch (C (req->bRequest, req->bmRequestType))
	{
		/*
		 *	DEVICE_REMOTE_WAKEUP and ENDPOINT_HALT are no-ops
		 *	for the integrated root hub.
		 */
	    case C (UR_CLEAR_FEATURE, UT_WRITE_DEVICE):
	    case C (UR_CLEAR_FEATURE, UT_WRITE_INTERFACE):
	    case C (UR_CLEAR_FEATURE, UT_WRITE_ENDPOINT):
		break;

	    case C (UR_GET_CONFIG, UT_READ_DEVICE):
		if (len > 0)
			{ *(char *)buf = sc->sc_conf; totlen = 1; }
		break;

	    case C (UR_GET_DESCRIPTOR, UT_READ_DEVICE):

#ifdef	USB_MSG
		printf ("uhci_root_ctrl_control wValue = 0x%04X\n", value);
#endif	USB_MSG

		switch (value >> 8)
		{
		    case UDESC_DEVICE:
			if ((value & 0xFF) != 0)
				{ err = USBD_IOERROR; goto ret; }

			totlen = l = MIN (len, USB_DEVICE_DESCRIPTOR_SIZE);
			USETW (uhci_devd.idVendor, sc->sc_id_vendor);
			memmove (buf, &uhci_devd, l);
			break;

		    case UDESC_CONFIG:
			if ((value & 0xFF) != 0)
				{ err = USBD_IOERROR; goto ret; }

			totlen = l = MIN (len, USB_CONFIG_DESCRIPTOR_SIZE);
			memmove (buf, &uhci_confd, l);
			buf = (char *)buf + l;
			len -= l;
			l = MIN (len, USB_INTERFACE_DESCRIPTOR_SIZE);
			totlen += l;
			memmove(buf, &uhci_ifcd, l);
			buf = (char *)buf + l;
			len -= l;
			l = MIN (len, USB_ENDPOINT_DESCRIPTOR_SIZE);
			totlen += l;
			memmove(buf, &uhci_endpd, l);

			break;

		    case UDESC_STRING:
			if (len == 0)
				break;

			*(char *)buf = 0;
			totlen = 1;

			switch (value & 0xFF)
			{
			    case 1: /* Vendor */
				totlen = usb_str (buf, len, sc->sc_vendor);
				break;
			    case 2: /* Product */
				totlen = usb_str (buf, len, "UHCI root hub");
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
			{ *(char *)buf = 0; totlen = 1; }
		break;

	    case C (UR_GET_STATUS, UT_READ_DEVICE):
		if (len > 1)
		{
			USETW (((struct usb_status *)buf)->wStatus, UDS_SELF_POWERED);
			totlen = 2;
		}
		break;

	    case C (UR_GET_STATUS, UT_READ_INTERFACE):
	    case C (UR_GET_STATUS, UT_READ_ENDPOINT):
		if (len > 1)
			{ USETW (((struct usb_status *)buf)->wStatus, 0); totlen = 2; }
		break;

	    case C (UR_SET_ADDRESS, UT_WRITE_DEVICE):
		if (value >= USB_MAX_DEVICES)
			{ err = USBD_IOERROR; goto ret; }
		sc->sc_addr = value;
		break;

	    case C (UR_SET_CONFIG, UT_WRITE_DEVICE):
		if (value != 0 && value != 1)
			{ err = USBD_IOERROR; goto ret; }
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
#ifdef	USB_MSG
		printf
		(	"uhci_root_ctrl_control: UR_CLEAR_PORT_FEATURE port = %d feature = %d\n",
			index, value
		);
#endif	USB_MSG

		if   (index == 1)
			port = UHCI_PORTSC1;
		elif (index == 2)
			port = UHCI_PORTSC2;
		else
			{ err = USBD_IOERROR; goto ret; }

		switch (value)
		{
		    case UHF_PORT_ENABLE:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x & ~UHCI_PORTSC_PE, port);
			break;

		    case UHF_PORT_SUSPEND:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x & ~UHCI_PORTSC_SUSP, port);
			break;

		    case UHF_PORT_RESET:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x & ~UHCI_PORTSC_PR, port);
			break;

		    case UHF_C_PORT_CONNECTION:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_CSC, port);
			break;

		    case UHF_C_PORT_ENABLE:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_POEDC, port);
			break;

		    case UHF_C_PORT_OVER_CURRENT:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_OCIC, port);
			break;

		    case UHF_C_PORT_RESET:
			sc->sc_isreset = 0;
			err = USBD_NORMAL_COMPLETION;
			goto ret;

		    case UHF_PORT_CONNECTION:
		    case UHF_PORT_OVER_CURRENT:
		    case UHF_PORT_POWER:
		    case UHF_PORT_LOW_SPEED:
		    case UHF_C_PORT_SUSPEND:
		    default:
			err = USBD_IOERROR;
			goto ret;
		}

		break;

	    case C (UR_GET_BUS_STATE, UT_READ_CLASS_OTHER):
		if   (index == 1)
			port = UHCI_PORTSC1;
		elif (index == 2)
			port = UHCI_PORTSC2;
		else
			{ err = USBD_IOERROR; goto ret; }

		if (len > 0)
		{
			x = uhci_read_port_or_mem_short (sc, port);
			*(char  *)buf = (x & UHCI_PORTSC_LS) >> UHCI_PORTSC_LS_SHIFT;
			totlen = 1;
		}

		break;

	    case C (UR_GET_DESCRIPTOR, UT_READ_CLASS_DEVICE):
		if (value != 0)
			{ err = USBD_IOERROR; goto ret; }

		l = MIN (len, USB_HUB_DESCRIPTOR_SIZE);
		totlen = l;
		memmove (buf, &uhci_hubd_piix, l);
		break;

	    case C (UR_GET_STATUS, UT_READ_CLASS_DEVICE):
		if (len != 4)
			{ err = USBD_IOERROR; goto ret; }

		memclr (buf, len);
		totlen = len;
		break;

	    case C (UR_GET_STATUS, UT_READ_CLASS_OTHER):
		if   (index == 1)
			port = UHCI_PORTSC1;
		elif (index == 2)
			port = UHCI_PORTSC2;
		else
			{ err = USBD_IOERROR; goto ret; }

		if (len != 4)
			{ err = USBD_IOERROR; goto ret; }

		x = uhci_read_port_or_mem_short (sc, port);
		status = change = 0;

		if (x & UHCI_PORTSC_CCS)
			status |= UPS_CURRENT_CONNECT_STATUS;
		if (x & UHCI_PORTSC_CSC)
			change |= UPS_C_CONNECT_STATUS;
		if (x & UHCI_PORTSC_PE)
			status |= UPS_PORT_ENABLED;
		if (x & UHCI_PORTSC_POEDC)
			change |= UPS_C_PORT_ENABLED;
		if (x & UHCI_PORTSC_OCI)
			status |= UPS_OVERCURRENT_INDICATOR;
		if (x & UHCI_PORTSC_OCIC)
			change |= UPS_C_OVERCURRENT_INDICATOR;
		if (x & UHCI_PORTSC_SUSP)
			status |= UPS_SUSPEND;
		if (x & UHCI_PORTSC_LSDA)
			status |= UPS_LOW_SPEED;

		status |= UPS_PORT_POWER;

		if (sc->sc_isreset)
			change |= UPS_C_PORT_RESET;

		USETW (ps.wPortStatus, status);
		USETW (ps.wPortChange, change);
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
		if   (index == 1)
			port = UHCI_PORTSC1;
		elif (index == 2)
			port = UHCI_PORTSC2;
		else
			{ err = USBD_IOERROR; goto ret; }

		switch (value)
		{
		    case UHF_PORT_ENABLE:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_PE, port);
			break;

		    case UHF_PORT_SUSPEND:
			x = URWMASK (uhci_read_port_or_mem_short (sc, port));
			uhci_write_port_or_mem_short (sc, x | UHCI_PORTSC_SUSP, port);
			break;

		    case UHF_PORT_RESET:
			err = uhci_portreset (sc, index);
			goto ret;

			/* Pretend we turned on power */
		    case UHF_PORT_POWER:
			err = USBD_NORMAL_COMPLETION;
			goto ret;

		    case UHF_C_PORT_CONNECTION:
		    case UHF_C_PORT_ENABLE:
		    case UHF_C_PORT_OVER_CURRENT:
		    case UHF_PORT_CONNECTION:
		    case UHF_PORT_OVER_CURRENT:
		    case UHF_PORT_LOW_SPEED:
		    case UHF_C_PORT_SUSPEND:
		    case UHF_C_PORT_RESET:
		    default:
			err = USBD_IOERROR;
			goto ret;
		}
		break;

	    default:
		err = USBD_IOERROR;
		goto ret;

	}	/* end switch */

	xfer->actlen = totlen;
	err = USBD_NORMAL_COMPLETION;
    ret:
	xfer->status = err;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	usb_transfer_complete (xfer);

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end uhci_root_ctrl_start */

/*
 ****************************************************************
 *	Abort a root control request				*
 ****************************************************************
 */
void
uhci_root_ctrl_abort (struct usbd_xfer *xfer)
{
	/* Nothing to do, all transfers are synchronous */

}	/* end uhci_root_ctrl_abort */

/*
 ****************************************************************
 *	Close the root pipe					*
 ****************************************************************
 */
void
uhci_root_ctrl_close (struct usbd_pipe *pipe_ptr)
{

}	/* end uhci_root_ctrl_close */

/*
 ****************************************************************
 *	Abort a root interrupt request				*
 ****************************************************************
 */
void
uhci_root_intr_abort (struct usbd_xfer *xfer)
{
	struct uhci_softc	*sc = (struct uhci_softc *)xfer->pipe->device->bus;

#if (0)	/*******************************************************/
	usb_uncallout(sc->sc_poll_handle, uhci_poll_hub, xfer);
#endif	/*******************************************************/

	sc->sc_intr_xfer = NULL;

	if (xfer->pipe->intrxfer == xfer)
	{
#ifdef	USB_MSG
		printf ("uhci_root_intr_abort: remove\n");
#endif	USB_MSG
		xfer->pipe->intrxfer = 0;
	}

	xfer->status = USBD_CANCELLED;

	usb_transfer_complete (xfer);

}	/* end uhci_root_intr_abort */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_root_ctrl_done (struct usbd_xfer *xfer)
{

}	/* end uhci_root_ctrl_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_root_intr_transfer (struct usbd_xfer *xfer)
{
	int	err;

	if ((err = usb_insert_transfer (xfer)) != NULL)
		return (err);

	return (uhci_root_intr_start (xfer->pipe->first_xfer));

}	/* end uhci_root_intr_transfer */

/*
 ****************************************************************
 *	Start a transfer on the root interrupt pipe		*
 ****************************************************************
 */
int
uhci_root_intr_start (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	struct uhci_softc	*sc = (struct uhci_softc *)pipe_ptr->device->bus;

#ifdef	USB_MSG
	printf ("uhci_root_intr_transfer: xfer=%p len=%d flags=%d\n", xfer, xfer->length, xfer->flags);
#endif	USB_MSG

	if (sc->sc_dying)
		return (USBD_IOERROR);

#if (0)	/*******************************************************/
	sc->sc_ival = MS_TO_TICKS(xfer->pipe->endpoint->edesc->bInterval);
	usb_callout(sc->sc_poll_handle, sc->sc_ival, uhci_poll_hub, xfer);
#endif	/*******************************************************/

	sc->sc_intr_xfer = xfer;

	return (USBD_IN_PROGRESS);

}	/* end uhci_root_intr_start */

/*
 ****************************************************************
 *	Close the root interrupt pipe				*
 ****************************************************************
 */
void
uhci_root_intr_close (struct usbd_pipe *pipe_ptr)
{
	struct uhci_softc *sc = (struct uhci_softc *)pipe_ptr->device->bus;

#if (0)	/*******************************************************/
	usb_uncallout (sc->sc_poll_handle, uhci_poll_hub, sc->sc_intr_xfer);
#endif	/*******************************************************/

	sc->sc_intr_xfer = NULL;

}	/* end uhci_root_intr_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_root_intr_done (struct usbd_xfer *xfer)
{

}	/* end uhci_root_intr_done */

/*
 ****************************************************************
 *	Põe na fila um pedido de transferência de controle	*
 ****************************************************************
 */
int
uhci_device_ctrl_transfer (struct usbd_xfer *xfer)
{
	int	err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (uhci_device_ctrl_start (xfer->pipe->first_xfer));

}	/* end uhci_device_ctrl_transfer */

/*
 ****************************************************************
 *	Inicia um pedido de transferência de controle		*
 ****************************************************************
 */
int
uhci_device_ctrl_start (struct usbd_xfer *xfer)
{
	struct uhci_softc	*sc = (struct uhci_softc *)xfer->pipe->device->bus;
	int			err;

#ifdef	USB_MSG
	printf ("uhci_device_ctrl_start: polling = %d\n", sc->sc_bus.use_polling);
#endif	USB_MSG

	if (sc->sc_dying)
		return (USBD_IOERROR);

	if (err = uhci_device_request (xfer))
		return (err);

#ifdef	USB_POLLING
	if (sc->sc_bus.use_polling)
		uhci_waitintr (sc, xfer);
#endif	USB_POLLING

	return (USBD_IN_PROGRESS);

}	/* end uhci_device_ctrl_start */

/*
 ****************************************************************
 *	Abort a device control request				*
 ****************************************************************
 */
void
uhci_device_ctrl_abort (struct usbd_xfer *xfer)
{
	uhci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end uhci_device_ctrl_abort */

/*
 ****************************************************************
 *	Close a device control pipe				*
 ****************************************************************
 */
void
uhci_device_ctrl_close (struct usbd_pipe *pipe_ptr)
{
}	/* end uhci_device_ctrl_close */

/*
 ****************************************************************
 *	Deallocate request data structures			*
 ****************************************************************
 */
void
uhci_device_ctrl_done (struct usbd_xfer *xfer)
{
	struct uhci_intr_info	*ii = &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_softc	*sc = ii->sc;
	struct uhci_pipe	*upipe = (struct uhci_pipe *)xfer->pipe;

	/* remove from active list */

	if (ii->next_intr != NULL)
		ii->next_intr->prev_intr = ii->prev_intr;

	*ii->prev_intr = ii->next_intr;

	if (upipe->pipe.device->speed == USB_SPEED_LOW)
		uhci_remove_ls_ctrl (sc, upipe->u.ctl.sqh);
	else
		uhci_remove_hs_ctrl (sc, upipe->u.ctl.sqh);

	if (upipe->u.ctl.length != 0)
		uhci_free_std_chain (sc, ii->stdstart->link.std, ii->stdend);

}	/* end uhci_device_ctrl_done */

/*
 ****************************************************************
 *	Põe na fila um pedido normal de transferência		*
 ****************************************************************
 */
int
uhci_device_intr_transfer (struct usbd_xfer *xfer)
{
	int	err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (uhci_device_intr_start (xfer->pipe->first_xfer));

}	/* end uhci_device_intr_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_device_intr_start (struct usbd_xfer *xfer)
{
	struct uhci_pipe	*upipe = (struct uhci_pipe *)xfer->pipe;
	struct usbd_device	*udev = upipe->pipe.device;
	struct uhci_softc	*sc = (struct uhci_softc *)udev->bus;
	struct uhci_intr_info	*ii = &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_soft_td	*data, *dataend;
	struct uhci_soft_qh	*sqh;
	int			err, isread, endpt, i, s;

	if (sc->sc_dying)
		return (USBD_IOERROR);

#ifdef	USB_MSG
	printf ("uhci_device_intr_transfer: xfer=%P len=%d flags=%d\n", xfer, xfer->length, xfer->flags);
#endif	USB_MSG

	endpt	= upipe->pipe.endpoint->edesc->bEndpointAddress;
	isread	= UE_GET_DIR(endpt) == UE_DIR_IN;

	upipe->u.intr.isread = isread;

	if (err = uhci_alloc_std_chain (upipe, sc, xfer->length, isread, xfer->flags, &xfer->dmabuf, &data, &dataend))
		return (err);

	dataend->td.td_status |= UHCI_TD_IOC;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	/* Set up interrupt info */

	ii->xfer	= xfer;
	ii->stdstart	= data;
	ii->stdend	= dataend;

#ifdef	USB_MSG
	printf ("uhci_device_intr_transfer: qhs[0]=%p\n", upipe->u.intr.qhs[0]);
#endif	USB_MSG

	for (i = 0; i < upipe->u.intr.npoll; i++)
	{
		sqh		= upipe->u.intr.qhs[i];
		sqh->elink	= data;
		sqh->qh.qh_elink = data->physaddr | UHCI_PTR_TD;
	}

	/* add to active list */

	if ((ii->next_intr = sc->sc_intrhead) != NULL)
		sc->sc_intrhead->prev_intr = &ii->next_intr;

	sc->sc_intrhead = ii; ii->prev_intr = &sc->sc_intrhead;

	xfer->status = USBD_IN_PROGRESS;

	splx (s);

	return (USBD_IN_PROGRESS);

}	/* end uhci_device_intr_start */

/*
 ****************************************************************
 *	Abort a device interrupt request			*
 ****************************************************************
 */
void
uhci_device_intr_abort (struct usbd_xfer *xfer)
{
	if (xfer->pipe->intrxfer == xfer)
	{
#ifdef	USB_MSG
		printf ("uhci_device_intr_abort: remove\n");
#endif	USB_MSG
		xfer->pipe->intrxfer = NULL;
	}

	uhci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end uhci_device_intr_abort */

/*
 ****************************************************************
 *	Close a device interrupt pipe				*
 ****************************************************************
 */
void
uhci_device_intr_close (struct usbd_pipe *pipe_ptr)
{
	struct uhci_pipe	*upipe = (struct uhci_pipe *)pipe_ptr;
	struct uhci_softc	*sc = (struct uhci_softc *)pipe_ptr->device->bus;
	int			i, npoll;
	int			s;

	/* Unlink descriptors from controller data structures */

	npoll = upipe->u.intr.npoll;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	for (i = 0; i < npoll; i++)
		uhci_remove_intr (sc, upipe->u.intr.qhs[i]);

	splx (s);

	/*
	 *	We now have to wait for any activity on the physical descriptors to stop.
	 */
	usb_delay_ms (&sc->sc_bus, 2);

	for (i = 0; i < npoll; i++)
		uhci_free_sqh (sc, upipe->u.intr.qhs[i]);

	free_byte (upipe->u.intr.qhs);

	/* XXX free other resources */

}	/* end uhci_device_intr_close */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_device_intr_done (struct usbd_xfer *xfer)
{
	struct uhci_intr_info		*ii = &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_softc		*sc = ii->sc;
	struct uhci_pipe		*upipe = (struct uhci_pipe *)xfer->pipe;
	struct uhci_soft_qh		*sqh;
	int				i, npoll;

#ifdef	USB_MSG
	printf ("uhci_intr_done: length=%d\n", xfer->actlen);
#endif	USB_MSG

	npoll = upipe->u.intr.npoll;

	for (i = 0; i < npoll; i++)
	{
		sqh		 = upipe->u.intr.qhs[i];
		sqh->elink	 = NULL;
		sqh->qh.qh_elink = UHCI_PTR_T;
	}

	uhci_free_std_chain (sc, ii->stdstart, NULL);

	/* XXX Wasteful */

	if (xfer->pipe->repeat)
	{
		struct uhci_soft_td	*data, *dataend;

#ifdef	USB_MSG
		printf ("uhci_device_intr_done: requeing\n");
#endif	USB_MSG

		/* This alloc cannot fail since we freed the chain above */

		uhci_alloc_std_chain
		(	upipe, sc, xfer->length, upipe->u.intr.isread,
			xfer->flags, &xfer->dmabuf, &data, &dataend
		);

		dataend->td.td_status |= UHCI_TD_IOC;

		ii->stdstart = data;
		ii->stdend = dataend;

		for (i = 0; i < npoll; i++)
		{
			sqh = upipe->u.intr.qhs[i];
			sqh->elink = data;
			sqh->qh.qh_elink = data->physaddr | UHCI_PTR_TD;
		}

		xfer->status = USBD_IN_PROGRESS;

		/* The ii is already on the examined list, just leave it */

	}
	else
	{
#ifdef	USB_MSG
		printf ("uhci_device_intr_done: removing\n");
#endif	USB_MSG

		/* Remove from active list */

		if (ii->next_intr != NULL)
			ii->next_intr->prev_intr = ii->prev_intr;
	
		*ii->prev_intr = ii->next_intr;
	}

}	/* end uhci_device_intr_done */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_device_clear_toggle (struct usbd_pipe *pipe_ptr)
{
	struct uhci_pipe	*upipe = (struct uhci_pipe *)pipe_ptr;

	upipe->nexttoggle = 0;

}	/* end uhci_device_clear_toggle */

/*
 ****************************************************************
 *	Põe na fila um pedido de transferência maciça		*
 ****************************************************************
 */
int
uhci_device_bulk_transfer (struct usbd_xfer *xfer)
{
	int	err;

	if (err = usb_insert_transfer (xfer))
		return (err);

	return (uhci_device_bulk_start (xfer->pipe->first_xfer));

}	/* end uhci_device_bulk_transfer */

/*
 ****************************************************************
 *	Inicia uma transferência maciça				*
 ****************************************************************
 */
int
uhci_device_bulk_start (struct usbd_xfer *xfer)
{
	struct uhci_pipe		*upipe	= (struct uhci_pipe *)xfer->pipe;
	struct usbd_device		*udev	= upipe->pipe.device;
	struct uhci_softc		*sc	= (struct uhci_softc *)udev->bus;
	struct uhci_intr_info		*ii	= &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_soft_td		*data, *dataend;
	struct uhci_soft_qh		*sqh;
	int				err, s;
	int				len, isread, endpt;

#ifdef	USB_MSG
	printf ("uhci_device_bulk_transfer: xfer=%p len=%d flags=%d\n", xfer, xfer->length, xfer->flags);
#endif	USB_MSG

	if (sc->sc_dying)
		return (USBD_IOERROR);

	len	= xfer->length;
	endpt	= upipe->pipe.endpoint->edesc->bEndpointAddress;
	isread	= UE_GET_DIR (endpt) == UE_DIR_IN;
	sqh	= upipe->u.bulk.sqh;

	upipe->u.bulk.isread = isread;
	upipe->u.bulk.length = len;

	err = uhci_alloc_std_chain (upipe, sc, len, isread, xfer->flags, &xfer->dmabuf, &data, &dataend);

	if (err)
		return (err);

	dataend->td.td_status |= UHCI_TD_IOC;

	/* Set up interrupt info */

	ii->xfer	= xfer;
	ii->stdstart	= data;
	ii->stdend	= dataend;

	sqh->elink	 = data;
	sqh->qh.qh_elink = data->physaddr | UHCI_PTR_TD;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	uhci_add_bulk (sc, sqh);

	/* add to active list */

	if ((ii->next_intr = sc->sc_intrhead) != NULL)
		sc->sc_intrhead->prev_intr = &ii->next_intr;

	sc->sc_intrhead = ii; ii->prev_intr = &sc->sc_intrhead;

#if (0)	/*******************************************************/
	if (xfer->timeout && !sc->sc_bus.use_polling) {
		usb_callout(xfer->timeout_handle, MS_TO_TICKS(xfer->timeout),
			    uhci_timeout, ii);
	}
#endif	/*******************************************************/

	xfer->status = USBD_IN_PROGRESS;

	splx (s);

#ifdef	USB_POLLING
	if (sc->sc_bus.use_polling)
		uhci_waitintr (sc, xfer);
#endif	USB_POLLING

	return (USBD_IN_PROGRESS);

}	/* end uhci_device_bulk_start */

/*
 ****************************************************************
 *	Abort a device bulk request				*
 ****************************************************************
 */
void
uhci_device_bulk_abort (struct usbd_xfer *xfer)
{
	uhci_abort_xfer (xfer, USBD_CANCELLED);

}	/* end uhci_device_bulk_abort */

/*
 ****************************************************************
 *	Close a device bulk pipe				*
 ****************************************************************
 */
void
uhci_device_bulk_close (struct usbd_pipe *pipe_ptr)
{
	struct uhci_pipe	*upipe	= (struct uhci_pipe *)pipe_ptr;
	struct usbd_device	*udev	= upipe->pipe.device;
	struct uhci_softc	*sc	= (struct uhci_softc *)udev->bus;

	uhci_free_sqh (sc, upipe->u.bulk.sqh);
	pipe_ptr->endpoint->savedtoggle = upipe->nexttoggle;

}	/* end uhci_device_bulk_close */

/*
 ****************************************************************
 *	Deallocate request data structures			*
 ****************************************************************
 */
void
uhci_device_bulk_done (struct usbd_xfer *xfer)
{
	struct uhci_intr_info	*ii	= &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_softc	*sc	= ii->sc;
	struct uhci_pipe	*upipe	= (struct uhci_pipe *)xfer->pipe;

	/* Remove from active list */

	if (ii->next_intr != NULL)
		ii->next_intr->prev_intr = ii->prev_intr;

	*ii->prev_intr = ii->next_intr;

	uhci_remove_bulk (sc, upipe->u.bulk.sqh);

	uhci_free_std_chain (sc, ii->stdstart, NULL);

#ifdef	USB_MSG
	printf ("uhci_bulk_done: length=%d\n", xfer->actlen);
#endif	USB_MSG

}	/* end uhci_device_bulk_done */

/*
 ****************************************************************
 *	Add bulk QH, called at splusb()				*
 ****************************************************************
 */
void
uhci_add_bulk (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh	*eqh;

	eqh		 = sc->sc_bulk_end;
	sqh->hlink	 = eqh->hlink;
	sqh->qh.qh_hlink = eqh->qh.qh_hlink;
	eqh->hlink	 = sqh;
	eqh->qh.qh_hlink = sqh->physaddr | UHCI_PTR_QH;
	sc->sc_bulk_end  = sqh;

	uhci_add_loop (sc);

}	/* end uhci_add_bulk */

/*
 ****************************************************************
 *	Remove bulk QH, called at splusb()			*
 ****************************************************************
 */
void
uhci_remove_bulk (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh	*pqh;

	uhci_rem_loop (sc);

	/* See comment in uhci_remove_hs_ctrl() */

	if ((sqh->qh.qh_elink & UHCI_PTR_T) == 0)
	{
		sqh->qh.qh_elink = UHCI_PTR_T;

		DELAY (UHCI_QH_REMOVE_DELAY);
	}

	pqh		 = uhci_find_prev_qh (sc->sc_bulk_start, sqh);
	pqh->hlink       = sqh->hlink;
	pqh->qh.qh_hlink = sqh->qh.qh_hlink;

	DELAY (UHCI_QH_REMOVE_DELAY);

	if (sc->sc_bulk_end == sqh)
		sc->sc_bulk_end = pqh;

}	/* end uhci_remove_bulk */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 *
 * Let the last QH loop back to the high speed control transfer QH.
 * This is what intel calls "bandwidth reclamation" and improves
 * USB performance a lot for some devices.
 * If we are already looping, just count it.
 */
void
uhci_add_loop (struct uhci_softc *sc)
{
	/* Note, we don't loop back the soft pointer */

	if (++sc->sc_loops == 1)
		sc->sc_last_qh->qh.qh_hlink = sc->sc_hctl_start->physaddr | UHCI_PTR_QH;

}	/* end uhci_add_loop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_rem_loop (struct uhci_softc *sc)
{
	if (--sc->sc_loops == 0)
		sc->sc_last_qh->qh.qh_hlink = UHCI_PTR_T;

}	/* end uhci_rem_loop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_noop (struct usbd_pipe *pipe_ptr)
{
}	/* end uhci_noop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_setup_isoc (struct usbd_pipe *pipe_ptr)
{
	struct uhci_pipe	*upipe	= (struct uhci_pipe *)pipe_ptr;
	struct usbd_device	*udev	= upipe->pipe.device;
	struct uhci_softc	*sc	= (struct uhci_softc *)udev->bus;
	int			addr	= upipe->pipe.device->address;
	int			endpt	= upipe->pipe.endpoint->edesc->bEndpointAddress;
	int			rd	= UE_GET_DIR (endpt) == UE_DIR_IN;
	struct uhci_soft_td	*std, *vstd;
	unsigned		token;
	struct iso		*iso;
	int			i, s;

	iso = &upipe->u.iso;
	iso->stds = malloc_byte (UHCI_VFRAMELIST_COUNT * sizeof (struct uhci_soft_td *));

	token = rd ? UHCI_TD_IN (0, endpt, addr, 0) : UHCI_TD_OUT(0, endpt, addr, 0);

	/* Allocate the TDs and mark as inactive; */

	for (i = 0; i < UHCI_VFRAMELIST_COUNT; i++)
	{
		if ((std = uhci_alloc_std (sc)) == NULL)
			goto bad;

		std->td.td_status = UHCI_TD_IOS; /* iso, inactive */
		std->td.td_token = token;
		iso->stds[i] = std;
	}

	/* Insert TDs into schedule */

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	for (i = 0; i < UHCI_VFRAMELIST_COUNT; i++)
	{
		std = iso->stds[i];
		vstd = sc->sc_vframes[i].htd;
		std->link = vstd->link;
		std->td.td_link = vstd->td.td_link;
		vstd->link.std = std;
		vstd->td.td_link = (std->physaddr | UHCI_PTR_TD);
	}

	splx (s);

	iso->next  = -1;
	iso->inuse = 0;

	return (USBD_NORMAL_COMPLETION);

	/*
	 *	x
	 */
 bad:
	while (--i >= 0)
		uhci_free_std (sc, iso->stds[i]);

	free_byte (iso->stds);

	return (USBD_NOMEM);

}	/* end uhci_setup_isoc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_device_request (struct usbd_xfer *xfer)
{
	struct uhci_pipe		*upipe	= (struct uhci_pipe *)xfer->pipe;
	struct usb_device_request	*req	= &xfer->request;
	struct usbd_device		*udev	= upipe->pipe.device;
	struct uhci_softc		*sc	= (struct uhci_softc *)udev->bus;
	int				addr	= udev->address;
	int				endpt	= upipe->pipe.endpoint->edesc->bEndpointAddress;
	struct uhci_intr_info		*ii	= &((struct uhci_xfer *)xfer)->iinfo;
	struct uhci_soft_td		*setup, *data, *stat, *next, *dataend;
	struct uhci_soft_qh		*sqh;
	int				len;
	ulong				ls;
	int				err;
	int				isread;
	int				s;

#ifdef	USB_MSG
	printf
	(	"uhci_device_request: type=0x%02x, request=0x%02x, "
		"wValue=0x%04x, wIndex=0x%04x len=%d, addr=%d, endpt=%d\n",
		req->bmRequestType, req->bRequest, UGETW (req->wValue),
		UGETW (req->wIndex), UGETW (req->wLength),
		addr, endpt
	);
#endif	USB_MSG

	ls	= udev->speed == USB_SPEED_LOW ? UHCI_TD_LS : 0;
	isread	= req->bmRequestType & UT_READ;
	len	= UGETW (req->wLength);

	setup	= upipe->u.ctl.setup;
	stat	= upipe->u.ctl.stat;
	sqh	= upipe->u.ctl.sqh;

	/* Set up data transaction */

	if (len != 0)
	{
		upipe->nexttoggle = 1;

		err = uhci_alloc_std_chain
		(	upipe, sc, len, isread, xfer->flags,
			&xfer->dmabuf, &data, &dataend
		);

		if (err)
			return (err);

		next = data;
		dataend->link.std = stat;
		dataend->td.td_link = stat->physaddr | UHCI_PTR_VF | UHCI_PTR_TD;
	}
	else
	{
		next = stat;
	}

	upipe->u.ctl.length = len;

	memmove (upipe->u.ctl.reqdma, req, sizeof (*req));

	setup->link.std		= next;
	setup->td.td_link	= next->physaddr | UHCI_PTR_VF | UHCI_PTR_TD;
	setup->td.td_status	= UHCI_TD_SET_ERRCNT (3) | ls | UHCI_TD_ACTIVE;
	setup->td.td_token	= UHCI_TD_SETUP (sizeof *req, endpt, addr);
	setup->td.td_buffer	= VIRT_TO_PHYS_ADDR (upipe->u.ctl.reqdma);

	stat->link.std		= NULL;
	stat->td.td_link	= UHCI_PTR_T;
	stat->td.td_status	= UHCI_TD_SET_ERRCNT(3) | ls | UHCI_TD_ACTIVE | UHCI_TD_IOC;
	stat->td.td_token	= (isread ? UHCI_TD_OUT (0, endpt, addr, 1) : UHCI_TD_IN (0, endpt, addr, 1));
	stat->td.td_buffer	= 0;

	/* Set up interrupt info */

	ii->xfer	= xfer;
	ii->stdstart	= setup;
	ii->stdend	= stat;

	sqh->elink	 = setup;
	sqh->qh.qh_elink = setup->physaddr | UHCI_PTR_TD;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	if (udev->speed == USB_SPEED_LOW)
		uhci_add_ls_ctrl (sc, sqh);
	else
		uhci_add_hs_ctrl (sc, sqh);

	/* add to active list */

	if ((ii->next_intr = sc->sc_intrhead) != NULL)
		sc->sc_intrhead->prev_intr = &ii->next_intr;

	sc->sc_intrhead = ii; ii->prev_intr = &sc->sc_intrhead;

#if (0)	/*******************************************************/
	if (xfer->timeout && !sc->sc_bus.use_polling)
	{
		usb_callout(xfer->timeout_handle, MS_TO_TICKS(xfer->timeout), uhci_timeout, ii);
	}
#endif	/*******************************************************/

	xfer->status = USBD_IN_PROGRESS;

	splx (s);

	return (USBD_NORMAL_COMPLETION);

}	/* end uhci_device_request */

#ifdef	USB_POLLING
/*
 ****************************************************************
 *	Aguarda a interrupção					*
 ****************************************************************
 */
void
uhci_waitintr (struct uhci_softc *sc, struct usbd_xfer *xfer)
{
	int			timo = xfer->timeout;
	struct uhci_intr_info	*ii;

#ifdef	USB_MSG
	printf ("uhci_waitintr: timeout = %dms\n", timo);
#endif	USB_MSG

	xfer->status = USBD_IN_PROGRESS;

	for (/* acima */; timo >= 0; timo--)
	{
		usb_delay_ms (&sc->sc_bus, 1);

#ifdef	USB_MSG
		printf ("uhci_waitintr: 0x%04X\n", uhci_read_port_or_mem_short (sc, UHCI_STS));
#endif	USB_MSG

		if (uhci_read_port_or_mem_short (sc, UHCI_STS) & UHCI_STS_ALLINTRS)
			uhci_intr1 (sc);

		if (xfer->status != USBD_IN_PROGRESS)
			return;
	}

	/* Timeout */

	printf ("uhci_waitintr: tempo esgotado\n");

	/* Procura na lista ativa */

	for (ii = sc->sc_intrhead; ii != NULL && ii->xfer != xfer; ii = ii->next_intr)
		/* vazio */;

	uhci_idone (ii);

}	/* end uhci_waitintr */
#endif	USB_POLLING

/*
 ****************************************************************
 *	Abort a device request					*
 ****************************************************************
 *
 * If this routine is called at splusb() it guarantees that the request
 * will be removed from the hardware scheduling and that the callback
 * for it will be called with USBD_CANCELLED status.
 * It's impossible to guarantee that the requested transfer will not
 * have happened since the hardware runs concurrently.
 * If the transaction has already happened we rely on the ordinary
 * interrupt processing to process it.
 */
void
uhci_abort_xfer (struct usbd_xfer *xfer, int status)
{
	struct uhci_xfer	*uxfer = (struct uhci_xfer *)xfer;
	struct uhci_intr_info	*ii = &uxfer->iinfo;
	struct uhci_pipe	*upipe = (struct uhci_pipe *)xfer->pipe;
	struct uhci_softc	*sc = (struct uhci_softc *)upipe->pipe.device->bus;
	struct uhci_soft_td	*std;
	int			s;

#ifdef	USB_MSG
	printf ("uhci_abort_xfer: xfer = %P, status = %d\n", xfer, status);
#endif	USB_MSG

	if (sc->sc_dying)
	{
		/* If we're dying, just do the software part */

		s = splx (irq_pl_vec[sc->sc_bus.irq]);

		xfer->status = status;	/* make software ignore it */
#if (0)	/*******************************************************/
		usb_uncallout (xfer->timeout_handle, uhci_timeout, xfer);
#endif	/*******************************************************/
		usb_transfer_complete (xfer);

		splx (s);

		return;
	}

#if (0)	/*******************************************************/
	if (xfer->device->bus->intr_context || !curproc)
		printf ("uhci_abort_xfer: not in process context\n");
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	/*
	 *	If an abort is already in progress then just wait for it to complete and return
	 */
	if (uxfer->uhci_xfer_flags & UHCI_XFER_ABORTING)
	{
		/* No need to wait if we're aborting from a timeout. */

		if (status == USBD_TIMEOUT)
			return;

		/* Override the status which might be USBD_TIMEOUT */

		xfer->status = status;

#if (0)	/*******************************************************/
		uxfer->uhci_xfer_flags |= UHCI_XFER_ABORTWAIT;
#endif	/*******************************************************/

		EVENTWAIT (&uxfer->uhci_xfer_event, PBLKIO);

#if (0)	/*******************************************************/
		while (uxfer->uhci_xfer_flags & UHCI_XFER_ABORTING)
			tsleep(&uxfer->uhci_xfer_flags, PZERO, "uhciaw", 0);
#endif	/*******************************************************/

		return;
	}
#endif	/*******************************************************/

	/*
	 *	Step 1: Make interrupt routine and hardware ignore xfer.
	 */
	s = splx (irq_pl_vec[sc->sc_bus.irq]);

#if (0)	/*******************************************************/
	uxfer->uhci_xfer_flags |= UHCI_XFER_ABORTING;
#endif	/*******************************************************/

	xfer->status = status;	/* make software ignore it */

#if (0)	/*******************************************************/
	usb_uncallout(xfer->timeout_handle, uhci_timeout, ii);
	usb_rem_task(xfer->pipe->device, &UXFER(xfer)->abort_task);
#endif	/*******************************************************/

#ifdef	USB_MSG
	printf ("uhci_abort_xfer: stop ii=%p\n", ii);
#endif	USB_MSG

	for (std = ii->stdstart; std != NULL; std = std->link.std)
		std->td.td_status &= ~(UHCI_TD_ACTIVE | UHCI_TD_IOC);

	splx (s);

	/* 
	 *	Step 2: Wait until we know hardware has finished any possible
	 *	use of the xfer.  Also make sure the soft interrupt routine has run.
	 */
	usb_delay_ms (upipe->pipe.device->bus, 2); /* Hardware finishes in 1ms */

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

	usb_schedsoftintr (&sc->sc_bus);

	splx (s);
		
	/*
	 *	Step 3: Execute callback.
	 */
	xfer->hcpriv = ii;

	s = splx (irq_pl_vec[sc->sc_bus.irq]);

#if (0)	/*******************************************************/
	uxfer->uhci_xfer_flags &= ~UHCI_XFER_ABORTING;
	EVENTDONE (&uxfer->uhci_xfer_event);
#endif	/*******************************************************/

	usb_transfer_complete (xfer);

	splx (s);

}	/* end uhci_abort_xfer */

/*
 ****************************************************************
 *	Add high speed control QH, called at splusb()		*
 ****************************************************************
 */
void
uhci_add_hs_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh	*eqh;

#ifdef	USB_MSG
	printf ("uhci_add_ctrl: sqh = %P\n", sqh);
#endif	USB_MSG

	eqh		 = sc->sc_hctl_end;
	sqh->hlink       = eqh->hlink;
	sqh->qh.qh_hlink = eqh->qh.qh_hlink;
	eqh->hlink       = sqh;
	eqh->qh.qh_hlink = sqh->physaddr | UHCI_PTR_QH;
	sc->sc_hctl_end  = sqh;

#if (0)	/*******************************************************/
#ifdef UHCI_CTL_LOOP
	uhci_add_loop (sc);
#endif
#endif	/*******************************************************/

}	/* end uhci_add_hs_ctrl */

/*
 ****************************************************************
 *	Remove high speed control QH, called at splusb()	*
 ****************************************************************
 */
void
uhci_remove_hs_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh		*pqh;

#ifdef	USB_MSG
	printf ("uhci_remove_hs_ctrl: sqh=%p\n", sqh);
#endif	USB_MSG

#if (0)	/*******************************************************/
#ifdef UHCI_CTL_LOOP
	uhci_rem_loop (sc);
#endif
#endif	/*******************************************************/

	/*
	 * The T bit should be set in the elink of the QH so that the HC
	 * doesn't follow the pointer.  This condition may fail if the
	 * the transferred packet was short so that the QH still points
	 * at the last used TD.
	 * In this case we set the T bit and wait a little for the HC
	 * to stop looking at the TD.
	 */
	if ((sqh->qh.qh_elink & UHCI_PTR_T) == 0)
	{
		sqh->qh.qh_elink = UHCI_PTR_T;

		DELAY (UHCI_QH_REMOVE_DELAY);
	}

	pqh		 = uhci_find_prev_qh (sc->sc_hctl_start, sqh);
	pqh->hlink	 = sqh->hlink;
	pqh->qh.qh_hlink = sqh->qh.qh_hlink;

	DELAY (UHCI_QH_REMOVE_DELAY);

	if (sc->sc_hctl_end == sqh)
		sc->sc_hctl_end = pqh;

}	/* end uhci_remove_hs_ctrl */

/*
 ****************************************************************
 *	Add low speed control QH, called at splusb()		*
 ****************************************************************
 */
void
uhci_add_ls_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh *eqh;

#ifdef	USB_MSG
	printf ("uhci_add_ls_ctrl: sqh=%p\n", sqh);
#endif	USB_MSG

	eqh			= sc->sc_lctl_end;
	sqh->hlink		= eqh->hlink;
	sqh->qh.qh_hlink	= eqh->qh.qh_hlink;
	eqh->hlink		= sqh;
	eqh->qh.qh_hlink	= sqh->physaddr | UHCI_PTR_QH;
	sc->sc_lctl_end		= sqh;

}	/* end uhci_add_ls_ctrl */

/*
 ****************************************************************
 *	Remove low speed control QH, called at splusb		*
 ****************************************************************
 */
void
uhci_remove_ls_ctrl (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	struct uhci_soft_qh		*pqh;

#ifdef	USB_MSG
	printf ("uhci_remove_ls_ctrl: sqh=%p\n", sqh);
#endif	USB_MSG

	/* See comment in uhci_remove_hs_ctrl() */

	if ((sqh->qh.qh_elink & UHCI_PTR_T) == 0)
	{
		sqh->qh.qh_elink = UHCI_PTR_T;

		DELAY (UHCI_QH_REMOVE_DELAY);
	}

	pqh			= uhci_find_prev_qh(sc->sc_lctl_start, sqh);
	pqh->hlink		= sqh->hlink;
	pqh->qh.qh_hlink	= sqh->qh.qh_hlink;

	DELAY (UHCI_QH_REMOVE_DELAY);

	if (sc->sc_lctl_end == sqh)
		sc->sc_lctl_end = pqh;

}	/* end uhci_remove_ls_ctrl */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_free_std_chain (struct uhci_softc *sc, struct uhci_soft_td *std, struct uhci_soft_td *stdend)
{
	struct uhci_soft_td	*p;

	for (/* acima */; std != stdend; std = p)
	{
		p = std->link.std;

		uhci_free_std (sc, std);
	}

}	/* end uhci_free_std_chain */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_alloc_std_chain
(	struct uhci_pipe *upipe, struct uhci_softc *sc, int len, int isread, ushort flags,
	void **dma, struct uhci_soft_td **sp, struct uhci_soft_td **ep
)
{
	struct uhci_soft_td	*p, *lastp;
	ulong			lastlink;
	int			i, ntd, l, tog, maxp;
	ulong			status;
	int			addr = upipe->pipe.device->address;
	int			endpt = upipe->pipe.endpoint->edesc->bEndpointAddress;

#ifdef	USB_MSG
	printf
	(	"uhci_alloc_std_chain: addr=%d endpt=%d len=%d speed=%d flags=0x%x\n",
		addr, UE_GET_ADDR(endpt), len, upipe->pipe.device->speed, flags
	);
#endif	USB_MSG

	if ((maxp = UGETW (upipe->pipe.endpoint->edesc->wMaxPacketSize)) == 0)
	{
		printf ("uhci_alloc_std_chain: maxp=0\n");
		return (USBD_INVAL);
	}

	ntd = (len + maxp - 1) / maxp;

	if ((flags & USBD_FORCE_SHORT_XFER) && len % maxp == 0)
		ntd++;

#ifdef	USB_MSG
	printf ("uhci_alloc_std_chain: maxp=%d ntd=%d\n", maxp, ntd);
#endif	USB_MSG

	if (ntd == 0)
	{
		*sp = *ep = 0;
		printf ("uhci_alloc_std_chain: ntd=0\n");
		return (USBD_NORMAL_COMPLETION);
	}

	tog = upipe->nexttoggle;

	if (ntd % 2 == 0)
		tog ^= 1;

	upipe->nexttoggle = tog ^ 1;
	lastp		  = NULL;
	lastlink	  = UHCI_PTR_T;
	ntd--;

	status = UHCI_TD_ZERO_ACTLEN (UHCI_TD_SET_ERRCNT (3) | UHCI_TD_ACTIVE);

	if (upipe->pipe.device->speed == USB_SPEED_LOW)
		status |= UHCI_TD_LS;

	if (flags & USBD_SHORT_XFER_OK)
		status |= UHCI_TD_SPD;

	for (i = ntd; i >= 0; i--)
	{
		if ((p = uhci_alloc_std (sc)) == NULL)
		{
			uhci_free_std_chain (sc, lastp, NULL);
			return (USBD_NOMEM);
		}

		p->link.std	= lastp;
		p->td.td_link	= lastlink | UHCI_PTR_VF | UHCI_PTR_TD;
		lastp		= p;
		lastlink	= p->physaddr;
		p->td.td_status	= status;

		if (i == ntd)
		{
			/* last TD */

			l = len % maxp;

			if (l == 0 && !(flags & USBD_FORCE_SHORT_XFER))
				l = maxp;

			*ep = p;
		}
		else
		{
			l = maxp;
		}

		p->td.td_token = isread ? UHCI_TD_IN (l, endpt, addr, tog) : UHCI_TD_OUT (l, endpt, addr, tog);

		p->td.td_buffer = VIRT_TO_PHYS_ADDR (*dma + i * maxp);

		tog ^= 1;
	}

	*sp = lastp;

#ifdef	USB_MSG
	printf ("uhci_alloc_std_chain: nexttog=%d\n", upipe->nexttoggle);
#endif	USB_MSG

	return (USBD_NORMAL_COMPLETION);

}	/* end uhci_alloc_std_chain */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct uhci_soft_qh *
uhci_find_prev_qh (struct uhci_soft_qh *pqh, struct uhci_soft_qh *sqh)
{
#ifdef	USB_MSG
	printf ("uhci_find_prev_qh: pqh=%p sqh=%p\n", pqh, sqh);
#endif	USB_MSG

	for (/* acima */; pqh->hlink != sqh; pqh = pqh->hlink)
		/* vazio */;

	return (pqh);

}	/* end uhci_find_prev_qh */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
const char	*const usbd_error_strs[] =
{
	"NORMAL_COMPLETION",
	"IN_PROGRESS",
	"PENDING_REQUESTS",
	"NOT_STARTED",
	"INVAL",
	"NOMEM",
	"CANCELLED",
	"BAD_ADDRESS",
	"IN_USE",
	"NO_ADDR",
	"SET_ADDR_FAILED",
	"NO_POWER",
	"TOO_DEEP",
	"IOERROR",
	"NOT_CONFIGURED",
	"TIMEOUT",
	"SHORT_XFER",
	"STALLED",
	"INTERRUPTED",
	"XXX",
};

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
const char *
usbd_errstr (int err)
{
	static char buffer[5];

	if (err < USBD_ERROR_MAX)
	{
		return usbd_error_strs[err];
	}
	else
	{
		snprintf (buffer, sizeof buffer, "%d", err);
		return (buffer);
	}

}	/* end usbd_errstr */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Aloca um descritor de transferências			*
 ****************************************************************
 *
 *
 * Memory management routines.
 *  uhci_alloc_std allocates TDs
 *  uhci_alloc_sqh allocates QHs
 * These two routines do their own free list management,
 * partly for speed, partly because allocating DMAable memory
 * has page size granularaity so much memory would be wasted if
 * only one TD/QH (32 bytes) was placed in each allocated chunk.
 */
struct uhci_soft_td *
uhci_alloc_std (struct uhci_softc *sc)
{
	struct uhci_soft_td	*std;
	int			i, offs, byte_size;
	void			*virt_addr;

	if (sc->sc_freetds == NULL)
	{
		byte_size = UHCI_STD_SIZE * UHCI_STD_CHUNK;

		if ((virt_addr = (void *)PGTOBY (malloce (M_CORE, BYUPPG (byte_size)))) == NULL)
			return (NULL);

		for (i = 0; i < UHCI_STD_CHUNK; i++)
		{
			offs		= i * UHCI_STD_SIZE;
			std		= (struct uhci_soft_td *)(virt_addr + offs);
			std->physaddr	= VIRT_TO_PHYS_ADDR (std);
			std->link.std	= sc->sc_freetds;
			sc->sc_freetds	= std;
		}
	}

	std		= sc->sc_freetds;
	sc->sc_freetds	= std->link.std;

	memclr (&std->td, sizeof (struct uhci_td));

	return (std);

}	/* end uhci_alloc_std */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_free_std (struct uhci_softc *sc, struct uhci_soft_td *std)
{
	std->link.std = sc->sc_freetds;
	sc->sc_freetds = std;

}	/* end uhci_free_std */

/*
 ****************************************************************
 *	Aloca um cabeçalho de fila				*
 ****************************************************************
 */
struct uhci_soft_qh *
uhci_alloc_sqh (struct uhci_softc *sc)
{
	struct uhci_soft_qh	*sqh;
	int			i, offs, byte_size;
	void			*virt_addr;

	if (sc->sc_freeqhs == NULL)
	{
		byte_size = UHCI_SQH_SIZE * UHCI_SQH_CHUNK;

		if ((virt_addr = (void *)PGTOBY (malloce (M_CORE, BYUPPG (byte_size)))) == NULL)
			return (NULL);

		for (i = 0; i < UHCI_SQH_CHUNK; i++)
		{
			offs		= i * UHCI_SQH_SIZE;
			sqh		= (struct uhci_soft_qh  *)(virt_addr + offs);
			sqh->physaddr	= VIRT_TO_PHYS_ADDR (sqh);
			sqh->hlink	= sc->sc_freeqhs;
			sc->sc_freeqhs	= sqh;
		}
	}

	sqh		= sc->sc_freeqhs;
	sc->sc_freeqhs	= sqh->hlink;

	memclr (&sqh->qh, sizeof (struct uhci_qh));

	return (sqh);

}	/* end uhci_alloc_sqh */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_free_sqh (struct uhci_softc *sc, struct uhci_soft_qh *sqh)
{
	sqh->hlink	= sc->sc_freeqhs;
	sc->sc_freeqhs	= sqh;

}	/* end uhci_free_sqh */

/*
 ****************************************************************
 *	Rotina de Interrupção					*
 ****************************************************************
 */
void
uhci_intr (struct intr_frame frame)
{
	struct uhci_softc	*sc = usb_data[frame.if_unit].usb_data_ptr;

	if (sc == NULL || sc->sc_dying)
		return;

#ifdef	USB_MSG
	printf ("uhci_intr: real interrupt\n");
#endif	USB_MSG

#ifdef	USB_POLLING
	if (sc->sc_bus.use_polling)
	{
		printf ("uhci_intr: ignored interrupt while polling\n");
		return;
	}
#endif	USB_POLLING

	uhci_intr1 (sc);

}	/* end uhci_intr */

/*
 ****************************************************************
 *	Rotina auxiliar de interrupção				*
 ****************************************************************
 */
void
uhci_intr1 (struct uhci_softc *sc)
{
	int		status, ack;

#if (0)	/*******************************************************/
	/*
	 *	Se for prematuro, desliga as interrupções
	 */
	if (sc->sc_bus.bdev == NULL)
	{
		uhci_write_port_or_mem_short (sc, 0xFFFF, UHCI_STS);	/* ack pending interrupts */
		uhci_run (sc, 0);					/* stop the controller */
		uhci_write_port_or_mem_short (sc, 0, UHCI_INTR);	/* disable interrupts */
		return;
	}
#endif	/*******************************************************/

	/*
	 *	Verifica se a interrupção é realmente para este controlador
	 */
	if ((status = uhci_read_port_or_mem_short (sc, UHCI_STS) & UHCI_STS_ALLINTRS) == 0)
		return;

#ifdef	USB_MSG
	printf ("uhci_intr1: Veio interrupção, status = %04X\n", status);
#endif	USB_MSG

	if (sc->sc_suspend != PWR_RESUME)
	{
		printf ("usb%d:	Interrupt while not operating ignored\n", sc->sc_bus.unit);
		uhci_write_port_or_mem_short (sc, status, UHCI_STS);		/* acknowledge the ints */
		return;
	}

	/*
	 *	Examina o estado
	 */
	ack = 0;

	if (status & UHCI_STS_USBINT)
		ack |= UHCI_STS_USBINT;

	if (status & UHCI_STS_USBEI)
		ack |= UHCI_STS_USBEI;

	if (status & UHCI_STS_RD)
	{
		ack |= UHCI_STS_RD;
		printf ("usb%d: Resume detect\n", sc->sc_bus.unit);
	}

	if (status & UHCI_STS_HSE)
	{
		ack |= UHCI_STS_HSE;
		printf ("usb%d: Host system error\n", sc->sc_bus.unit);
	}

	if (status & UHCI_STS_HCPE)
	{
		ack |= UHCI_STS_HCPE;
		printf ("usb%d: Host controller process error\n", sc->sc_bus.unit);
	}

	if (status & UHCI_STS_HCH)
	{
		/* no acknowledge needed */

		if (!sc->sc_dying)
			printf ("usb%d: Host controller halted\n", sc->sc_bus.unit);

		sc->sc_dying = 1;
	}

	if (!ack)
		return;		/* nada a confirmar */

	uhci_write_port_or_mem_short (sc, ack, UHCI_STS);		/* Dá o ACK */

	sc->sc_bus.no_intrs++;

	usb_schedsoftintr (&sc->sc_bus);

}	/* end uhci_intr1 */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_softintr (void *v)
{
	struct uhci_softc	*sc = v;
	struct uhci_intr_info 	*ii;

	sc->sc_bus.intr_context++;

	/*
	 *	Interrupts on UHCI really suck.  When the host controller
	 *	interrupts because a transfer is completed there is no
	 *	way of knowing which transfer it was.  You can scan down
	 *	the TDs and QHs of the previous frame to limit the search,
	 *	but that assumes that the interrupt was not delayed by more
	 *	than 1 ms, which may not always be true (e.g. after debug
	 *	output on a slow console).
	 *	We scan all interrupt descriptors to see if any have
	 *	completed.
	 */
	for (ii = sc->sc_intrhead; ii != NULL; ii = ii->next_intr)
		uhci_check_intr (sc, ii);

	sc->sc_bus.intr_context--;

}	/* end uhci_softintr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_poll (struct usbd_bus *bus)
{
	struct uhci_softc	*sc = (struct uhci_softc *)bus;

	if (uhci_read_port_or_mem_short (sc, UHCI_STS) & UHCI_STS_ALLINTRS)
		uhci_intr1 (sc);

}	/* end uhci_poll */

/*
 ****************************************************************
 *	Check for an interrupt					*
 ****************************************************************
 */
void
uhci_check_intr (struct uhci_softc *sc, struct uhci_intr_info *ii)
{
	struct uhci_soft_td	*std, *lstd;
	ulong			status;

	if (ii->xfer->status == USBD_CANCELLED || ii->xfer->status == USBD_TIMEOUT)
	{
#ifdef	USB_MSG
		printf ("uhci_check_intr: aborted xfer = %P\n", ii->xfer);
#endif	USB_MSG
		return;
	}

	if (ii->stdstart == NULL)
		return;

	lstd = ii->stdend;

	/*
	 * If the last TD is still active we need to check whether there
	 * is a an error somewhere in the middle, or whether there was a
	 * short packet (SPD and not ACTIVE).
	 */
	if (lstd->td.td_status & UHCI_TD_ACTIVE)
	{
#ifdef	USB_MSG
		printf ("uhci_check_intr: active ii = %P\n", ii);
#endif	USB_MSG

		for (std = ii->stdstart; std != lstd; std = std->link.std)
		{
			status = std->td.td_status;

			/* If there's an active TD the xfer isn't done */

			if (status & UHCI_TD_ACTIVE)
				break;

			/* Any kind of error makes the xfer done */

			if (status & UHCI_TD_STALLED)
				goto done;

			/* We want short packets, and it is short: it's done */

			if
			(	(status & UHCI_TD_SPD) &&
				UHCI_TD_GET_ACTLEN (status) < UHCI_TD_GET_MAXLEN (std->td.td_token)
			)
				goto done;
		}

#ifdef	USB_MSG
		printf ("uhci_check_intr: ii = %P std = %P still active\n", ii, ii->stdstart);
#endif	USB_MSG

		return;
	}

	/*
	 *	x
	 */
    done:
#ifdef	USB_MSG
	printf ("uhci_check_intr: ii= %P done\n", ii);
#endif	USB_MSG

#if (0)	/*******************************************************/
	usb_uncallout(ii->xfer->timeout_handle, uhci_timeout, ii);
	usb_rem_task(ii->xfer->pipe->device, &UXFER(ii->xfer)->abort_task);
#endif	/*******************************************************/

	uhci_idone (ii);

}	/* end uhci_check_intr */

/*
 ****************************************************************
 *	Called at splusb					*
 ****************************************************************
 */
void
uhci_idone (struct uhci_intr_info *ii)
{
	struct usbd_xfer	*xfer = ii->xfer;
	struct uhci_pipe	*upipe = (struct uhci_pipe *)xfer->pipe;
	struct uhci_soft_td	*std;
	ulong			status	= 0, nstatus;
	int			actlen;

#ifdef	USB_MSG
	printf ("uhci_idone: ii = %P\n", ii);
#endif	USB_MSG

	if (xfer->nframes != 0)
	{
		/* Isoc transfer, do things differently */

		struct uhci_soft_td	**stds = upipe->u.iso.stds;
		int			i, n, nframes, len;

#ifdef	USB_MSG
		printf ("uhci_idone: ii = %P isoc ready\n", ii);
#endif	USB_MSG

		nframes	= xfer->nframes;
		actlen	= 0;
		n	= ((struct uhci_xfer *)xfer)->curframe;

		for (i = 0; i < nframes; i++)
		{
			std = stds[n];

			if (++n >= UHCI_VFRAMELIST_COUNT)
				n = 0;

			status		   = std->td.td_status;
			len		   = UHCI_TD_GET_ACTLEN(status);
			xfer->frlengths[i] = len;
			actlen		  += len;
		}

		upipe->u.iso.inuse -= nframes;
		xfer->actlen	    = actlen;
		xfer->status	    = USBD_NORMAL_COMPLETION;

		goto end;
	}

	/*
	 *	Calcula o tamanho efetivamente transferido
	 */
	actlen = 0;

	for (std = ii->stdstart; std != NULL; std = std->link.std)
	{
		nstatus = std->td.td_status;

		if (nstatus & UHCI_TD_ACTIVE)
			break;

		status = nstatus;

		if (UHCI_TD_GET_PID (std->td.td_token) != UHCI_TD_PID_SETUP)
		{
			actlen += UHCI_TD_GET_ACTLEN (status);
		}
		else
		{
			/*
			 *	UHCI will report CRCTO in addition to a STALL or NAK
			 *	for a SETUP transaction.  See section 3.2.2, "TD
			 *	CONTROL AND STATUS".
			 */
			if (status & (UHCI_TD_STALLED|UHCI_TD_NAK))
				status &= ~UHCI_TD_CRCTO;
		}
	}

	/*
	 *	If there are left over TDs we need to update the toggle
	 */
	if (std != NULL)
		upipe->nexttoggle = UHCI_TD_GET_DT (std->td.td_token);

	status &= UHCI_TD_ERROR;

#ifdef	USB_MSG
	printf ("uhci_idone: actlen = %d, status = 0x%x\n", actlen, status);
#endif	USB_MSG

	xfer->actlen = actlen;

	if (status != 0)
	{
		if (status == UHCI_TD_STALLED)
			xfer->status = USBD_STALLED;
		else
			xfer->status = USBD_IOERROR; /* more info XXX */
	}
	else
	{
		xfer->status = USBD_NORMAL_COMPLETION;
	}

	/*
	 *	x
	 */
    end:
	usb_transfer_complete (xfer);

#ifdef	USB_MSG
	printf ("uhci_idone: ii = %P done\n", ii);
#endif	USB_MSG

}	/* end uhci_idone */

#ifdef	USB_MSG
/*
 ****************************************************************
 *	Le 1 byte de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
uhci_read_port_or_mem_char (const struct uhci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port (up->sc_bus.base_addr + reg));
	else
		return (*(char *)(up->sc_bus.base_addr + reg));

}	/* end uhci_write_port_or_mem_char */
#endif	USB_MSG

/*
 ****************************************************************
 *	Le 2 bytes de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
uhci_read_port_or_mem_short (const struct uhci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port_short (up->sc_bus.base_addr + reg));
	else
		return (*(short *)(up->sc_bus.base_addr + reg));

}	/* end uhci_read_port_or_mem_short */

/*
 ****************************************************************
 *	Escreve 2 bytes em um registro (da porta ou memória)	*
 ****************************************************************
 */
void
uhci_write_port_or_mem_short (const struct uhci_softc *up, int value, int reg)
{
	if (up->sc_bus.port_mapped)
		write_port_short (value, up->sc_bus.base_addr + reg);
	else
		*(short *)(up->sc_bus.base_addr + reg) = value;

}	/* end uhci_write_port_or_mem_short */

#ifdef	USB_MSG
/*
 ****************************************************************
 *	Le 4 bytes de um registro (da porta ou memória)		*
 ****************************************************************
 */
int
uhci_read_port_or_mem_long (const struct uhci_softc *up, int reg)
{
	if (up->sc_bus.port_mapped)
		return (read_port_long (up->sc_bus.base_addr + reg));
	else
		return (*(long *)(up->sc_bus.base_addr + reg));

}	/* end uhci_read_port_or_mem_long */
#endif	USB_MSG

/*
 ****************************************************************
 *	Escreve 4 bytes em um registro (da porta ou memória)	*
 ****************************************************************
 */
void
uhci_write_port_or_mem_long (const struct uhci_softc *up, int value, int reg)
{
	if (up->sc_bus.port_mapped)
		write_port_long (value, up->sc_bus.base_addr + reg);
	else
		*(long *)(up->sc_bus.base_addr + reg) = value;

}	/* end uhci_write_port_or_mem_long */

#ifdef	USB_POLLING
/*
 ****************************************************************
 * This routine is executed periodically and simulates interrupts
 * from the root controller interrupt pipe for port status change.
 ****************************************************************
 */
void	usb_hub_explore (int unit);

void
uhci_poll_hub (int unit)
{
	struct uhci_softc	*sc = usb_data[unit].usb_data_ptr;

	if
	(	(uhci_read_port_or_mem_short (sc, UHCI_PORTSC1) & (UHCI_PORTSC_CSC|UHCI_PORTSC_OCIC)) == 0 &&
		(uhci_read_port_or_mem_short (sc, UHCI_PORTSC2) & (UHCI_PORTSC_CSC|UHCI_PORTSC_OCIC)) == 0
	)
		return;

	usb_hub_explore (unit);

}	/* uhci_poll_hub */
#endif	USB_POLLING

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
uhci_allocm (struct usbd_bus *bus, void **dma, ulong size)
{
	if ((*dma = malloc_byte (size)) == NULL)
	{
		return (USBD_NOMEM);
	}
	else
	{
		memclr (*dma, size);

		return (USBD_NORMAL_COMPLETION);
	}

}	/* end uhci_allocm */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
uhci_freem (struct usbd_bus *bus, void **dma)
{
	free_byte (*dma); *dma = NULL;

}	/* end uhci_freem */
#endif	/*******************************************************/
