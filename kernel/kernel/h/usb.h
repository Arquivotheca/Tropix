/*
 ****************************************************************
 *								*
 *			<sys/usb.h>				*
 *								*
 *	Definições relativas ao USB				*
 *								*
 *	Versão	4.2.0, de 28.01.02				*
 *		4.6.0, de 22.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.0				*
 * 								*
 ****************************************************************
 */

#define	USB_H			/* Para definir os protótipos */

/*
 ****************************************************************
 *	Definições dependentes do HARDWARE			*
 ****************************************************************
 */
#define	PCI_INTERFACE_UHCI	0x00
#define PCI_INTERFACE_OHCI	0x10
#define PCI_INTERFACE_EHCI	0x20

/*
 ******	Registros do PCI ****************************************
 */
/*** PCI config registers ***/

#define PCI_USBREV		0x60	/* USB protocol revision */
#define  PCI_USBREV_MASK	0xFF
#define  PCI_USBREV_PRE_1_0	0x00
#define  PCI_USBREV_1_0		0x10
#define  PCI_USBREV_1_1		0x11
#define  PCI_USBREV_2_0		0x20

#define PCI_LEGSUP		0xC0	/* Legacy Support register */
#define  PCI_LEGSUP_USBPIRQDEN	0x2000	/* USB PIRQ D Enable */

#define PCI_CBMEM		0x10	/* configuration base memory */
#define PCI_CBIO		0x20	/* configuration base IO */

#define USB_MAX_DEVICES		128
#define USB_START_ADDR		0

#define USB_CONTROL_ENDPOINT	0

#if (0)	/*******************************************************/
#define USB_STACK_VERSION	2
#define USB_MAX_ENDPOINTS	16
#define USB_FRAMES_PER_SECOND	1000
#endif	/*******************************************************/

/*
 *	Versão LITTLE-ENDIAN
 */
#define UGETW(w)		(*(ushort *)(w))
#define USETW(w,v)		(*(ushort *)(w) = (v))
#define UGETDW(w)		(*(ulong *)(w))
#define USETDW(w,v)		(*(ulong *)(w) = (v))
#define USETW2(w,h,l)		((w)[0] = (char)(l), (w)[1] = (char)(h))

/*
 ******	Estrutura de um pedido **********************************
 */
struct usb_device_request
{
	char		bmRequestType;
	char		bRequest;
	char		wValue[2];
	char		wIndex[2];
	char		wLength[2];
};

#define UT_WRITE		0x00
#define UT_READ			0x80
#define UT_STANDARD		0x00
#define UT_CLASS		0x20
#define UT_VENDOR		0x40
#define UT_DEVICE		0x00
#define UT_INTERFACE		0x01
#define UT_ENDPOINT		0x02
#define UT_OTHER		0x03

#define UT_READ_DEVICE			(UT_READ  | UT_STANDARD | UT_DEVICE)
#define UT_READ_INTERFACE		(UT_READ  | UT_STANDARD | UT_INTERFACE)
#define UT_READ_ENDPOINT		(UT_READ  | UT_STANDARD | UT_ENDPOINT)
#define UT_WRITE_DEVICE			(UT_WRITE | UT_STANDARD | UT_DEVICE)
#define UT_WRITE_INTERFACE		(UT_WRITE | UT_STANDARD | UT_INTERFACE)
#define UT_WRITE_ENDPOINT		(UT_WRITE | UT_STANDARD | UT_ENDPOINT)
#define UT_READ_CLASS_DEVICE		(UT_READ  | UT_CLASS	| UT_DEVICE)
#define UT_READ_CLASS_INTERFACE		(UT_READ  | UT_CLASS	| UT_INTERFACE)
#define UT_READ_CLASS_OTHER		(UT_READ  | UT_CLASS	| UT_OTHER)
#define UT_READ_CLASS_ENDPOINT		(UT_READ  | UT_CLASS	| UT_ENDPOINT)
#define UT_WRITE_CLASS_DEVICE		(UT_WRITE | UT_CLASS	| UT_DEVICE)
#define UT_WRITE_CLASS_INTERFACE	(UT_WRITE | UT_CLASS	| UT_INTERFACE)
#define UT_WRITE_CLASS_OTHER		(UT_WRITE | UT_CLASS	| UT_OTHER)
#define UT_WRITE_CLASS_ENDPOINT		(UT_WRITE | UT_CLASS	| UT_ENDPOINT)
#define UT_READ_VENDOR_DEVICE		(UT_READ  | UT_VENDOR	| UT_DEVICE)
#define UT_READ_VENDOR_INTERFACE	(UT_READ  | UT_VENDOR	| UT_INTERFACE)
#define UT_READ_VENDOR_OTHER		(UT_READ  | UT_VENDOR	| UT_OTHER)
#define UT_READ_VENDOR_ENDPOINT		(UT_READ  | UT_VENDOR	| UT_ENDPOINT)
#define UT_WRITE_VENDOR_DEVICE		(UT_WRITE | UT_VENDOR	| UT_DEVICE)
#define UT_WRITE_VENDOR_INTERFACE	(UT_WRITE | UT_VENDOR	| UT_INTERFACE)
#define UT_WRITE_VENDOR_OTHER		(UT_WRITE | UT_VENDOR	| UT_OTHER)
#define UT_WRITE_VENDOR_ENDPOINT	(UT_WRITE | UT_VENDOR	| UT_ENDPOINT)

/* Requests */

#define UR_GET_STATUS				0x00
#define UR_CLEAR_FEATURE			0x01
#define UR_SET_FEATURE				0x03
#define UR_SET_ADDRESS				0x05
#define UR_GET_DESCRIPTOR			0x06
#define 	UDESC_DEVICE			0x01
#define 	UDESC_CONFIG			0x02
#define 	UDESC_STRING			0x03
#define 	UDESC_INTERFACE			0x04
#define 	UDESC_ENDPOINT			0x05
#define 	UDESC_DEVICE_QUALIFIER		0x06
#define 	UDESC_OTHER_SPEED_CONFIGURATION	0x07
#define 	UDESC_INTERFACE_POWER		0x08
#define 	UDESC_OTG			0x09
#define 	UDESC_CS_DEVICE			0x21	/* class specific */
#define 	UDESC_CS_CONFIG			0x22
#define 	UDESC_CS_STRING			0x23
#define 	UDESC_CS_INTERFACE		0x24
#define 	UDESC_CS_ENDPOINT		0x25
#define 	UDESC_HUB			0x29
#define UR_SET_DESCRIPTOR			0x07
#define UR_GET_CONFIG				0x08
#define UR_SET_CONFIG				0x09
#define UR_GET_INTERFACE			0x0A
#define UR_SET_INTERFACE			0x0B
#define UR_SYNCH_FRAME				0x0C

/* Feature numbers */

#define UF_ENDPOINT_HALT	0
#define UF_DEVICE_REMOTE_WAKEUP	1
#define UF_TEST_MODE		2

#define USB_MAX_IPACKET		8		/* maximum size of the initial packet */
#define USB_2_MAX_CTRL_PACKET	64
#define USB_2_MAX_BULK_PACKET	512

/*
 ****************************************************************
 *	Os diversos descritores					*
 ****************************************************************
 */
/*
 ****** "device descriptor" *************************************
 */
#define UD_USB_2_0	0x0200
#define UD_IS_USB2(d)	(UGETW ((d)->bcdUSB) >= UD_USB_2_0)

#define USB_DEVICE_DESCRIPTOR_SIZE 18

struct usb_device_descriptor
{
	char		bLength;
	char		bDescriptorType;
	char		bcdUSB[2];
	char		bDeviceClass;
	char		bDeviceSubClass;
	char		bDeviceProtocol;
	char		bMaxPacketSize;

	/* The fields below are not part of the initial descriptor */

	char		idVendor[2];
	char		idProduct[2];
	char		bcdDevice[2];
	char		iManufacturer;
	char		iProduct;
	char		iSerialNumber;
	char		bNumConfigurations;
};

/*
 ****** "config descriptor" *************************************
 */
#define UC_BUS_POWERED		0x80
#define UC_SELF_POWERED		0x40
#define UC_REMOTE_WAKEUP	0x20

#define UC_POWER_FACTOR		2

#define USB_CONFIG_DESCRIPTOR_SIZE 9

struct usb_config_descriptor
{
	char		bLength;
	char		bDescriptorType;
	char		wTotalLength[2];
	char		bNumInterface;
	char		bConfigurationValue;
	char		iConfiguration;
	char		bmAttributes;
	char		bMaxPower;		/* max current in 2 mA units */
};

/*
 ****** "interface descriptor" **********************************
 */
enum { USB_INTERFACE_DESCRIPTOR_SIZE = 9 };

#define UE_GET_DIR(a)	((a) & 0x80)
#define UE_SET_DIR(a,d)	((a) | (((d) & 1) << 7))
#define UE_DIR_IN	0x80
#define UE_DIR_OUT	0x00
#define UE_ADDR		0x0f
#define UE_GET_ADDR(a)	((a) & UE_ADDR)

#define UE_XFERTYPE	0x03
#define  UE_CONTROL	0x00
#define  UE_ISOCHRONOUS	0x01
#define  UE_BULK	0x02
#define  UE_INTERRUPT	0x03
#define UE_GET_XFERTYPE(a)	((a) & UE_XFERTYPE)
#define UE_ISO_TYPE	0x0c
#define  UE_ISO_ASYNC	0x04
#define  UE_ISO_ADAPT	0x08
#define  UE_ISO_SYNC	0x0c
#define UE_GET_ISO_TYPE(a)	((a) & UE_ISO_TYPE)

struct usb_interface_descriptor
{
	char		bLength;
	char		bDescriptorType;
	char		bInterfaceNumber;
	char		bAlternateSetting;
	char		bNumEndpoints;
	char		bInterfaceClass;
	char		bInterfaceSubClass;
	char		bInterfaceProtocol;
	char		iInterface;
};

/*
 ****** "endpoint descriptor" ***********************************
 */
#define USB_ENDPOINT_DESCRIPTOR_SIZE 7

struct usb_endpoint_descriptor
{
	char		bLength;
	char		bDescriptorType;
	char		bEndpointAddress;
	char		bmAttributes;
	char		wMaxPacketSize[2];
	char		bInterval;
};

/*
 ****** "string descriptor" *************************************
 */
#define USB_MAX_STRING_LEN	128
#define USB_LANGUAGE_TABLE	0	/* # of the string language id table */

struct usb_string_descriptor
{
	char		bLength;
	char		bDescriptorType;
	char		bString[127][2];
};

/*
 ****** "hub descriptor" ****************************************
 */
#define UR_GET_BUS_STATE	0x02
#define UR_CLEAR_TT_BUFFER	0x08
#define UR_RESET_TT		0x09
#define UR_GET_TT_STATE		0x0A
#define UR_STOP_TT		0x0B

/* Hub features */

#define USB_HUB_MAX_DEPTH	5

#define UHF_C_HUB_LOCAL_POWER	0
#define UHF_C_HUB_OVER_CURRENT	1
#define UHF_PORT_CONNECTION	0
#define UHF_PORT_ENABLE		1
#define UHF_PORT_SUSPEND	2
#define UHF_PORT_OVER_CURRENT	3
#define UHF_PORT_RESET		4
#define UHF_PORT_POWER		8
#define UHF_PORT_LOW_SPEED	9
#define UHF_C_PORT_CONNECTION	16
#define UHF_C_PORT_ENABLE	17
#define UHF_C_PORT_SUSPEND	18
#define UHF_C_PORT_OVER_CURRENT	19
#define UHF_C_PORT_RESET	20
#define UHF_PORT_TEST		21
#define UHF_PORT_INDICATOR	22

#define UHD_PWR			0x0003
#define  UHD_PWR_GANGED		0x0000
#define  UHD_PWR_INDIVIDUAL	0x0001
#define  UHD_PWR_NO_SWITCH	0x0002
#define UHD_COMPOUND		0x0004
#define UHD_OC			0x0018
#define  UHD_OC_GLOBAL		0x0000
#define  UHD_OC_INDIVIDUAL	0x0008
#define  UHD_OC_NONE		0x0010
#define UHD_TT_THINK		0x0060
#define  UHD_TT_THINK_8		0x0000
#define  UHD_TT_THINK_16	0x0020
#define  UHD_TT_THINK_24	0x0040
#define  UHD_TT_THINK_32	0x0060
#define UHD_PORT_IND		0x0080

#define UHD_PWRON_FACTOR 2

#define UHD_NOT_REMOV(desc, i)	(((desc)->DeviceRemovable[(i)/8] >> ((i) % 8)) & 1)

#define USB_HUB_DESCRIPTOR_SIZE 9		/* includes deprecated PortPowerCtrlMask */

struct usb_hub_descriptor
{
	char		bDescLength;
	char		bDescriptorType;
	char		bNbrPorts;
	char		wHubCharacteristics[2];
	char		bPwrOn2PwrGood;		/* delay in 2 ms units */
	char		bHubContrCurrent;
	char		DeviceRemovable[32];	/* max 255 ports */
	char		PortPowerCtrlMask[1];	/* deprecated */
};

/*
 ****** "device qualifier" **************************************
 */
#define USB_DEVICE_QUALIFIER_SIZE 10

struct usb_device_qualifier
{
	char		bLength;
	char		bDescriptorType;
	char		bcdUSB[2];
	char		bDeviceClass;
	char		bDeviceSubClass;
	char		bDeviceProtocol;
	char		bMaxPacketSize0;
	char		bNumConfigurations;
	char		bReserved;
};

#if (0)	/*******************************************************/
typedef struct
{
	char		bLength;
	char		bDescriptorType;
	char		bmAttributes;

#define UOTG_SRP	0x01
#define UOTG_HNP	0x02

}	usb_otg_descriptor_t;

/* OTG feature selectors */

#define UOTG_B_HNP_ENABLE	3
#define UOTG_A_HNP_SUPPORT	4
#define UOTG_A_ALT_HNP_SUPPORT	5
#endif	/*******************************************************/

/*
 ****** descritor de estado *************************************
 */
struct usb_status
{
	char		wStatus[2];
};

/* Device status flags */

#define UDS_SELF_POWERED		0x0001
#define UDS_REMOTE_WAKEUP		0x0002

/* Endpoint status flags */

#define UES_HALT			0x0001

struct usb_hub_status
{
	char		wHubStatus[2];
	char		wHubChange[2];
};

#define UHS_LOCAL_POWER			0x0001
#define UHS_OVER_CURRENT		0x0002

struct usb_port_status
{
	char		wPortStatus[2];
	char		wPortChange[2];
};

#define UPS_CURRENT_CONNECT_STATUS	0x0001
#define UPS_PORT_ENABLED		0x0002
#define UPS_SUSPEND			0x0004
#define UPS_OVERCURRENT_INDICATOR	0x0008
#define UPS_RESET			0x0010
#define UPS_PORT_POWER			0x0100
#define UPS_LOW_SPEED			0x0200
#define UPS_HIGH_SPEED			0x0400
#define UPS_PORT_TEST			0x0800
#define UPS_PORT_INDICATOR		0x1000

#define UPS_C_CONNECT_STATUS		0x0001
#define UPS_C_PORT_ENABLED		0x0002
#define UPS_C_SUSPEND			0x0004
#define UPS_C_OVERCURRENT_INDICATOR	0x0008
#define UPS_C_PORT_RESET		0x0010

/* Device class codes */

#define UDCLASS_IN_INTERFACE		0x00
#define UDCLASS_COMM			0x02
#define UDCLASS_HUB			0x09
#define  UDSUBCLASS_HUB			0x00
#define  UDPROTO_FSHUB			0x00
#define  UDPROTO_HSHUBSTT		0x01
#define  UDPROTO_HSHUBMTT		0x02
#define UDCLASS_DIAGNOSTIC		0xDC
#define UDCLASS_WIRELESS		0xE0
#define  UDSUBCLASS_RF			0x01
#define   UDPROTO_BLUETOOTH		0x01
#define UDCLASS_VENDOR			0xFF

/* Interface class codes */

#define UICLASS_UNSPEC			0x00

#define UICLASS_AUDIO			0x01
#define  UISUBCLASS_AUDIOCONTROL	1
#define  UISUBCLASS_AUDIOSTREAM		2
#define  UISUBCLASS_MIDISTREAM		3

#define UICLASS_CDC			0x02		/* communication */
#define	 UISUBCLASS_DIRECT_LINE_CONTROL_MODEL		1
#define  UISUBCLASS_ABSTRACT_CONTROL_MODEL		2
#define	 UISUBCLASS_TELEPHONE_CONTROL_MODEL		3
#define	 UISUBCLASS_MULTICHANNEL_CONTROL_MODEL		4
#define	 UISUBCLASS_CAPI_CONTROLMODEL			5
#define	 UISUBCLASS_ETHERNET_NETWORKING_CONTROL_MODEL	6
#define	 UISUBCLASS_ATM_NETWORKING_CONTROL_MODEL	7
#define   UIPROTO_CDC_AT				1

#define UICLASS_HID			0x03
#define  UISUBCLASS_BOOT		1
#define  UIPROTO_BOOT_KEYBOARD		1

#define UICLASS_PHYSICAL		0x05

#define UICLASS_IMAGE			0x06

#define UICLASS_PRINTER			0x07
#define  UISUBCLASS_PRINTER		1
#define  UIPROTO_PRINTER_UNI		1
#define  UIPROTO_PRINTER_BI		2
#define  UIPROTO_PRINTER_1284		3

#define UICLASS_MASS			0x08
#define  UISUBCLASS_RBC			1
#define  UISUBCLASS_SFF8020I		2
#define  UISUBCLASS_QIC157		3
#define  UISUBCLASS_UFI			4
#define  UISUBCLASS_SFF8070I		5
#define  UISUBCLASS_SCSI		6
#define  UIPROTO_MASS_CBI_I		0
#define  UIPROTO_MASS_CBI		1
#define  UIPROTO_MASS_BBB_OLD		2	/* Not in the spec anymore */
#define  UIPROTO_MASS_BBB		80	/* 'P' for the Iomega Zip drive */

#define UICLASS_HUB			0x09
#define  UISUBCLASS_HUB			0
#define  UIPROTO_FSHUB			0
#define  UIPROTO_HSHUBSTT		0	/* Yes, same as previous */
#define  UIPROTO_HSHUBMTT		1

#define UICLASS_CDC_DATA		0x0A
#define  UISUBCLASS_DATA		0
#define   UIPROTO_DATA_ISDNBRI		0x30    /* Physical iface */
#define   UIPROTO_DATA_HDLC		0x31    /* HDLC */
#define   UIPROTO_DATA_TRANSPARENT	0x32    /* Transparent */
#define   UIPROTO_DATA_Q921M		0x50    /* Management for Q921 */
#define   UIPROTO_DATA_Q921		0x51    /* Data for Q921 */
#define   UIPROTO_DATA_Q921TM		0x52    /* TEI multiplexer for Q921 */
#define   UIPROTO_DATA_V42BIS		0x90    /* Data compression */
#define   UIPROTO_DATA_Q931		0x91    /* Euro-ISDN */
#define   UIPROTO_DATA_V120		0x92    /* V.24 rate adaption */
#define   UIPROTO_DATA_CAPI		0x93    /* CAPI 2.0 commands */
#define   UIPROTO_DATA_HOST_BASED	0xFD    /* Host based driver */
#define   UIPROTO_DATA_PUF		0xFE    /* see Prot. Unit Func. Desc.*/
#define   UIPROTO_DATA_VENDOR		0xFF    /* Vendor specific */

#define UICLASS_SMARTCARD		0x0B

/*#define UICLASS_FIRM_UPD		0x0C*/

#define UICLASS_SECURITY		0x0D

#define UICLASS_DIAGNOSTIC		0xDC

#define UICLASS_WIRELESS		0xE0
#define  UISUBCLASS_RF			0x01
#define   UIPROTO_BLUETOOTH		0x01

#define UICLASS_APPL_SPEC		0xFE
#define  UISUBCLASS_FIRMWARE_DOWNLOAD	1
#define  UISUBCLASS_IRDA		2
#define  UIPROTO_IRDA			0

#define UICLASS_VENDOR			0xFF

/*
 ******	Tempos (em ms) ******************************************
 */
/*
 *	Minimum time a device needs to be powered down to go through
 *	a power cycle.  XXX Are these time in the spec?
 */
#define USB_POWER_DOWN_TIME		200
#define USB_PORT_POWER_DOWN_TIME	100

#if 0
/* These are the values from the spec */
#define USB_PORT_RESET_DELAY		10
#define USB_PORT_ROOT_RESET_DELAY	50
#define USB_PORT_RESET_RECOVERY		10
#define USB_PORT_POWERUP_DELAY		100
#define USB_SET_ADDRESS_SETTLE		2
#define USB_RESUME_DELAY		(20*5)
#define USB_RESUME_WAIT			10
#define USB_RESUME_RECOVERY		10
#define USB_EXTRA_POWER_UP_TIME		0
#else
/* Allow for marginal (i.e. non-conforming) devices */
#define USB_PORT_RESET_DELAY		50
#define USB_PORT_ROOT_RESET_DELAY	250
#define USB_PORT_RESET_RECOVERY		250
#define USB_PORT_POWERUP_DELAY		300
#define USB_SET_ADDRESS_SETTLE		10
#define USB_RESUME_DELAY		(50*5)
#define USB_RESUME_WAIT			50
#define USB_RESUME_RECOVERY		50
#define USB_EXTRA_POWER_UP_TIME		20
#endif

#define USB_BUS_RESET_DELAY		100

#define USB_MIN_POWER			100 /* mA */
#define USB_MAX_POWER			500 /* mA */

#define USB_UNCONFIG_NO			0
#define USB_UNCONFIG_INDEX		(-1)

/*
 ****************************************************************
 *	Estruturas de Dados					*
 ****************************************************************
 */
#define	NUSB		16		/* Número máximo de controladores */

#define USB_PL		2		/* Prioridade de interrupção */

enum { FALSE, TRUE };

/*
 ******	Protótipos de funções ***********************************
 ******	x **************************************************
 */
struct usb_data
{
	void		*usb_data_ptr;		/* Ponteiro para a estrutura do controlador */
	char		usb_data_type;		/* UHCI, OHCI ou EHCI */
	char		usb_data_stolen;	/* Já foi sombreado? */
};

/*
 ****** Valores retornados pelas funções auxiliares *************
 */
enum
{
	USBD_NORMAL_COMPLETION = 0,		/* sucesso */
	USBD_IN_PROGRESS,

	USBD_PENDING_REQUESTS,			/* erros */
	USBD_NOT_STARTED,
	USBD_INVAL,
	USBD_NOMEM,
	USBD_CANCELLED,
	USBD_BAD_ADDRESS,
	USBD_IN_USE,
	USBD_NO_ADDR,
	USBD_SET_ADDR_FAILED,
	USBD_NO_POWER,
	USBD_TOO_DEEP,
	USBD_IOERROR,
	USBD_NOT_CONFIGURED,
	USBD_TIMEOUT,
	USBD_SHORT_XFER,
	USBD_STALLED,
	USBD_INTERRUPTED,

	USBD_ERROR_MAX
};

/*
 ******	Valores retornados pelas funções de "probe" *************
 */
#define UPROBE_VENDOR_PRODUCT_REV			(-10)
#define UPROBE_VENDOR_PRODUCT				(-20)
#define UPROBE_VENDOR_DEVCLASS_DEVPROTO			(-30)
#define UPROBE_DEVCLASS_DEVSUBCLASS_DEVPROTO		(-40)
#define UPROBE_DEVCLASS_DEVSUBCLASS			(-50)
#define UPROBE_VENDOR_PRODUCT_REV_CONF_IFACE		(-60)
#define UPROBE_VENDOR_PRODUCT_CONF_IFACE		(-70)
#define UPROBE_VENDOR_IFACESUBCLASS_IFACEPROTO		(-80)
#define UPROBE_VENDOR_IFACESUBCLASS			(-90)
#define UPROBE_IFACECLASS_IFACESUBCLASS_IFACEPROTO	(-100)
#define UPROBE_IFACECLASS_IFACESUBCLASS			(-110)
#define UPROBE_IFACECLASS				(-120)
#define UPROBE_IFACECLASS_GENERIC			(-130)
#define UPROBE_GENERIC					(-140)
#define UPROBE_NONE					(+1)

/* Open flags */
#define USBD_EXCLUSIVE_USE	0x01

/* Use default (specified by ep. desc.) interval on interrupt pipe */
#define USBD_DEFAULT_INTERVAL	(-1)

/* Request flags */
#define USBD_NO_COPY		0x01	/* do not copy data to DMA buffer */
#define USBD_SYNCHRONOUS	0x02	/* wait for completion */

#define USBD_SHORT_XFER_OK	0x04	/* allow short reads */
#define USBD_FORCE_SHORT_XFER	0x08	/* force last short packet on write */

#define USBD_NO_TIMEOUT		0
#define USBD_DEFAULT_TIMEOUT	5000	/* ms = 5 s */

/*
 * The usb_task structs form a queue of things to run in the USB event
 * thread.  Normally this is just device discovery when a connect/disconnect
 * has been detected.  But it may also be used by drivers that need to
 * perform (short) tasks that must have a process context.
 */
struct usb_task
{
	struct usb_task		*next_task, **prev_task;

	void			(*fun)(void *);
	void			*arg;
	char			onqueue;
};

#if (0)	/*******************************************************/
void usb_add_task(struct usbd_device * dev, struct usb_task *task);
void usb_rem_task(struct usbd_device * dev, struct usb_task *task);
#define usb_init_task(t, f, a) ((t)->fun = (f), (t)->arg = (a), (t)->onqueue = 0)

struct usb_devno
{
	ushort	 ud_vendor;
	ushort	 ud_product;
};
const struct usb_devno *usb_probe_device(const struct usb_devno *tbl,
	uint nentries, uint sz, ushort	 vendor, ushort	 product);
#define usb_lookup(tbl, vendor, product) \
	usb_probe_device((const struct usb_devno *)(tbl), sizeof (tbl) / sizeof ((tbl)[0]), sizeof ((tbl)[0]), (vendor), (product))
#define	USB_PRODUCT_ANY		0xFFFF
#endif	/*******************************************************/

struct usb_device_stats
{
	ulong	uds_requests[4];	/* indexed by transfer type UE_* */
};

/*
 ******	Estrutura "device" **************************************
 */
enum					/* Estado do dispositivo */
{
	DS_NOTPRESENT,			/* ausente */
	DS_ALIVE,			/* presente */
	DS_ATTACHED,			/* anexado */
	DS_BUSY				/* aberto */
};

#define	DF_ENABLED		1	/* device should be probed/attached */
#define	DF_FIXEDCLASS		2	/* devclass specified at create time */
#define	DF_WILDCARD		4	/* unit was originally wildcard */
#define	DF_DESCMALLOCED		8	/* description was malloced */
#define	DF_QUIET		16	/* don't print verbose attach message */
#define	DF_DONENOPROBE		32	/* don't execute DEVICE_NOPROBE again */
#define	DF_EXTERNALSOFTC	64	/* softc not allocated by us */

struct device
{
	/*
	 *	Encadeamentos
	 */
	struct device		*next;		/* Próximo da lista global */
	struct device		*parent;	/* Dispositivo pai */
	struct device		*sibling;	/* Irmão */
	struct device		*children;	/* Filhos */

	/*
	 *	Informações sobre o dispositivo
	 */
	char			*nameunit;	/* Nome */

	struct devclass		*devclass;	/* Classe */
	int			unit;		/* Posição na classe */

	struct driver		*driver;	/* Driver */

#if (0)	/*******************************************************/
	char			*desc;		/* Descrição (NINGUÉM USA) */
#endif	/*******************************************************/

	char			state;		/* Estado */
	char			pad;
	ushort			flags;		/* Indicadores (ver acima) */

	void			*ivars;		/* Informações (para o "probe" e o "attach") */
	void			*softc;		/* Parte específica */
};

/*
 ******	Estrutura de um "driver" ********************************
 */
struct driver
{
	const char	*name;				/* Nome do "driver" */

	int		(*probe)  (struct device *);	/* As diversas funções */
	int		(*attach) (struct device *);
	int		(*detach) (struct device *);

	struct driver	*next;
};

/*
 ******	Estrutura "devclass" ************************************
 */
enum { MAX_DEV_in_CLASS = 16 };

struct devclass
{
	char			*name;			/* Nome da classe */
	struct driver		*driver_list;		/* Lista de "drivers" */
	struct device		*devices[MAX_DEV_in_CLASS]; /* Dispositivos anexados */
	struct devclass		*next;			/* Próxima classe */
};

/*
 ******	Tabelas de métodos **************************************
 */
struct usbd_bus;		/* Declarações antecipadas */
struct usbd_xfer;
struct usbd_pipe;

struct usbd_bus_methods
{
	int			(*open_pipe) (struct usbd_pipe *pipe);
	void			(*soft_intr) (void *);
	void			(*do_poll) (struct usbd_bus *);
	int			(*allocm) (struct usbd_bus *, void **, ulong bufsize);
	void			(*freem) (struct usbd_bus *, void **);
	struct usbd_xfer	*(*allocx) (struct usbd_bus *);
	void			(*freex) (struct usbd_bus *, struct usbd_xfer *);
};

struct usbd_pipe_methods
{
	int			(*transfer) (struct usbd_xfer *xfer);
	int			(*start) (struct usbd_xfer *xfer);
	void			(*abort) (struct usbd_xfer *xfer);
	void			(*close) (struct usbd_pipe *pipe);
	void			(*cleartoggle) (struct usbd_pipe *pipe);
	void			(*done) (struct usbd_xfer *xfer);
};

#define USBD_RESTART_MAX	5

struct usbd_port
{
	struct usb_port_status	status;
	ushort			power;		/* mA of current on port */
	char			portno;
	char			restartcnt;
	struct usbd_device	*device;	/* Connected device */
	struct usbd_device	*parent;	/* The ports hub */
};

struct usbd_hub
{
	int				(*explore)(struct usbd_device * hub);
	void				*hubsoftc;
	struct usb_hub_descriptor	hubdesc;
	struct usbd_port	        ports[1];
};

/*
 ******	Estrutura "usbd_bus" ************************************
 */
#define USBREV_UNKNOWN			0
#define USBREV_PRE_1_0			1
#define USBREV_1_0			2
#define USBREV_1_1			3
#define USBREV_2_0			4
#define USBREV_STR			{ "unknown", "pre 1.0", "1.0", "1.1", "2.0" }

struct usbd_bus
{
	/* Filled by HC driver */

	ulong				base_addr;
	int				irq;
	int				unit;
	int				port_mapped;

	struct device			*bdev;			/* base device, host adapter */
	const struct usbd_bus_methods	*methods;
	ulong				pipe_size;		/* size of a pipe struct */

	/* Filled by usb driver */

	struct usbd_device		*root_hub;
	struct usbd_device		*devices[USB_MAX_DEVICES];

#if (0)	/*******************************************************/
	EVENT				needs_explore;		/* a hub a signalled a change */
#endif	/*******************************************************/

	struct usb_softc		*usbctl;
	struct usb_device_stats		stats;

	int				intr_context;
	uint				no_intrs;
	int				usbrev;			/* USB revision */

#ifdef	USB_POLLING
	char				use_polling;
#endif	USB_POLLING

	char				exploring_hub;
};

/*
 ******	Estrutura "usbd_endpoint" *******************************
 */
struct usbd_endpoint
{
	struct usb_endpoint_descriptor	*edesc;
	int				refcnt;
	int				savedtoggle;
};

/*
 ******	Estrutura "usbd_device" *********************************
 */
#define USBD_NOLANG			(-1)

#define USB_SPEED_LOW			1
#define USB_SPEED_FULL			2
#define USB_SPEED_HIGH			3

struct usbd_device
{
	struct usbd_bus			*bus;			/* our controller */
	struct usbd_pipe		*default_pipe;		/* pipe 0 */

	char				address;		/* device addess */
	char				config;			/* current configuration # */
	char				depth;			/* distance from root hub */
	char				speed;			/* low/full/high speed */
	char				self_powered;		/* flag for self powered */
	ushort				power;			/* mA the device uses */
	short				langid;			/* language for strings */
	ulong				cookie;			/* unique connection id */

	struct usbd_port		*powersrc;		/* upstream hub port, or 0 */
	struct usbd_device		*myhub;			/* upstream hub */
	struct usbd_port		*myhsport;		/* closest high speed port */
	struct usbd_endpoint		def_ep;			/* for pipe 0 */
	struct usb_endpoint_descriptor	def_ep_desc;		/* for pipe 0 */
	struct usbd_interface		*ifaces;		/* array of all interfaces */
	struct usb_device_descriptor	ddesc;			/* device descriptor */
	struct usb_config_descriptor	*cdesc;			/* full config descr */
	const struct usbd_quirks	*quirks;		/* device quirks, always set */
	struct usbd_hub			*hub;			/* only if this is a hub */
	struct device			**subdevs;		/* sub-devices, 0 terminated */
};

struct usbd_interface
{
	struct usbd_device		*device;
	struct usb_interface_descriptor	*idesc;

	int				index;
	int				altindex;

	struct usbd_endpoint		*endpoints;
	void				*priv;
	struct usbd_pipe		*pipe_list;
};

struct usbd_pipe
{
	struct usbd_interface		*iface;
	struct usbd_device		*device;
	struct usbd_endpoint		*endpoint;

	int				refcnt;
	char				running;
	char				aborting;

	struct usbd_xfer		*first_xfer, **last_xfer;

	struct usbd_pipe		*next_pipe, **prev_pipe;

	struct usbd_xfer		*intrxfer;	/* used for repeating requests */
	char				repeat;
	int				interval;

	const struct usbd_pipe_methods	*methods; 	/* Filled by HC driver */
};

typedef void	(*usbd_callback)(struct usbd_xfer *, void *, int);

#define URQ_REQUEST			0x01
#define URQ_AUTO_DMABUF			0x10
#define URQ_DEV_DMABUF			0x20

struct usbd_xfer
{
	struct usbd_pipe		*pipe;
	void				*priv;
	void				*buffer;
	ulong				length;
	ulong				actlen;
	ushort				flags;
	ulong				timeout;
	int				status;
	usbd_callback			callback;

	EVENT				done;

	/* For control pipe */
	struct usb_device_request	request;

	/* For isoc */
	ushort				*frlengths;
	int				nframes;

	/* For memory allocation */
	struct usbd_device		*device;
	void				*dmabuf;		/* Virtual */

	int				rqflags;

	struct usbd_xfer		*next_xfer;

	void				*hcpriv; /* private use by the HC driver */

#if (0)	/*******************************************************/
	TIMEOUT				timeout_handle;
#endif	/*******************************************************/
};

/*
 ******	Informações para "probe" e "attach" *********************
 */
struct usb_attach_arg
{
	int			port;
	int			configno;
	int			ifaceno;
	int			vendor;
	int			product;
	int			release;
	int			probelvl;

	struct usbd_device	*device;	/* current device */
	struct usbd_interface	*iface;		/* current interface */

	int			usegeneric;

	struct usbd_interface	**ifaces;	/* all interfaces */
	int			nifaces;	/* number of interfaces */
};

/* XXX these values are used to statically bind some elements in the USB tree
 * to specific driver instances. This should be somehow emulated in FreeBSD
 * but can be done later on.
 * The values are copied from the files.usb file in the NetBSD sources.
 */
#define UHUBCF_PORT_DEFAULT		-1
#define UHUBCF_CONFIGURATION_DEFAULT	-1
#define UHUBCF_INTERFACE_DEFAULT	-1
#define UHUBCF_VENDOR_DEFAULT		-1
#define UHUBCF_PRODUCT_DEFAULT		-1
#define UHUBCF_RELEASE_DEFAULT		-1

#if (0)	/*******************************************************/
#define	uhubcf_port		cf_loc[UHUBCF_PORT]
#define	uhubcf_configuration	cf_loc[UHUBCF_CONFIGURATION]
#define	uhubcf_interface	cf_loc[UHUBCF_INTERFACE]
#define	uhubcf_vendor		cf_loc[UHUBCF_VENDOR]
#define	uhubcf_product		cf_loc[UHUBCF_PRODUCT]
#define	uhubcf_release		cf_loc[UHUBCF_RELEASE]
#endif	/*******************************************************/

#define	UHUB_UNK_PORT		UHUBCF_PORT_DEFAULT		/* wildcarded 'port' */
#define	UHUB_UNK_CONFIGURATION	UHUBCF_CONFIGURATION_DEFAULT	/* wildcarded 'configuration' */
#define	UHUB_UNK_INTERFACE	UHUBCF_INTERFACE_DEFAULT	/* wildcarded 'interface' */
#define	UHUB_UNK_VENDOR		UHUBCF_VENDOR_DEFAULT		/* wildcarded 'vendor' */
#define	UHUB_UNK_PRODUCT	UHUBCF_PRODUCT_DEFAULT		/* wildcarded 'product' */
#define	UHUB_UNK_RELEASE	UHUBCF_RELEASE_DEFAULT		/* wildcarded 'release' */

/*
 ******	Indicadores de "usbd_devinfo" ***************************
 */
enum { USBD_SHOW_DEVICE_CLASS = 1, USBD_SHOW_INTERFACE_CLASS };

/*
 ******	Quirks **************************************************
 */
struct usbd_quirks
{
	ulong			uq_flags;	/* Device problems: */
};

#define UQ_NO_SET_PROTO		0x0001	/* cannot handle SET PROTOCOL */
#define UQ_SWAP_UNICODE		0x0002	/* has some Unicode strings swapped */
#define UQ_MS_REVZ		0x0004	/* mouse has Z-axis reversed */
#define UQ_NO_STRINGS		0x0008	/* string descriptors are broken */
#define UQ_BAD_ADC		0x0010	/* bad audio spec version number */
#define UQ_BUS_POWERED		0x0020	/* device is bus powered, despite claim */
#define UQ_BAD_AUDIO		0x0040	/* device claims audio class, but isn't */
#define UQ_SPUR_BUT_UP		0x0080	/* spurious mouse button up events */
#define UQ_AU_NO_XU		0x0100	/* audio device has broken extension unit */
#define UQ_POWER_CLAIM		0x0200	/* hub lies about power status */
#define UQ_AU_NO_FRAC		0x0400	/* don't adjust for fractional samples */
#define UQ_AU_INP_ASYNC		0x0800	/* input is async despite claim of adaptive */
#define UQ_ASSUME_CM_OVER_DATA	0x1000	/* modem device breaks on cm over data */
#define UQ_BROKEN_BIDIR		0x2000	/* printer has broken bidir mode */

/*
 ******	Protótipos de funções ***********************************
 */
struct device	*device_add_child (struct device *dev, const char *name, int unit);
int		device_probe_and_attach (struct device *dev);
int		usbd_do_request (struct usbd_device *dev, struct usb_device_request *req, void *data);
int		usb_attach (struct device *);
int		usb_str (struct usb_string_descriptor *p, int l, char *s);
int		usb_insert_transfer (struct usbd_xfer *xfer);
void		usb_transfer_complete (struct usbd_xfer *xfer);
int		usbd_new_device (struct device *parent, struct usbd_bus *bus, int depth,
                int speed, int port, struct usbd_port *up);
int		usbd_getnewaddr (struct usbd_bus *bus);
int		usbd_setup_pipe (struct usbd_device *dev, struct usbd_interface *iface,
		struct usbd_endpoint *ep, int ival, struct usbd_pipe **pipe_ptr);
void		usbd_kill_pipe (struct usbd_pipe *pipe_ptr);
void		usbd_remove_device (struct usbd_device *dev, struct usbd_port *up);
int		usbd_get_desc (struct usbd_device *dev, int type, int index, int len, void *desc);
int		usbd_reload_device_desc (struct usbd_device *dev);
int		usbd_set_address (struct usbd_device *dev, int addr);
int		usbd_probe_and_attach (struct device *parent, struct usbd_device *dev, int port, int addr);
int 		usbd_clear_endpoint_stall (struct usbd_pipe *pipe_ptr);
void		usbd_add_dev_event (int type, struct usbd_device *udev);
const char	*usbd_errstr (int err);
struct usbd_xfer *usbd_alloc_xfer (struct usbd_device *dev);

void		usbd_setup_default_xfer
		(	struct usbd_xfer *xfer, struct usbd_device *dev,
			void *priv, ulong timeout, struct usb_device_request *req,
			void *buffer, ulong length, ushort flags, usbd_callback callback
		);

int		usbd_do_request (struct usbd_device *dev, struct usb_device_request *req, void *data);

int		usbd_do_request_flags
		(	struct usbd_device *dev, struct usb_device_request *req,
			void *data, ushort flags, int *actlen, ulong timo
		);

int		usbd_do_request_flags_pipe
		(	struct usbd_device *dev, struct usbd_pipe *pipe_ptr,
			struct usb_device_request *req, void *data,
			ushort flags, int *actlen, ulong timeout
		);

int		usbd_transfer (struct usbd_xfer *xfer);
int		usbd_sync_transfer (struct usbd_xfer *xfer);
int 		usbd_free_xfer (struct usbd_xfer *xfer);
int		usbd_xfer_isread (struct usbd_xfer *xfer);

void		usb_delay_ms (struct usbd_bus *bus, int ms);
void		usb_schedsoftintr (struct usbd_bus * bus);
int		usbd_get_config_desc (struct usbd_device * dev, int confidx, struct usb_config_descriptor *d);
int		usbd_get_device_status (struct usbd_device * dev, struct usb_status *st);
int		usbd_set_config_index (struct usbd_device * dev, int index, int msg);
int		device_delete_child(struct device *dev, struct device *child);
struct device	*make_device (struct device *parent, const char *name, int unit);
struct devclass	*devclass_find_or_create (const char *classname, int create);
int		devclass_add_driver(struct devclass *dc, struct driver *driver);
void		add_driver_to_class (struct devclass *dc, struct driver *driver);
int		usbd_get_device_desc (struct usbd_device * dev, struct usb_device_descriptor *d);
int		uhub_explore (struct usbd_device * dev);
void		usbd_devinfo(struct usbd_device * dev, int showclass, char *cp);
void		usb_task_thread (void);
void		usb_event_thread (struct usb_softc *);
void		usb_discover (struct usb_softc *sc);
int		usbd_read_report_desc(struct usbd_interface * ifc, void **descp, int *sizep);
void		device_set_desc_copy(struct device *dev, const char* desc);

int 		usbd_clear_endpoint_stall_async(struct usbd_pipe *pipe);

int		usbd_device2interface_handle
		(	struct usbd_device * dev, uchar ifaceno,
			struct usbd_interface * *iface
		);

void		usbd_delay_ms(struct usbd_device * dev, unsigned int ms);
int		usbd_set_port_feature(struct usbd_device * dev, int port, int sel);
int		usbd_get_port_status(struct usbd_device * dev, int port, struct usb_port_status *ps);
int		usbd_clear_port_feature(struct usbd_device * dev, int port, int sel);
void		usb_disconnect_port(struct usbd_port *up, struct device *parent);
int		usbd_reset_port(struct usbd_device * dev, int port, struct usb_port_status *ps);
void		usb_needs_explore (struct usbd_device * dev);

int		usbd_open_pipe_intr
		(	struct usbd_interface * iface, uchar address, uchar flags, struct usbd_pipe **pipe,
			void * priv, void *buffer, ulong len, usbd_callback cb, int ival
		);

int 		usbd_open_pipe_ival
		(	struct usbd_interface *iface, uchar address,
			uchar flags, struct usbd_pipe **pipe_ptr, int ival
		);

void		usbd_setup_xfer
		(	struct usbd_xfer *xfer, struct usbd_pipe *pipe,
			void * priv, void *buffer,
			ulong length, ushort flags, ulong timeout,
			usbd_callback
		);

int		usbd_bulk_transfer
		(	struct usbd_xfer *xfer, struct usbd_pipe *pipe_ptr,
			ushort flags, ulong timeout, void *buf, ulong *size, char *lbl
		);

int		usbd_abort_pipe(struct usbd_pipe *pipe);
int		usbd_close_pipe(struct usbd_pipe *pipe);
int		usbd_do_request_async (struct usbd_device *, struct usb_device_request *req, void *data);
void		*usbd_alloc_buffer (struct usbd_xfer *xfer, ulong size);
void		usbd_free_buffer (struct usbd_xfer *xfer);
int		usbd_set_interface (struct usbd_interface *iface, int altidx);
int		usbd_fill_iface_data (struct usbd_device *dev, int ifaceidx, int altidx);
int		usbd_endpoint_count (struct usbd_interface *iface, uchar *count);
void		usbd_get_xfer_status (struct usbd_xfer *xfer, void **priv, void **buffer, ulong *count, int *status);
int		usb_create_classes (void);

int		usbd_open_pipe
		(	struct usbd_interface *iface, uchar address, uchar flags,
			struct usbd_pipe **pipe_ptr
		);

struct usb_endpoint_descriptor	*usbd_interface2endpoint_descriptor (struct usbd_interface *, uchar index);
const struct usbd_quirks	*usbd_find_quirk (struct usb_device_descriptor *);

/*
 ******	Variáveis externas **************************************
 */
extern struct usb_data				usb_data[];
extern int					usb_unit;

extern struct devclass				*usb_class_list;

extern struct driver				usb_driver;
extern struct driver				uhub_driver;
extern struct driver				ums_driver;
extern struct driver				ulp_driver;
extern struct driver				ud_driver;

extern const struct usbd_quirks			usbd_no_quirk;
