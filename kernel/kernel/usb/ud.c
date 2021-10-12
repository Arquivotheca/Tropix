/*
 ****************************************************************
 *								*
 *			ud.c					*
 *								*
 *	"Driver" para Dispositivos de Massa USB			*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.6.0, de 29.09.04				*
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
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/usb.h"

#include "../h/disktb.h"
#include "../h/scsi.h"
#include "../h/kfile.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/devmajor.h"
 
#include "../h/inode.h"
#include "../h/ioctl.h"

#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

#include "usbdevs.h"

/*
 ******	Declaração do Driver ************************************
 */
int	ud_probe (struct device *);
int	ud_attach (struct device *);
int	ud_detach (struct device *);

const struct driver ud_driver =
{
	"ud",

	ud_probe,
	ud_attach,
	ud_detach
};

/*
 ****** Constantes **********************************************
 */
#define UD_TIMEOUT		5000		/* msecs */
#define SHORT_INQUIRY_LENGTH	36 
#define SSS_START		0x01

#define	UD_NRETRY		5

enum						/* Sentido da transferência */
{
	DIR_NONE,
	DIR_IN,
	DIR_OUT
};

/* Bulk-Only features */

#define UR_BBB_RESET		0xFF		/* Bulk-Only reset */
#define UR_BBB_GET_MAX_LUN	0xFE		/* Get maximum lun */

/*
 ****** "Command Block Wrapper" *********************************
 */
#define CBWSIGNATURE		0x43425355

#define CBWFLAGS_OUT		0x00
#define CBWFLAGS_IN		0x80

#define CBWCDBLENGTH		16

typedef struct
{
	uchar		dCBWSignature[4];
	uchar		dCBWTag[4];
	uchar		dCBWDataTransferLength[4];
	uchar		bCBWFlags;
	uchar		bCBWLUN;
	uchar		bCDBLength;
	uchar		CBWCDB[CBWCDBLENGTH];

}	ud_bbb_cbw_t;

#define	UD_BBB_CBW_SIZE	31

/*
 ****** "Command Status Wrapper" ********************************
 */
#define CSWSIGNATURE		0x53425355
#define CSWSIGNATURE_OLYMPUS_C1	0x55425355

#define CSWSTATUS_GOOD		0x0
#define CSWSTATUS_FAILED	0x1
#define CSWSTATUS_PHASE		0x2

typedef struct
{
	uchar		dCSWSignature[4];
	uchar		dCSWTag[4];
	uchar		dCSWDataResidue[4];
	uchar		bCSWStatus;

}	ud_bbb_csw_t;

#define	UD_BBB_CSW_SIZE	13

/*
 ****** "CBI features" ******************************************
 */
#define UR_CBI_ADSC		0x00

#define IDB_TYPE_CCI		0x00
#define IDB_VALUE_PASS		0x00
#define IDB_VALUE_FAIL		0x01
#define IDB_VALUE_PHASE		0x02
#define IDB_VALUE_PERSISTENT	0x03
#define IDB_VALUE_STATUS_MASK	0x03

typedef union
{
	struct
	{
		uchar	type;
		uchar	value;

	}	common;

	struct
	{
		uchar	asc;
		uchar	ascq;

	}	ufi;

}	ud_cbi_sbl_t;

typedef char		ud_cbi_cbl_t[16];

/*
 ****** Protocolos suportados ***********************************
 */
enum					/* Protocolos físicos */
{
	UD_PROTO_BBB		= 1,
	UD_PROTO_CBI		= 2,
	UD_PROTO_CBI_I		= 3
};

enum					/* Protocolos de comandos */
{
	UD_PROTO_SCSI		= 1,
	UD_PROTO_ATAPI		= 2,
	UD_PROTO_UFI		= 3,
	UD_PROTO_RBC		= 4
};

enum	{ ATAPI_COMMAND_LENGTH = 12 };	/* ATAPI commands are always 12 bytes */

/*
 ****** Idiossincrasias *****************************************
 */
#define NO_QUIRKS		0x0000
#define NO_TEST_UNIT_READY	0x0001 	/* The drive does not support Test Unit Ready */
#define RS_NO_CLEAR_UA		0x0002 	/* The drive does not reset the Unit Attention state after REQUEST
					 * SENSE has been sent. The INQUIRY command does not reset the UA
					 * either, and so CAM runs in circles trying to retrieve the initial
					 * INQUIRY data. */
#define NO_START_STOP		0x0004 	/* The drive does not support START STOP.  */
#define FORCE_SHORT_INQUIRY	0x0008 	/* Don't ask for full inquiry data (255b).  */
#define SHUTTLE_INIT		0x0010 	/* Needs to be initialised the Shuttle way */
#define ALT_IFACE_1		0x0020 	/* Drive needs to be switched to alternate iface 1 */
#define FLOPPY_SPEED		0x0040 	/* Drive does not do 1Mb/s, but just floppy speeds (20kb/s) */
#define IGNORE_RESIDUE		0x0080 	/* The device can't count and gets the residual of transfers wrong */
#define NO_GETMAXLUN		0x0100 	/* No GetMaxLun call */
#define WRONG_CSWSIG		0x0200 	/* The device uses a weird CSWSIGNATURE. */
#define NO_INQUIRY		0x0400 	/* Device cannot handle INQUIRY so fake a generic response */
#define NO_INQUIRY_EVPD		0x0800 	/* Device cannot handle INQUIRY EVPD, return CHECK CONDITION */

/*
 ****** Estados da transferência ********************************
 */
enum
{
	TSTATE_ATTACH		= 0,	/* in attach */
	TSTATE_IDLE		= 1,

	TSTATE_BBB_COMMAND	= 2,	/* CBW transfer */
	TSTATE_BBB_DATA		= 3,	/* Data transfer */
	TSTATE_BBB_DCLEAR	= 4,	/* clear endpt stall */
	TSTATE_BBB_STATUS1	= 5,	/* clear endpt stall */
	TSTATE_BBB_SCLEAR	= 6,	/* clear endpt stall */
	TSTATE_BBB_STATUS2	= 7,	/* CSW transfer */
	TSTATE_BBB_RESET1	= 8,	/* reset command */
	TSTATE_BBB_RESET2	= 9,	/* in clear stall */
	TSTATE_BBB_RESET3	= 10,	/* out clear stall */

	TSTATE_CBI_COMMAND	= 11,	/* command transfer */
	TSTATE_CBI_DATA		= 12,	/* data transfer */
	TSTATE_CBI_STATUS	= 13,	/* status transfer */
	TSTATE_CBI_DCLEAR	= 14,	/* clear ep stall */
	TSTATE_CBI_SCLEAR	= 15,	/* clear ep stall */
	TSTATE_CBI_RESET1	= 16,	/* reset command */
	TSTATE_CBI_RESET2	= 17,	/* in clear stall */
	TSTATE_CBI_RESET3	= 18,	/* out clear stall */

	TSTATE_STATES		= 19	/* No. de estados acima */
};

/*
 ****** Índices para o vetor de descritores *********************
 */
enum
{
	XFER_BBB_CBW		= 0,	/* Bulk-Only */
	XFER_BBB_DATA		= 1,
	XFER_BBB_DCLEAR		= 2,
	XFER_BBB_CSW1		= 3,
	XFER_BBB_CSW2		= 4,
	XFER_BBB_SCLEAR		= 5,
	XFER_BBB_RESET1		= 6,
	XFER_BBB_RESET2		= 7,
	XFER_BBB_RESET3		= 8,

	XFER_NR			= 9	/* No. de descritores */
};

enum
{
	XFER_CBI_CB		= 0,	/* CBI */
	XFER_CBI_DATA		= 1,
	XFER_CBI_STATUS		= 2,
	XFER_CBI_DCLEAR		= 3,
	XFER_CBI_SCLEAR		= 4,
	XFER_CBI_RESET1		= 5,
	XFER_CBI_RESET2		= 6,
	XFER_CBI_RESET3		= 7
};

/*
 ****** Resultados dos comandos *********************************
 */
enum
{
	STATUS_CMD_OK		= 0,	/* everything ok */
	STATUS_CMD_UNKNOWN	= 1,	/* will have to fetch sense */
	STATUS_CMD_FAILED	= 2,	/* transfer was ok, command failed */
	STATUS_WIRE_FAILED	= 3	/* couldn't even get command across */
};

/*
 ****** Informações sobre uma unidade ***************************
 */
struct ud_softc;

typedef void			(*sc_xfer_cb_f)
				(	struct ud_softc *sc, void *priv,
					int residual, int status
				);

struct ud_softc
{
	struct device		*sc_dev;		/* base device */
	struct usbd_device	*sc_udev;		/* USB device */

	int			sc_irq;			/* No. do IRQ */

	int			sc_maxlun;		/* No. de unidades lógicas */

	uchar			sc_phys_proto;		/* Protocolo físico */
	uchar			sc_cmd_proto;		/* Protocolo de comandos */
	ushort			sc_quirks;		/* Idiossincrasias desta unidade */

	struct usbd_interface	*sc_iface;		/* interface */
	int			sc_ifaceno;		/* MS iface number */

	SCSI			sc_scsi;		/* Estrutura SCSI */

	DEVHEAD			sc_utab;		/* Cabeça da lista de pedidos */

	DISKTB			*sc_disktb;		/* Ponteiro para a DISKTB */

	LOCK			sc_busy;		/* Unidade ocupada */

	/*
	 *	Os diversos "pipes"
	 */
	char			sc_bulkin;		/* bulk-in Endpoint Address */
	char			sc_bulkout;		/* bulk-out Endpoint Address */
	char			sc_intrin;		/* intr-in Endp. (CBI) */

	struct usbd_pipe	*sc_bulkin_pipe;
	struct usbd_pipe	*sc_bulkout_pipe;
	struct usbd_pipe	*sc_intrin_pipe;

	/*
	 *	Funções de controle e transferência (dependentes do protocolo)
	 */
	void			(*sc_reset) (struct ud_softc *, int);

	void			(*sc_transfer)
				(	struct ud_softc *, int,
					void *, int, void *, int, int, sc_xfer_cb_f, void *
				);

	void			(*sc_state) (struct usbd_xfer *xfer, void *priv, int);

	int			(*sc_transform) (struct ud_softc *, BHEAD *);

	/*
	 *	Descritores de transferência
	 */
	struct usbd_xfer	*sc_xfer[XFER_NR];

	/*
	 *	Bulk specific variables for transfers in progress
	 */
	ud_bbb_cbw_t		cbw;			/* command block wrapper */
	ud_bbb_csw_t		csw;			/* command status wrapper*/

	/*
	 *	CBI specific variables for transfers in progress
	 */
	ud_cbi_cbl_t		cbl;			/* command block */
	ud_cbi_sbl_t		sbl;			/* status block */

	struct usb_device_request	request;

	/*
	 *	Informações sobre a transferência em curso
	 */
	struct scsi_sense_data	sc_sense_data;
	int			sc_sense;
	int			sc_xfer_state;
	int			sc_xfer_dir;		/* data direction */
	void			*sc_xfer_data;		/* data buffer */
	int			sc_xfer_datalen;	/* (maximum) length */
	ulong			sc_xfer_actlen;		/* actual length */
	sc_xfer_cb_f		sc_xfer_cb;		/* callback */
	void			*sc_xfer_priv;		/* for callback */
	int			sc_xfer_status;

	BHEAD			sc_raw_bhead;		/* Para as operações "raw" */
};

/*
 ****** Variáveis globais ***************************************
 */
entry struct devclass	*ud_devclass;
entry DEVHEAD		udtab;				/* Cabeca da lista de dp's e do major */

#ifdef	USB_MSG
entry const char	*ud_states[TSTATE_STATES+1] =
{
	/* should be kept in sync with the list at transfer_state */
	"Attach",
	"Idle",
	"BBB CBW",
	"BBB Data",
	"BBB Data bulk-in/-out clear stall",
	"BBB CSW, 1st attempt",
	"BBB CSW bulk-in clear stall",
	"BBB CSW, 2nd attempt",
	"BBB Reset",
	"BBB bulk-in clear stall",
	"BBB bulk-out clear stall",
	"CBI Command",
	"CBI Data",
	"CBI Status",
	"CBI Data bulk-in/-out clear stall",
	"CBI Status intr-in clear stall",
	"CBI Reset",
	"CBI bulk-in clear stall",
	"CBI bulk-out clear stall",
	NULL
};
#endif	USB_MSG

/*
 ****** Tabela de idiossincrasias *******************************
 */
#define	Q_ANY		0xFFFFFFFF
#define	Q_EOT		0xFFFFFFFE

typedef	struct
{
	ulong		q_vendor;		/* Vendor */
	ulong		q_product;		/* Product */
	ulong		q_release;		/* Release */

	uchar		q_cmd_proto;		/* Protocolo de comandos */
	uchar		q_phys_proto;		/* Protocolo físico */

	ushort		q_quirks;		/* Idiossincasias */

}	UDQUIRKS;

const UDQUIRKS	ud_quirks[] =
{
	{	USB_VENDOR_ASAHIOPTICAL,	Q_ANY,				Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		RS_NO_CLEAR_UA
	},
	{	USB_VENDOR_FUJIPHOTO,		USB_PRODUCT_FUJIPHOTO_MASS0100,	Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		RS_NO_CLEAR_UA
	},
	{	USB_VENDOR_GENESYS,		USB_PRODUCT_GENESYS_GL641USB2IDE, Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		FORCE_SHORT_INQUIRY|NO_START_STOP|IGNORE_RESIDUE
	},
	{	USB_VENDOR_GENESYS,		USB_PRODUCT_GENESYS_GL641USB,	Q_ANY,
		UD_PROTO_SCSI, UD_PROTO_BBB,
		FORCE_SHORT_INQUIRY|NO_START_STOP|IGNORE_RESIDUE
	},
	{	USB_VENDOR_HITACHI,		USB_PRODUCT_HITACHI_DVDCAM_USB,	Q_ANY,
		UD_PROTO_ATAPI, UD_PROTO_CBI_I,
		NO_INQUIRY
	},
	{	USB_VENDOR_HP,			USB_PRODUCT_HP_CDW8200,		Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		NO_TEST_UNIT_READY|NO_START_STOP
	},
	{	USB_VENDOR_INSYSTEM,		USB_PRODUCT_INSYSTEM_USBCABLE,	Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI,
		NO_TEST_UNIT_READY|NO_START_STOP|ALT_IFACE_1
	},
	{	USB_VENDOR_IOMEGA,		USB_PRODUCT_IOMEGA_ZIP100,	Q_ANY,
		/* XXX This is not correct as there are Zip drives that use ATAPI */
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		NO_TEST_UNIT_READY
	},
	{	USB_VENDOR_MELCO,		USB_PRODUCT_MELCO_DUBPXXG,	Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		FORCE_SHORT_INQUIRY|NO_START_STOP|IGNORE_RESIDUE
	},
	{	USB_VENDOR_MICROTECH,		USB_PRODUCT_MICROTECH_DPCM,	Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_CBI,
		NO_TEST_UNIT_READY|NO_START_STOP
	},
	{	USB_VENDOR_MSYSTEMS,		USB_PRODUCT_MSYSTEMS_DISKONKEY,	Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		IGNORE_RESIDUE|NO_GETMAXLUN|RS_NO_CLEAR_UA
	},
	{	USB_VENDOR_OLYMPUS,		USB_PRODUCT_OLYMPUS_C1,		Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		WRONG_CSWSIG
	},
	{	USB_VENDOR_SCANLOGIC,		USB_PRODUCT_SCANLOGIC_SL11R,	Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		NO_QUIRKS
	},
	{	USB_VENDOR_SHUTTLE,		USB_PRODUCT_SHUTTLE_EUSB,	Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		NO_TEST_UNIT_READY|NO_START_STOP|SHUTTLE_INIT
	},
	{	USB_VENDOR_SIGMATEL,		USB_PRODUCT_SIGMATEL_I_BEAD100,	Q_ANY,
		UD_PROTO_SCSI,			UD_PROTO_BBB,
		SHUTTLE_INIT
	},
	{	USB_VENDOR_SONY,		USB_PRODUCT_SONY_DSC,		Q_ANY,
		UD_PROTO_RBC,			UD_PROTO_CBI,
		NO_QUIRKS
	},
	{	USB_VENDOR_SONY,		USB_PRODUCT_SONY_MSC,		Q_ANY,
		UD_PROTO_RBC,			UD_PROTO_CBI,
		NO_QUIRKS
	},
	{	USB_VENDOR_TREK,		USB_PRODUCT_TREK_THUMBDRIVE_8MB, Q_ANY,
	        UD_PROTO_ATAPI,			UD_PROTO_BBB,
		IGNORE_RESIDUE
	},
	{	USB_VENDOR_YANO,		USB_PRODUCT_YANO_U640MO,	Q_ANY,
		UD_PROTO_ATAPI,			UD_PROTO_CBI_I,
		FORCE_SHORT_INQUIRY
	},
	{	Q_EOT,				Q_EOT,				Q_EOT,
		0,				0,
		NO_QUIRKS
	}
};

/*
 ******	Protótipos de funções ***********************************
 */
void		udstart (struct ud_softc *sc);
void		ud_intr (struct ud_softc *sc, void *priv, int residual, int status);

int		ud_get_proto_and_quirks (struct ud_softc *sc);
void		ud_get_protos_and_quirks (struct usb_attach_arg *uaa,
						uchar *cmd_proto, uchar *phys_proto, ushort *quirks);
void		ud_init_shuttle (struct ud_softc *sc);
void		ud_bbb_get_max_lun (struct ud_softc *sc);

void		ud_bbb_reset (struct ud_softc *sc, int status);
void		ud_bbb_transfer (struct ud_softc *sc, int lun, void *cmd, int cmdlen,
				    void *data, int datalen, int dir, sc_xfer_cb_f cb, void *priv);
void		ud_bbb_state (struct usbd_xfer *xfer, void *priv, int err);

int		ud_cbi_adsc (struct ud_softc *sc, char *buffer, int buflen, struct usbd_xfer *xfer);
void		ud_cbi_reset (struct ud_softc *sc, int status);
void		ud_cbi_transfer (struct ud_softc *sc, int lun, void *cmd, int cmdlen,
				    void *data, int datalen, int dir, sc_xfer_cb_f cb, void *priv);
void		ud_cbi_state (struct usbd_xfer *xfer, void *priv, int err);

int		ud_scsi_transform  (struct ud_softc *sc, BHEAD *bp);
int		ud_ufi_transform   (struct ud_softc *sc, BHEAD *bp);
int		ud_atapi_transform (struct ud_softc *sc, BHEAD *bp);
int		ud_rbc_transform   (struct ud_softc *sc, BHEAD *bp);

int		ud_setup_transfer
		(	struct ud_softc *sc, struct usbd_pipe *pipe_ptr,
			void *buffer, int buflen, int flags, struct usbd_xfer *xfer
		);

int		ud_setup_ctrl_transfer
		(	struct ud_softc *sc, struct usbd_device *udev,
			struct usb_device_request *req,  void *buffer,
			int buflen, int flags, struct usbd_xfer *xfer
		);

void		ud_clear_endpoint_stall
		(	struct ud_softc *sc, uchar endpt, struct usbd_pipe *pipe_ptr,
			int state, struct usbd_xfer *xfer
		);

int		ud_scsi_internal_cmd (BHEAD *bp);
int		ud_update_disktb (struct ud_softc *sc);

/*
 ****************************************************************
 *	Função de "probe"					*
 ****************************************************************
 */
int
ud_probe (struct device *dev)
{
	struct usb_attach_arg		*uaa = dev->ivars;
	struct usb_interface_descriptor	*id;
	uchar				cmd_proto, phys_proto;
	ushort				quirks;

	if
	(	uaa == NULL || uaa->iface == NULL || (id = uaa->iface->idesc) == NULL ||
		id->bInterfaceClass != UICLASS_MASS
	)
		return (UPROBE_NONE);

	ud_get_protos_and_quirks (uaa, &cmd_proto, &phys_proto, &quirks);

	switch (cmd_proto)
	{
	    case UD_PROTO_SCSI:
	    case UD_PROTO_ATAPI:
		break;

	    case UD_PROTO_UFI:
		printf ("ud: No momento, o protocolo UFI NÃO está sendo suportado\n");
		return (UPROBE_NONE);

	    case UD_PROTO_RBC:
		printf ("ud: No momento, o protocolo RBC NÃO está sendo suportado\n");
		return (UPROBE_NONE);

	    default:
		return (UPROBE_NONE);
	}

	switch (phys_proto)
	{
	    case UD_PROTO_BBB:
	    case UD_PROTO_CBI:
	    case UD_PROTO_CBI_I:
		break;

	    default:
		return (UPROBE_NONE);
	}

	return (UPROBE_DEVCLASS_DEVSUBCLASS);

}	/* end ud_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
ud_attach (struct device *dev)
{
        struct usb_attach_arg		*uaa   = dev->ivars;
	struct usbd_interface		*iface = uaa->iface;
	struct usbd_device		*udev  = uaa->device;
	struct usb_interface_descriptor	*id    = iface->idesc;
        struct ud_softc			*sc;
	struct usb_endpoint_descriptor	*ed;
	int				i, err;
	char				*devinfo;

	/*
	 *	Aloca a estrutura "softc"
	 */
	if ((sc = dev->softc = malloc_byte (sizeof (struct ud_softc))) == NULL)
		return (-1);

	memclr (sc, sizeof (struct ud_softc));

	sc->sc_dev	= dev; 
	sc->sc_iface	= uaa->iface;
	sc->sc_ifaceno	= uaa->ifaceno;
	sc->sc_udev	= udev;

	dev->nameunit[2] += 'a' - '0';

	if ((devinfo = malloc_byte (1024)) == NULL)
		{ printf ("%s: memória esgotada\n", dev->nameunit); goto bad; }

	usbd_devinfo (uaa->device, 0, devinfo);

#if (0)	/*******************************************************/
	device_set_desc_copy (dev, devinfo);
#endif	/*******************************************************/

	printf ("\e[31m");
	printf
	(	"%s: %s, iclass %d/%d\n", dev->nameunit,
		devinfo, id->bInterfaceClass, id->bInterfaceSubClass
	);

	free_byte (devinfo);

	ud_get_protos_and_quirks (uaa, &sc->sc_cmd_proto, &sc->sc_phys_proto, &sc->sc_quirks);

	printf ("\e[0m");

#if (0)	/*******************************************************/
	if (sc->sc_quirks)
		printf ("%s: quirks = %P\n", dev->nameunit, sc->sc_quirks);
#endif	/*******************************************************/

#ifndef CBI_I
	if (sc->sc_phys_proto == UD_PROTO_CBI_I)
	{
		/* See beginning of file for comment on the use of CBI with CCI */

		sc->sc_phys_proto = UD_PROTO_CBI;
	}
#endif

	if (sc->sc_quirks & ALT_IFACE_1)
	{
		if (err = usbd_set_interface (0, 1))
		{
			printf ("%s: NÃO consegui comutar para ALT_IFACE_1\n", dev->nameunit);
			/* ud_detach(self) */
			goto bad;
		}
	}

	/*
	 *	In addition to the Control endpoint the following endpoints
	 *	are required:
	 *	a) bulk-in endpoint.
	 *	b) bulk-out endpoint.
	 *	   and for Control/Bulk/Interrupt with CCI (CBI_I)
	 *	c) intr-in
	 *
	 *	The endpoint addresses are not fixed, so we have to read them
	 *	from the device descriptors of the current interface.
	 */
	for (i = 0 ; i < id->bNumEndpoints ; i++)
	{
		if ((ed = usbd_interface2endpoint_descriptor (sc->sc_iface, i)) == NULL)
		{
			printf ("%s: NÃO consegui ler o descritor do endpoint\n", dev->nameunit);
			goto bad;
		}

		if (UE_GET_DIR (ed->bEndpointAddress) == UE_DIR_IN && (ed->bmAttributes & UE_XFERTYPE) == UE_BULK)
		{
			sc->sc_bulkin = ed->bEndpointAddress;
		}
		elif (UE_GET_DIR (ed->bEndpointAddress) == UE_DIR_OUT && (ed->bmAttributes & UE_XFERTYPE) == UE_BULK)
		{
			sc->sc_bulkout = ed->bEndpointAddress;
		}
		elif
		(	sc->sc_phys_proto == UD_PROTO_CBI_I &&
			UE_GET_DIR (ed->bEndpointAddress) == UE_DIR_IN &&
			(ed->bmAttributes & UE_XFERTYPE) == UE_INTERRUPT
		)
		{
			sc->sc_intrin = ed->bEndpointAddress;

			if (UGETW (ed->wMaxPacketSize) > 2)
				printf ("%s: intr size = %d\n", dev->nameunit, UGETW (ed->wMaxPacketSize));
		}
	}

	/*
	 *	Verifica se encontrou todos os "endpoints" necessários
	 */
	if (!sc->sc_bulkin || !sc->sc_bulkout || (sc->sc_phys_proto == UD_PROTO_CBI_I && !sc->sc_intrin))
	{
		printf
		(	"%s: endpoint NÃO foi encontrado %d/%d/%d\n", dev->nameunit,
			sc->sc_bulkin, sc->sc_bulkout, sc->sc_intrin
		);

		goto bad;
	}

	/*
	 *	Abre os bulk-pipes de leitura e escrita
	 */
	if (err = usbd_open_pipe (sc->sc_iface, sc->sc_bulkout, USBD_EXCLUSIVE_USE, &sc->sc_bulkout_pipe))
	{
		printf ("%s: NÃO pude abrir o bulk pipe (%d) de saída\n", dev->nameunit, sc->sc_bulkout);
		goto bad;
	}

	if (err = usbd_open_pipe (sc->sc_iface, sc->sc_bulkin, USBD_EXCLUSIVE_USE, &sc->sc_bulkin_pipe))
	{
		printf ("%s: NÃO pude abrir o bulk pipe (%d) de entrada\n", dev->nameunit, sc->sc_bulkin);
		goto bad;
	}

	/*
	 *	Open the intr-in pipe if the protocol is CBI with CCI.
	 *	Note: early versions of the Zip drive do have an interrupt pipe, but
	 *	this pipe is unused
	 *
	 *	We do not open the interrupt pipe as an interrupt pipe, but as a
	 *	normal bulk endpoint. We send an IN transfer down the wire at the
	 *	appropriate time, because we know exactly when to expect data on
	 *	that endpoint. This saves bandwidth, but more important, makes the
	 *	code for handling the data on that endpoint simpler. No data
	 *	arriving concurently.
	 */
	if (sc->sc_phys_proto == UD_PROTO_CBI_I)
	{
		if (err = usbd_open_pipe (sc->sc_iface, sc->sc_intrin, USBD_EXCLUSIVE_USE, &sc->sc_intrin_pipe))
		{
			printf ("%s: NÃO pude abrir o intr pipe (%d)\n", dev->nameunit, sc->sc_intrin);
			goto bad;
		}
	}

	sc->sc_xfer_state = TSTATE_ATTACH;

	sc->sc_irq = sc->sc_bulkin_pipe->device->bus->irq;

	/*
	 *	Aloca os descritores de transferência
	 */
	for (i = 0; i < XFER_NR; i++)
	{
		if ((sc->sc_xfer[i] = usbd_alloc_xfer (uaa->device)) == NULL)
		{
			printf ("%s: memória esgotada\n", dev->nameunit);
			goto bad;
		}
	}

	/*
	 *	Escolhe o conjunto apropriado de funções para transferência
	 */
	if (sc->sc_phys_proto == UD_PROTO_BBB)
	{
		sc->sc_reset	= ud_bbb_reset;
		sc->sc_transfer	= ud_bbb_transfer;
		sc->sc_state	= ud_bbb_state;
	}
	else
	{
		sc->sc_reset	= ud_cbi_reset;
		sc->sc_transfer	= ud_cbi_transfer;
		sc->sc_state	= ud_cbi_state;
	}

	switch (sc->sc_cmd_proto)
	{
	    case UD_PROTO_SCSI:
		sc->sc_transform = ud_scsi_transform;
		break;

	    case UD_PROTO_UFI:
		sc->sc_transform = ud_ufi_transform;
		break;

	    case UD_PROTO_ATAPI:
		sc->sc_transform = ud_atapi_transform;
		break;

	    case UD_PROTO_RBC:
		sc->sc_transform = ud_rbc_transform;
		break;
	}

	if (sc->sc_quirks & SHUTTLE_INIT)
		ud_init_shuttle (sc);

	/*
	 *	Obtém o número de unidades lógicas
	 */
	if (sc->sc_phys_proto == UD_PROTO_BBB)
		ud_bbb_get_max_lun (sc);
	else
		sc->sc_maxlun = 0;

	sc->sc_xfer_state = TSTATE_IDLE;
	ud_devclass	  = dev->devclass;

	/*
	 *	Prepara a estrutura SCSI
	 */
	sc->sc_scsi.scsi_dev_nm = dev->nameunit;
	sc->sc_scsi.scsi_cmd    = ud_scsi_internal_cmd;

	/*
	 *	Cria as entradas na tabela de partições
	 */
	if (ud_update_disktb (sc) < 0)
	{
		printf ("%s: NÃO consegui criar entradas na DISKTB\n", dev->nameunit);
		goto bad;
	}

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (sc->sc_bulkout_pipe)
		usbd_close_pipe (sc->sc_bulkout_pipe);

	if (sc->sc_bulkin_pipe)
		usbd_close_pipe (sc->sc_bulkin_pipe);

	if (sc->sc_intrin_pipe)
		usbd_close_pipe (sc->sc_intrin_pipe);

	for (i = 0; i < XFER_NR; i++)
	{
		if (sc->sc_xfer[i] != NULL)
			usbd_free_xfer (sc->sc_xfer[i]);
	}

	free_byte (sc);	dev->softc = NULL;

	return (-1);

}	/* end ud_attach */

/*
 ****************************************************************
 *	Função para desanexar o dispositivo			*
 ****************************************************************
 */
int
ud_detach (struct device *dev)
{
	struct ud_softc		*sc = dev->softc;
	int			i;

	/*
	 *	Consistência
	 */
	if ((sc = dev->softc) == NULL)
		return (-1);

	/*
	 *	Remove as entradas na DISKTB
	 */
	if (disktb_remove_partitions (sc->sc_disktb) < 0)
		return (-1);

	/*
	 *	Remove os descritores de transfência e os "pipes"
	 */
	for (i = 0; i < XFER_NR; i++)
	{
		if (sc->sc_xfer[i] != NULL)
			usbd_free_xfer (sc->sc_xfer[i]);
	}

	if (sc->sc_bulkout_pipe)
		usbd_close_pipe (sc->sc_bulkout_pipe);

	if (sc->sc_bulkin_pipe)
		usbd_close_pipe (sc->sc_bulkin_pipe);

	if (sc->sc_intrin_pipe)
		usbd_close_pipe (sc->sc_intrin_pipe);

	return (0);

}	/* end ud_detach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
udopen (dev_t dev, int oflag)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	SCSI			*sp;
	int			unit;

	/*
	 *	Prólogo
	 */
	if (ud_devclass == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if (up->p_lock || (oflag & O_LOCK) && up->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	/* Realiza o "open" do SCSI */

	sp = &sc->sc_scsi;

	if (scsi_open (sp, dev, oflag) < 0)
		return (-1);

	sp->scsi_nopen++;	up->p_nopen++;

	return (0);

}	/* end udopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
int
udclose (dev_t dev, int flag)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	int			unit;
	SCSI			*sp;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Atualiza os contadores
	 */
	if (--up->p_nopen <= 0)
		up->p_lock = 0;

	sp = &sc->sc_scsi;

	if (--sp->scsi_nopen <= 0)
		scsi_close (sp, dev);

	return (0);

}	/* end udclose */

/*
 ****************************************************************
 *	Função de "strategy"					*
 ****************************************************************
 */
int
udstrategy (BHEAD *bp)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	int			unit;
	daddr_t			bn;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (bp->b_phdev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS || ud_devclass == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	bn = bp->b_phblkno;

	/*
	 *	Verifica se é um comando regular de leitura ou escrita
	 */
	if ((bp->b_flags & B_STAT) == 0)
	{
		SCSI		*sp = &sc->sc_scsi;
		char		*cp;

		/* Verifica a validade do bloco pedido */

		if (bn < 0 || bn + BYTOBL (bp->b_base_count) > up->p_size)
		{
			bp->b_error  = ENXIO;
			bp->b_flags |= B_ERROR;

			EVENTDONE (&bp->b_done);

			return (-1);
		}

		/* Prepara o comando */

		bp->b_cmd_len = 10;

		cp = bp->b_cmd_txt;

		if ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)	/* Write */
			cp[0] = SCSI_CMD_WRITE_BIG;
		else							/* Read */
			cp[0] = SCSI_CMD_READ_BIG;

		cp[1] = 0;
		*(long *)&cp[2] = long_endian_cv
				((bp->b_phblkno + up->p_offset) >> (sp->scsi_blshift - BLSHIFT));
		cp[6] = 0;
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> sp->scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	Insere o pedido na fila e, se a unidade estiver ociosa, inicia a operação
	 */
	bp->b_cylin	= bn + up->p_offset;
	bp->b_dev_nm	= up->p_name;
	bp->b_retry_cnt = UD_NRETRY;
	bp->b_ptr	= NULL;

	splx (irq_pl_vec[sc->sc_irq]);

	insdsort (&sc->sc_utab, bp);	usb_busy++;

	if (TAS (&sc->sc_busy) >= 0)
		udstart (sc);

	spl0 ();

	return (0);

}	/* end udstrategy */

/*
 ****************************************************************
 *	Inicia a operação em uma unidade			*
 ****************************************************************
 */
void
udstart (struct ud_softc *sc)
{
	DEVHEAD		*dp = &sc->sc_utab;
	BHEAD		*bp;

	/*
	 *	Obtém o primeiro pedido da fila
	 */
	SPINLOCK (&dp->v_lock);

	if ((bp = dp->v_first) == NOBHEAD)
	{
		/* A fila está vazia */

		SPINFREE (&dp->v_lock);
		CLEAR (&sc->sc_busy);

		return;
	}

	SPINFREE (&dp->v_lock);

	/*
	 *	Compatibiliza o comando
	 */
	(*sc->sc_transform) (sc, bp);

#ifdef	MSG
	if (CSWT (39))
	{
		printf ("%s: Iniciando comando SCSI %s\n", sc->sc_dev->nameunit, scsi_cmd_name (bp->b_cmd_txt[0]));
	}
#endif	MSG

	/*
	 *	Aciona a unidade
	 */
	(*sc->sc_transfer)
	(	sc, 0,
		bp->b_cmd_txt, bp->b_cmd_len,
		bp->b_base_addr, bp->b_base_count,
		bp->b_flags & B_READ ? DIR_IN : DIR_OUT,
		ud_intr, bp
	);

}	/* end udstart */

/*
 ****************************************************************
 *	Função de "read"					*
 ****************************************************************
 */
int
udread (IOREQ *iop)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	int			unit;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (iop->io_dev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, udstrategy, &sc->sc_raw_bhead, B_READ, 0 /* dma */);

	return (UNDEF);

}	/* end udread */

/*
 ****************************************************************
 *	Função de "write"					*
 ****************************************************************
 */
int
udwrite (IOREQ *iop)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	int			unit;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (iop->io_dev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, udstrategy, &sc->sc_raw_bhead, B_WRITE, 0 /* dma */);

	return (UNDEF);

}	/* end udwrite */

/*
 ****************************************************************
 *	Função de "ioctl"					*
 ****************************************************************
 */
int
udctl (dev_t dev, int cmd, int arg, int flag)
{
	DISKTB			*up;
	struct device		*device;
	struct ud_softc		*sc;
	int			unit;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (cmd == DKISADISK)
		return (0);

	return (scsi_ctl (&sc->sc_scsi, dev, cmd, arg));

}	/* end udctl */

/*
 ****************************************************************
 *	Função chamada pela interrupção ao término da operação	*
 ****************************************************************
 */
void
ud_intr (struct ud_softc *sc, void *priv, int residual, int status)
{
	BHEAD		*bp = priv;
	int		sense;
	static char	sense_cmd[6] = { SCSI_CMD_REQUEST_SENSE, 0, 0, 0,
					 sizeof (struct scsi_sense_data) };

	if (sc->sc_sense != 0)
	{
		/* Analisa o resultado do comando REQUEST SENSE */

		sense = sc->sc_sense_data.flags & 0x0F;

#if (0)	/*******************************************************/
		if (sense == 2 /* Unit not ready */)
			goto cmd_ok;
#endif	/*******************************************************/

#ifdef	MSG
		if (CSWT (39))
		{
			printf
			(	"%s: cmd = %s, sense = %02X\n",
				sc->sc_dev->nameunit, scsi_cmd_name (bp->b_cmd_txt[0]), sense
			);
		}
#endif	MSG

		/* Prepara o retorno */

		sc->sc_sense = 0;

		if ((sense = scsi_sense (&sc->sc_scsi, sense)) > 0)
			goto cmd_ok;		/* Conseguiu recuperar */

		if (sense == 0)
		{
			/* Tenta repetir o comando */

			if (--bp->b_retry_cnt > 0)
				goto again;
		}

		/* Erro irrecuperável */

		bp->b_error  = EIO;
		bp->b_flags |= B_ERROR;
	}
	elif (/*** sc->sc_sense == 0 && ***/ status != STATUS_CMD_OK)
	{
		/* Envia o SCSI_CMD_REQUEST_SENSE */

		sc->sc_sense = 1;

		(*sc->sc_transfer)
		(	sc, 0, sense_cmd, sizeof (sense_cmd),
			&sc->sc_sense_data, sizeof (sc->sc_sense_data),
			DIR_IN, ud_intr, bp
		);

		return;
	}

    cmd_ok:
	if (remdsort (&sc->sc_utab) != bp)
		panic ("ud_intr: remdsort () != bp\n");

	usb_busy--;

	bp->b_residual = residual;

	EVENTDONE (&bp->b_done);

    again:
	udstart (sc);

}	/* end ud_intr */

/*
 ****************************************************************
 *	Inicializa o "shuttle"					*
 ****************************************************************
 */
void
ud_init_shuttle (struct ud_softc *sc)
{
	struct usb_device_request	req;
	char				status[2];

	/*
	 *	The Linux driver does this, but no one can tell us what the
	 *	command does
	 */
	req.bmRequestType	= UT_READ_VENDOR_DEVICE;
	req.bRequest		= 1;	/* XXX unknown command */

	USETW (req.wValue, 0);
	USETW (req.wIndex, sc->sc_ifaceno);
	USETW (req.wLength, sizeof (status));

	usbd_do_request (sc->sc_udev, &req, &status);

#ifdef	USB_MSG
	printf ("%s: Shuttle init retornou 0x%02x%02x\n", sc->sc_dev->nameunit, status[0], status[1]);
#endif	USB_MSG

}	/* end ud_init_shuttle */

/*
 ****************************************************************
 *	Obtém o número de unidades lógicas			*
 ****************************************************************
 */
void
ud_bbb_get_max_lun (struct ud_softc *sc)
{
	struct usb_interface_descriptor	*id;
	struct usb_device_request	req;

	id = sc->sc_iface->idesc;

	req.bmRequestType	= UT_READ_CLASS_INTERFACE;
	req.bRequest		= UR_BBB_GET_MAX_LUN;

	USETW (req.wValue, 0);
	USETW (req.wIndex, id->bInterfaceNumber);
	USETW (req.wLength, 1);

	switch (usbd_do_request (sc->sc_udev, &req, &sc->sc_maxlun))
	{
	    case USBD_NORMAL_COMPLETION:
		break;

	    case USBD_STALLED:
	    case USBD_SHORT_XFER:
	    default:
		sc->sc_maxlun = 0;
		break;
	}

}	/* end ud_bbb_get_max_lun */

/*
 ****************************************************************
 *	Dá um "reset" na unidade (BBB)				*
 ****************************************************************
 */
void
ud_bbb_reset (struct ud_softc *sc, int status)
{
	struct usbd_device	*udev = sc->sc_udev;

	/*
	 *	Reset recovery (5.3.4 in Universal Serial Bus Mass Storage Class)
	 *
	 *	For Reset Recovery the host shall issue in the following order:
	 *	a) a Bulk-Only Mass Storage Reset
	 *	b) a Clear Feature HALT to the Bulk-In endpoint
	 *	c) a Clear Feature HALT to the Bulk-Out endpoint
	 *
	 *	This is done in 3 steps, states:
	 *	TSTATE_BBB_RESET1
	 *	TSTATE_BBB_RESET2
	 *	TSTATE_BBB_RESET3
	 *
	 *	If the reset doesn't succeed, the device should be port reset.
	 */
#ifdef	USB_MSG
	printf ("%s: Bulk Reset\n", sc->sc_dev->nameunit);
#endif	USB_MSG

	sc->sc_xfer_state = TSTATE_BBB_RESET1;
	sc->sc_xfer_status = status;

	/* reset is a class specific interface write */

	sc->request.bmRequestType	= UT_WRITE_CLASS_INTERFACE;
	sc->request.bRequest		= UR_BBB_RESET;

	USETW (sc->request.wValue, 0);
	USETW (sc->request.wIndex, sc->sc_ifaceno);
	USETW (sc->request.wLength, 0);

	ud_setup_ctrl_transfer (sc, udev, &sc->request, NULL, 0, 0, sc->sc_xfer[XFER_BBB_RESET1]);

}	/* end ud_bbb_reset */

/*
 ****************************************************************
 *	Inicia uma transferência (BBB)				*
 ****************************************************************
 */
void
ud_bbb_transfer (struct ud_softc *sc, int lun, void *cmd, int cmdlen,
		    void *data, int datalen, int dir, sc_xfer_cb_f cb, void *priv)
{
	/*
	 *	Do a Bulk-Only transfer with cmdlen bytes from cmd, possibly
	 *	a data phase of datalen bytes from/to the device and finally a
	 *	csw read phase.
	 *	If the data direction was inbound a maximum of datalen bytes
	 *	is stored in the buffer pointed to by data.
	 *
	 *	ud_bbb_transfer initialises the transfer and lets the state
	 *	machine in ud_bbb_state handle the completion. It uses the
	 *	following states:
	 *	TSTATE_BBB_COMMAND
	 *	  -> TSTATE_BBB_DATA
	 *	  -> TSTATE_BBB_STATUS
	 *	  -> TSTATE_BBB_STATUS2
	 *	  -> TSTATE_BBB_IDLE
	 *
	 *	An error in any of those states will invoke
	 *	ud_bbb_reset.
	 *
	 *	Determine the direction of the data transfer and the length.
	 *
	 *	dCBWDataTransferLength (datalen) :
	 *	  This field indicates the number of bytes of data that the host
	 *	  intends to transfer on the IN or OUT Bulk endpoint(as indicated by
	 *	  the Direction bit) during the execution of this command. If this
	 *	  field is set to 0, the device will expect that no data will be
	 *	  transferred IN or OUT during this command, regardless of the value
	 *	  of the Direction bit defined in dCBWFlags.
	 *
	 *	dCBWFlags (dir) :
	 *	  The bits of the Flags field are defined as follows:
	 *	    Bits 0-6  reserved
	 *	    Bit  7    Direction - this bit shall be ignored if the
	 *                                dCBWDataTransferLength field is zero.
	 *	              0 = data Out from host to device
	 *	              1 = data In from device to host
	 */

	/*
	 *	Fill in the Command Block Wrapper
	 *	We fill in all the fields, so there is no need to bzero it first
	 */
	USETDW (sc->cbw.dCBWSignature, CBWSIGNATURE);

	/* We don't care about the initial value, as long as the values are unique */

	USETDW (sc->cbw.dCBWTag, UGETDW (sc->cbw.dCBWTag) + 1);
	USETDW (sc->cbw.dCBWDataTransferLength, datalen);

	/* DIR_NONE is treated as DIR_OUT (0x00) */

	sc->cbw.bCBWFlags	= (dir == DIR_IN ? CBWFLAGS_IN : CBWFLAGS_OUT);
	sc->cbw.bCBWLUN		= lun;
	sc->cbw.bCDBLength	= cmdlen;

	memmove (sc->cbw.CBWCDB, cmd, cmdlen);

	/* store the details for the data transfer phase */

	sc->sc_xfer_dir		= dir;
	sc->sc_xfer_data	= data;
	sc->sc_xfer_datalen	= datalen;
	sc->sc_xfer_actlen	= 0;
	sc->sc_xfer_cb		= cb;
	sc->sc_xfer_priv	= priv;
	sc->sc_xfer_status	= STATUS_CMD_OK;

	/* move from idle to the command state */

	sc->sc_xfer_state = TSTATE_BBB_COMMAND;

	/* Send the CBW from host to device via bulk-out endpoint */

	if
	(	ud_setup_transfer
		(	sc, sc->sc_bulkout_pipe,
			&sc->cbw, UD_BBB_CBW_SIZE, 0, sc->sc_xfer[XFER_BBB_CBW]
		)
	)
	{
		ud_bbb_reset (sc, STATUS_WIRE_FAILED);
	}

}	/* end ud_bbb_transfer */

/*
 ****************************************************************
 *	Máquina de estados (BBB)				*
 ****************************************************************
 */
void
ud_bbb_state (struct usbd_xfer *xfer, void *priv, int err)
{
	struct ud_softc		*sc = priv;
	struct usbd_xfer	*next_xfer;
	int			residual;

	/*
	 *	State handling for BBB transfers.
	 *
	 *	The subroutine is rather long. It steps through the states given in
	 *	Annex A of the Bulk-Only specification.
	 *	Each state first does the error handling of the previous transfer
	 *	and then prepares the next transfer.
	 *	Each transfer is done asynchroneously so after the request/transfer
	 *	has been submitted you will find a 'return;'.
	 */
#ifdef	USB_MSG
	printf
	(	"ud_bbb_state (%s): state = %d (%s), xfer = %P, error = %s\n",
		sc->sc_dev->nameunit, sc->sc_xfer_state,
		ud_states[sc->sc_xfer_state], xfer, usbd_errstr (err)
	);
#endif	USB_MSG

	switch (sc->sc_xfer_state)
	{
	/***** Bulk Transfer *****/
	    case TSTATE_BBB_COMMAND:
		/* Command transport phase, error handling */
		if (err)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: erro ao enviar CBW\n", sc->sc_dev->nameunit);
#endif	MSG

			/*
			 *	If the device detects that the CBW is invalid, then
			 *	the device may STALL both bulk endpoints and require
			 *	a Bulk-Reset
			 */
			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;
		}

		/* Data transport phase, setup transfer */

		sc->sc_xfer_state = TSTATE_BBB_DATA;

		if (sc->sc_xfer_dir == DIR_IN)
		{
			if
			(	ud_setup_transfer
				(	sc, sc->sc_bulkin_pipe,
					sc->sc_xfer_data,
					sc->sc_xfer_datalen,
					USBD_SHORT_XFER_OK,
					sc->sc_xfer[XFER_BBB_DATA]
				)
			)
				ud_bbb_reset (sc, STATUS_WIRE_FAILED);

			return;
		}
		elif (sc->sc_xfer_dir == DIR_OUT)
		{
			if
			(	ud_setup_transfer
				(	sc, sc->sc_bulkout_pipe,
					sc->sc_xfer_data,
					sc->sc_xfer_datalen,
					0,			/* fixed length transfer */
					sc->sc_xfer[XFER_BBB_DATA]
				)
			)
				ud_bbb_reset (sc, STATUS_WIRE_FAILED);

			return;
		}
		else
		{
			printf ("%s: dados fora de fase\n", sc->sc_dev->nameunit);
		}

		/* FALLTHROUGH if no data phase, err == 0 */
	    case TSTATE_BBB_DATA:
		/* Command transport phase, error handling (ignored if no data
		 * phase (fallthrough from previous state)) */
		if (sc->sc_xfer_dir != DIR_NONE)
		{
			/* retrieve the length of the transfer that was done */

			usbd_get_xfer_status (xfer, NULL, NULL, &sc->sc_xfer_actlen, NULL);

			if (err)
			{
#ifdef	MSG
				if (CSWT (39))
				{
					printf
					(	"%s: erro %s %d bytes (%s)\n",
						sc->sc_dev->nameunit,
						sc->sc_xfer_dir == DIR_IN ? "lendo" : "escrevendo",
						sc->sc_xfer_datalen, usbd_errstr (err)
					);
				}
#endif	MSG
				if (err == USBD_STALLED)
				{
					ud_clear_endpoint_stall
					(	sc,
						(sc->sc_xfer_dir == DIR_IN ?
							sc->sc_bulkin : sc->sc_bulkout),
						(sc->sc_xfer_dir == DIR_IN ?
							sc->sc_bulkin_pipe : sc->sc_bulkout_pipe),
						TSTATE_BBB_DCLEAR,
						sc->sc_xfer[XFER_BBB_DCLEAR]
					);
					return;
				}
				else
				{
					/* Unless the error is a pipe stall the
					 * error is fatal.
					 */
					ud_bbb_reset (sc, STATUS_WIRE_FAILED);
					return;
				}
			}
		}

		/* FALLTHROUGH, err == 0 (no data phase or successfull) */

	    case TSTATE_BBB_DCLEAR:	/* stall clear after data phase */
	    case TSTATE_BBB_SCLEAR:	/* stall clear after status phase */
		/*
		 *	Reading of CSW after bulk stall condition in data phase
		 *	(TSTATE_BBB_DATA2) or bulk-in stall condition after
		 *	reading CSW (TSTATE_BBB_SCLEAR).
		 *	In the case of no data phase or successfull data phase,
		 *	err == 0 and the following if block is passed.
		 */
		if (err)	/* should not occur */
		{
			/* try the transfer below, even if clear stall failed */
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: bulk-%s stall clear failed (%s)\n",
					sc->sc_dev->nameunit,
					(sc->sc_xfer_dir == DIR_IN? "in":"out"),
					usbd_errstr (err)
				);
			}
#endif	MSG
			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;
		}

		/* Status transport phase, setup transfer */
		if (sc->sc_xfer_state == TSTATE_BBB_COMMAND ||
		    sc->sc_xfer_state == TSTATE_BBB_DATA ||
		    sc->sc_xfer_state == TSTATE_BBB_DCLEAR)
		{
		    	/*
			 *	After no data phase, successfull data phase and
			 *	after clearing bulk-in/-out stall condition
			 */
			sc->sc_xfer_state = TSTATE_BBB_STATUS1;
			next_xfer = sc->sc_xfer[XFER_BBB_CSW1];
		}
		else
		{
			/* After first attempt of fetching CSW */

			sc->sc_xfer_state = TSTATE_BBB_STATUS2;
			next_xfer = sc->sc_xfer[XFER_BBB_CSW2];
		}

		/* Read the Command Status Wrapper via bulk-in endpoint. */

		if (ud_setup_transfer(sc, sc->sc_bulkin_pipe, &sc->csw, UD_BBB_CSW_SIZE, 0, next_xfer))
		{
			ud_bbb_reset(sc, STATUS_WIRE_FAILED);
			return;
		}

		return;

	    case TSTATE_BBB_STATUS1:	/* first attempt */
	    case TSTATE_BBB_STATUS2:	/* second attempt */
		/* Status transfer, error handling */
		if (err)
		{
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: Failed to read CSW, %s%s\n",
					sc->sc_dev->nameunit, usbd_errstr (err),
					(sc->sc_xfer_state == TSTATE_BBB_STATUS1 ? ", retrying" : "")
				);
			}
#endif	MSG
			/*
			 *	If this was the first attempt at fetching the CSW
			 *	retry it, otherwise fail.
			 */
			if (sc->sc_xfer_state == TSTATE_BBB_STATUS1)
			{
				ud_clear_endpoint_stall
				(	sc,
					sc->sc_bulkin, sc->sc_bulkin_pipe,
					TSTATE_BBB_SCLEAR,
					sc->sc_xfer[XFER_BBB_SCLEAR]
				);
			}
			else
			{
				ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			}

			return;
		}

		/* Translate weird command-status signatures. */

		if ((sc->sc_quirks & WRONG_CSWSIG) && UGETDW (sc->csw.dCSWSignature) == CSWSIGNATURE_OLYMPUS_C1)
			USETDW (sc->csw.dCSWSignature, CSWSIGNATURE);

		residual = UGETDW (sc->csw.dCSWDataResidue);

		if (residual == 0 && sc->sc_xfer_datalen - sc->sc_xfer_actlen != 0)
			residual = sc->sc_xfer_datalen - sc->sc_xfer_actlen;

		/* Check CSW and handle any error */

		if (UGETDW (sc->csw.dCSWSignature) != CSWSIGNATURE)
		{
			/*
			 *	Invalid CSW: Wrong signature or wrong tag might
			 *	indicate that the device is confused -> reset it
			 */
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: Invalid CSW: sig 0x%08x should be 0x%08x\n",
					sc->sc_dev->nameunit,
					UGETDW (sc->csw.dCSWSignature), CSWSIGNATURE
				);
			}
#endif	MSG
			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;
		}
		elif (UGETDW (sc->csw.dCSWTag) != UGETDW (sc->cbw.dCBWTag))
		{
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: Invalid CSW: tag %d should be %d\n",
					sc->sc_dev->nameunit,
					UGETDW(sc->csw.dCSWTag), UGETDW(sc->cbw.dCBWTag)
				);
			}
#endif	MSG
			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;

		/* CSW is valid here */
		}
		elif (sc->csw.bCSWStatus > CSWSTATUS_PHASE)
		{
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: Invalid CSW: status %d > %d\n",
					sc->sc_dev->nameunit, sc->csw.bCSWStatus, CSWSTATUS_PHASE
				);
			}
#endif	MSG
			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;
		}
		elif (sc->csw.bCSWStatus == CSWSTATUS_PHASE)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: Phase Error, residual = %d\n", sc->sc_dev->nameunit, residual);
#endif	MSG

			ud_bbb_reset (sc, STATUS_WIRE_FAILED);
			return;

		}
		elif (sc->sc_xfer_actlen > sc->sc_xfer_datalen)
		{
			/* Buffer overrun! Don't let this go by unnoticed */
			panic
			(	"%s: transferred %db instead of %db",
				sc->sc_dev->nameunit,
				sc->sc_xfer_actlen, sc->sc_xfer_datalen
			);

		}
		elif (sc->csw.bCSWStatus == CSWSTATUS_FAILED)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: Command Failed, res = %d\n", sc->sc_dev->nameunit, residual);
#endif	MSG

			/* SCSI command failed but transfer was succesful */
			sc->sc_xfer_state = TSTATE_IDLE;
			sc->sc_xfer_cb (sc, sc->sc_xfer_priv, residual, STATUS_CMD_FAILED);
			return;

		}
		else
		{	/* success */
			sc->sc_xfer_state = TSTATE_IDLE;
			sc->sc_xfer_cb (sc, sc->sc_xfer_priv, residual, STATUS_CMD_OK);
			return;
		}

	/***** Bulk Reset *****/
	    case TSTATE_BBB_RESET1:
		if (err)
			printf ("%s: BBB reset failed, %s\n", sc->sc_dev->nameunit, usbd_errstr (err));

		ud_clear_endpoint_stall
		(	sc,
			sc->sc_bulkin, sc->sc_bulkin_pipe,
			TSTATE_BBB_RESET2, sc->sc_xfer[XFER_BBB_RESET2]
		);

		return;

	    case TSTATE_BBB_RESET2:
		if (err)	/* should not occur */
			printf ("%s: BBB bulk-in clear stall failed, %s\n", sc->sc_dev->nameunit, usbd_errstr (err));
			/* no error recovery, otherwise we end up in a loop */

		ud_clear_endpoint_stall
		(	sc,
			sc->sc_bulkout, sc->sc_bulkout_pipe, TSTATE_BBB_RESET3,
			sc->sc_xfer[XFER_BBB_RESET3]
		);

		return;

	    case TSTATE_BBB_RESET3:
		if (err)	/* should not occur */
			printf ("%s: BBB bulk-out clear stall failed, %s\n", sc->sc_dev->nameunit, usbd_errstr (err));
			/* no error recovery, otherwise we end up in a loop */

		sc->sc_xfer_state = TSTATE_IDLE;

		if (sc->sc_xfer_priv)
		{
			(*sc->sc_xfer_cb)
			(	sc, sc->sc_xfer_priv,
				sc->sc_xfer_datalen,
				sc->sc_xfer_status
			);
		}

		return;

	/***** Default *****/
	    default:
		panic ("%s: Estado desconhecido (%d)", sc->sc_dev->nameunit, sc->sc_xfer_state);
	}

}	/* end ud_bbb_state */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ud_cbi_adsc (struct ud_softc *sc, char *buffer, int buflen, struct usbd_xfer *xfer)
{
#if (0)	/*******************************************************/
	KASSERT(sc->sc_proto & (UD_PROTO_CBI|UD_PROTO_CBI_I),
		("%s: ud_cbi_adsc: wrong sc->sc_proto 0x%02x\n",
			sc->sc_dev->nameunit, sc->sc_proto));
	usbd_interface2device_handle (sc->sc_iface, &udev);
#endif	/*******************************************************/

	sc->request.bmRequestType	= UT_WRITE_CLASS_INTERFACE;
	sc->request.bRequest		= UR_CBI_ADSC;

	USETW (sc->request.wValue, 0);
	USETW (sc->request.wIndex, sc->sc_ifaceno);
	USETW (sc->request.wLength, buflen);

	return (ud_setup_ctrl_transfer (sc, sc->sc_udev, &sc->request, buffer, buflen, 0, xfer));

}	/* end ud_cbi_adsc */

/*
 ****************************************************************
 *	Dá um "reset" na unidade (CBI)				*
 ****************************************************************
 */
#define	SEND_DIAGNOSTIC_CMDLEN	12

void
ud_cbi_reset (struct ud_softc *sc, int status)
{
#if (0)	/*******************************************************/
	KASSERT(sc->sc_proto & (UD_PROTO_CBI|UD_PROTO_CBI_I),
		("%s: ud_cbi_reset: wrong sc->sc_proto 0x%02x\n",
			sc->sc_dev->nameunit, sc->sc_proto));
#endif	/*******************************************************/

	/*
	 *	Command Block Reset Protocol
	 *
	 *	First send a reset request to the device. Then clear
	 *	any possibly stalled bulk endpoints.
	 *
	 *	This is done in 3 steps, states:
	 *	TSTATE_CBI_RESET1
	 *	TSTATE_CBI_RESET2
	 *	TSTATE_CBI_RESET3
	 *
	 *	If the reset doesn't succeed, the device should be port reset.
	 */
#if (0)	/*******************************************************/
	printf ("%s: CBI Reset\n", sc->sc_dev->nameunit);

	KASSERT(sizeof(sc->cbl) >= SEND_DIAGNOSTIC_CMDLEN,
		("%s: CBL struct is too small (%ld < %d)\n",
			sc->sc_dev->nameunit,
			(long)sizeof(sc->cbl), SEND_DIAGNOSTIC_CMDLEN));
#endif	/*******************************************************/

	sc->sc_xfer_state = TSTATE_CBI_RESET1;
	sc->sc_xfer_status = status;

	/*
	 *	The 0x1D code is the SEND DIAGNOSTIC command. To distingiush between
	 *	the two the last 10 bytes of the cbl is filled with 0xFF (section
	 *	2.2 of the CBI spec).
	 */
	sc->cbl[0] = 0x1D;	/* Command Block Reset */
	sc->cbl[1] = 0x04;

#if (0)	/*******************************************************/
	int		i;

	for (i = 2; i < SEND_DIAGNOSTIC_CMDLEN; i++)
		sc->cbl[i] = 0xff;
#endif	/*******************************************************/

	memset (&sc->cbl[2], 0xFF, SEND_DIAGNOSTIC_CMDLEN - 2);

	ud_cbi_adsc (sc, sc->cbl, SEND_DIAGNOSTIC_CMDLEN, sc->sc_xfer[XFER_CBI_RESET1]);

	/* XXX if the command fails we should reset the port on the bub */

}	/* end ud_cbi_reset */

/*
 ****************************************************************
 *	Inicia uma transferência (CBI)				*
 ****************************************************************
 */
void
ud_cbi_transfer (struct ud_softc *sc, int lun, void *cmd, int cmdlen,
		    void *data, int datalen, int dir, sc_xfer_cb_f cb, void *priv)
{
#if (0)	/*******************************************************/
	KASSERT(sc->sc_proto & (UD_PROTO_CBI|UD_PROTO_CBI_I),
		("%s: ud_cbi_transfer: wrong sc->sc_proto 0x%02x\n",
			sc->sc_dev->nameunit, sc->sc_proto));
#endif	/*******************************************************/

	/*
	 * Do a CBI transfer with cmdlen bytes from cmd, possibly
	 * a data phase of datalen bytes from/to the device and finally a
	 * csw read phase.
	 * If the data direction was inbound a maximum of datalen bytes
	 * is stored in the buffer pointed to by data.
	 *
	 * ud_cbi_transfer initialises the transfer and lets the state
	 * machine in ud_cbi_state handle the completion. It uses the
	 * following states:
	 * TSTATE_CBI_COMMAND
	 *   -> XXX fill in
	 *
	 * An error in any of those states will invoke
	 * ud_cbi_reset.
	 */

#if (0)	/*******************************************************/
	/* check the given arguments */
	KASSERT(datalen == 0 || data != NULL,
		("%s: datalen > 0, but no buffer",sc->sc_dev->nameunit));
	KASSERT(datalen == 0 || dir != DIR_NONE,
		("%s: direction is NONE while datalen is not zero\n",
			sc->sc_dev->nameunit));
#endif	/*******************************************************/

	/* store the details for the data transfer phase */

	sc->sc_xfer_dir		= dir;
	sc->sc_xfer_data	= data;
	sc->sc_xfer_datalen	= datalen;
	sc->sc_xfer_actlen	= 0;
	sc->sc_xfer_cb		= cb;
	sc->sc_xfer_priv	= priv;
	sc->sc_xfer_status	= STATUS_CMD_OK;

	/* move from idle to the command state */

	sc->sc_xfer_state	= TSTATE_CBI_COMMAND;

#if (0)	/*******************************************************/
	DIF(UDMASS_CBI, ud_cbi_dump_cmd(sc, cmd, cmdlen));
#endif	/*******************************************************/

	/* Send the Command Block from host to device via control endpoint */

	if (ud_cbi_adsc (sc, cmd, cmdlen, sc->sc_xfer[XFER_CBI_CB]))
		ud_cbi_reset (sc, STATUS_WIRE_FAILED);

}	/* end ud_cbi_transfer */

/*
 ****************************************************************
 *	Máquina de estados (CBI)				*
 ****************************************************************
 */
void
ud_cbi_state (struct usbd_xfer *xfer, void *priv, int err)
{
	struct ud_softc		*sc = priv;

#ifdef	USB_MSG
	printf
	(	"%s: estado = %d (%s), xfer = %P, %s\n",
		sc->sc_dev->nameunit, sc->sc_xfer_state,
		ud_states[sc->sc_xfer_state], xfer, usbd_errstr (err))
	);
#endif	USB_MSG

	/*
	 *	State handling for CBI transfers.
	 */
	switch (sc->sc_xfer_state)
	{
	    /***** CBI Transfer *****/
	    case TSTATE_CBI_COMMAND:
		if (err == USBD_STALLED)
		{
			printf ("%s: Command Transport failed\n", sc->sc_dev->nameunit);

			/* Status transport by control pipe (section 2.3.2.1).
			 * The command contained in the command block failed.
			 *
			 * The control pipe has already been unstalled by the
			 * USB stack.
			 * Section 2.4.3.1.1 states that the bulk in endpoints
			 * should not be stalled at this point.
			 */

			sc->sc_xfer_state = TSTATE_IDLE;
			sc->sc_xfer_cb (sc, sc->sc_xfer_priv, sc->sc_xfer_datalen, STATUS_CMD_FAILED);
			return;
		}

		if (err)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: failed to send ADSC\n", sc->sc_dev->nameunit);
#endif	MSG
			ud_cbi_reset (sc, STATUS_WIRE_FAILED);
			return;
		}

		sc->sc_xfer_state = TSTATE_CBI_DATA;

		if (sc->sc_xfer_dir == DIR_IN)
		{
			if
			(	ud_setup_transfer
				(	sc, sc->sc_bulkin_pipe,
					sc->sc_xfer_data, sc->sc_xfer_datalen,
					USBD_SHORT_XFER_OK,
					sc->sc_xfer[XFER_CBI_DATA]
				)
			)
			{
				ud_cbi_reset (sc, STATUS_WIRE_FAILED);
			}
		}
		elif (sc->sc_xfer_dir == DIR_OUT)
		{
			if
			(	ud_setup_transfer
				(	sc, sc->sc_bulkout_pipe,
					sc->sc_xfer_data, sc->sc_xfer_datalen,
					0,	/* fixed length transfer */
					sc->sc_xfer[XFER_CBI_DATA]
				)
			)
			{
				ud_cbi_reset (sc, STATUS_WIRE_FAILED);
			}
		}
		elif (sc->sc_phys_proto == UD_PROTO_CBI_I)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: no data phase\n", sc->sc_dev->nameunit);
#endif	MSG
			sc->sc_xfer_state = TSTATE_CBI_STATUS;

			if
			(	ud_setup_transfer
				(	sc, sc->sc_intrin_pipe,
					&sc->sbl, sizeof(sc->sbl),
					0,	/* fixed length transfer */
					sc->sc_xfer[XFER_CBI_STATUS]
				)
			)
			{
				ud_cbi_reset(sc, STATUS_WIRE_FAILED);
			}
		}
		else
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: no data phase\n", sc->sc_dev->nameunit);
#endif	MSG
			/* No command completion interrupt. Request sense data */

			sc->sc_xfer_state = TSTATE_IDLE;
			sc->sc_xfer_cb (sc, sc->sc_xfer_priv, 0, STATUS_CMD_UNKNOWN);
		}

		return;

	    case TSTATE_CBI_DATA:
		/* retrieve the length of the transfer that was done */
		usbd_get_xfer_status (xfer, NULL, NULL, &sc->sc_xfer_actlen, NULL);

		if (err)
		{
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: Data-%s %db failed, %s\n",
					sc->sc_dev->nameunit,
					(sc->sc_xfer_dir == DIR_IN ? "in" : "out"),
					sc->sc_xfer_datalen, usbd_errstr (err)
				);
			}
#endif	MSG
			if (err == USBD_STALLED)
			{
				ud_clear_endpoint_stall
				(	sc,
					sc->sc_bulkin, sc->sc_bulkin_pipe,
					TSTATE_CBI_DCLEAR,
					sc->sc_xfer[XFER_CBI_DCLEAR]
				);
			}
			else
			{
				ud_cbi_reset (sc, STATUS_WIRE_FAILED);
			}
			return;
		}

#if (0)	/*******************************************************/
		DIF(UDMASS_CBI, if (sc->sc_xfer_dir == DIR_IN)
					ud_dump_buffer(sc, sc->sc_xfer_data,
						sc->sc_xfer_actlen, 48));
#endif	/*******************************************************/

		if (sc->sc_phys_proto == UD_PROTO_CBI_I)
		{
			sc->sc_xfer_state = TSTATE_CBI_STATUS;

			if
			(	ud_setup_transfer
				(	sc, sc->sc_intrin_pipe,
					&sc->sbl, sizeof (sc->sbl),
					0,	/* fixed length transfer */
					sc->sc_xfer[XFER_CBI_STATUS]
				)
			)
			{
				ud_cbi_reset (sc, STATUS_WIRE_FAILED);
			}
		}
		else
		{
			/*
			 *	No command completion interrupt. Request
			 *	sense to get status of command.
			 */
			sc->sc_xfer_state = TSTATE_IDLE;

			sc->sc_xfer_cb
			(	sc, sc->sc_xfer_priv,
				sc->sc_xfer_datalen - sc->sc_xfer_actlen,
				STATUS_CMD_UNKNOWN
			);
		}
		return;

	    case TSTATE_CBI_STATUS:
		if (err)
		{
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: Status Transport failed\n", sc->sc_dev->nameunit);
#endif	MSG
			/* Status transport by interrupt pipe (section 2.3.2.2) */

			if (err == USBD_STALLED)
			{
				ud_clear_endpoint_stall
				(	sc,
					sc->sc_intrin, sc->sc_intrin_pipe,
					TSTATE_CBI_SCLEAR,
					sc->sc_xfer[XFER_CBI_SCLEAR]
				);
			}
			else
			{
				ud_cbi_reset(sc, STATUS_WIRE_FAILED);
			}

			return;
		}

		/* Dissect the information in the buffer */

		if (sc->sc_cmd_proto == UD_PROTO_UFI)
		{
			int	status;

			/*
			 *	Section 3.4.3.1.3 specifies that the UFI command
			 *	protocol returns an ASC and ASCQ in the interrupt
			 *	data block.
			 */
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: UFI CCI, ASC = 0x%02x, ASCQ = 0x%02x\n",
					sc->sc_dev->nameunit,
					sc->sbl.ufi.asc, sc->sbl.ufi.ascq
				);
			}
#endif	MSG
			if (sc->sbl.ufi.asc == 0 && sc->sbl.ufi.ascq == 0)
				status = STATUS_CMD_OK;
			else
				status = STATUS_CMD_FAILED;

			sc->sc_xfer_state = TSTATE_IDLE;

			sc->sc_xfer_cb
			(	sc, sc->sc_xfer_priv,
				sc->sc_xfer_datalen - sc->sc_xfer_actlen,
				status
			);
		}
		else
		{
			/* Command Interrupt Data Block */
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: type=0x%02x, value=0x%02x\n",
					sc->sc_dev->nameunit,
					sc->sbl.common.type, sc->sbl.common.value
				);
			}
#endif	MSG
			if (sc->sbl.common.type == IDB_TYPE_CCI)
			{
				int	err_code;

				if ((sc->sbl.common.value & IDB_VALUE_STATUS_MASK) == IDB_VALUE_PASS)
				{
					err_code = STATUS_CMD_OK;
				}
				elif
				(	(sc->sbl.common.value & IDB_VALUE_STATUS_MASK) == IDB_VALUE_FAIL ||
					(sc->sbl.common.value & IDB_VALUE_STATUS_MASK) == IDB_VALUE_PERSISTENT
				)
				{
					err_code = STATUS_CMD_FAILED;
				}
				else
				{
					err_code = STATUS_WIRE_FAILED;
				}

				sc->sc_xfer_state = TSTATE_IDLE;

				sc->sc_xfer_cb
				(	sc, sc->sc_xfer_priv,
					sc->sc_xfer_datalen-sc->sc_xfer_actlen,
					err_code
				);
			}
		}
		return;

	    case TSTATE_CBI_DCLEAR:
		if (err)	/* should not occur */
		{
#ifdef	MSG
			if (CSWT (39))
			{
				printf
				(	"%s: CBI bulk-in/out stall clear failed, %s\n",
					sc->sc_dev->nameunit, usbd_errstr (err)
				);
			}
#endif	MSG
			ud_cbi_reset (sc, STATUS_WIRE_FAILED);
		}

		sc->sc_xfer_state = TSTATE_IDLE;

		sc->sc_xfer_cb (sc, sc->sc_xfer_priv, sc->sc_xfer_datalen, STATUS_CMD_FAILED);
		return;

	    case TSTATE_CBI_SCLEAR:
		if (err)	/* should not occur */
		{
			printf
			(	"%s: CBI intr-in stall clear failed, %s\n",
			       sc->sc_dev->nameunit, usbd_errstr (err)
			);
		}

		/* Something really bad is going on. Reset the device */
		ud_cbi_reset(sc, STATUS_CMD_FAILED);
		return;

	    /***** CBI Reset *****/
	    case TSTATE_CBI_RESET1:
		if (err)
		{
			printf
			(	"%s: CBI reset failed, %s\n",
				sc->sc_dev->nameunit, usbd_errstr (err)
			);
		}

		ud_clear_endpoint_stall
		(	sc,
			sc->sc_bulkin, sc->sc_bulkin_pipe, TSTATE_CBI_RESET2,
			sc->sc_xfer[XFER_CBI_RESET2]
		);
		return;

	    case TSTATE_CBI_RESET2:
		if (err)	/* should not occur */
		{
			printf
			(	"%s: CBI bulk-in stall clear failed, %s\n",
			       sc->sc_dev->nameunit, usbd_errstr (err)
			);
			/* no error recovery, otherwise we end up in a loop */
		}

		ud_clear_endpoint_stall
		(	sc,
			sc->sc_bulkout, sc->sc_bulkout_pipe, TSTATE_CBI_RESET3,
			sc->sc_xfer[XFER_CBI_RESET3]
		);
		return;

	    case TSTATE_CBI_RESET3:
		if (err)	/* should not occur */
		{
			printf
			(	"%s: CBI bulk-out stall clear failed, %s\n",
				sc->sc_dev->nameunit, usbd_errstr (err)
			);
			/* no error recovery, otherwise we end up in a loop */
		}

		sc->sc_xfer_state = TSTATE_IDLE;

		if (sc->sc_xfer_priv)
		{
			sc->sc_xfer_cb
			(	sc, sc->sc_xfer_priv,
				sc->sc_xfer_datalen,
				sc->sc_xfer_status
			);
		}

		return;

	    /***** Default *****/
	    default:
		panic ("%s: estado desconhecido (%d)", sc->sc_dev->nameunit, sc->sc_xfer_state);
	}

}	/* end ud_cbi_state */

/*
 ****************************************************************
 *	Compatibiliza comandos SCSI				*
 ****************************************************************
 */
int
ud_scsi_transform (struct ud_softc *sc, BHEAD *bp)
{
	switch (bp->b_cmd_txt[0])
	{
	    case SCSI_CMD_TEST_UNIT_READY:
		if (sc->sc_quirks & NO_TEST_UNIT_READY)
		{
			memset (bp->b_cmd_txt, 0, bp->b_cmd_len);

			bp->b_cmd_txt[0] = SCSI_CMD_START_STOP;
			bp->b_cmd_txt[4] = SSS_START;
		}

		break;

	    case SCSI_CMD_INQUIRY:
		if (sc->sc_quirks & FORCE_SHORT_INQUIRY)
			bp->b_cmd_txt[4] = SHORT_INQUIRY_LENGTH;

		break;

	    default:
		break;
	}

	return (1);

}	/* end ud_scsi_transform */

/*
 ****************************************************************
 *	Compatibiliza comandos ATAPI (8070i)			*
 ****************************************************************
 */
int
ud_atapi_transform (struct ud_softc *sc, BHEAD *bp)
{
	/* An ATAPI command is always 12 bytes in length */

	bp->b_cmd_len = ATAPI_COMMAND_LENGTH;

	switch (bp->b_cmd_txt[0])
	{
	    case SCSI_CMD_TEST_UNIT_READY:
		if (sc->sc_quirks & NO_TEST_UNIT_READY)
		{
			memset (bp->b_cmd_txt, 0, bp->b_cmd_len);

			bp->b_cmd_txt[0] = SCSI_CMD_START_STOP;
			bp->b_cmd_txt[4] = SSS_START;
		}

		break;

	    case SCSI_CMD_INQUIRY:
		if (sc->sc_quirks & FORCE_SHORT_INQUIRY)
			bp->b_cmd_txt[4] = SHORT_INQUIRY_LENGTH;

		break;

	    default:
		break;
	}

	return (0);

}	/* end ud_atapi_transform */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int ud_ufi_transform (struct ud_softc *sc, BHEAD *bp)
{
	return (0);

}	/* end ud_ufi_transform */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int ud_rbc_transform (struct ud_softc *sc, BHEAD *bp)
{
	return (0);

}	/* end ud_rbc_transform */

/*
 ****************************************************************
 *	Obtém os protocolos e os "quirks"			*
 ****************************************************************
 */
void
ud_get_protos_and_quirks (struct usb_attach_arg *uaa, uchar *cmd_proto, uchar *phys_proto, ushort *quirks)
{
	struct usb_interface_descriptor	*id    = uaa->iface->idesc;
	struct usbd_device		*udev  = uaa->device;
	struct usb_device_descriptor	*dd    = &udev->ddesc;
	const UDQUIRKS			*qp;

	/*
	 *	Tratamento especial para Y-E
	 */
	if
	(	UGETW (dd->idVendor)  == USB_VENDOR_YEDATA &&
		UGETW (dd->idProduct) == USB_PRODUCT_YEDATA_FLASHBUSTERU
	)
	{
		/* As revisões < 1.28 NÃO tratam o "interrupt endpoint" direito */

		if (UGETW (dd->bcdDevice) < 0x128)
			{ *cmd_proto = UD_PROTO_UFI; *phys_proto = UD_PROTO_CBI; }
		else
			{ *cmd_proto = UD_PROTO_UFI; *phys_proto = UD_PROTO_CBI_I; }

		/*
		 *	As revisões < 1.28 NÃO aceitam o comando TEST UNIT READY
		 *	A  revisão    1.28 atrapalha-se com o comando TEST UNIT READY
		 */
		if (UGETW (dd->bcdDevice) <= 0x128)
			*quirks |= NO_TEST_UNIT_READY;

		*quirks |= RS_NO_CLEAR_UA|FLOPPY_SPEED;

		return;
	}

	/*
	 *	Consulta a tabela de aberrações
	 */
	for (qp = ud_quirks; qp->q_vendor != Q_EOT; qp++)
	{
		if
		(	(qp->q_vendor  == UGETW (dd->idVendor)  || qp->q_vendor  == Q_ANY) &&
			(qp->q_product == UGETW (dd->idProduct) || qp->q_product == Q_ANY)
		)
		{
		    	if (qp->q_release == Q_ANY)
			{
				*cmd_proto	= qp->q_cmd_proto;
				*phys_proto	= qp->q_phys_proto;
				*quirks		= qp->q_quirks;

				return;
			}

			if (qp->q_release == UGETW (dd->bcdDevice))
			{
				*cmd_proto	= qp->q_cmd_proto;
				*phys_proto	= qp->q_phys_proto;
				*quirks		= qp->q_quirks;

				return;
			}
		}
	}

	/*
	 *	Verifica se é um dispositivo bem comportado
	 */
	switch (id->bInterfaceSubClass)
	{
	    case UISUBCLASS_SCSI:
		*cmd_proto = UD_PROTO_SCSI;
		break;

	    case UISUBCLASS_UFI:
		*cmd_proto = UD_PROTO_UFI;
		break;

	    case UISUBCLASS_RBC:
		*cmd_proto = UD_PROTO_RBC;
		break;

	    case UISUBCLASS_SFF8020I:
	    case UISUBCLASS_SFF8070I:
		*cmd_proto = UD_PROTO_ATAPI;
		break;
	}

	switch (id->bInterfaceProtocol)
	{
	    case UIPROTO_MASS_CBI:
		*phys_proto = UD_PROTO_CBI;
		break;

	    case UIPROTO_MASS_CBI_I:
		*phys_proto = UD_PROTO_CBI_I;
		break;

	    case UIPROTO_MASS_BBB_OLD:
	    case UIPROTO_MASS_BBB:
		*phys_proto = UD_PROTO_BBB;
		break;
	}

	*quirks = NO_QUIRKS;

}	/* end ud_get_protos_and_quirks */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ud_setup_transfer (struct ud_softc *sc, struct usbd_pipe *pipe_ptr,
			void *buffer, int buflen, int flags, struct usbd_xfer *xfer)
{
	int		err;

	usbd_setup_xfer
	(	xfer, pipe_ptr, (void *)sc,
		buffer, buflen, flags, UD_TIMEOUT, sc->sc_state
	);

	err = usbd_transfer (xfer);

	if (err && err != USBD_IN_PROGRESS)
	{
		printf ("%s: erro ao iniciar transferência (%s)\n", sc->sc_dev->nameunit, usbd_errstr (err));
		return (err);
	}

	return (USBD_NORMAL_COMPLETION);

}	/* end ud_setup_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ud_setup_ctrl_transfer (struct ud_softc *sc, struct usbd_device *udev, struct usb_device_request *req,
			void *buffer, int buflen, int flags, struct usbd_xfer *xfer)
{
	int		err;

	usbd_setup_default_xfer
	(	xfer, udev, sc, UD_TIMEOUT, req,
		buffer, buflen, flags, sc->sc_state
	);

	err = usbd_transfer (xfer);

	if (err && err != USBD_IN_PROGRESS)
	{
		printf ("%s: erro ao iniciar transferência de controle (%s)\n", sc->sc_dev->nameunit, usbd_errstr (err));

		/* do not reset, as this would make us loop */

		return (err);
	}

	return (USBD_NORMAL_COMPLETION);

}	/* end ud_setup_ctrl_transfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ud_clear_endpoint_stall (struct ud_softc *sc, uchar endpt, struct usbd_pipe *pipe_ptr,
				int state, struct usbd_xfer *xfer)
{
#ifdef	MSG
	if (CSWT (39))
		printf ("%s: Clear endpoint 0x%02x stall\n", sc->sc_dev->nameunit, endpt);
#endif	MSG

	sc->sc_xfer_state = state;

	pipe_ptr->methods->cleartoggle (pipe_ptr);

	sc->request.bmRequestType	= UT_WRITE_ENDPOINT;
	sc->request.bRequest		= UR_CLEAR_FEATURE;

	USETW (sc->request.wValue, UF_ENDPOINT_HALT);
	USETW (sc->request.wIndex, endpt);
	USETW (sc->request.wLength, 0);

	ud_setup_ctrl_transfer (sc, sc->sc_udev, &sc->request, NULL, 0, 0, xfer);

}	/* end ud_clear_endpoint_stall */

/*
 ****************************************************************
 *	Processa um comando SCSI interno			*
 ****************************************************************
 *
 *	Função chamada pelo módulo "dev/scsi.c"
 */
int
ud_scsi_internal_cmd (BHEAD *bp)
{
	struct ud_softc		*sc;
	struct device		*device;
	int			unit;
	DISKTB			*up;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (bp->b_phdev)) == NODISKTB)
		return (-1);

	unit = up->p_target;

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ud_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (bp->b_cmd_txt[0] == SCSI_CMD_TEST_UNIT_READY)
		return (0);

	bp->b_flags |= B_STAT;
	bp->b_blkno  = bp->b_phblkno = 0;
	bp->b_done   = 0;

	udstrategy (bp);

	EVENTWAIT (&bp->b_done, PBLKIO);

#ifdef	MSG
	if (CSWT (39))
		printf ("ud_scsi_internal_cmd: Fim do comando SCSI %s\n", scsi_cmd_name (bp->b_cmd_txt[0]));
#endif	MSG

	return (geterror (bp));

}	/* end ud_scsi_internal_cmd */

/*
 ****************************************************************
 *	Aloca duas entradas na tabela de partições		*
 ****************************************************************
 */
int
ud_update_disktb (struct ud_softc *sc)
{
	DISKTB		*pp;
	SCSI		*sp;
	dev_t		dev;
	int		target;
	char		dev_nm[16];

	for (target = 0; /* abaixo */; target++)
	{
		struct device	*dp;

		if (target >= MAX_DEV_in_CLASS)
			{ u.u_error = ENXIO; return (-1); }

		if ((dp = ud_devclass->devices[target]) == NULL)
			continue;

		if (dp->softc == sc)
			break;
	}

	/*
	 *	Prepara o essencial da primeira entrada (para todo o dispositivo)
	 */
	sp = &sc->sc_scsi; sp->scsi_dev_nm = sc->sc_dev->nameunit;

	if ((pp = next_disktb) + 2 >= end_disktb)
		{ printf ("ud_alloc_disktb_entry: DISKTB cheia\n"); return (-1); }

	strcpy (pp->p_name, sp->scsi_dev_nm);

	pp->p_offset	= 0;
   	pp->p_size	= 0;		/* Completado por "scsi_open", que NÃO lerá a tabela de partições */

   /***	pp->p_type	= 0;	***/
   /***	pp->p_flags	= 0;	***/
   /***	pp->p_lock	= 0;	***/
   /***	pp->p_blshift	= ...;	***/	/* Completado por "scsi_open" */

	pp->p_dev	= dev = MAKEDEV (UD_MAJOR, pp - disktb);
   	pp->p_unit	= 0;
	pp->p_target	= target;

   /***	pp->p_head	= ....;	***/
   /***	pp->p_sect	= ....;	***/
   /***	pp->p_cyl	= ....;	***/

   /***	pp->p_nopen	= 0;	   ***/
   /***	pp->p_sb	= NOSB;	   ***/
   /***	pp->p_inode	= NOINODE; ***/

	/*
	 *	Obtém as características do dispositivo
	 */
	if (scsi_open (sp, dev, O_READ) < 0)
		{ printf ("%s: erro em \"scsi_open\"\n", sp->scsi_dev_nm); return (-1); }

	sp->scsi_nopen++;

	/*
	 *	Lê a tabela de partições, completando as entradas na DISKTB
	 */
	if ((sc->sc_disktb = disktb_create_partitions (pp)) == NODISKTB)
		{ printf ("%s: erro na leitura da tabela de partições\n", sp->scsi_dev_nm); return (-1); }

	/*
	 *	Atualiza o "/dev"
	 */
	if (rootdir == NOINODE)
		return (0);		/* Ainda não pode ler INODEs */

	strcpy (dev_nm, "/dev/");

	for (pp = sc->sc_disktb; memeq (pp->p_name, sp->scsi_dev_nm, 3); pp++)
	{	
		strcpy (dev_nm + 5, pp->p_name);
		kmkdev (dev_nm, IFBLK|0740, pp->p_dev);

		dev_nm[5] = 'r'; strcpy (dev_nm + 6, pp->p_name);
		kmkdev (dev_nm, IFCHR|0640, pp->p_dev);
	}

	return (0);

}	/* end ud_update_disktb */

#if (0)	/*******************************************************/
 /*
 * Generic functions to handle transfers
 */
void
ud_reset(struct ud_softc *sc, sc_xfer_cb_f cb, void *priv)
{
	sc->sc_xfer_cb = cb;
	sc->sc_xfer_priv = priv;

	/* The reset is a forced reset, so no error (yet) */
	sc->reset(sc, STATUS_CMD_OK);
}


/* RBC specific functions */
int
ud_rbc_transform(struct ud_softc *sc, unsigned char *cmd, int cmdlen,
		     unsigned char **rcmd, int *rcmdlen)
{
	switch (cmd[0]) {
	/* these commands are defined in RBC: */
	case READ_10:
	case READ_CAPACITY:
	case SCSI_CMD_START_STOP:
	case SYNCHRONIZE_CACHE:
	case WRITE_10:
	case 0x2f: /* VERIFY_10 is absent from scsi_all.h??? */
	case SCSI_CMD_INQUIRY:
	case MODE_SELECT_10:
	case MODE_SENSE_10:
	case SCSI_CMD_TEST_UNIT_READY:
	case WRITE_BUFFER:
	 /* The following commands are not listed in my copy of the RBC specs.
	  * CAM however seems to want those, and at least the Sony DSC device
	  * appears to support those as well */
	case REQUEST_SENSE:
	case PREVENT_ALLOW:
		*rcmd = cmd;		/* We don't need to copy it */
		*rcmdlen = cmdlen;
		return 1;
	/* All other commands are not legal in RBC */
	default:
		printf ("%s: Unsupported RBC command 0x%02x",
			sc->sc_dev->nameunit, cmd[0]);
		printf ("\n");
		return 0;	/* failure */
	}
}

/*
 * UFI specific functions
 */
Static int
ud_ufi_transform(struct ud_softc *sc, unsigned char *cmd, int cmdlen,
		    unsigned char **rcmd, int *rcmdlen)
{
	/* A UFI command is always 12 bytes in length */
	KASSERT(*rcmdlen >= UFI_COMMAND_LENGTH,
		("rcmdlen = %d < %d, buffer too small",
		 *rcmdlen, UFI_COMMAND_LENGTH));

	*rcmdlen = UFI_COMMAND_LENGTH;
	memset(*rcmd, 0, UFI_COMMAND_LENGTH);

	switch (cmd[0]) {
	/* Commands of which the format has been verified. They should work.
	 * Copy the command into the (zeroed out) destination buffer.
	 */
	case SCSI_CMD_TEST_UNIT_READY:
		if (sc->sc_quirks &  NO_TEST_UNIT_READY) {
			/* Some devices do not support this command.
			 * Start Stop Unit should give the same results
			 */
			DPRINTF(UDMASS_UFI, ("%s: Converted TEST_UNIT_READY "
				"to START_UNIT\n", sc->sc_dev->nameunit));
			(*rcmd)[0] = SCSI_CMD_START_STOP;
			(*rcmd)[4] = SSS_START;
		} else {
			memcpy(*rcmd, cmd, cmdlen);
		}
		return 1;

	case REZERO_UNIT:
	case REQUEST_SENSE:
	case SCSI_CMD_INQUIRY:
	case SCSI_CMD_START_STOP:
	case SEND_DIAGNOSTIC:
	case PREVENT_ALLOW:
	case READ_CAPACITY:
	case READ_10:
	case WRITE_10:
	case POSITION_TO_ELEMENT:	/* SEEK_10 */
	case MODE_SELECT_10:
	case MODE_SENSE_10:
	case READ_12:
	case WRITE_12:
		memcpy(*rcmd, cmd, cmdlen);
		return 1;

	/* Other UFI commands: FORMAT_UNIT, READ_FORMAT_CAPACITY,
	 * VERIFY, WRITE_AND_VERIFY.
	 * These should be checked whether they somehow can be made to fit.
	 */

	default:
		printf ("%s: Unsupported UFI command 0x%02x\n",
			sc->sc_dev->nameunit, cmd[0]);
		return 0;	/* failure */
	}
}

/*
 * 8070i (ATAPI) specific functions
 */
Static int
ud_atapi_transform(struct ud_softc *sc, unsigned char *cmd, int cmdlen,
		      unsigned char **rcmd, int *rcmdlen)
{
	/* An ATAPI command is always 12 bytes in length. */
	KASSERT(*rcmdlen >= ATAPI_COMMAND_LENGTH,
		("rcmdlen = %d < %d, buffer too small",
		 *rcmdlen, ATAPI_COMMAND_LENGTH));

	*rcmdlen = ATAPI_COMMAND_LENGTH;
	memset(*rcmd, 0, ATAPI_COMMAND_LENGTH);

	switch (cmd[0]) {
	/* Commands of which the format has been verified. They should work.
	 * Copy the command into the (zeroed out) destination buffer.
	 */
	case SCSI_CMD_INQUIRY:
		memcpy(*rcmd, cmd, cmdlen);
		/* some drives wedge when asked for full inquiry information. */
		if (sc->sc_quirks & FORCE_SHORT_INQUIRY)
			(*rcmd)[4] = SHORT_INQUIRY_LENGTH;
		return 1;

	case SCSI_CMD_TEST_UNIT_READY:
		if (sc->sc_quirks & NO_TEST_UNIT_READY) {
			KASSERT(*rcmdlen >= sizeof(struct scsi_start_stop_unit),
				("rcmdlen = %d < %ld, buffer too small",
				 *rcmdlen,
				 (long)sizeof(struct scsi_start_stop_unit)));
			DPRINTF(UDMASS_SCSI, ("%s: Converted TEST_UNIT_READY "
				"to START_UNIT\n", sc->sc_dev->nameunit));
			memset(*rcmd, 0, *rcmdlen);
			(*rcmd)[0] = SCSI_CMD_START_STOP;
			(*rcmd)[4] = SSS_START;
			return 1;
		}
		/* fallthrough */
	case REZERO_UNIT:
	case REQUEST_SENSE:
	case SCSI_CMD_START_STOP:
	case SEND_DIAGNOSTIC:
	case PREVENT_ALLOW:
	case READ_CAPACITY:
	case READ_10:
	case WRITE_10:
	case POSITION_TO_ELEMENT:	/* SEEK_10 */
	case SYNCHRONIZE_CACHE:
	case MODE_SELECT_10:
	case MODE_SENSE_10:
		memcpy(*rcmd, cmd, cmdlen);
		return 1;

	case READ_12:
	case WRITE_12:
	default:
		printf ("%s: Unsupported ATAPI command 0x%02x\n",
			sc->sc_dev->nameunit, cmd[0]);
		return 0;	/* failure */
	}
}
#endif	/*******************************************************/
