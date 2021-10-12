/*
 ****************************************************************
 *								*
 *			usb_subr.c				*
 *								*
 *	"Driver" para dispositivo USB				*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.6.0, de 27.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.0				*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"

#include "../h/usb.h"
#include "../usb/usbdevs.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
#define	USB_KNOWNDEV_NOPROD	0x01		/* probe on vendor only */

const char * const usbd_error_strs[] =
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

#define	USB_KNOWNDEV
#ifdef	USB_KNOWNDEV
/*
 ****************************************************************
 *	Dispositivos reconhecidos				*
 ****************************************************************
 */
struct usb_knowndev
{
	ushort	vendor;
	ushort	product;
	int	flags;
	char	*vendorname, *productname;
};

const struct usb_knowndev usb_knowndevs[] =
{
	{
	    USB_VENDOR_3COM, USB_PRODUCT_3COM_HOMECONN,
	    0,
	    "3Com",
	    "HomeConnect USB Camera",
	},
	{
	    USB_VENDOR_3COM, USB_PRODUCT_3COM_3C19250,
	    0,
	    "3Com",
	    "3C19250 Ethernet adapter",
	},
	{
	    USB_VENDOR_3COM, USB_PRODUCT_3COM_USR56K,
	    0,
	    "3Com",
	    "U.S.Robotics 56000 Voice Faxmodem Pro",
	},
	{
	    USB_VENDOR_3COM, USB_PRODUCT_3COM_3C460,
	    0,
	    "3Com",
	    "HomeConnect 3C460",
	},
	{
	    USB_VENDOR_3COM, USB_PRODUCT_3COM_3C460B,
	    0,
	    "3Com",
	    "HomeConnect 3C460B",
	},
	{
	    USB_VENDOR_3COMUSR, USB_PRODUCT_3COMUSR_OFFICECONN,
	    0,
	    "U.S. Robotics",
	    "3Com OfficeConnect Analog Modem",
	},
	{
	    USB_VENDOR_3COMUSR, USB_PRODUCT_3COMUSR_USRISDN,
	    0,
	    "U.S. Robotics",
	    "3Com U.S. Robotics Pro ISDN TA",
	},
	{
	    USB_VENDOR_3COMUSR, USB_PRODUCT_3COMUSR_HOMECONN,
	    0,
	    "U.S. Robotics",
	    "3Com HomeConnect camera",
	},
	{
	    USB_VENDOR_3COMUSR, USB_PRODUCT_3COMUSR_USR56K,
	    0,
	    "U.S. Robotics",
	    "U.S.Robotics 56000 Voice Faxmodem Pro",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX1,
	    0,
	    "AboCom Systems",
	    "XX1",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX2,
	    0,
	    "AboCom Systems",
	    "XX2",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_URE450,
	    0,
	    "AboCom Systems",
	    "URE450 Ethernet Adapter",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_UFE1000,
	    0,
	    "AboCom Systems",
	    "UFE1000 Fast Ethernet Adapter",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_DSB650TX_PNA,
	    0,
	    "AboCom Systems",
	    "1/10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX4,
	    0,
	    "AboCom Systems",
	    "XX4",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX5,
	    0,
	    "AboCom Systems",
	    "XX5",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX6,
	    0,
	    "AboCom Systems",
	    "XX6",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX7,
	    0,
	    "AboCom Systems",
	    "XX7",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX8,
	    0,
	    "AboCom Systems",
	    "XX8",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX9,
	    0,
	    "AboCom Systems",
	    "XX9",
	},
	{
	    USB_VENDOR_ABOCOM, USB_PRODUCT_ABOCOM_XX10,
	    0,
	    "AboCom Systems",
	    "XX10",
	},
	{
	    USB_VENDOR_ACCTON, USB_PRODUCT_ACCTON_USB320_EC,
	    0,
	    "Accton Technology",
	    "USB320-EC Ethernet Adapter",
	},
	{
	    USB_VENDOR_ACCTON, USB_PRODUCT_ACCTON_SS1001,
	    0,
	    "Accton Technology",
	    "SpeedStream Ethernet Adapter",
	},
	{
	    USB_VENDOR_ACERP, USB_PRODUCT_ACERP_ACERSCAN_C310U,
	    0,
	    "Acer Peripherals",
	    "Acerscan C310U",
	},
	{
	    USB_VENDOR_ACERP, USB_PRODUCT_ACERP_ACERSCAN_320U,
	    0,
	    "Acer Peripherals",
	    "Acerscan 320U",
	},
	{
	    USB_VENDOR_ACERP, USB_PRODUCT_ACERP_ACERSCAN_640U,
	    0,
	    "Acer Peripherals",
	    "Acerscan 640U",
	},
	{
	    USB_VENDOR_ACERP, USB_PRODUCT_ACERP_ACERSCAN_620U,
	    0,
	    "Acer Peripherals",
	    "Acerscan 620U",
	},
	{
	    USB_VENDOR_ACTIVEWIRE, USB_PRODUCT_ACTIVEWIRE_IOBOARD,
	    0,
	    "ActiveWire",
	    "I/O Board",
	},
	{
	    USB_VENDOR_ACTIVEWIRE, USB_PRODUCT_ACTIVEWIRE_IOBOARD_FW1,
	    0,
	    "ActiveWire",
	    "I/O Board, rev. 1 firmware",
	},
	{
	    USB_VENDOR_ACTIONTEC, USB_PRODUCT_ACTIONTEC_UAT1,
	    0,
	    "Actiontec Electronics",
	    "UAT1 Wireless Ethernet adapter",
	},
	{
	    USB_VENDOR_ADMTEK, USB_PRODUCT_ADMTEK_PEGASUS,
	    0,
	    "ADMtek",
	    "AN986 USB Ethernet adapter",
	},
	{
	    USB_VENDOR_ADMTEK, USB_PRODUCT_ADMTEK_PEGASUSII,
	    0,
	    "ADMtek",
	    "AN8511 USB Ethernet adapter",
	},
	{
	    USB_VENDOR_ADS, USB_PRODUCT_ADS_UBS10BT,
	    0,
	    "ADS Technologies",
	    "UBS-10BT Ethernet adapter",
	},
	{
	    USB_VENDOR_AGATE, USB_PRODUCT_AGATE_QDRIVE,
	    0,
	    "Agate Technologies",
	    "Q-Drive",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCAN1212U,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan 1212U",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCAN1236U,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan 1236U",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANTOUCH,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan Touch",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCAN1212U2,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan 1212U",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE40,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e40",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE50,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e50",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE20,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e20",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE25,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e25",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE26,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e26",
	},
	{
	    USB_VENDOR_AGFA, USB_PRODUCT_AGFA_SNAPSCANE52,
	    0,
	    "AGFA-Gevaert",
	    "SnapScan e52",
	},
	{
	    USB_VENDOR_AKS, USB_PRODUCT_AKS_USBHASP,
	    0,
	    "Aladdin Knowledge Systems",
	    "USB-HASP 0.06",
	},
	{
	    USB_VENDOR_ALCOR2, USB_PRODUCT_ALCOR2_KBD_HUB,
	    0,
	    "Alcor Micro",
	    "Kbd Hub",
	},
	{
	    USB_VENDOR_ALCOR, USB_PRODUCT_ALCOR_MA_KBD_HUB,
	    0,
	    "Alcor Micro",
	    "MacAlly Kbd Hub",
	},
	{
	    USB_VENDOR_ALCOR, USB_PRODUCT_ALCOR_AU9814,
	    0,
	    "Alcor Micro",
	    "AU9814 Hub",
	},
	{
	    USB_VENDOR_ALCOR, USB_PRODUCT_ALCOR_SM_KBD,
	    0,
	    "Alcor Micro",
	    "MicroConnectors/StrongMan Keyboard",
	},
	{
	    USB_VENDOR_ALCOR, USB_PRODUCT_ALCOR_NEC_KBD_HUB,
	    0,
	    "Alcor Micro",
	    "NEC Kbd Hub",
	},
	{
	    USB_VENDOR_ALTEC, USB_PRODUCT_ALTEC_ADA70,
	    0,
	    "Altec Lansing Technologies",
	    "ADA70 Speakers",
	},
	{
	    USB_VENDOR_ALTEC, USB_PRODUCT_ALTEC_ASC495,
	    0,
	    "Altec Lansing Technologies",
	    "ASC495 Speakers",
	},
	{
	    USB_VENDOR_APC, USB_PRODUCT_APC_UPSPRO500,
	    0,
	    "American Power Conversion",
	    "Back-UPS Pro 500",
	},
	{
	    USB_VENDOR_ANCHOR, USB_PRODUCT_ANCHOR_EZUSB,
	    0,
	    "Anchor Chips",
	    "EZUSB",
	},
	{
	    USB_VENDOR_ANCHOR, USB_PRODUCT_ANCHOR_EZLINK,
	    0,
	    "Anchor Chips",
	    "EZLINK",
	},
	{
	    USB_VENDOR_AOX, USB_PRODUCT_AOX_USB101,
	    0,
	    "AOX",
	    "USB ethernet controller engine",
	},
	{
	    USB_VENDOR_APPLE, USB_PRODUCT_APPLE_OPTMOUSE,
	    0,
	    "Apple Computer",
	    "Optical mouse",
	},
	{
	    USB_VENDOR_APPLE, USB_PRODUCT_APPLE_SPEAKERS,
	    0,
	    "Apple Computer",
	    "Speakers",
	},
	{
	    USB_VENDOR_ASAHIOPTICAL, USB_PRODUCT_ASAHIOPTICAL_OPTIO230,
	    0,
	    "Asahi Optical",
	    "Digital camera",
	},
	{
	    USB_VENDOR_ASAHIOPTICAL, USB_PRODUCT_ASAHIOPTICAL_OPTIO330,
	    0,
	    "Asahi Optical",
	    "Digital camera",
	},
	{
	    USB_VENDOR_ASIX, USB_PRODUCT_ASIX_AX88172,
	    0,
	    "ASIX Electronics",
	    "USB 2.0 10/100 ethernet controller",
	},
	{
	    USB_VENDOR_ATEN, USB_PRODUCT_ATEN_UC1284,
	    0,
	    "ATEN International",
	    "Parallel printer adapter",
	},
	{
	    USB_VENDOR_ATEN, USB_PRODUCT_ATEN_UC10T,
	    0,
	    "ATEN International",
	    "10Mbps ethernet adapter",
	},
	{
	    USB_VENDOR_ATEN, USB_PRODUCT_ATEN_UC232A,
	    0,
	    "ATEN International",
	    "Serial adapter",
	},
	{
	    USB_VENDOR_ATMEL, USB_PRODUCT_ATMEL_UHB124,
	    0,
	    "Atmel",
	    "UHB124 hub",
	},
	{
	    USB_VENDOR_ATMEL, USB_PRODUCT_ATMEL_DWL120,
	    0,
	    "Atmel",
	    "DWL-120 Wireless adapter",
	},
	{
	    USB_VENDOR_AVISION, USB_PRODUCT_AVISION_1200U,
	    0,
	    "Avision",
	    "1200U scanner",
	},
	{
	    USB_VENDOR_BELKIN2, USB_PRODUCT_BELKIN2_F5U002,
	    0,
	    "Belkin Components",
	    "F5U002 Parallel printer adapter",
	},
	{
	    USB_VENDOR_BELKIN, USB_PRODUCT_BELKIN_USB2LAN,
	    0,
	    "Belkin Components",
	    "USB to LAN Converter",
	},
	{
	    USB_VENDOR_BELKIN, USB_PRODUCT_BELKIN_F5U103,
	    0,
	    "Belkin Components",
	    "F5U103 Serial adapter",
	},
	{
	    USB_VENDOR_BELKIN, USB_PRODUCT_BELKIN_F5U109,
	    0,
	    "Belkin Components",
	    "F5U109 Serial adapter",
	},
	{
	    USB_VENDOR_BELKIN, USB_PRODUCT_BELKIN_F5U120,
	    0,
	    "Belkin Components",
	    "F5U120-PC Hub",
	},
	{
	    USB_VENDOR_BELKIN, USB_PRODUCT_BELKIN_F5U208,
	    0,
	    "Belkin Components",
	    "F5U208 VideoBus II",
	},
	{
	    USB_VENDOR_BILLIONTON, USB_PRODUCT_BILLIONTON_USB100,
	    0,
	    "Billionton Systems",
	    "USB100N 10/100 FastEthernet Adapter",
	},
	{
	    USB_VENDOR_BILLIONTON, USB_PRODUCT_BILLIONTON_USBLP100,
	    0,
	    "Billionton Systems",
	    "USB100LP",
	},
	{
	    USB_VENDOR_BILLIONTON, USB_PRODUCT_BILLIONTON_USBEL100,
	    0,
	    "Billionton Systems",
	    "USB100EL",
	},
	{
	    USB_VENDOR_BILLIONTON, USB_PRODUCT_BILLIONTON_USBE100,
	    0,
	    "Billionton Systems",
	    "USBE100",
	},
	{
	    USB_VENDOR_BROTHER, USB_PRODUCT_BROTHER_HL1050,
	    0,
	    "Brother Industries",
	    "HL-1050 laser printer",
	},
	{
	    USB_VENDOR_BTC, USB_PRODUCT_BTC_BTC7932,
	    0,
	    "Behavior Tech. Computer",
	    "Keyboard with mouse port",
	},
	{
	    USB_VENDOR_CANON, USB_PRODUCT_CANON_N656U,
	    0,
	    "Canon",
	    "CanoScan N656U",
	},
	{
	    USB_VENDOR_CANON, USB_PRODUCT_CANON_N1240U,
	    0,
	    "Canon",
	    "CanoScan N1240U",
	},
	{
	    USB_VENDOR_CANON, USB_PRODUCT_CANON_S10,
	    0,
	    "Canon",
	    "PowerShot S10",
	},
	{
	    USB_VENDOR_CANON, USB_PRODUCT_CANON_S100,
	    0,
	    "Canon",
	    "PowerShot S100",
	},
	{
	    USB_VENDOR_CANON, USB_PRODUCT_CANON_S200,
	    0,
	    "Canon",
	    "PowerShot S200",
	},
	{
	    USB_VENDOR_CATC, USB_PRODUCT_CATC_NETMATE,
	    0,
	    "Computer Access Technology",
	    "Netmate ethernet adapter",
	},
	{
	    USB_VENDOR_CATC, USB_PRODUCT_CATC_NETMATE2,
	    0,
	    "Computer Access Technology",
	    "Netmate2 ethernet adapter",
	},
	{
	    USB_VENDOR_CATC, USB_PRODUCT_CATC_CHIEF,
	    0,
	    "Computer Access Technology",
	    "USB Chief Bus & Protocol Analyzer",
	},
	{
	    USB_VENDOR_CATC, USB_PRODUCT_CATC_ANDROMEDA,
	    0,
	    "Computer Access Technology",
	    "Andromeda hub",
	},
	{
	    USB_VENDOR_CASIO, USB_PRODUCT_CASIO_NAMELAND,
	    0,
	    "CASIO",
	    "CASIO Nameland EZ-USB",
	},
	{
	    USB_VENDOR_CHERRY, USB_PRODUCT_CHERRY_MY3000KBD,
	    0,
	    "Cherry Mikroschalter",
	    "My3000 keyboard",
	},
	{
	    USB_VENDOR_CHERRY, USB_PRODUCT_CHERRY_MY3000HUB,
	    0,
	    "Cherry Mikroschalter",
	    "My3000 hub",
	},
	{
	    USB_VENDOR_CHIC, USB_PRODUCT_CHIC_MOUSE1,
	    0,
	    "Chic Technology",
	    "mouse",
	},
	{
	    USB_VENDOR_CHIC, USB_PRODUCT_CHIC_CYPRESS,
	    0,
	    "Chic Technology",
	    "Cypress USB Mouse",
	},
	{
	    USB_VENDOR_CHICONY, USB_PRODUCT_CHICONY_KB8933,
	    0,
	    "Chicony Electronics",
	    "KB-8933 keyboard",
	},
	{
	    USB_VENDOR_COMPAQ, USB_PRODUCT_COMPAQ_PJB100,
	    0,
	    "Compaq Computers",
	    "Personal Jukebox PJB100",
	},
	{
	    USB_VENDOR_CONNECTIX, USB_PRODUCT_CONNECTIX_QUICKCAM,
	    0,
	    "Connectix",
	    "QuickCam",
	},
	{
	    USB_VENDOR_COREGA, USB_PRODUCT_COREGA_ETHER_USB_T,
	    0,
	    "Corega",
	    "Ether USB-T",
	},
	{
	    USB_VENDOR_COREGA, USB_PRODUCT_COREGA_FETHER_USB_TX,
	    0,
	    "Corega",
	    "FEther USB-TX",
	},
	{
	    USB_VENDOR_COREGA, USB_PRODUCT_COREGA_FETHER_USB_TXS,
	    0,
	    "Corega",
	    "FEther USB-TXS",
	},
	{
	    USB_VENDOR_CREATIVE, USB_PRODUCT_CREATIVE_NOMAD_II,
	    0,
	    "Creative",
	    "Nomad II MP3 player",
	},
	{
	    USB_VENDOR_CTX, USB_PRODUCT_CTX_EX1300,
	    0,
	    "Chuntex",
	    "Ex1300 hub",
	},
	{
	    USB_VENDOR_CYPRESS, USB_PRODUCT_CYPRESS_MOUSE,
	    0,
	    "Cypress Semiconductor",
	    "mouse",
	},
	{
	    USB_VENDOR_CYPRESS, USB_PRODUCT_CYPRESS_THERMO,
	    0,
	    "Cypress Semiconductor",
	    "thermometer",
	},
	{
	    USB_VENDOR_CYPRESS, USB_PRODUCT_CYPRESS_FMRADIO,
	    0,
	    "Cypress Semiconductor",
	    "FM Radio",
	},
	{
	    USB_VENDOR_CYPRESS, USB_PRODUCT_CYPRESS_SLIM_HUB,
	    0,
	    "Cypress Semiconductor",
	    "Slim Hub",
	},
	{
	    USB_VENDOR_DAISY, USB_PRODUCT_DAISY_DMC,
	    0,
	    "Daisy Technology",
	    "USB MultiMedia Reader",
	},
	{
	    USB_VENDOR_DALLAS, USB_PRODUCT_DALLAS_J6502,
	    0,
	    "Dallas Semiconductor",
	    "J-6502 speakers",
	},
	{
	    USB_VENDOR_DIAMOND, USB_PRODUCT_DIAMOND_RIO500USB,
	    0,
	    "Diamond",
	    "Rio 500 USB",
	},
	{
	    USB_VENDOR_DIGI, USB_PRODUCT_DIGI_ACCELEPORT2,
	    0,
	    "Digi International",
	    "AccelePort USB 2",
	},
	{
	    USB_VENDOR_DIGI, USB_PRODUCT_DIGI_ACCELEPORT4,
	    0,
	    "Digi International",
	    "AccelePort USB 4",
	},
	{
	    USB_VENDOR_DIGI, USB_PRODUCT_DIGI_ACCELEPORT8,
	    0,
	    "Digi International",
	    "AccelePort USB 8",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DUBE100,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX4,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650C,
	    0,
	    "D-Link",
	    "10Mbps ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX1,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX_PNA,
	    0,
	    "D-Link",
	    "1/10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX3,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650TX2,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_DLINK, USB_PRODUCT_DLINK_DSB650,
	    0,
	    "D-Link",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_EIZO, USB_PRODUCT_EIZO_HUB,
	    0,
	    "EIZO",
	    "hub",
	},
	{
	    USB_VENDOR_EIZO, USB_PRODUCT_EIZO_MONITOR,
	    0,
	    "EIZO",
	    "monitor",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_MOUSE29UO,
	    0,
	    "Elecom",
	    "mouse 29UO",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_LDUSBTX0,
	    0,
	    "Elecom",
	    "LD-USB/TX",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_LDUSBTX1,
	    0,
	    "Elecom",
	    "LD-USB/TX",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_LDUSBLTX,
	    0,
	    "Elecom",
	    "LD-USBL/TX",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_LDUSBTX2,
	    0,
	    "Elecom",
	    "LD-USB/TX",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_UCSGT,
	    0,
	    "Elecom",
	    "UC-SGT",
	},
	{
	    USB_VENDOR_ELECOM, USB_PRODUCT_ELECOM_LDUSBTX3,
	    0,
	    "Elecom",
	    "LD-USB/TX",
	},
	{
	    USB_VENDOR_ELSA, USB_PRODUCT_ELSA_MODEM1,
	    0,
	    "ELSA",
	    "ELSA Modem Board",
	},
	{
	    USB_VENDOR_ELSA, USB_PRODUCT_ELSA_USB2ETHERNET,
	    0,
	    "ELSA",
	    "Microlink USB2Ethernet",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_1S,
	    0,
	    "Entrega",
	    "1S serial connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_2S,
	    0,
	    "Entrega",
	    "2S serial connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_1S25,
	    0,
	    "Entrega",
	    "1S25 serial connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_4S,
	    0,
	    "Entrega",
	    "4S serial connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_E45,
	    0,
	    "Entrega",
	    "E45 Ethernet adapter",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_CENTRONICS,
	    0,
	    "Entrega",
	    "Centronics connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_1S9,
	    0,
	    "Entrega",
	    "1S9 serial connector",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_EZUSB,
	    0,
	    "Entrega",
	    "EZ-USB",
	},
	{
	    USB_VENDOR_ENTREGA, USB_PRODUCT_ENTREGA_2U4S,
	    0,
	    "Entrega",
	    "2U4S serial connector/usb hub",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_PRINTER1,
	    0,
	    "Seiko Epson",
	    "USB Printer",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_PRINTER2,
	    0,
	    "Seiko Epson",
	    "ISD USB Smart Cable for Mac",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_PRINTER3,
	    0,
	    "Seiko Epson",
	    "ISD USB Smart Cable",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_PRINTER5,
	    0,
	    "Seiko Epson",
	    "USB Printer",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_636,
	    0,
	    "Seiko Epson",
	    "Perfection 636U / 636Photo scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_610,
	    0,
	    "Seiko Epson",
	    "Perfection 610 scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1200,
	    0,
	    "Seiko Epson",
	    "Perfection 1200U / 1200Photo scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1600,
	    0,
	    "Seiko Epson",
	    "Expression 1600 scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1640,
	    0,
	    "Seiko Epson",
	    "Perfection 1640SU scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1240,
	    0,
	    "Seiko Epson",
	    "Perfection 1240U / 1240Photo scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_640U,
	    0,
	    "Seiko Epson",
	    "Perfection 640U scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1250,
	    0,
	    "Seiko Epson",
	    "Perfection 1250U / 1250Photo scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1650,
	    0,
	    "Seiko Epson",
	    "Perfection 1650 scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_GT9700F,
	    0,
	    "Seiko Epson",
	    "GT-9700F scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_GT9300UF,
	    0,
	    "Seiko Epson",
	    "GT-9300UF scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1260,
	    0,
	    "Seiko Epson",
	    "Perfection 1260 scanner",
	},
	{
	    USB_VENDOR_EPSON, USB_PRODUCT_EPSON_1660,
	    0,
	    "Seiko Epson",
	    "Perfection 1660 scanner",
	},
	{
	    USB_VENDOR_ETEK, USB_PRODUCT_ETEK_1COM,
	    0,
	    "e-TEK Labs",
	    "Serial port",
	},
	{
	    USB_VENDOR_EXTENDED, USB_PRODUCT_EXTENDED_XTNDACCESS,
	    0,
	    "Extended Systems",
	    "XTNDAccess IrDA",
	},
	{
	    USB_VENDOR_GOHUBS, USB_PRODUCT_GOHUBS_GOCOM232,
	    0,
	    "GoHubs",
	    "GoCOM232 Serial converter",
	},
	{
	    USB_VENDOR_GRAVIS, USB_PRODUCT_GRAVIS_GAMEPADPRO,
	    0,
	    "Advanced Gravis Computer Tech.",
	    "GamePad Pro",
	},
	{
	    USB_VENDOR_GREENHOUSE, USB_PRODUCT_GREENHOUSE_KANA21,
	    0,
	    "GREENHOUSE",
	    "CF-writer with Portable MP3 Player",
	},
	{
	    USB_VENDOR_GRIFFIN, USB_PRODUCT_GRIFFIN_IMATE,
	    0,
	    "Griffin Technology",
	    "iMate, ADB adapter",
	},
	{
	    USB_VENDOR_FREECOM, USB_PRODUCT_FREECOM_DVD,
	    0,
	    "Freecom",
	    "Connector for DVD drive",
	},
	{
	    USB_VENDOR_FTDI, USB_PRODUCT_FTDI_SERIAL_8U100AX,
	    0,
	    "Future Technology Devices",
	    "8U100AX Serial converter",
	},
	{
	    USB_VENDOR_FTDI, USB_PRODUCT_FTDI_SERIAL_8U232AM,
	    0,
	    "Future Technology Devices",
	    "8U232AM Serial converter",
	},
	{
	    USB_VENDOR_FUJIPHOTO, USB_PRODUCT_FUJIPHOTO_MASS0100,
	    0,
	    "Fuji Photo Film",
	    "Mass Storage",
	},
	{
	    USB_VENDOR_FUJITSU, USB_PRODUCT_FUJITSU_AH_F401U,
	    0,
	    "Fujitsu",
	    "AH-F401U Air H device",
	},
	{
	    USB_VENDOR_GENERALINSTMNTS, USB_PRODUCT_GENERALINSTMNTS_SB5100,
	    0,
	    "General Instruments (Motorola)",
	    "SURFboard SB5100 Cable modem",
	},
	{
	    USB_VENDOR_GENESYS, USB_PRODUCT_GENESYS_GL650,
	    0,
	    "Genesys Logic",
	    "GL650 Hub",
	},
	{
	    USB_VENDOR_GENESYS, USB_PRODUCT_GENESYS_GL641USB,
	    0,
	    "Genesys Logic",
	    "GL641USB CompactFlash Card Reader",
	},
	{
	    USB_VENDOR_GENESYS, USB_PRODUCT_GENESYS_GL641USB2IDE,
	    0,
	    "Genesys Logic",
	    "GL641USB USB-IDE Bridge",
	},
	{
	    USB_VENDOR_HAL, USB_PRODUCT_HAL_IMR001,
	    0,
	    "HAL Corporation",
	    "Crossam2+USB IR commander",
	},
	{
	    USB_VENDOR_HAGIWARA, USB_PRODUCT_HAGIWARA_FGSM,
	    0,
	    "Hagiwara Sys-Com",
	    "FlashGate SmartMedia Card Reader",
	},
	{
	    USB_VENDOR_HAGIWARA, USB_PRODUCT_HAGIWARA_FGCF,
	    0,
	    "Hagiwara Sys-Com",
	    "FlashGate CompactFlash Card Reader",
	},
	{
	    USB_VENDOR_HAGIWARA, USB_PRODUCT_HAGIWARA_FG,
	    0,
	    "Hagiwara Sys-Com",
	    "FlashGate",
	},
	{
	    USB_VENDOR_HANDSPRING, USB_PRODUCT_HANDSPRING_VISOR,
	    0,
	    "Handspring",
	    "Handspring Visor",
	},
	{
	    USB_VENDOR_HANDSPRING, USB_PRODUCT_HANDSPRING_TREO,
	    0,
	    "Handspring",
	    "Handspring Treo",
	},
	{
	    USB_VENDOR_HAUPPAUGE, USB_PRODUCT_HAUPPAUGE_WINTV_USB_FM,
	    0,
	    "Hauppauge Computer Works",
	    "WinTV USB FM",
	},
	{
	    USB_VENDOR_HAWKING, USB_PRODUCT_HAWKING_UF100,
	    0,
	    "Hawking Technologies",
	    "10/100 USB Ethernet",
	},
	{
	    USB_VENDOR_HITACHI, USB_PRODUCT_HITACHI_DVDCAM_USB,
	    0,
	    "Hitachi, Ltd.",
	    "DVDCAM USB HS Interface",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_895C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 895C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_4100C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 4100C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_S20,
	    0,
	    "Hewlett Packard",
	    "Photosmart S20",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_880C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 880C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_4200C,
	    0,
	    "Hewlett Packard",
	    "ScanJet 4200C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_CDWRITERPLUS,
	    0,
	    "Hewlett Packard",
	    "CD-Writer Plus",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_KBDHUB,
	    0,
	    "Hewlett Packard",
	    "Multimedia Keyboard Hub",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_6200C,
	    0,
	    "Hewlett Packard",
	    "ScanJet 6200C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_S20b,
	    0,
	    "Hewlett Packard",
	    "PhotoSmart S20",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_815C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 815C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_3300C,
	    0,
	    "Hewlett Packard",
	    "ScanJet 3300C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_CDW8200,
	    0,
	    "Hewlett Packard",
	    "CD-Writer Plus 8200e",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_1220C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 1220C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_810C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 810C/812C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_4300C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 4300C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_G85XI,
	    0,
	    "Hewlett Packard",
	    "OfficeJet G85xi",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_1200,
	    0,
	    "Hewlett Packard",
	    "LaserJet 1200",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_5200C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 5200C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_830C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 830C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_3400CSE,
	    0,
	    "Hewlett Packard",
	    "ScanJet 3400cse",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_6300C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 6300C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_840C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 840c",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_2200C,
	    0,
	    "Hewlett Packard",
	    "ScanJet 2200C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_5300C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 5300C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_4400C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 4400C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_970CSE,
	    0,
	    "Hewlett Packard",
	    "Deskjet 970Cse",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_5400C,
	    0,
	    "Hewlett Packard",
	    "Scanjet 5400C",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_930C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 930c",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_P2000U,
	    0,
	    "Hewlett Packard",
	    "Inkjet P-2000U",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_640C,
	    0,
	    "Hewlett Packard",
	    "DeskJet 640c",
	},
	{
	    USB_VENDOR_HP, USB_PRODUCT_HP_P1100,
	    0,
	    "Hewlett Packard",
	    "Photosmart P1100",
	},
	{
	    USB_VENDOR_HP2, USB_PRODUCT_HP2_C500,
	    0,
	    "Hewlett Packard",
	    "PhotoSmart C500",
	},
	{
	    USB_VENDOR_IBM, USB_PRODUCT_IBM_USBCDROMDRIVE,
	    0,
	    "IBM Corporation",
	    "USB CD-ROM Drive",
	},
	{
	    USB_VENDOR_INSIDEOUT, USB_PRODUCT_INSIDEOUT_EDGEPORT4,
	    0,
	    "Inside Out Networks",
	    "EdgePort/4 serial ports",
	},
	{
	    USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_F5U002,
	    0,
	    "In-System Design",
	    "Parallel printer adapter",
	},
	{
	    USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_ATAPI,
	    0,
	    "In-System Design",
	    "ATAPI adapter",
	},
	{
	    USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_ISD110,
	    0,
	    "In-System Design",
	    "IDE adapter ISD110",
	},
	{
	    USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_ISD105,
	    0,
	    "In-System Design",
	    "IDE adapter ISD105",
	},
	{
	    USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_USBCABLE,
	    0,
	    "In-System Design",
	    "USB cable",
	},
	{
	    USB_VENDOR_INTEL, USB_PRODUCT_INTEL_EASYPC_CAMERA,
	    0,
	    "Intel",
	    "Easy PC Camera",
	},
	{
	    USB_VENDOR_INTEL, USB_PRODUCT_INTEL_TESTBOARD,
	    0,
	    "Intel",
	    "82930 test board",
	},
	{
	    USB_VENDOR_INTERSIL, USB_PRODUCT_INTERSIL_PRISM_2X,
	    0,
	    "Intersil",
	    "Prism2.x or Atmel WLAN",
	},
	{
	    USB_VENDOR_IODATA, USB_PRODUCT_IODATA_USBETT,
	    0,
	    "I/O Data",
	    "USB ETT",
	},
	{
	    USB_VENDOR_IODATA, USB_PRODUCT_IODATA_USBETTX,
	    0,
	    "I/O Data",
	    "USB ETTX",
	},
	{
	    USB_VENDOR_IODATA, USB_PRODUCT_IODATA_USBETTXS,
	    0,
	    "I/O Data",
	    "USB ETTX",
	},
	{
	    USB_VENDOR_IODATA, USB_PRODUCT_IODATA_USBRSAQ,
	    0,
	    "I/O Data",
	    "USB serial adapter USB-RSAQ1",
	},
	{
	    USB_VENDOR_IOMEGA, USB_PRODUCT_IOMEGA_ZIP100,
	    0,
	    "Iomega",
	    "Zip 100",
	},
	{
	    USB_VENDOR_IOMEGA, USB_PRODUCT_IOMEGA_ZIP250,
	    0,
	    "Iomega",
	    "Zip 250",
	},
	{
	    USB_VENDOR_JVC, USB_PRODUCT_JVC_GR_DX95,
	    0,
	    "JVC",
	    "GR-DX95",
	},
	{
	    USB_VENDOR_JRC, USB_PRODUCT_JRC_AH_J3001V_J3002V,
	    0,
	    "Japan Radio Company",
	    "AirH\" PHONE AH-J3001V/J3002V",
	},
	{
	    USB_VENDOR_KLSI, USB_PRODUCT_KLSI_DUH3E10BT,
	    0,
	    "Kawasaki LSI",
	    "USB ethernet controller engine",
	},
	{
	    USB_VENDOR_KLSI, USB_PRODUCT_KLSI_DUH3E10BTN,
	    0,
	    "Kawasaki LSI",
	    "USB ethernet controller engine",
	},
	{
	    USB_VENDOR_KAWATSU, USB_PRODUCT_KAWATSU_MH4000P,
	    0,
	    "Kawatsu Semiconductor",
	    "MiniHub 4000P",
	},
	{
	    USB_VENDOR_KEISOKUGIKEN, USB_PRODUCT_KEISOKUGIKEN_USBDAQ,
	    0,
	    "Keisokugiken",
	    "HKS-0200 USBDAQ",
	},
	{
	    USB_VENDOR_KLSI, USB_PRODUCT_KLSI_DUH3E10BT,
	    0,
	    "Kawasaki LSI",
	    "10BT Ethernet adapter, in the DU-H3E",
	},
	{
	    USB_VENDOR_KENSINGTON, USB_PRODUCT_KENSINGTON_ORBIT,
	    0,
	    "Kensington",
	    "Orbit USB/PS2 trackball",
	},
	{
	    USB_VENDOR_KENSINGTON, USB_PRODUCT_KENSINGTON_TURBOBALL,
	    0,
	    "Kensington",
	    "TurboBall",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA28,
	    0,
	    "Keyspan",
	    "USA-28 serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA28X,
	    0,
	    "Keyspan",
	    "USA-28X serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA19,
	    0,
	    "Keyspan",
	    "USA-19 serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA18X,
	    0,
	    "Keyspan",
	    "USA-18X serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA19W,
	    0,
	    "Keyspan",
	    "USA-19W serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA49W,
	    0,
	    "Keyspan",
	    "USA-49W serial adapter",
	},
	{
	    USB_VENDOR_KEYSPAN, USB_PRODUCT_KEYSPAN_USA19QW,
	    0,
	    "Keyspan",
	    "USA-19QW serial adapter",
	},
	{
	    USB_VENDOR_KINGSTON, USB_PRODUCT_KINGSTON_KNU101TX,
	    0,
	    "Kingston Technology",
	    "KNU101TX USB Ethernet",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC220,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC220",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC260,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC260",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC265,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC265",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC290,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC290",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC240,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC240",
	},
	{
	    USB_VENDOR_KODAK, USB_PRODUCT_KODAK_DC280,
	    0,
	    "Eastman Kodak",
	    "Digital Science DC280",
	},
	{
	    USB_VENDOR_KONICA, USB_PRODUCT_KONICA_CAMERA,
	    0,
	    "Konica",
	    "Digital Color Camera",
	},
	{
	    USB_VENDOR_KYE, USB_PRODUCT_KYE_NICHE,
	    0,
	    "KYE Systems",
	    "Niche mouse",
	},
	{
	    USB_VENDOR_KYE, USB_PRODUCT_KYE_NETSCROLL,
	    0,
	    "KYE Systems",
	    "Genius NetScroll mouse",
	},
	{
	    USB_VENDOR_KYE, USB_PRODUCT_KYE_FLIGHT2000,
	    0,
	    "KYE Systems",
	    "Flight 2000 joystick",
	},
	{
	    USB_VENDOR_KYE, USB_PRODUCT_KYE_VIVIDPRO,
	    0,
	    "KYE Systems",
	    "ColorPage Vivid-Pro scanner",
	},
	{
	    USB_VENDOR_LACIE, USB_PRODUCT_LACIE_HD,
	    0,
	    "LaCie",
	    "Hard Disk",
	},
	{
	    USB_VENDOR_LACIE, USB_PRODUCT_LACIE_CDRW,
	    0,
	    "LaCie",
	    "CD R/W",
	},
	{
	    USB_VENDOR_LEXAR, USB_PRODUCT_LEXAR_JUMPSHOT,
	    0,
	    "Lexar Media",
	    "jumpSHOT CompactFlash Reader",
	},
	{
	    USB_VENDOR_LEXMARK, USB_PRODUCT_LEXMARK_S2450,
	    0,
	    "Lexmark International",
	    "Optra S 2450",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_MAUSB2,
	    0,
	    "Linksys",
	    "Camedia MAUSB-2",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB10TX1,
	    0,
	    "Linksys",
	    "USB10TX",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB10T,
	    0,
	    "Linksys",
	    "USB10T Ethernet",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB100TX,
	    0,
	    "Linksys",
	    "USB100TX Ethernet",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB100H1,
	    0,
	    "Linksys",
	    "USB100H1 Ethernet/HPNA",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB10TA,
	    0,
	    "Linksys",
	    "USB10TA Ethernet",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB10TX2,
	    0,
	    "Linksys",
	    "USB10TX",
	},
	{
	    USB_VENDOR_LINKSYS, USB_PRODUCT_LINKSYS_USB200M,
	    0,
	    "Linksys",
	    "USB 2.0 10/100 ethernet controller",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_M2452,
	    0,
	    "Logitech",
	    "M2452 keyboard",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_M4848,
	    0,
	    "Logitech",
	    "M4848 mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_PAGESCAN,
	    0,
	    "Logitech",
	    "PageScan",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_QUICKCAMWEB,
	    0,
	    "Logitech",
	    "QuickCam Web",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_QUICKCAMPRO,
	    0,
	    "Logitech",
	    "QuickCam Pro",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_QUICKCAMEXP,
	    0,
	    "Logitech",
	    "QuickCam Express",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_QUICKCAM,
	    0,
	    "Logitech",
	    "QuickCam",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_N43,
	    0,
	    "Logitech",
	    "N43",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_N48,
	    0,
	    "Logitech",
	    "N48 mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_MBA47,
	    0,
	    "Logitech",
	    "M-BA47 mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_WMMOUSE,
	    0,
	    "Logitech",
	    "WingMan Gaming Mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_BD58,
	    0,
	    "Logitech",
	    "BD58 mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_UN58A,
	    0,
	    "Logitech",
	    "iFeel Mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_BB13,
	    0,
	    "Logitech",
	    "USB-PS/2 Trackball",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_WMPAD,
	    0,
	    "Logitech",
	    "WingMan GamePad Extreme",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_WMRPAD,
	    0,
	    "Logitech",
	    "WingMan RumblePad",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_WMJOY,
	    0,
	    "Logitech",
	    "WingMan Force joystick",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_RK53,
	    0,
	    "Logitech",
	    "Cordless mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_RB6,
	    0,
	    "Logitech",
	    "Cordless keyboard",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_MX700,
	    0,
	    "Logitech",
	    "Cordless optical mouse",
	},
	{
	    USB_VENDOR_LOGITECH, USB_PRODUCT_LOGITECH_QUICKCAMPRO2,
	    0,
	    "Logitech",
	    "QuickCam Pro",
	},
	{
	    USB_VENDOR_LUCENT, USB_PRODUCT_LUCENT_EVALKIT,
	    0,
	    "Lucent",
	    "USS-720 evaluation kit",
	},
	{
	    USB_VENDOR_LUWEN, USB_PRODUCT_LUWEN_EASYDISK,
	    0,
	    "Luwen",
	    "EasyDisc",
	},
	{
	    USB_VENDOR_MACALLY, USB_PRODUCT_MACALLY_MOUSE1,
	    0,
	    "Macally",
	    "mouse",
	},
	{
	    USB_VENDOR_MCT, USB_PRODUCT_MCT_HUB0100,
	    0,
	    "MCT",
	    "Hub",
	},
	{
	    USB_VENDOR_MCT, USB_PRODUCT_MCT_DU_H3SP_USB232,
	    0,
	    "MCT",
	    "D-Link DU-H3SP USB BAY Hub",
	},
	{
	    USB_VENDOR_MCT, USB_PRODUCT_MCT_USB232,
	    0,
	    "MCT",
	    "USB-232 Interface",
	},
	{
	    USB_VENDOR_MCT, USB_PRODUCT_MCT_SITECOM_USB232,
	    0,
	    "MCT",
	    "Sitecom USB-232 Products",
	},
	{
	    USB_VENDOR_MELCO, USB_PRODUCT_MELCO_LUATX1,
	    0,
	    "Melco",
	    "LUA-TX Ethernet",
	},
	{
	    USB_VENDOR_MELCO, USB_PRODUCT_MELCO_LUATX5,
	    0,
	    "Melco",
	    "LUA-TX Ethernet",
	},
	{
	    USB_VENDOR_MELCO, USB_PRODUCT_MELCO_LUA2TX5,
	    0,
	    "Melco",
	    "LUA2-TX Ethernet",
	},
	{
	    USB_VENDOR_MELCO, USB_PRODUCT_MELCO_LUAKTX,
	    0,
	    "Melco",
	    "LUA-KTX Ethernet",
	},
	{
	    USB_VENDOR_MELCO, USB_PRODUCT_MELCO_DUBPXXG,
	    0,
	    "Melco",
	    "USB-IDE Bridge: DUB-PxxG",
	},
	{
	    USB_VENDOR_METRICOM, USB_PRODUCT_METRICOM_RICOCHET_GS,
	    0,
	    "Metricom",
	    "Ricochet GS",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_SIDEPREC,
	    0,
	    "Microsoft",
	    "SideWinder Precision Pro",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_INTELLIMOUSE,
	    0,
	    "Microsoft",
	    "IntelliMouse",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_NATURALKBD,
	    0,
	    "Microsoft",
	    "Natural Keyboard Elite",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_DDS80,
	    0,
	    "Microsoft",
	    "Digital Sound System 80",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_SIDEWINDER,
	    0,
	    "Microsoft",
	    "Sidewinder Precision Racing Wheel",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_INETPRO,
	    0,
	    "Microsoft",
	    "Internet Keyboard Pro",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_INTELLIEYE,
	    0,
	    "Microsoft",
	    "IntelliEye mouse",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_INETPRO2,
	    0,
	    "Microsoft",
	    "Internet Keyboard Pro",
	},
	{
	    USB_VENDOR_MICROSOFT, USB_PRODUCT_MICROSOFT_MN110,
	    0,
	    "Microsoft",
	    "10/100 USB NIC",
	},
	{
	    USB_VENDOR_MICROTECH, USB_PRODUCT_MICROTECH_SCSIDB25,
	    0,
	    "Microtech",
	    "USB-SCSI-DB25",
	},
	{
	    USB_VENDOR_MICROTECH, USB_PRODUCT_MICROTECH_SCSIHD50,
	    0,
	    "Microtech",
	    "USB-SCSI-HD50",
	},
	{
	    USB_VENDOR_MICROTECH, USB_PRODUCT_MICROTECH_DPCM,
	    0,
	    "Microtech",
	    "USB CameraMate",
	},
	{
	    USB_VENDOR_MICROTECH, USB_PRODUCT_MICROTECH_FREECOM,
	    0,
	    "Microtech",
	    "Freecom USB-IDE",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_336CX,
	    0,
	    "Microtek",
	    "Phantom 336CX - C3 scanner",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_X6U,
	    0,
	    "Microtek",
	    "ScanMaker X6 - X6U",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_C6,
	    0,
	    "Microtek",
	    "Phantom C6 scanner",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_336CX2,
	    0,
	    "Microtek",
	    "Phantom 336CX - C3 scanner",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_V6USL,
	    0,
	    "Microtek",
	    "ScanMaker V6USL",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_V6USL2,
	    0,
	    "Microtek",
	    "ScanMaker V6USL",
	},
	{
	    USB_VENDOR_MICROTEK, USB_PRODUCT_MICROTEK_V6UL,
	    0,
	    "Microtek",
	    "ScanMaker V6UL",
	},
	{
	    USB_VENDOR_MIDIMAN, USB_PRODUCT_MIDIMAN_MIDISPORT2X2,
	    0,
	    "Midiman",
	    "Midisport 2x2",
	},
	{
	    USB_VENDOR_MINOLTA, USB_PRODUCT_MINOLTA_2300,
	    0,
	    "Minolta",
	    "Dimage 2300",
	},
	{
	    USB_VENDOR_MINOLTA, USB_PRODUCT_MINOLTA_S304,
	    0,
	    "Minolta",
	    "Dimage S304",
	},
	{
	    USB_VENDOR_MINOLTA, USB_PRODUCT_MINOLTA_X,
	    0,
	    "Minolta",
	    "Dimage X",
	},
	{
	    USB_VENDOR_MITSUMI, USB_PRODUCT_MITSUMI_CDRRW,
	    0,
	    "Mitsumi",
	    "CD-R/RW Drive",
	},
	{
	    USB_VENDOR_MOTOROLA, USB_PRODUCT_MOTOROLA_MC141555,
	    0,
	    "Motorola",
	    "MC141555 hub controller",
	},
	{
	    USB_VENDOR_MOTOROLA, USB_PRODUCT_MOTOROLA_SB4100,
	    0,
	    "Motorola",
	    "SB4100 USB Cable Modem",
	},
	{
	    USB_VENDOR_MULTITECH, USB_PRODUCT_MULTITECH_ATLAS,
	    0,
	    "MultiTech",
	    "MT5634ZBA-USB modem",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_1200CU,
	    0,
	    "Mustek Systems",
	    "1200 CU scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_600CU,
	    0,
	    "Mustek Systems",
	    "600 CU scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_1200USB,
	    0,
	    "Mustek Systems",
	    "1200 USB scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_1200UB,
	    0,
	    "Mustek Systems",
	    "1200 UB scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_1200USBPLUS,
	    0,
	    "Mustek Systems",
	    "1200 USB Plus scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_1200CUPLUS,
	    0,
	    "Mustek Systems",
	    "1200 CU Plus scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_BEARPAW1200F,
	    0,
	    "Mustek Systems",
	    "BearPaw 1200F scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_BEARPAW1200TA,
	    0,
	    "Mustek Systems",
	    "BearPaw 1200TA scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_600USB,
	    0,
	    "Mustek Systems",
	    "600 USB scanner",
	},
	{
	    USB_VENDOR_MUSTEK, USB_PRODUCT_MUSTEK_MDC800,
	    0,
	    "Mustek Systems",
	    "MDC-800 digital camera",
	},
	{
	    USB_VENDOR_MSYSTEMS, USB_PRODUCT_MSYSTEMS_DISKONKEY,
	    0,
	    "M-Systems",
	    "DiskOnKey",
	},
	{
	    USB_VENDOR_NATIONAL, USB_PRODUCT_NATIONAL_BEARPAW1200,
	    0,
	    "National Semiconductor",
	    "BearPaw 1200",
	},
	{
	    USB_VENDOR_NATIONAL, USB_PRODUCT_NATIONAL_BEARPAW2400,
	    0,
	    "National Semiconductor",
	    "BearPaw 2400",
	},
	{
	    USB_VENDOR_NEC, USB_PRODUCT_NEC_HUB,
	    0,
	    "NEC",
	    "hub",
	},
	{
	    USB_VENDOR_NEC, USB_PRODUCT_NEC_HUB_B,
	    0,
	    "NEC",
	    "hub",
	},
	{
	    USB_VENDOR_NEODIO, USB_PRODUCT_NEODIO_ND5010,
	    0,
	    "Neodio",
	    "Multi-format Flash Controller",
	},
	{
	    USB_VENDOR_NETCHIP, USB_PRODUCT_NETCHIP_TURBOCONNECT,
	    0,
	    "NetChip Technology",
	    "Turbo-Connect",
	},
	{
	    USB_VENDOR_NETGEAR, USB_PRODUCT_NETGEAR_EA101,
	    0,
	    "BayNETGEAR",
	    "Ethernet adapter",
	},
	{
	    USB_VENDOR_NETGEAR, USB_PRODUCT_NETGEAR_FA120,
	    0,
	    "BayNETGEAR",
	    "USB 2.0 Ethernet adapter",
	},
	{
	    USB_VENDOR_NIKON, USB_PRODUCT_NIKON_E990,
	    0,
	    "Nikon",
	    "Digital Camera E990",
	},
	{
	    USB_VENDOR_OLYMPUS, USB_PRODUCT_OLYMPUS_C1,
	    0,
	    "Olympus",
	    "C-1 Digital Camera",
	},
	{
	    USB_VENDOR_OLYMPUS, USB_PRODUCT_OLYMPUS_C700,
	    0,
	    "Olympus",
	    "C-700 Ultra Zoom",
	},
	{
	    USB_VENDOR_OMNIVISION, USB_PRODUCT_OMNIVISION_OV511,
	    0,
	    "OmniVision",
	    "OV511 Camera",
	},
	{
	    USB_VENDOR_OMNIVISION, USB_PRODUCT_OMNIVISION_OV511PLUS,
	    0,
	    "OmniVision",
	    "OV511+ Camera",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_SERIAL,
	    0,
	    "Palm Computing",
	    "USB Serial Adaptor",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_M500,
	    0,
	    "Palm Computing",
	    "Palm m500",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_M505,
	    0,
	    "Palm Computing",
	    "Palm m505",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_M515,
	    0,
	    "Palm Computing",
	    "Palm m515",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_I705,
	    0,
	    "Palm Computing",
	    "Palm i705",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_TUNGSTEN_Z,
	    0,
	    "Palm Computing",
	    "Palm Tungsten Z",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_M125,
	    0,
	    "Palm Computing",
	    "Palm m125",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_M130,
	    0,
	    "Palm Computing",
	    "Palm m130",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_TUNGSTEN_T,
	    0,
	    "Palm Computing",
	    "Palm Tungsten T",
	},
	{
	    USB_VENDOR_PALM, USB_PRODUCT_PALM_ZIRE,
	    0,
	    "Palm Computing",
	    "Palm Zire",
	},
	{
	    USB_VENDOR_PANASONIC, USB_PRODUCT_PANASONIC_SDCAAE,
	    0,
	    "Panasonic (Matsushita)",
	    "MultiMediaCard Adapter",
	},
	{
	    USB_VENDOR_PERACOM, USB_PRODUCT_PERACOM_SERIAL1,
	    0,
	    "Peracom Networks",
	    "Serial Converter",
	},
	{
	    USB_VENDOR_PERACOM, USB_PRODUCT_PERACOM_ENET,
	    0,
	    "Peracom Networks",
	    "Ethernet adapter",
	},
	{
	    USB_VENDOR_PERACOM, USB_PRODUCT_PERACOM_ENET3,
	    0,
	    "Peracom Networks",
	    "At Home Ethernet Adapter",
	},
	{
	    USB_VENDOR_PERACOM, USB_PRODUCT_PERACOM_ENET2,
	    0,
	    "Peracom Networks",
	    "Ethernet adapter",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_DSS350,
	    0,
	    "Philips",
	    "DSS 350 Digital Speaker System",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_DSS,
	    0,
	    "Philips",
	    "DSS XXX Digital Speaker System",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_HUB,
	    0,
	    "Philips",
	    "hub",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_PCA646VC,
	    0,
	    "Philips",
	    "PCA646VC PC Camera",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_PCVC680K,
	    0,
	    "Philips",
	    "PCVC680K Vesta Pro PC Camera",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_DSS150,
	    0,
	    "Philips",
	    "DSS 150 Digital Speaker System",
	},
	{
	    USB_VENDOR_PHILIPS, USB_PRODUCT_PHILIPS_DIVAUSB,
	    0,
	    "Philips",
	    "DIVA USB mp3 player",
	},
	{
	    USB_VENDOR_PHILIPSSEMI, USB_PRODUCT_PHILIPSSEMI_HUB1122,
	    0,
	    "Philips Semiconductors",
	    "hub",
	},
	{
	    USB_VENDOR_PIENGINEERING, USB_PRODUCT_PIENGINEERING_PS2USB,
	    0,
	    "P.I. Engineering",
	    "PS2 to Mac USB Adapter",
	},
	{
	    USB_VENDOR_PLX, USB_PRODUCT_PLX_TESTBOARD,
	    0,
	    "PLX",
	    "test board",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2X300,
	    0,
	    "Primax Electronics",
	    "G2-200 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2E300,
	    0,
	    "Primax Electronics",
	    "G2E-300 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2300,
	    0,
	    "Primax Electronics",
	    "G2-300 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2E3002,
	    0,
	    "Primax Electronics",
	    "G2E-300 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_9600,
	    0,
	    "Primax Electronics",
	    "Colorado USB 9600 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_600U,
	    0,
	    "Primax Electronics",
	    "Colorado 600u scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_6200,
	    0,
	    "Primax Electronics",
	    "Visioneer 6200 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_19200,
	    0,
	    "Primax Electronics",
	    "Colorado USB 19200 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_1200U,
	    0,
	    "Primax Electronics",
	    "Colorado 1200u scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G600,
	    0,
	    "Primax Electronics",
	    "G2-600 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_636I,
	    0,
	    "Primax Electronics",
	    "ReadyScan 636i",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2600,
	    0,
	    "Primax Electronics",
	    "G2-600 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_G2E600,
	    0,
	    "Primax Electronics",
	    "G2E-600 scanner",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_COMFORT,
	    0,
	    "Primax Electronics",
	    "Comfort",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_MOUSEINABOX,
	    0,
	    "Primax Electronics",
	    "Mouse-in-a-Box",
	},
	{
	    USB_VENDOR_PRIMAX, USB_PRODUCT_PRIMAX_PCGAUMS1,
	    0,
	    "Primax Electronics",
	    "Sony PCGA-UMS1",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_PL2301,
	    0,
	    "Prolific Technology",
	    "PL2301 Host-Host interface",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_PL2302,
	    0,
	    "Prolific Technology",
	    "PL2302 Host-Host interface",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_RSAQ2,
	    0,
	    "Prolific Technology",
	    "PL2303 Serial adapter (IODATA USB-RSAQ2)",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_PL2303,
	    0,
	    "Prolific Technology",
	    "PL2303 Serial adapter (ATEN/IOGEAR UC232A)",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_PL2305,
	    0,
	    "Prolific Technology",
	    "Parallel printer adapter",
	},
	{
	    USB_VENDOR_PROLIFIC, USB_PRODUCT_PROLIFIC_ATAPI4,
	    0,
	    "Prolific Technology",
	    "ATAPI-4 Bridge Controller",
	},
	{
	    USB_VENDOR_PUTERCOM, USB_PRODUCT_PUTERCOM_UPA100,
	    0,
	    "Putercom",
	    "USB-1284 BRIDGE",
	},
	{
	    USB_VENDOR_QTRONIX, USB_PRODUCT_QTRONIX_980N,
	    0,
	    "Qtronix",
	    "Scorpion-980N keyboard",
	},
	{
	    USB_VENDOR_QUICKSHOT, USB_PRODUCT_QUICKSHOT_STRIKEPAD,
	    0,
	    "Quickshot",
	    "USB StrikePad",
	},
	{
	    USB_VENDOR_RAINBOW, USB_PRODUCT_RAINBOW_IKEY2000,
	    0,
	    "Rainbow Technologies",
	    "i-Key 2000",
	},
	{
	    USB_VENDOR_REALTEK, USB_PRODUCT_REALTEK_USBKR100,
	    0,
	    "RealTek",
	    "USBKR100 USB Ethernet (GREEN HOUSE)",
	},
	{
	    USB_VENDOR_ROLAND, USB_PRODUCT_ROLAND_UM1,
	    0,
	    "Roland",
	    "UM-1 MIDI I/F",
	},
	{
	    USB_VENDOR_ROLAND, USB_PRODUCT_ROLAND_UM880N,
	    0,
	    "Roland",
	    "EDIROL UM-880 MIDI I/F (native)",
	},
	{
	    USB_VENDOR_ROLAND, USB_PRODUCT_ROLAND_UM880G,
	    0,
	    "Roland",
	    "EDIROL UM-880 MIDI I/F (generic)",
	},
	{
	    USB_VENDOR_ROCKFIRE, USB_PRODUCT_ROCKFIRE_GAMEPAD,
	    0,
	    "Rockfire",
	    "gamepad 203USB",
	},
	{
	    USB_VENDOR_RATOC, USB_PRODUCT_RATOC_REXUSB60,
	    0,
	    "RATOC Systems, Inc.",
	    "USB serial adapter REX-USB60",
	},
	{
	    USB_VENDOR_SAMSUNG, USB_PRODUCT_SAMSUNG_ML6060,
	    0,
	    "Samsung Electronics",
	    "ML-6060 laser printer",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR05A,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-05a",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR05,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-05",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR31,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-31",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR12,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-12",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR09,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-09",
	},
	{
	    USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDDR75,
	    0,
	    "SanDisk Corp",
	    "ImageMate SDDR-75",
	},
	{
	    USB_VENDOR_SANYO, USB_PRODUCT_SANYO_SCP4900,
	    0,
	    "Sanyo Electric",
	    "Sanyo SCP-4900 USB Phone",
	},
	{
	    USB_VENDOR_SCANLOGIC, USB_PRODUCT_SCANLOGIC_SL11R,
	    0,
	    "ScanLogic",
	    "SL11R IDE Adapter",
	},
	{
	    USB_VENDOR_SCANLOGIC, USB_PRODUCT_SCANLOGIC_336CX,
	    0,
	    "ScanLogic",
	    "Phantom 336CX - C3 scanner",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSB,
	    0,
	    "Shuttle Technology",
	    "E-USB Bridge",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSCSI,
	    0,
	    "Shuttle Technology",
	    "eUSCSI Bridge",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_SDDR09,
	    0,
	    "Shuttle Technology",
	    "ImageMate SDDR09",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_ZIOMMC,
	    0,
	    "Shuttle Technology",
	    "eUSB MultiMediaCard Adapter",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_HIFD,
	    0,
	    "Shuttle Technology",
	    "Sony Hifd",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSBATAPI,
	    0,
	    "Shuttle Technology",
	    "eUSB ATA/ATAPI Adapter",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_CF,
	    0,
	    "Shuttle Technology",
	    "eUSB CompactFlash Adapter",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSCSI_B,
	    0,
	    "Shuttle Technology",
	    "eUSCSI Bridge",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSCSI_C,
	    0,
	    "Shuttle Technology",
	    "eUSCSI Bridge",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_CDRW,
	    0,
	    "Shuttle Technology",
	    "CD-RW Device",
	},
	{
	    USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSBORCA,
	    0,
	    "Shuttle Technology",
	    "eUSB ORCA Quad Reader",
	},
	{
	    USB_VENDOR_SIEMENS, USB_PRODUCT_SIEMENS_SPEEDSTREAM,
	    0,
	    "Siemens",
	    "SpeedStream USB",
	},
	{
	    USB_VENDOR_SIGMATEL, USB_PRODUCT_SIGMATEL_I_BEAD100,
	    0,
	    "Sigmatel",
	    "i-Bead 100 MP3 Player",
	},
	{
	    USB_VENDOR_SIIG, USB_PRODUCT_SIIG_DIGIFILMREADER,
	    0,
	    "SIIG",
	    "DigiFilm-Combo Reader",
	},
	{
	    USB_VENDOR_SILICONPORTALS, USB_PRODUCT_SILICONPORTALS_YAPPH_NF,
	    0,
	    "Silicon Portals",
	    "YAP Phone (no firmware)",
	},
	{
	    USB_VENDOR_SILICONPORTALS, USB_PRODUCT_SILICONPORTALS_YAPPHONE,
	    0,
	    "Silicon Portals",
	    "YAP Phone",
	},
	{
	    USB_VENDOR_SIRIUS, USB_PRODUCT_SIRIUS_ROADSTER,
	    0,
	    "Sirius Technologies",
	    "NetComm Roadster II 56 USB",
	},
	{
	    USB_VENDOR_SMARTBRIDGES, USB_PRODUCT_SMARTBRIDGES_SMARTLINK,
	    0,
	    "SmartBridges",
	    "SmartLink USB ethernet adapter",
	},
	{
	    USB_VENDOR_SMARTBRIDGES, USB_PRODUCT_SMARTBRIDGES_SMARTNIC,
	    0,
	    "SmartBridges",
	    "smartNIC 2 PnP Adapter",
	},
	{
	    USB_VENDOR_SMC, USB_PRODUCT_SMC_2102USB,
	    0,
	    "Standard Microsystems",
	    "10Mbps ethernet adapter",
	},
	{
	    USB_VENDOR_SMC, USB_PRODUCT_SMC_2202USB,
	    0,
	    "Standard Microsystems",
	    "10/100 ethernet adapter",
	},
	{
	    USB_VENDOR_SMC, USB_PRODUCT_SMC_2206USB,
	    0,
	    "Standard Microsystems",
	    "EZ Connect USB Ethernet Adapter",
	},
	{
	    USB_VENDOR_SMC2, USB_PRODUCT_SMC2_2020HUB,
	    0,
	    "Standard Microsystems",
	    "USB Hub",
	},
	{
	    USB_VENDOR_SMC3, USB_PRODUCT_SMC3_2662WUSB,
	    0,
	    "Standard Microsystems",
	    "2662W-AR Wireless Adapter",
	},
	{
	    USB_VENDOR_SOHOWARE, USB_PRODUCT_SOHOWARE_NUB100,
	    0,
	    "SOHOware",
	    "10/100 USB Ethernet",
	},
	{
	    USB_VENDOR_SOLIDYEAR, USB_PRODUCT_SOLIDYEAR_KEYBOARD,
	    0,
	    "Solid Year",
	    "Solid Year USB keyboard",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_DSC,
	    0,
	    "Sony",
	    "DSC cameras",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_MSACUS1,
	    0,
	    "Sony",
	    "Memorystick MSAC-US1",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_MSC,
	    0,
	    "Sony",
	    "MSC memory stick slot",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_35,
	    0,
	    "Sony",
	    "Sony Clie v3.5",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_40,
	    0,
	    "Sony",
	    "Sony Clie v4.0",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_40_MS,
	    0,
	    "Sony",
	    "Sony Clie v4.0 Memory Stick slot",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_S360,
	    0,
	    "Sony",
	    "Sony Clie s360",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_41_MS,
	    0,
	    "Sony",
	    "Sony Clie v4.1 Memory Stick slot",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_41,
	    0,
	    "Sony",
	    "Sony Clie v4.1",
	},
	{
	    USB_VENDOR_SONY, USB_PRODUCT_SONY_CLIE_NX60,
	    0,
	    "Sony",
	    "Sony Clie nx60",
	},
	{
	    USB_VENDOR_SOURCENEXT, USB_PRODUCT_SOURCENEXT_KEIKAI8,
	    0,
	    "SOURCENEXT",
	    "KeikaiDenwa 8",
	},
	{
	    USB_VENDOR_SOURCENEXT, USB_PRODUCT_SOURCENEXT_KEIKAI8_CHG,
	    0,
	    "SOURCENEXT",
	    "KeikaiDenwa 8 with charger",
	},
	{
	    USB_VENDOR_STMICRO, USB_PRODUCT_STMICRO_COMMUNICATOR,
	    0,
	    "STMicroelectronics",
	    "USB Communicator",
	},
	{
	    USB_VENDOR_STSN, USB_PRODUCT_STSN_STSN0001,
	    0,
	    "STSN",
	    "Internet Access Device",
	},
	{
	    USB_VENDOR_SUNTAC, USB_PRODUCT_SUNTAC_DS96L,
	    0,
	    "SUN Corporation",
	    "SUNTAC U-Cable type D2",
	},
	{
	    USB_VENDOR_SUNTAC, USB_PRODUCT_SUNTAC_IS96U,
	    0,
	    "SUN Corporation",
	    "SUNTAC Ir-Trinity",
	},
	{
	    USB_VENDOR_SUNTAC, USB_PRODUCT_SUNTAC_PS64P1,
	    0,
	    "SUN Corporation",
	    "SUNTAC U-Cable type P1",
	},
	{
	    USB_VENDOR_SUNTAC, USB_PRODUCT_SUNTAC_VS10U,
	    0,
	    "SUN Corporation",
	    "SUNTAC Slipper U",
	},
	{
	    USB_VENDOR_SUN, USB_PRODUCT_SUN_KEYBOARD,
	    0,
	    "Sun Microsystems",
	    "Type 6 USB keyboard",
	},
	{
	    USB_VENDOR_SUN, USB_PRODUCT_SUN_MOUSE,
	    0,
	    "Sun Microsystems",
	    "Type 6 USB mouse",
	},
	{
	    USB_VENDOR_DIAMOND2, USB_PRODUCT_DIAMOND2_SUPRAEXPRESS56K,
	    0,
	    "Diamond (Supra)",
	    "Supra Express 56K modem",
	},
	{
	    USB_VENDOR_DIAMOND2, USB_PRODUCT_DIAMOND2_SUPRA2890,
	    0,
	    "Diamond (Supra)",
	    "SupraMax 2890 56K Modem",
	},
	{
	    USB_VENDOR_DIAMOND2, USB_PRODUCT_DIAMOND2_RIO600USB,
	    0,
	    "Diamond (Supra)",
	    "Rio 600 USB",
	},
	{
	    USB_VENDOR_DIAMOND2, USB_PRODUCT_DIAMOND2_RIO800USB,
	    0,
	    "Diamond (Supra)",
	    "Rio 800 USB",
	},
	{
	    USB_VENDOR_TAUGA, USB_PRODUCT_TAUGA_CAMERAMATE,
	    0,
	    "Taugagreining HF",
	    "CameraMate (DPCM_USB)",
	},
	{
	    USB_VENDOR_TDK, USB_PRODUCT_TDK_UPA9664,
	    0,
	    "TDK",
	    "USB-PDC Adapter UPA9664",
	},
	{
	    USB_VENDOR_TDK, USB_PRODUCT_TDK_UCA1464,
	    0,
	    "TDK",
	    "USB-cdmaOne Adapter UCA1464",
	},
	{
	    USB_VENDOR_TDK, USB_PRODUCT_TDK_UHA6400,
	    0,
	    "TDK",
	    "USB-PHS Adapter UHA6400",
	},
	{
	    USB_VENDOR_TDK, USB_PRODUCT_TDK_UPA6400,
	    0,
	    "TDK",
	    "USB-PHS Adapter UPA6400",
	},
	{
	    USB_VENDOR_TEAC, USB_PRODUCT_TEAC_FD05PUB,
	    0,
	    "TEAC",
	    "FD-05PUB floppy",
	},
	{
	    USB_VENDOR_TELEX, USB_PRODUCT_TELEX_MIC1,
	    0,
	    "Telex Communications",
	    "Enhanced USB Microphone",
	},
	{
	    USB_VENDOR_TI, USB_PRODUCT_TI_UTUSB41,
	    0,
	    "Texas Instruments",
	    "UT-USB41 hub",
	},
	{
	    USB_VENDOR_TI, USB_PRODUCT_TI_TUSB2046,
	    0,
	    "Texas Instruments",
	    "TUSB2046 hub",
	},
	{
	    USB_VENDOR_THRUST, USB_PRODUCT_THRUST_FUSION_PAD,
	    0,
	    "Thrustmaster",
	    "Fusion Digital Gamepad",
	},
	{
	    USB_VENDOR_TOSHIBA, USB_PRODUCT_TOSHIBA_POCKETPC_E740,
	    0,
	    "Toshiba Corporation",
	    "PocketPC e740",
	},
	{
	    USB_VENDOR_TREK, USB_PRODUCT_TREK_THUMBDRIVE,
	    0,
	    "Trek Technology",
	    "ThumbDrive",
	},
	{
	    USB_VENDOR_TREK, USB_PRODUCT_TREK_THUMBDRIVE_8MB,
	    0,
	    "Trek Technology",
	    "ThumbDrive_8MB",
	},
	{
	    USB_VENDOR_ULTIMA, USB_PRODUCT_ULTIMA_1200UBPLUS,
	    0,
	    "Ultima",
	    "1200 UB Plus scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA1236U,
	    0,
	    "UMAX Data Systems",
	    "Astra 1236U Scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA1220U,
	    0,
	    "UMAX Data Systems",
	    "Astra 1220U Scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA2000U,
	    0,
	    "UMAX Data Systems",
	    "Astra 2000U Scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA2100U,
	    0,
	    "UMAX Data Systems",
	    "Astra 2100U Scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA2200U,
	    0,
	    "UMAX Data Systems",
	    "Astra 2200U Scanner",
	},
	{
	    USB_VENDOR_UMAX, USB_PRODUCT_UMAX_ASTRA3400,
	    0,
	    "UMAX Data Systems",
	    "Astra 3400 Scanner",
	},
	{
	    USB_VENDOR_UNIACCESS, USB_PRODUCT_UNIACCESS_PANACHE,
	    0,
	    "Universal Access",
	    "Panache Surf USB ISDN Adapter",
	},
	{
	    USB_VENDOR_VIDZMEDIA, USB_PRODUCT_VIDZMEDIA_MONSTERTV,
	    0,
	    "VidzMedia Pte Ltd",
	    "MonsterTV P2H",
	},
	{
	    USB_VENDOR_VISION, USB_PRODUCT_VISION_VC6452V002,
	    0,
	    "VLSI Vision",
	    "CPiA Camera",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_7600,
	    0,
	    "Visioneer",
	    "OneTouch 7600",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_5300,
	    0,
	    "Visioneer",
	    "OneTouch 5300",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_3000,
	    0,
	    "Visioneer",
	    "Scanport 3000",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_6100,
	    0,
	    "Visioneer",
	    "OneTouch 6100",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_6200,
	    0,
	    "Visioneer",
	    "OneTouch 6200",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_8100,
	    0,
	    "Visioneer",
	    "OneTouch 8100",
	},
	{
	    USB_VENDOR_VISIONEER, USB_PRODUCT_VISIONEER_8600,
	    0,
	    "Visioneer",
	    "OneTouch 8600",
	},
	{
	    USB_VENDOR_WACOM, USB_PRODUCT_WACOM_CT0405U,
	    0,
	    "WACOM",
	    "CT-0405-U Tablet",
	},
	{
	    USB_VENDOR_WACOM, USB_PRODUCT_WACOM_GRAPHIRE,
	    0,
	    "WACOM",
	    "Graphire",
	},
	{
	    USB_VENDOR_WACOM, USB_PRODUCT_WACOM_INTUOSA5,
	    0,
	    "WACOM",
	    "Intuos A5",
	},
	{
	    USB_VENDOR_WACOM, USB_PRODUCT_WACOM_GD0912U,
	    0,
	    "WACOM",
	    "Intuos 9x12 Graphics Tablet",
	},
	{
	    USB_VENDOR_XIRLINK, USB_PRODUCT_XIRLINK_PCCAM,
	    0,
	    "Xirlink",
	    "IBM PC Camera",
	},
	{
	    USB_VENDOR_YEDATA, USB_PRODUCT_YEDATA_FLASHBUSTERU,
	    0,
	    "Y-E Data",
	    "Flashbuster-U",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_UX256,
	    0,
	    "YAMAHA",
	    "UX256 MIDI I/F",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_UX96,
	    0,
	    "YAMAHA",
	    "UX96 MIDI I/F",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_RTA54I,
	    0,
	    "YAMAHA",
	    "NetVolante RTA54i Broadband&ISDN Router",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_RTA55I,
	    0,
	    "YAMAHA",
	    "NetVolante RTA55i Broadband VoIP Router",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_RTW65B,
	    0,
	    "YAMAHA",
	    "NetVolante RTW65b Broadband Wireless Router",
	},
	{
	    USB_VENDOR_YAMAHA, USB_PRODUCT_YAMAHA_RTW65I,
	    0,
	    "YAMAHA",
	    "NetVolante RTW65i Broadband&ISDN Wireless Router",
	},
	{
	    USB_VENDOR_YANO, USB_PRODUCT_YANO_U640MO,
	    0,
	    "Yano",
	    "U640MO-03",
	},
	{
	    USB_VENDOR_ZOOM, USB_PRODUCT_ZOOM_2986L,
	    0,
	    "Zoom Telephonics",
	    "2986L Fax modem",
	},
	{
	    USB_VENDOR_ZYXEL, USB_PRODUCT_ZYXEL_OMNI56K,
	    0,
	    "ZyXEL Communication",
	    "Omni 56K Plus",
	},
	{
	    USB_VENDOR_ZYXEL, USB_PRODUCT_ZYXEL_980N,
	    0,
	    "ZyXEL Communication",
	    "Scorpion-980N keyboard",
	},
	{
	    USB_VENDOR_AOX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AOX",
	    NULL,
	},
	{
	    USB_VENDOR_ATMEL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Atmel",
	    NULL,
	},
	{
	    USB_VENDOR_MITSUMI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Mitsumi",
	    NULL,
	},
	{
	    USB_VENDOR_HP, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hewlett Packard",
	    NULL,
	},
	{
	    USB_VENDOR_ADAPTEC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Adaptec",
	    NULL,
	},
	{
	    USB_VENDOR_NATIONAL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "National Semiconductor",
	    NULL,
	},
	{
	    USB_VENDOR_ACERLABS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Acer Labs",
	    NULL,
	},
	{
	    USB_VENDOR_FTDI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Future Technology Devices",
	    NULL,
	},
	{
	    USB_VENDOR_NEC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "NEC",
	    NULL,
	},
	{
	    USB_VENDOR_KODAK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Eastman Kodak",
	    NULL,
	},
	{
	    USB_VENDOR_MELCO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Melco",
	    NULL,
	},
	{
	    USB_VENDOR_CREATIVE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Creative",
	    NULL,
	},
	{
	    USB_VENDOR_ADI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ADI Systems",
	    NULL,
	},
	{
	    USB_VENDOR_CATC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Computer Access Technology",
	    NULL,
	},
	{
	    USB_VENDOR_SMC2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Standard Microsystems",
	    NULL,
	},
	{
	    USB_VENDOR_GRAVIS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Advanced Gravis Computer Tech.",
	    NULL,
	},
	{
	    USB_VENDOR_SUN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Sun Microsystems",
	    NULL,
	},
	{
	    USB_VENDOR_TAUGA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Taugagreining HF",
	    NULL,
	},
	{
	    USB_VENDOR_AMD, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Advanced Micro Devices",
	    NULL,
	},
	{
	    USB_VENDOR_LEXMARK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Lexmark International",
	    NULL,
	},
	{
	    USB_VENDOR_NANAO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "NANAO",
	    NULL,
	},
	{
	    USB_VENDOR_ALPS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alps Electric",
	    NULL,
	},
	{
	    USB_VENDOR_THRUST, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Thrustmaster",
	    NULL,
	},
	{
	    USB_VENDOR_TI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Texas Instruments",
	    NULL,
	},
	{
	    USB_VENDOR_ANALOGDEVICES, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Analog Devices",
	    NULL,
	},
	{
	    USB_VENDOR_KYE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "KYE Systems",
	    NULL,
	},
	{
	    USB_VENDOR_DIAMOND2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Diamond (Supra)",
	    NULL,
	},
	{
	    USB_VENDOR_MICROSOFT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Microsoft",
	    NULL,
	},
	{
	    USB_VENDOR_PRIMAX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Primax Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_AMP, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AMP",
	    NULL,
	},
	{
	    USB_VENDOR_CHERRY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Cherry Mikroschalter",
	    NULL,
	},
	{
	    USB_VENDOR_MEGATRENDS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "American Megatrends",
	    NULL,
	},
	{
	    USB_VENDOR_LOGITECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Logitech",
	    NULL,
	},
	{
	    USB_VENDOR_BTC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Behavior Tech. Computer",
	    NULL,
	},
	{
	    USB_VENDOR_PHILIPS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Philips",
	    NULL,
	},
	{
	    USB_VENDOR_SANYO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Sanyo Electric",
	    NULL,
	},
	{
	    USB_VENDOR_CONNECTIX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Connectix",
	    NULL,
	},
	{
	    USB_VENDOR_KENSINGTON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Kensington",
	    NULL,
	},
	{
	    USB_VENDOR_LUCENT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Lucent",
	    NULL,
	},
	{
	    USB_VENDOR_STMICRO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "STMicroelectronics",
	    NULL,
	},
	{
	    USB_VENDOR_YAMAHA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "YAMAHA",
	    NULL,
	},
	{
	    USB_VENDOR_COMPAQ, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Compaq Computers",
	    NULL,
	},
	{
	    USB_VENDOR_HITACHI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hitachi, Ltd.",
	    NULL,
	},
	{
	    USB_VENDOR_ACERP, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Acer Peripherals",
	    NULL,
	},
	{
	    USB_VENDOR_VISIONEER, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Visioneer",
	    NULL,
	},
	{
	    USB_VENDOR_CANON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Canon",
	    NULL,
	},
	{
	    USB_VENDOR_NIKON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Nikon",
	    NULL,
	},
	{
	    USB_VENDOR_IBM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "IBM Corporation",
	    NULL,
	},
	{
	    USB_VENDOR_CYPRESS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Cypress Semiconductor",
	    NULL,
	},
	{
	    USB_VENDOR_EPSON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Seiko Epson",
	    NULL,
	},
	{
	    USB_VENDOR_RAINBOW, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Rainbow Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_IODATA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "I/O Data",
	    NULL,
	},
	{
	    USB_VENDOR_TDK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "TDK",
	    NULL,
	},
	{
	    USB_VENDOR_3COMUSR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "U.S. Robotics",
	    NULL,
	},
	{
	    USB_VENDOR_METHODE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Methode Electronics Far East",
	    NULL,
	},
	{
	    USB_VENDOR_MAXISWITCH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Maxi Switch",
	    NULL,
	},
	{
	    USB_VENDOR_LOCKHEEDMER, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Lockheed Martin Energy Research",
	    NULL,
	},
	{
	    USB_VENDOR_FUJITSU, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Fujitsu",
	    NULL,
	},
	{
	    USB_VENDOR_TOSHIBAAM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Toshiba America Electronic Components",
	    NULL,
	},
	{
	    USB_VENDOR_MICROMACRO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Micro Macro Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_KONICA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Konica",
	    NULL,
	},
	{
	    USB_VENDOR_LITEON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Lite-On Technology",
	    NULL,
	},
	{
	    USB_VENDOR_FUJIPHOTO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Fuji Photo Film",
	    NULL,
	},
	{
	    USB_VENDOR_PHILIPSSEMI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Philips Semiconductors",
	    NULL,
	},
	{
	    USB_VENDOR_TATUNG, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Tatung Co. Of America",
	    NULL,
	},
	{
	    USB_VENDOR_SCANLOGIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ScanLogic",
	    NULL,
	},
	{
	    USB_VENDOR_MYSON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Myson Technology",
	    NULL,
	},
	{
	    USB_VENDOR_DIGI2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Digi International",
	    NULL,
	},
	{
	    USB_VENDOR_ITTCANON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ITT Canon",
	    NULL,
	},
	{
	    USB_VENDOR_ALTEC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Altec Lansing Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_PANASONIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Panasonic (Matsushita)",
	    NULL,
	},
	{
	    USB_VENDOR_IIYAMA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Iiyama",
	    NULL,
	},
	{
	    USB_VENDOR_SHUTTLE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Shuttle Technology",
	    NULL,
	},
	{
	    USB_VENDOR_SAMSUNG, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Samsung Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_ANNABOOKS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Annabooks",
	    NULL,
	},
	{
	    USB_VENDOR_JVC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "JVC",
	    NULL,
	},
	{
	    USB_VENDOR_CHICONY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Chicony Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_BROTHER, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Brother Industries",
	    NULL,
	},
	{
	    USB_VENDOR_DALLAS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Dallas Semiconductor",
	    NULL,
	},
	{
	    USB_VENDOR_ACER, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Acer",
	    NULL,
	},
	{
	    USB_VENDOR_3COM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "3Com",
	    NULL,
	},
	{
	    USB_VENDOR_AZTECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aztech Systems",
	    NULL,
	},
	{
	    USB_VENDOR_BELKIN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Belkin Components",
	    NULL,
	},
	{
	    USB_VENDOR_KAWATSU, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Kawatsu Semiconductor",
	    NULL,
	},
	{
	    USB_VENDOR_APC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "American Power Conversion",
	    NULL,
	},
	{
	    USB_VENDOR_CONNECTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Advanced Connectek USA",
	    NULL,
	},
	{
	    USB_VENDOR_NETCHIP, 0,
	    USB_KNOWNDEV_NOPROD,
	    "NetChip Technology",
	    NULL,
	},
	{
	    USB_VENDOR_ALTRA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ALTRA",
	    NULL,
	},
	{
	    USB_VENDOR_ATI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ATI Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_AKS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aladdin Knowledge Systems",
	    NULL,
	},
	{
	    USB_VENDOR_UNIACCESS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Universal Access",
	    NULL,
	},
	{
	    USB_VENDOR_XIRLINK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Xirlink",
	    NULL,
	},
	{
	    USB_VENDOR_ANCHOR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Anchor Chips",
	    NULL,
	},
	{
	    USB_VENDOR_SONY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Sony",
	    NULL,
	},
	{
	    USB_VENDOR_VISION, 0,
	    USB_KNOWNDEV_NOPROD,
	    "VLSI Vision",
	    NULL,
	},
	{
	    USB_VENDOR_ASAHIKASEI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Asahi Kasei Microsystems",
	    NULL,
	},
	{
	    USB_VENDOR_ATEN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ATEN International",
	    NULL,
	},
	{
	    USB_VENDOR_MUSTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Mustek Systems",
	    NULL,
	},
	{
	    USB_VENDOR_TELEX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Telex Communications",
	    NULL,
	},
	{
	    USB_VENDOR_PERACOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Peracom Networks",
	    NULL,
	},
	{
	    USB_VENDOR_ALCOR2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alcor Micro",
	    NULL,
	},
	{
	    USB_VENDOR_WACOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "WACOM",
	    NULL,
	},
	{
	    USB_VENDOR_ETEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "e-TEK Labs",
	    NULL,
	},
	{
	    USB_VENDOR_EIZO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "EIZO",
	    NULL,
	},
	{
	    USB_VENDOR_ELECOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Elecom",
	    NULL,
	},
	{
	    USB_VENDOR_HAUPPAUGE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hauppauge Computer Works",
	    NULL,
	},
	{
	    USB_VENDOR_BAFO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "BAFO/Quality Computer Accessories",
	    NULL,
	},
	{
	    USB_VENDOR_YEDATA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Y-E Data",
	    NULL,
	},
	{
	    USB_VENDOR_AVM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AVM GmbH",
	    NULL,
	},
	{
	    USB_VENDOR_QUICKSHOT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Quickshot",
	    NULL,
	},
	{
	    USB_VENDOR_ROLAND, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Roland",
	    NULL,
	},
	{
	    USB_VENDOR_ROCKFIRE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Rockfire",
	    NULL,
	},
	{
	    USB_VENDOR_RATOC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "RATOC Systems, Inc.",
	    NULL,
	},
	{
	    USB_VENDOR_ZYXEL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ZyXEL Communication",
	    NULL,
	},
	{
	    USB_VENDOR_ALCOR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alcor Micro",
	    NULL,
	},
	{
	    USB_VENDOR_IOMEGA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Iomega",
	    NULL,
	},
	{
	    USB_VENDOR_ATREND, 0,
	    USB_KNOWNDEV_NOPROD,
	    "A-Trend Technology",
	    NULL,
	},
	{
	    USB_VENDOR_AID, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Advanced Input Devices",
	    NULL,
	},
	{
	    USB_VENDOR_LACIE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "LaCie",
	    NULL,
	},
	{
	    USB_VENDOR_OMNIVISION, 0,
	    USB_KNOWNDEV_NOPROD,
	    "OmniVision",
	    NULL,
	},
	{
	    USB_VENDOR_INSYSTEM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "In-System Design",
	    NULL,
	},
	{
	    USB_VENDOR_APPLE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Apple Computer",
	    NULL,
	},
	{
	    USB_VENDOR_DIGI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Digi International",
	    NULL,
	},
	{
	    USB_VENDOR_QTRONIX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Qtronix",
	    NULL,
	},
	{
	    USB_VENDOR_ELSA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ELSA",
	    NULL,
	},
	{
	    USB_VENDOR_BRAINBOXES, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Brainboxes Limited",
	    NULL,
	},
	{
	    USB_VENDOR_ULTIMA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Ultima",
	    NULL,
	},
	{
	    USB_VENDOR_AXIOHM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Axiohm Transaction Solutions",
	    NULL,
	},
	{
	    USB_VENDOR_MICROTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Microtek",
	    NULL,
	},
	{
	    USB_VENDOR_SUNTAC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SUN Corporation",
	    NULL,
	},
	{
	    USB_VENDOR_LEXAR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Lexar Media",
	    NULL,
	},
	{
	    USB_VENDOR_SYMBOL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Symbol Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_GENESYS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Genesys Logic",
	    NULL,
	},
	{
	    USB_VENDOR_FUJI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Fuji Electric",
	    NULL,
	},
	{
	    USB_VENDOR_KEITHLEY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Keithley Instruments",
	    NULL,
	},
	{
	    USB_VENDOR_EIZONANAO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "EIZO Nanao",
	    NULL,
	},
	{
	    USB_VENDOR_KLSI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Kawasaki LSI",
	    NULL,
	},
	{
	    USB_VENDOR_FFC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "FFC",
	    NULL,
	},
	{
	    USB_VENDOR_ANKO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Anko Electronic",
	    NULL,
	},
	{
	    USB_VENDOR_PIENGINEERING, 0,
	    USB_KNOWNDEV_NOPROD,
	    "P.I. Engineering",
	    NULL,
	},
	{
	    USB_VENDOR_AOC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AOC International",
	    NULL,
	},
	{
	    USB_VENDOR_CHIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Chic Technology",
	    NULL,
	},
	{
	    USB_VENDOR_BARCO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Barco Display Systems",
	    NULL,
	},
	{
	    USB_VENDOR_BRIDGE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Bridge Information",
	    NULL,
	},
	{
	    USB_VENDOR_SOLIDYEAR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Solid Year",
	    NULL,
	},
	{
	    USB_VENDOR_BIORAD, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Bio-Rad Laboratories",
	    NULL,
	},
	{
	    USB_VENDOR_MACALLY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Macally",
	    NULL,
	},
	{
	    USB_VENDOR_ACTLABS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Act Labs",
	    NULL,
	},
	{
	    USB_VENDOR_ALARIS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alaris",
	    NULL,
	},
	{
	    USB_VENDOR_APEX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Apex",
	    NULL,
	},
	{
	    USB_VENDOR_AVISION, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Avision",
	    NULL,
	},
	{
	    USB_VENDOR_TEAC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "TEAC",
	    NULL,
	},
	{
	    USB_VENDOR_LINKSYS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Linksys",
	    NULL,
	},
	{
	    USB_VENDOR_ACERSA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Acer Semiconductor America",
	    NULL,
	},
	{
	    USB_VENDOR_SIGMATEL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Sigmatel",
	    NULL,
	},
	{
	    USB_VENDOR_AIWA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aiwa",
	    NULL,
	},
	{
	    USB_VENDOR_ACARD, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ACARD Technology",
	    NULL,
	},
	{
	    USB_VENDOR_PROLIFIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Prolific Technology",
	    NULL,
	},
	{
	    USB_VENDOR_SIEMENS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Siemens",
	    NULL,
	},
	{
	    USB_VENDOR_ADVANCELOGIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Avance Logic",
	    NULL,
	},
	{
	    USB_VENDOR_HAGIWARA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hagiwara Sys-Com",
	    NULL,
	},
	{
	    USB_VENDOR_MINOLTA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Minolta",
	    NULL,
	},
	{
	    USB_VENDOR_CTX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Chuntex",
	    NULL,
	},
	{
	    USB_VENDOR_ASKEY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Askey Computer",
	    NULL,
	},
	{
	    USB_VENDOR_SAITEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Saitek",
	    NULL,
	},
	{
	    USB_VENDOR_ALCATELT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alcatel Telecom",
	    NULL,
	},
	{
	    USB_VENDOR_AGFA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AGFA-Gevaert",
	    NULL,
	},
	{
	    USB_VENDOR_ASIAMD, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Asia Microelectronic Development",
	    NULL,
	},
	{
	    USB_VENDOR_BIZLINK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Bizlink International",
	    NULL,
	},
	{
	    USB_VENDOR_KEYSPAN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Keyspan",
	    NULL,
	},
	{
	    USB_VENDOR_AASHIMA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aashima Technology",
	    NULL,
	},
	{
	    USB_VENDOR_MULTITECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "MultiTech",
	    NULL,
	},
	{
	    USB_VENDOR_ADS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ADS Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_ALCATELM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alcatel Microelectronics",
	    NULL,
	},
	{
	    USB_VENDOR_SIRIUS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Sirius Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_BOSTON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Boston Acoustics",
	    NULL,
	},
	{
	    USB_VENDOR_SMC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Standard Microsystems",
	    NULL,
	},
	{
	    USB_VENDOR_PUTERCOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Putercom",
	    NULL,
	},
	{
	    USB_VENDOR_MCT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "MCT",
	    NULL,
	},
	{
	    USB_VENDOR_DIGITALSTREAM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Digital Stream",
	    NULL,
	},
	{
	    USB_VENDOR_AUREAL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aureal Semiconductor",
	    NULL,
	},
	{
	    USB_VENDOR_MIDIMAN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Midiman",
	    NULL,
	},
	{
	    USB_VENDOR_LINKSYS2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Linksys",
	    NULL,
	},
	{
	    USB_VENDOR_GRIFFIN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Griffin Technology",
	    NULL,
	},
	{
	    USB_VENDOR_SANDISK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SanDisk Corp",
	    NULL,
	},
	{
	    USB_VENDOR_BRIMAX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Brimax",
	    NULL,
	},
	{
	    USB_VENDOR_AXIS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Axis Communications",
	    NULL,
	},
	{
	    USB_VENDOR_ABL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ABL Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_ALFADATA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alfadata Computer",
	    NULL,
	},
	{
	    USB_VENDOR_NATIONALTECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "National Technical Systems",
	    NULL,
	},
	{
	    USB_VENDOR_ONNTO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Onnto",
	    NULL,
	},
	{
	    USB_VENDOR_BE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Be",
	    NULL,
	},
	{
	    USB_VENDOR_ADMTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ADMtek",
	    NULL,
	},
	{
	    USB_VENDOR_COREGA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Corega",
	    NULL,
	},
	{
	    USB_VENDOR_FREECOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Freecom",
	    NULL,
	},
	{
	    USB_VENDOR_MICROTECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Microtech",
	    NULL,
	},
	{
	    USB_VENDOR_GENERALINSTMNTS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "General Instruments (Motorola)",
	    NULL,
	},
	{
	    USB_VENDOR_OLYMPUS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Olympus",
	    NULL,
	},
	{
	    USB_VENDOR_ABOCOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AboCom Systems",
	    NULL,
	},
	{
	    USB_VENDOR_KEISOKUGIKEN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Keisokugiken",
	    NULL,
	},
	{
	    USB_VENDOR_APG, 0,
	    USB_KNOWNDEV_NOPROD,
	    "APG Cash Drawer",
	    NULL,
	},
	{
	    USB_VENDOR_BUG, 0,
	    USB_KNOWNDEV_NOPROD,
	    "B.U.G.",
	    NULL,
	},
	{
	    USB_VENDOR_ALLIEDTELESYN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Allied Telesyn International",
	    NULL,
	},
	{
	    USB_VENDOR_AVERMEDIA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AVerMedia Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_SIIG, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SIIG",
	    NULL,
	},
	{
	    USB_VENDOR_CASIO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "CASIO",
	    NULL,
	},
	{
	    USB_VENDOR_APTIO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Aptio Products",
	    NULL,
	},
	{
	    USB_VENDOR_ARASAN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Arasan Chip Systems",
	    NULL,
	},
	{
	    USB_VENDOR_ALLIEDCABLE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Allied Cable",
	    NULL,
	},
	{
	    USB_VENDOR_STSN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "STSN",
	    NULL,
	},
	{
	    USB_VENDOR_ZOOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Zoom Telephonics",
	    NULL,
	},
	{
	    USB_VENDOR_BROADLOGIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "BroadLogic",
	    NULL,
	},
	{
	    USB_VENDOR_HANDSPRING, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Handspring",
	    NULL,
	},
	{
	    USB_VENDOR_ACTIONSTAR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Action Star Enterprise",
	    NULL,
	},
	{
	    USB_VENDOR_PALM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Palm Computing",
	    NULL,
	},
	{
	    USB_VENDOR_SOURCENEXT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SOURCENEXT",
	    NULL,
	},
	{
	    USB_VENDOR_ACCTON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Accton Technology",
	    NULL,
	},
	{
	    USB_VENDOR_DIAMOND, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Diamond",
	    NULL,
	},
	{
	    USB_VENDOR_NETGEAR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "BayNETGEAR",
	    NULL,
	},
	{
	    USB_VENDOR_ACTIVEWIRE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ActiveWire",
	    NULL,
	},
	{
	    USB_VENDOR_PORTGEAR, 0,
	    USB_KNOWNDEV_NOPROD,
	    "PortGear",
	    NULL,
	},
	{
	    USB_VENDOR_METRICOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Metricom",
	    NULL,
	},
	{
	    USB_VENDOR_ADESSOKBTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ADESSO/Kbtek America",
	    NULL,
	},
	{
	    USB_VENDOR_JATON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Jaton",
	    NULL,
	},
	{
	    USB_VENDOR_APT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "APT Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_BOCARESEARCH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Boca Research",
	    NULL,
	},
	{
	    USB_VENDOR_ANDREA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Andrea Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_BURRBROWN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Burr-Brown Japan",
	    NULL,
	},
	{
	    USB_VENDOR_2WIRE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "2Wire",
	    NULL,
	},
	{
	    USB_VENDOR_AIPTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AIPTEK International",
	    NULL,
	},
	{
	    USB_VENDOR_SMARTBRIDGES, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SmartBridges",
	    NULL,
	},
	{
	    USB_VENDOR_BILLIONTON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Billionton Systems",
	    NULL,
	},
	{
	    USB_VENDOR_EXTENDED, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Extended Systems",
	    NULL,
	},
	{
	    USB_VENDOR_MSYSTEMS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "M-Systems",
	    NULL,
	},
	{
	    USB_VENDOR_AUTHENTEC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AuthenTec",
	    NULL,
	},
	{
	    USB_VENDOR_ALATION, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Alation Systems",
	    NULL,
	},
	{
	    USB_VENDOR_GOHUBS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "GoHubs",
	    NULL,
	},
	{
	    USB_VENDOR_BIOMETRIC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "American Biometric Company",
	    NULL,
	},
	{
	    USB_VENDOR_TOSHIBA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Toshiba Corporation",
	    NULL,
	},
	{
	    USB_VENDOR_YANO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Yano",
	    NULL,
	},
	{
	    USB_VENDOR_KINGSTON, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Kingston Technology",
	    NULL,
	},
	{
	    USB_VENDOR_BLUEWATER, 0,
	    USB_KNOWNDEV_NOPROD,
	    "BlueWater Systems",
	    NULL,
	},
	{
	    USB_VENDOR_AGILENT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Agilent Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_PORTSMITH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Portsmith",
	    NULL,
	},
	{
	    USB_VENDOR_ADIRONDACK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Adirondack Wire & Cable",
	    NULL,
	},
	{
	    USB_VENDOR_BECKHOFF, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Beckhoff",
	    NULL,
	},
	{
	    USB_VENDOR_INTERSIL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Intersil",
	    NULL,
	},
	{
	    USB_VENDOR_ALTIUS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Altius Solutions",
	    NULL,
	},
	{
	    USB_VENDOR_ARRIS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Arris Interactive",
	    NULL,
	},
	{
	    USB_VENDOR_ACTIVCARD, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ACTIVCARD",
	    NULL,
	},
	{
	    USB_VENDOR_ACTISYS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ACTiSYS",
	    NULL,
	},
	{
	    USB_VENDOR_AFOURTECH, 0,
	    USB_KNOWNDEV_NOPROD,
	    "A-FOUR TECH",
	    NULL,
	},
	{
	    USB_VENDOR_AIMEX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AIMEX",
	    NULL,
	},
	{
	    USB_VENDOR_ADDONICS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Addonics Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_AKAI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "AKAI professional M.I.",
	    NULL,
	},
	{
	    USB_VENDOR_ARESCOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ARESCOM",
	    NULL,
	},
	{
	    USB_VENDOR_BAY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Bay Associates",
	    NULL,
	},
	{
	    USB_VENDOR_ALTERA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Altera",
	    NULL,
	},
	{
	    USB_VENDOR_TREK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Trek Technology",
	    NULL,
	},
	{
	    USB_VENDOR_ASAHIOPTICAL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Asahi Optical",
	    NULL,
	},
	{
	    USB_VENDOR_BOCASYSTEMS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Boca Systems",
	    NULL,
	},
	{
	    USB_VENDOR_BROADCOM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Broadcom",
	    NULL,
	},
	{
	    USB_VENDOR_GREENHOUSE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "GREENHOUSE",
	    NULL,
	},
	{
	    USB_VENDOR_GEOCAST, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Geocast Network Systems",
	    NULL,
	},
	{
	    USB_VENDOR_NEODIO, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Neodio",
	    NULL,
	},
	{
	    USB_VENDOR_TODOS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Todos Data System",
	    NULL,
	},
	{
	    USB_VENDOR_HAL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "HAL Corporation",
	    NULL,
	},
	{
	    USB_VENDOR_NEC2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "NEC",
	    NULL,
	},
	{
	    USB_VENDOR_ATI2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ATI",
	    NULL,
	},
	{
	    USB_VENDOR_ASIX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "ASIX Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_REALTEK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "RealTek",
	    NULL,
	},
	{
	    USB_VENDOR_AGATE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Agate Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_DMI, 0,
	    USB_KNOWNDEV_NOPROD,
	    "DMI",
	    NULL,
	},
	{
	    USB_VENDOR_LUWEN, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Luwen",
	    NULL,
	},
	{
	    USB_VENDOR_SMC3, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Standard Microsystems",
	    NULL,
	},
	{
	    USB_VENDOR_HAWKING, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hawking Technologies",
	    NULL,
	},
	{
	    USB_VENDOR_MOTOROLA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Motorola",
	    NULL,
	},
	{
	    USB_VENDOR_PLX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "PLX",
	    NULL,
	},
	{
	    USB_VENDOR_ASANTE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Asante",
	    NULL,
	},
	{
	    USB_VENDOR_JRC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Japan Radio Company",
	    NULL,
	},
	{
	    USB_VENDOR_ACERCM, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Acer Communications & Multimedia Inc.",
	    NULL,
	},
	{
	    USB_VENDOR_BELKIN2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Belkin Components",
	    NULL,
	},
	{
	    USB_VENDOR_MOBILITY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Mobility",
	    NULL,
	},
	{
	    USB_VENDOR_SHARK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Shark",
	    NULL,
	},
	{
	    USB_VENDOR_SILICONPORTALS, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Silicon Portals",
	    NULL,
	},
	{
	    USB_VENDOR_SOHOWARE, 0,
	    USB_KNOWNDEV_NOPROD,
	    "SOHOware",
	    NULL,
	},
	{
	    USB_VENDOR_UMAX, 0,
	    USB_KNOWNDEV_NOPROD,
	    "UMAX Data Systems",
	    NULL,
	},
	{
	    USB_VENDOR_INSIDEOUT, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Inside Out Networks",
	    NULL,
	},
	{
	    USB_VENDOR_ENTREGA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Entrega",
	    NULL,
	},
	{
	    USB_VENDOR_ACTIONTEC, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Actiontec Electronics",
	    NULL,
	},
	{
	    USB_VENDOR_DLINK, 0,
	    USB_KNOWNDEV_NOPROD,
	    "D-Link",
	    NULL,
	},
	{
	    USB_VENDOR_VIDZMEDIA, 0,
	    USB_KNOWNDEV_NOPROD,
	    "VidzMedia Pte Ltd",
	    NULL,
	},
	{
	    USB_VENDOR_DAISY, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Daisy Technology",
	    NULL,
	},
	{
	    USB_VENDOR_INTEL, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Intel",
	    NULL,
	},
	{
	    USB_VENDOR_HP2, 0,
	    USB_KNOWNDEV_NOPROD,
	    "Hewlett Packard",
	    NULL,
	},
	{ 0, 0, 0, NULL, NULL, }
};
#endif	USB_KNOWNDEV

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
		return (usbd_error_strs[err]);
	}
	else
	{
		snprintf (buffer, sizeof (buffer), "%d", err);
		return (buffer);
	}

}	/* end usbd_errstr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_string_desc (struct usbd_device *udev, int sindex, int langid, struct usb_string_descriptor *sdesc)
{
	struct usb_device_request	req;
	int				err, actlen;

	req.bmRequestType	= UT_READ_DEVICE;
	req.bRequest		= UR_GET_DESCRIPTOR;

	USETW2 (req.wValue, UDESC_STRING, sindex);
	USETW  (req.wIndex, langid);
	USETW  (req.wLength, 2);	/* only size bytes first */

	if (err = usbd_do_request_flags (udev, &req, sdesc, USBD_SHORT_XFER_OK, &actlen, USBD_DEFAULT_TIMEOUT))
		return (err);

	if (actlen < 2)
		return (USBD_SHORT_XFER);

	USETW (req.wLength, sdesc->bLength);	/* the whole string */

	if (err = usbd_do_request_flags (udev, &req, sdesc, USBD_SHORT_XFER_OK, &actlen, USBD_DEFAULT_TIMEOUT))
		return (err);

	if (actlen != sdesc->bLength)
		printf ("usbd_get_string_desc: expected %d, got %d\n", sdesc->bLength, actlen);

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_get_string_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
char *
usbd_get_string (struct usbd_device *udev, int si, char *buf)
{
	struct usb_string_descriptor		*usp;
	int					uni_swap, err, i, n;
	char					*s;
	ushort					c;

	buf[0] = '\0';

	if (si == 0)
		return (NOSTR);

	if (udev->quirks->uq_flags & UQ_NO_STRINGS)
		return (NOSTR);

	if ((usp = malloc_byte (sizeof (struct usb_string_descriptor))) == NULL)
		return (NOSTR);

	if (udev->langid == USBD_NOLANG)
	{
		/* Set up default language */

		err = usbd_get_string_desc (udev, USB_LANGUAGE_TABLE, 0, usp);

		if (err || usp->bLength < 4)
			udev->langid = 0;			/* Well, just pick something then */
		else
			udev->langid = UGETW (usp->bString[0]); /* Pick the first language as the default */
	}

	if (usbd_get_string_desc (udev, si, udev->langid, usp) != 0)
		{ free_byte (usp); return (NOSTR); }

	s	 = buf;
	n	 = usp->bLength / 2 - 1;
	uni_swap = udev->quirks->uq_flags & UQ_SWAP_UNICODE;

	for (i = 0; i < n; i++)
	{
		c = UGETW (usp->bString[i]);

		/* Convert from Unicode, handle buggy strings */

		if ((c & 0xFF00) == 0)
			*s++ = c;
		else if ((c & 0x00FF) == 0 && uni_swap)
			*s++ = c >> 8;
		else 
			*s++ = '?';
	}

	*s++ = 0;

	free_byte (usp);

	return (buf);

}	/* end usbd_get_string */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_trim_spaces (char *p)
{
	char		*q, *e;

	if (p == NULL)
		return;

	q = e = p;

	while (*q == ' ')	/* skip leading spaces */
		q++;

	while ((*p = *q++))	/* copy string */
	{
		if (*p++ != ' ') /* remember last non-space */
			e = p;
	}

	*e = 0;			/* kill trailing spaces */

}	/* end usbd_trim_spaces */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_devinfo_vp (struct usbd_device *udev, char *v, char *p, int usedev)
{
	struct usb_device_descriptor	*udd = &udev->ddesc;
	char				*vendor = NOSTR, *product = NOSTR;

	if (udev == NULL)
		{ v[0] = p[0] = '\0'; return; }

	if (usedev)
	{
		vendor = usbd_get_string (udev, udd->iManufacturer, v);
		usbd_trim_spaces (vendor);

		product = usbd_get_string (udev, udd->iProduct, p);
		usbd_trim_spaces (product);
	}
	else
	{
		vendor = product = NULL;
	}

#ifdef	USB_KNOWNDEV
	if (vendor == NULL || product == NULL)
	{
		const struct usb_knowndev	*kdp;

		for (kdp = usb_knowndevs; kdp->vendorname != NULL; kdp++)
		{
			if
			(	kdp->vendor == UGETW (udd->idVendor) && 
				(kdp->product == UGETW (udd->idProduct) ||
				(kdp->flags & USB_KNOWNDEV_NOPROD) != 0)
			)
				break;
		}

		if (kdp->vendorname != NULL)
		{
			if (vendor == NULL)
				vendor = kdp->vendorname;

			if (product == NULL)
				product = (kdp->flags & USB_KNOWNDEV_NOPROD) == 0 ? kdp->productname : NULL;
		}
	}
#endif	USB_KNOWNDEV

	if (vendor != NULL && *vendor)
		strcpy (v, vendor);
	else
		sprintf (v, "vendor 0x%04x", UGETW (udd->idVendor));

	if (product != NULL && *product)
		strcpy (p, product);
	else
		sprintf (p, "product 0x%04x", UGETW(udd->idProduct));

}	/* end usbd_devinfo_vp */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_printBCD (char *cp, int bcd)
{
	return (sprintf (cp, "%X.%02X", bcd >> 8, bcd & 0xFF));

}	/* end usbd_printBCD */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_devinfo (struct usbd_device *udev, int showclass, char *cp)
{
	struct usb_device_descriptor	*udd = &udev->ddesc;
	char				vendor[USB_MAX_STRING_LEN];
	char				product[USB_MAX_STRING_LEN];
	int				bcdDevice, bcdUSB;

	usbd_devinfo_vp (udev, vendor, product, 1);

	cp += sprintf (cp, "%s %s", vendor, product);

	if (showclass & USBD_SHOW_DEVICE_CLASS)
		cp += sprintf (cp, ", class %d/%d", udd->bDeviceClass, udd->bDeviceSubClass);

	bcdUSB = UGETW(udd->bcdUSB);
	bcdDevice = UGETW(udd->bcdDevice);

	cp += sprintf (cp, ", rev ");
	cp += usbd_printBCD (cp, bcdUSB);
	*cp++ = '/';

	cp += usbd_printBCD (cp, bcdDevice);
	cp += sprintf (cp, ", addr %d", udev->address);

	if (showclass & USBD_SHOW_INTERFACE_CLASS)
	{
		struct usbd_interface		*iface;
		struct usb_interface_descriptor	*id;

		/* fetch the interface handle for the first interface */

		usbd_device2interface_handle (udev, 0, &iface); id = iface->idesc;

		cp += sprintf (cp, ", iclass %d/%d", id->bInterfaceClass, id->bInterfaceSubClass);
	}

	*cp = 0;

}	/* end usbd_devinfo */

/*
 ****************************************************************
 *	Delay given a device handle				*
 ****************************************************************
 */
void
usbd_delay_ms (struct usbd_device *udev, unsigned int ms)
{
#if (0)	/*******************************************************/
	usb_delay_ms (udev->bus, ms);
#endif	/*******************************************************/

	DELAY ((ms + 1) * 1000);

}	/* end usbd_delay_ms */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_reset_port (struct usbd_device *udev, int port, struct usb_port_status *ps)
{
	struct usb_device_request	req;
	int				err, n;

	req.bmRequestType	= UT_WRITE_CLASS_OTHER;
	req.bRequest		= UR_SET_FEATURE;

	USETW (req.wValue, UHF_PORT_RESET);
	USETW (req.wIndex, port);
	USETW (req.wLength, 0);

	err = usbd_do_request (udev, &req, 0);

#ifdef	USB_MSG
	printf ("usbd_reset_port: port %d reset done, error=%s\n", port, usbd_errstr(err));
#endif	USB_MSG

	if (err)
		return (err);

	n = 10;

	do
	{
		/* Wait for device to recover from reset */

		usbd_delay_ms (udev, USB_PORT_RESET_DELAY);

		err = usbd_get_port_status (udev, port, ps);

		if (err)
			{ printf ("usbd_reset_port: get status failed %d\n", err); return (err); }

		/* If the device disappeared, just give up */

		if (!(UGETW (ps->wPortStatus) & UPS_CURRENT_CONNECT_STATUS))
			return (USBD_NORMAL_COMPLETION);

	}	while ((UGETW (ps->wPortChange) & UPS_C_PORT_RESET) == 0 && --n > 0);

	if (n == 0)
		return (USBD_TIMEOUT);

	err = usbd_clear_port_feature (udev, port, UHF_C_PORT_RESET);

#ifdef USB_DEBUG
	if (err)
		printf ("usbd_reset_port: clear port feature failed %d\n", err);
#endif

	/* Wait for the device to recover from reset */

	usbd_delay_ms (udev, USB_PORT_RESET_RECOVERY);

	return (err);

}	/* end usbd_reset_port */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct usb_interface_descriptor *
usbd_find_idesc (struct usb_config_descriptor *cd, int ifaceidx, int altidx)
{
	char				*p = (char *)cd;
	char				*End = p + UGETW (cd->wTotalLength);
	struct usb_interface_descriptor	*d;
	int				curidx, lastidx, curaidx = 0;

	for (curidx = lastidx = -1; p < End; /* vazio */)
	{
		d = (struct usb_interface_descriptor *)p;

		if (d->bLength == 0) /* bad descriptor */
			break;

		p += d->bLength;

		if (p <= End && d->bDescriptorType == UDESC_INTERFACE)
		{
			if (d->bInterfaceNumber != lastidx)
			{
				lastidx = d->bInterfaceNumber;
				curidx++;
				curaidx = 0;
			}
			else
			{
				curaidx++;
			}

			if (ifaceidx == curidx && altidx == curaidx)
				return (d);
		}
	}

	return (NULL);

}	/* end usbd_find_idesc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_fill_iface_data (struct usbd_device *udev, int ifaceidx, int altidx)
{
	struct usbd_interface		*ifc = &udev->ifaces[ifaceidx];
	struct usb_interface_descriptor	*idesc;
	char				*p, *End;
	int				endpt, nendpt;

#ifdef	USB_MSG
	printf ("usbd_fill_iface_data: ifaceidx=%d altidx=%d\n", ifaceidx, altidx);
#endif	USB_MSG

	if ((idesc = usbd_find_idesc (udev->cdesc, ifaceidx, altidx)) == NULL)
		return (USBD_INVAL);

	ifc->device	= udev;
	ifc->idesc	= idesc;
	ifc->index	= ifaceidx;
	ifc->altindex	= altidx;

	if ((nendpt = ifc->idesc->bNumEndpoints) != 0)
	{
		if ((ifc->endpoints = malloc_byte (nendpt * sizeof (struct usbd_endpoint))) == NULL)
			return (USBD_NOMEM);
	}
	else
	{
		ifc->endpoints = NULL;
	}

	ifc->priv = NULL;

	p   = (char *)ifc->idesc + ifc->idesc->bLength;
	End = (char *)udev->cdesc + UGETW (udev->cdesc->wTotalLength);

#define ed ((struct usb_endpoint_descriptor *)p)

	for (endpt = 0; endpt < nendpt; endpt++)
	{
		for (/* vazio */; p < End; p += ed->bLength)
		{
			if (p + ed->bLength <= End && ed->bLength != 0 && ed->bDescriptorType == UDESC_ENDPOINT)
				goto found;

			if (ed->bLength == 0 || ed->bDescriptorType == UDESC_INTERFACE)
				break;
		}

		/* passed end, or bad desc */

		printf
		(	"usbd_fill_iface_data: bad descriptor(s): %s\n",
			ed->bLength == 0 ? "0 length" :
			ed->bDescriptorType == UDESC_INTERFACE ? "iface desc": "out of data"
		);

		goto bad;

	    found:
		ifc->endpoints[endpt].edesc = ed;

		if (udev->speed == USB_SPEED_HIGH)
		{
			unsigned	mps;

			/* Control and bulk endpoints have max packet limits */

			switch (UE_GET_XFERTYPE (ed->bmAttributes))
			{
			    case UE_CONTROL:
				mps = USB_2_MAX_CTRL_PACKET;
				goto check;

			    case UE_BULK:
				mps = USB_2_MAX_BULK_PACKET;
			    check:
				if (UGETW (ed->wMaxPacketSize) != mps)
				{
					USETW (ed->wMaxPacketSize, mps);

					printf ("usbd_fill_iface_data: bad max packet size\n");

				}
				break;

			    default:
				break;
			}
		}

		ifc->endpoints[endpt].refcnt = 0;
		ifc->endpoints[endpt].savedtoggle = 0;
		p += ed->bLength;
	}

#undef ed

	ifc->pipe_list = NULL;

	return (USBD_NORMAL_COMPLETION);

	/*
	 *	x
	 */
     bad:
	if (ifc->endpoints != NULL)
		{ free_byte (ifc->endpoints); ifc->endpoints = NULL; }

	return (USBD_INVAL);

}	/* end usbd_fill_iface_data */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_free_iface_data (struct usbd_device *udev, int ifcno)
{
	struct usbd_interface	*ifc = &udev->ifaces[ifcno];

	if (ifc->endpoints)
		free_byte (ifc->endpoints);

}	/* end usbd_free_iface_data */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_set_config (struct usbd_device *udev, int conf)
{
	struct usb_device_request req;

	req.bmRequestType	= UT_WRITE_DEVICE;
	req.bRequest		= UR_SET_CONFIG;

	USETW (req.wValue, conf);
	USETW (req.wIndex, 0);
	USETW (req.wLength, 0);

	return (usbd_do_request (udev, &req, 0));

}	/* end usbd_set_config */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_set_config_index (struct usbd_device *udev, int index, int msg)
{
	struct usb_config_descriptor	*cdp;
	int				err, i, ifcidx, nifc, len, selfpowered, power;

#ifdef	USB_MSG
	printf ("usbd_set_config_index: udev = %P index = %d\n", udev, index);
#endif	USB_MSG

	/* XXX check that all interfaces are idle */

	if (udev->config != USB_UNCONFIG_NO)
	{
		nifc = udev->cdesc->bNumInterface;

		/* Check that all interfaces are idle */

		for (ifcidx = 0; ifcidx < nifc; ifcidx++)
		{
			if (udev->ifaces[ifcidx].pipe_list == NULL)
				continue;

			printf ("usbd_set_config_index: open pipes exist\n");

			return (USBD_IN_USE);
		}

		printf ("usbd_set_config_index: free old config\n");

		/* Free all configuration data structures */

		for (ifcidx = 0; ifcidx < nifc; ifcidx++)
			usbd_free_iface_data (udev, ifcidx);

		free_byte (udev->ifaces);
		free_byte (udev->cdesc);

		udev->ifaces	= NULL;
		udev->cdesc	= NULL;
		udev->config	= USB_UNCONFIG_NO;
	}

	/*
	 *	x
	 */
	if (index == USB_UNCONFIG_INDEX)
	{
		/* We are unconfiguring the device, so leave unallocated */

#ifdef	USB_MSG
		printf ("usbd_set_config_index: set config 0\n");
#endif	USB_MSG

		if (err = usbd_set_config (udev, USB_UNCONFIG_NO))
		{
			printf
			(	"usbd_set_config_index: setting config = 0 failed, error = %s\n",
				usbd_errstr (err)
			);
		}

		return (err);
	}

	/* Get the short descriptor */

	if ((cdp = malloc_byte (USB_CONFIG_DESCRIPTOR_SIZE)) == NULL)
		return (USBD_NOMEM);

	if (err = usbd_get_config_desc (udev, index, cdp))
{
printf ("%g: Retorno com erro %d de usbd_get_config_desc\n", err);
		goto bad;
}

	if ((len = UGETW (cdp->wTotalLength)) != USB_CONFIG_DESCRIPTOR_SIZE)
	{
		free_byte (cdp);

		if ((cdp = malloc_byte (len)) == NULL)
			return (USBD_NOMEM);
	}

	/* Get the full descriptor */

	for (i = 0; /* abaixo */; i++)
	{
		if (i >= 8)
			{ printf ("%g: Não consegui obter o descritor completo\n"); goto bad; }

		if (usbd_get_desc (udev, UDESC_CONFIG, index, len, cdp) == 0)
			break;

		usbd_delay_ms (udev, 200);
	}

	if (cdp->bDescriptorType != UDESC_CONFIG)
	{
		printf ("usbd_set_config_index: bad desc %d\n", cdp->bDescriptorType);
		err = USBD_INVAL;
		goto bad;
	}

	/* Figure out if the device is self or bus powered */

	selfpowered = 0;

	if (!(udev->quirks->uq_flags & UQ_BUS_POWERED) && (cdp->bmAttributes & UC_SELF_POWERED))
	{
		/* May be self powered */

		if (cdp->bmAttributes & UC_BUS_POWERED)
		{
			/* Must ask device */

			if (udev->quirks->uq_flags & UQ_POWER_CLAIM)
			{
				/*
				 *	Hub claims to be self powered, but isn't.
				 *	It seems that the power status can be
				 *	determined by the hub characteristics.
				 */
				struct usb_hub_descriptor	*hd;
				struct usb_device_request	req;

				if ((hd = malloc_byte (sizeof (struct usb_hub_descriptor))) == NULL)
					{ err = USBD_NOMEM; goto bad; }

				req.bmRequestType	= UT_READ_CLASS_DEVICE;
				req.bRequest		= UR_GET_DESCRIPTOR;

				USETW (req.wValue, 0);
				USETW (req.wIndex, 0);
				USETW (req.wLength, USB_HUB_DESCRIPTOR_SIZE);

				err = usbd_do_request (udev, &req, hd);

				if (!err && (UGETW(hd->wHubCharacteristics) & UHD_PWR_INDIVIDUAL))
					selfpowered = 1;

				printf
				(	"usbd_set_config_index: charac=0x%04x, error=%s\n",
					UGETW (hd->wHubCharacteristics), usbd_errstr(err)
				);

				free_byte (hd);
			}
			else
			{
				struct usb_status	*ds;

				if ((ds = malloc_byte (sizeof (struct usb_status))) == NULL)
					{ err = USBD_NOMEM; goto bad; }

				if   (err = usbd_get_device_status (udev, ds))
				{
					printf
					(	"usbd_set_config_index: status=0x%04x, error=%s\n",
						UGETW (ds->wStatus), usbd_errstr (err)
					);
				}
				elif (UGETW (ds->wStatus) & UDS_SELF_POWERED)
				{
					selfpowered = 1;
				}

				free_byte (ds);
			}
		}
		else
		{
			selfpowered = 1;
		}
	}

#ifdef	USB_MSG
	printf
	(	"usbd_set_config_index: (addr %d) cno=%d attr=0x%02x, selfpowered=%d, power=%d\n",
		cdp->bConfigurationValue, dev->address, cdp->bmAttributes,
		selfpowered, cdp->bMaxPower * 2
	);
#endif	USB_MSG

	/* Check if we have enough power */

	if (udev->powersrc == NULL)
		{ printf ("usbd_set_config_index: No power source?\n"); return (USBD_IOERROR); }

	power = cdp->bMaxPower * 2;

	if (power > udev->powersrc->power)
	{
		printf ("power exceeded %d %d\n", power, udev->powersrc->power);

		/* XXX print nicer message */

		if (msg)
		{
			printf
			(	"%s: device addr %d (config %d) exceeds power budget, %d mA > %d mA\n",
				udev->bus->bdev->nameunit, udev->address, 
				cdp->bConfigurationValue, power, udev->powersrc->power
			);
		}

		err = USBD_NO_POWER;
		goto bad;
	}

	udev->power        = power;
	udev->self_powered = selfpowered;

	/* Set the actual configuration value */

#ifdef	USB_MSG
	printf ("usbd_set_config_index: set config %d\n", cdp->bConfigurationValue);
#endif	USB_MSG

	if (err = usbd_set_config (udev, cdp->bConfigurationValue))
	{
		printf
		(	"usbd_set_config_index: setting config=%d failed, error=%s\n",
			cdp->bConfigurationValue, usbd_errstr(err)
		);
		goto bad;
	}

	/* Allocate and fill interface data */

	nifc = cdp->bNumInterface;

	if ((udev->ifaces = malloc_byte (nifc * sizeof (struct usbd_interface))) == NULL)
		{ err = USBD_NOMEM; goto bad; }

#ifdef	USB_MSG
	printf ("usbd_set_config_index: udev=%P cdesc=%P\n", udev, cdp);
#endif	USB_MSG

	udev->cdesc = cdp;
	udev->config = cdp->bConfigurationValue;

	for (ifcidx = 0; ifcidx < nifc; ifcidx++)
	{
		if (err = usbd_fill_iface_data (udev, ifcidx, 0))
		{
			while (--ifcidx >= 0)
				usbd_free_iface_data (udev, ifcidx);

			goto bad;
		}
	}

	return (USBD_NORMAL_COMPLETION);

 bad:
	free_byte (cdp);
	return (err);

}	/* end usbd_set_config_index */

/*
 ****************************************************************
 *	Abort the device control pipe				*
 ****************************************************************
 */
void
usbd_kill_pipe (struct usbd_pipe *pipe_ptr)
{
	usbd_abort_pipe (pipe_ptr);

	pipe_ptr->methods->close (pipe_ptr);

	pipe_ptr->endpoint->refcnt--;

	free_byte (pipe_ptr);

}	/* end usbd_kill_pipe */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usb_free_device (struct usbd_device *udev)
{
	int		ifcidx, nifc;

	if (udev->default_pipe != NULL)
		usbd_kill_pipe (udev->default_pipe);

	if (udev->ifaces != NULL)
	{
		nifc = udev->cdesc->bNumInterface;

		for (ifcidx = 0; ifcidx < nifc; ifcidx++)
			usbd_free_iface_data (udev, ifcidx);

		free_byte (udev->ifaces);
	}

	if (udev->cdesc != NULL)
		free_byte (udev->cdesc);

	if (udev->subdevs != NULL)
		free_byte (udev->subdevs);

	free_byte (udev);

}	/* end usb_free_device */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 *
 *	The general mechanism for detaching drivers works as follows: Each
 *	driver is responsible for maintaining a reference count on the
 *	number of outstanding references to its softc (e.g.  from
 *	processing hanging in a read or write).  The detach method of the
 *	driver decrements this counter and flags in the softc that the
 *	driver is dying and then wakes any sleepers.  It then sleeps on the
 *	softc.  Each place that can sleep must maintain the reference
 *	count.  When the reference count drops to -1 (0 is the normal value
 *	of the reference count) the a wakeup on the softc is performed
 *	signaling to the detach waiter that all references are gone.
 *
 *	Called from process context when we discover that a port has
 *	been disconnected.
 */
void
usb_disconnect_port (struct usbd_port *up, struct device *parent)
{
	struct usbd_device	*udev = up->device;
	const char		*hub_nm = parent->nameunit;
	char			dev_nm[16];
	int			i, ret;

#ifdef	USB_MSG
	printf ("uhub_disconnect: up = %P udev = %P port=%d\n", up, udev, up->portno);
#endif	USB_MSG

	if (udev == NULL)
		{ printf ("usb_disconnect_port: udev == NULL\n"); return; }

	if (udev->subdevs != NULL)
	{
#ifdef	USB_MSG
		printf ("usb_disconnect_port: disconnect subdevs\n");
#endif	USB_MSG

		for (i = 0; udev->subdevs[i] != NULL; i++)
		{
			const char	*str;

			if ((str = udev->subdevs[i]->nameunit) == NOSTR)
				str = "???";

			strscpy (dev_nm, str, sizeof (dev_nm));

#if (0)	/*******************************************************/
			strscpy (dev_nm, udev->subdevs[i]->nameunit, sizeof (dev_nm));
#endif	/*******************************************************/

			ret = device_delete_child (udev->subdevs[i]->parent, udev->subdevs[i]);

			printf ("Dispositivo \"%s\", \"%s\"", dev_nm, hub_nm);

			if (up->portno != 0)
				printf (", porta %d", up->portno);

			printf (", endereço %d ", udev->address);

			if (ret < 0)
				{ printf ("NÃO foi removido\n"); return; }

			printf ("removido\n");
		}
	}

	udev->bus->devices[udev->address] = NULL;	up->device = NULL;

	usb_free_device (udev);

}	/* end usb_disconnect_port */

/*
 ****************************************************************
 *	Endereça um dispositivo recém energizado		*
 ****************************************************************
 */
int
usbd_new_device (struct device *parent, struct usbd_bus *bus, int depth,
                int speed, int port, struct usbd_port *up)
{
	struct usbd_device		*udev, *adev, *hub;
	struct usb_device_descriptor	*dd;
	struct usb_port_status		ps;	/* PODE ESTAR NA PILHA ? */
	int				err;
	int				addr;
	int				i, p;
	static ulong			usb_cookie_no;

	if ((addr = usbd_getnewaddr (bus)) < 0)
	{
		printf ("usb%d: Não há mais endereços USB vagos\n", parent->unit);
		return (USBD_NO_ADDR);
	}

	if ((udev = malloc_byte (sizeof (struct usbd_device))) == NULL)
	{
		printf ("usb%d: Memória esgotada\n", parent->unit);
		return (USBD_NOMEM);
	}

	memclr (udev, sizeof (struct usbd_device));

	udev->bus = bus;

	/*
	 *	Set up default endpoint handle
	 */
	udev->def_ep.edesc = &udev->def_ep_desc;

	/*
	 *	Set up default endpoint descriptor
	 */
	udev->def_ep_desc.bLength		= USB_ENDPOINT_DESCRIPTOR_SIZE;
	udev->def_ep_desc.bDescriptorType	= UDESC_ENDPOINT;
	udev->def_ep_desc.bEndpointAddress	= USB_CONTROL_ENDPOINT;
	udev->def_ep_desc.bmAttributes		= UE_CONTROL;

	USETW (udev->def_ep_desc.wMaxPacketSize, USB_MAX_IPACKET);

	udev->def_ep_desc.bInterval		= 0;

	udev->quirks			= &usbd_no_quirk;
	udev->address			= USB_START_ADDR;
	udev->ddesc.bMaxPacketSize	= 0;
	udev->depth			= depth;
	udev->powersrc			= up;
	udev->myhub			= up->parent;

	up->device			= udev;

	/* Locate port on upstream high speed hub */

	for
	(	adev = udev, hub = up->parent;
		hub != NULL && hub->speed != USB_SPEED_HIGH;
		adev = hub, hub = hub->myhub
	)
		/* vazio */;

	if (hub)
	{
		for (p = 0; /* abaixo */; p++)
		{
			if (p >= hub->hub->hubdesc.bNbrPorts)
				panic ("usbd_new_device: cannot find HS port\n");

			if (hub->hub->ports[p].device == adev)
			{
				udev->myhsport = &hub->hub->ports[p];
#ifdef	USB_MSG
				printf ("%g: high speed port %d\n", p + 1);
#endif	USB_MSG
				break;
			}
		}
	}
	else
	{
		udev->myhsport = NULL;
	}

	udev->speed		= speed;
	udev->langid		= USBD_NOLANG;
	udev->cookie		= ++usb_cookie_no;

	/*
	 *	Establish the default pipe
	 */
	if (err = usbd_setup_pipe (udev, 0, &udev->def_ep, USBD_DEFAULT_INTERVAL, &udev->default_pipe))
		{ usbd_remove_device (udev, up); return (err); }

	/*
	 *	Set the address.  Do this early; some devices need that.
	 *	Try a few times in case the device is slow (i.e. outside specs.)
	 */
	for (i = 0; i < 15; i++)
	{
		if ((err = usbd_set_address (udev, addr)) == 0)
			break;

		usbd_delay_ms (udev, 200);

		if ((i & 3) == 3)
		{
			printf ("%g: set address %d failed - trying a port reset\n", addr);

			usbd_reset_port (up->parent, port, &ps);
		}
	}

	if (err)
	{
		printf ("%g: set address %d failed\n", addr);
		err = USBD_SET_ADDR_FAILED;
		usbd_remove_device (udev, up);
		return (err);
	}

	/* Allow device time to set new address */

	usbd_delay_ms (udev, USB_SET_ADDRESS_SETTLE);

	udev->address		= addr;	/* New device address now */
	bus->devices[addr]	= udev;

	dd = &udev->ddesc;

	/* Get the first 8 bytes of the device descriptor. */

	if (err = usbd_get_desc (udev, UDESC_DEVICE, 0, USB_MAX_IPACKET, dd))
	{
		printf
		(	"usb%d: Não consegui obter o primeiro descritor do end. %d\n",
			parent->unit, addr
		);

		usbd_remove_device (udev, up);
		return (err);
	}

	if (speed == USB_SPEED_HIGH)
	{
		/* Max packet size must be 64 (sec 5.5.3) */

		if (dd->bMaxPacketSize != USB_2_MAX_CTRL_PACKET)
			dd->bMaxPacketSize = USB_2_MAX_CTRL_PACKET;
	}

#ifdef	USB_MSG
	printf
	(	"usbd_new_device: adding unit addr = %d, rev = %02x, class = %d\n",
		addr, UGETW (dd->bcdUSB), dd->bDeviceClass
	);

	printf
	(
		"\tsubclass = %d, protocol = %d, maxpacket = %d, len = %d, speed = %d\n", 
		dd->bDeviceSubClass, dd->bDeviceProtocol, dd->bMaxPacketSize, dd->bLength, udev->speed
	);
#endif	USB_MSG

	if (dd->bDescriptorType != UDESC_DEVICE)
	{
		/* Illegal device descriptor */

		printf ("usbd_new_device: illegal descriptor %d\n", dd->bDescriptorType);
		usbd_remove_device (udev, up);
		return (USBD_INVAL);
	}

	if (dd->bLength < USB_DEVICE_DESCRIPTOR_SIZE)
	{
		printf ("usbd_new_device: bad length %d\n", dd->bLength);
		usbd_remove_device (udev, up);
		return (USBD_INVAL);
	}

	USETW (udev->def_ep_desc.wMaxPacketSize, dd->bMaxPacketSize);

	if (err = usbd_reload_device_desc (udev))
	{
		printf ("usbd_new_device: addr=%d, getting full desc failed\n", addr);
		usbd_remove_device (udev, up);
		return (err);
	}

#ifdef	USB_MSG
	printf
	(	"usbd_new_device: idVendor = %04X, idProduct = %04X, bcdDevice = %04X, "
		"iSerialNumber = %02X, bNumConfigurations = %02X\n",
		UGETW (dd->idVendor), UGETW (dd->idProduct), UGETW (dd->bcdDevice),
		dd->iSerialNumber, dd->bNumConfigurations
	);

	printf ("usbd_new_device: setting device address = %d\n", addr);
#endif	USB_MSG

	/* Assume 100mA bus powered for now. Changed when configured */

	udev->power		= USB_MIN_POWER;
	udev->self_powered	= 0;

#ifdef	USB_MSG
	printf ("usbd_new_device: new dev (addr %d), udev = %P, parent = %P\n", addr, udev, parent);
#endif	USB_MSG

	if (err = usbd_probe_and_attach (parent, udev, port, addr))
		{ usbd_remove_device (udev, up); return (err); }

  	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_new_device */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_getnewaddr (struct usbd_bus *bus)
{
	int		addr;

	for (addr = 1; addr < USB_MAX_DEVICES; addr++)
	{
		if (bus->devices[addr] == 0)
			return (addr);
	}

	return (-1);

}	/* end usbd_getnewaddr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_setup_pipe (struct usbd_device *udev, struct usbd_interface *iface,
		struct usbd_endpoint *ep, int ival, struct usbd_pipe **pipe_ptr)
{
	struct usbd_pipe	*p;
	int			err;

	if ((p = malloc_byte (udev->bus->pipe_size)) == NULL)
		{ printf ("Memória esgotada\n"); return (USBD_NOMEM); }

	p->device	= udev;
	p->iface	= iface;
	p->endpoint	= ep;
	p->refcnt	= 1;
	p->intrxfer	= 0;
	p->running	= 0;
	p->aborting	= 0;
	p->repeat	= 0;
	p->interval	= ival;

	ep->refcnt     += 1;

	p->first_xfer = NULL; p->last_xfer = &p->first_xfer;

	if (err = udev->bus->methods->open_pipe (p))
	{
		printf
		(	"usbd_setup_pipe: endpoint = 0x%X failed, error = %s\n",
			 ep->edesc->bEndpointAddress, usbd_errstr (err)
		);

		free_byte (p); return (err);
	}

#if (0)	/*******************************************************/
	/* Clear any stall and make sure DATA0 toggle will be used next */

	if (UE_GET_ADDR (ep->edesc->bEndpointAddress) != USB_CONTROL_ENDPOINT)
		usbd_clear_endpoint_stall (p);
#endif	/*******************************************************/

	*pipe_ptr = p;

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_setup_pipe */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_remove_device (struct usbd_device *udev, struct usbd_port *up)
{
	if (udev->default_pipe != NULL)
		usbd_kill_pipe (udev->default_pipe);

	up->device				= NULL;
	udev->bus->devices[udev->address]	= NULL;

	free_byte (udev);

}	/* end usbd_remove_device */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_reload_device_desc (struct usbd_device *udev)
{
	int		err = 0, i;

#ifdef	USB_MSG
	printf ("usbd_reload_device_desc\n");
#endif	USB_MSG

	/* Get the full device descriptor */

	for (i = 0; /* abaixo */; ++i)
	{
		if (i >= 8)
			return (err);
			
		if ((err = usbd_get_device_desc (udev, &udev->ddesc)) == 0)
			break;

		usbd_delay_ms (udev, 200);
	}

	/* Figure out what's wrong with this device */

	udev->quirks = usbd_find_quirk (&udev->ddesc);

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_reload_device_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_probe_and_attach (struct device *parent, struct usbd_device *udev, int port, int addr)
{
	struct usb_attach_arg		uaa;
	struct usb_device_descriptor	*dd = &udev->ddesc;
	int				found, i, confi, nifaces;
	int				err;
	struct usbd_interface		**ifaces;
	struct device			*bdev;

	/* 
	 *	XXX uaa is a auto var. Not a problem as it _should_ be used only
	 *	during probe and attach. Should be changed however.
	 */
	if ((bdev = device_add_child (parent, NULL, -1)) == NULL)
	{
		printf ("%s: Device creation failed\n", udev->bus->bdev->nameunit);
		return (USBD_INVAL);
	}

	bdev->ivars	= &uaa;
	bdev->flags    |= DF_QUIET;

	uaa.device	= udev;
	uaa.iface	= NULL;
	uaa.ifaces	= NULL;
	uaa.nifaces	= 0;
	uaa.usegeneric	= 0;
	uaa.port	= port;
	uaa.configno	= UHUB_UNK_CONFIGURATION;
	uaa.ifaceno	= UHUB_UNK_INTERFACE;
	uaa.vendor	= UGETW (dd->idVendor);
	uaa.product	= UGETW (dd->idProduct);
	uaa.release	= UGETW (dd->bcdDevice);

#ifdef	USB_MSG
	printf ("\e[31m");
	printf
	(	"usbd_probe_and_attach: vendor = %04X, product = %04X, release = %04X\n",
		uaa.vendor, uaa.product, uaa.release
	);
	printf  ("\e[0m");
#endif	USB_MSG

	/*
	 *	First try with device specific drivers
	 */
#ifdef	USB_MSG
	printf ("usbd_probe_and_attach: trying device specific drivers\n");
#endif	USB_MSG

	if (device_probe_and_attach (bdev) == 0)
	{
		if ((udev->subdevs = malloc_byte (2 * sizeof (struct device *))) == NULL)
			return (USBD_NOMEM);

		udev->subdevs[0] = bdev;
		udev->subdevs[1] = NULL;

		return (USBD_NORMAL_COMPLETION);
	}

#ifdef	USB_MSG
	printf ("usbd_probe_and_attach: no device specific driver found\n");

	printf ("usbd_probe_and_attach: looping over %d configurations\n", dd->bNumConfigurations);
#endif	USB_MSG

	/*
	 *	Aloca o vetor de ponteiros
	 */
	if ((ifaces = malloc_byte (256 * sizeof (struct usbd_interface *))) == NULL)
		return (USBD_NOMEM);

	/*
	 *	Next try with interface drivers
	 */
	for (confi = 0; confi < dd->bNumConfigurations; confi++)
	{
#ifdef	USB_MSG
		printf ("usbd_probe_and_attach: trying config idx=%d\n", confi);
#endif	USB_MSG

		if (err = usbd_set_config_index (udev, confi, 1))
		{
			printf
			(	"%s: port %d, set config at addr %d failed, err = %d\n",
				parent->nameunit, port, addr, err
			);

#if (0)	/*******************************************************/
			device_delete_child (parent, bdev);
#endif	/*******************************************************/

			free_byte (ifaces); return (err);
		}

		nifaces		= udev->cdesc->bNumInterface;
		uaa.configno	= udev->cdesc->bConfigurationValue;

		for (i = 0; i < nifaces; i++)
			ifaces[i] = &udev->ifaces[i];

		uaa.ifaces	= ifaces;
		uaa.nifaces	= nifaces;

		if ((udev->subdevs = malloc_byte ((nifaces + 1) * sizeof (struct device *))) == NULL)
		{
#if (0)	/*******************************************************/
			device_delete_child (parent, bdev);
#endif	/*******************************************************/

			free_byte (ifaces); return (USBD_NOMEM);
		}

		found = 0;

		for (i = 0; i < nifaces; i++)
		{
			if (ifaces[i] == NULL)
				continue; /* interface already claimed */

			uaa.iface	= ifaces[i];
			uaa.ifaceno	= ifaces[i]->idesc->bInterfaceNumber;

#if (0)	/*******************************************************/
			if (dv = (device_probe_and_attach ((bdev)) == 0 ? (bdev) : 0))
#endif	/*******************************************************/

			if (device_probe_and_attach (bdev) == 0)
			{
				udev->subdevs[found++] = bdev;
				udev->subdevs[found]   = NULL;

				ifaces[i] = 0; /* consumed */

				/* create another child for the next iface */

				if ((bdev = device_add_child (parent, NULL, -1)) == NULL)
				{
					printf ("%s: Device creation failed\n", udev->bus->bdev->nameunit);
					free_byte (ifaces); return (USBD_NORMAL_COMPLETION);
				}

				bdev->ivars = &uaa;
				bdev->flags |= DF_QUIET;
			}
		}

		if (found != 0)
		{
			/* remove the last created child again; it is unused */

			device_delete_child (parent, bdev);
			free_byte (ifaces); return (USBD_NORMAL_COMPLETION);
		}

		free_byte (udev->subdevs);	udev->subdevs = NULL;

	}

	/* No interfaces were attached in any of the configurations */

	if (dd->bNumConfigurations > 1)			/* don't change if only 1 config */
		usbd_set_config_index (udev, 0, 0);

#ifdef	USB_MSG
	printf ("usbd_probe_and_attach: no interface drivers found\n");

	printf ("usbd_probe_and_attach: trying the generic driver\n");
#endif	USB_MSG

	/* Finally try the generic driver */

	uaa.iface	= NULL;
	uaa.usegeneric	= 1;
	uaa.configno	= UHUB_UNK_CONFIGURATION;
	uaa.ifaceno	= UHUB_UNK_INTERFACE;

#if (0)	/*******************************************************/
	if (dv = (device_probe_and_attach ((bdev)) == 0 ? (bdev) : 0))
#endif	/*******************************************************/

	if (device_probe_and_attach (bdev) == 0)
	{
		if ((udev->subdevs = malloc_byte (2 * sizeof (struct device *))) == NULL)
			{ free_byte (ifaces); return (USBD_NOMEM); }

		udev->subdevs[0] = bdev;
		udev->subdevs[1] = NULL;

		free_byte (ifaces); return (USBD_NORMAL_COMPLETION);
	}

	/* 
	 * The generic attach failed, but leave the device as it is.
	 * We just did not find any drivers, that's all.  The device is
	 * fully operational and not harming anyone.
	 */
#ifdef	USB_MSG
	printf ("usbd_probe_and_attach: generic attach failed\n");
#endif	USB_MSG

#if (0)	/*******************************************************/
	device_delete_child (parent, bdev);
#endif	/*******************************************************/

	free_byte (ifaces); return (USBD_NORMAL_COMPLETION);

}	/* end usbd_probe_and_attach */

/*
 ****************************************************************
 *	Espera alguns milisegundos				*
 ****************************************************************
 */
void
usb_delay_ms (struct usbd_bus *bus, int ms)
{
	/* Wait at least two clock ticks so we know the time has passed */

	DELAY ((ms + 1) * 1000);

#if (0)	/*******************************************************/
	if (bus->use_polling || cold)
		delay((ms+1) * 1000);
	else
		tsleep(&ms, PRIBIO, "usbdly", (ms*hz+999)/1000 + 1);
#endif	/*******************************************************/

}	/* end usb_delay_ms */
