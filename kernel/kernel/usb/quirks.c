/*
 ****************************************************************
 *								*
 *			quirks.c				*
 *								*
 *	Identifica dispositivos mal comportados			*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.5.0, de 26.01.04				*
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
 ****** Tabela de Dispositivos Esdrúxulos ***********************
 */
#define ANY	0xFFFF

const struct usbd_quirk_entry
{
	ushort			idVendor;
	ushort			idProduct;
	ushort			bcdDevice;

	struct usbd_quirks	quirks;

}	usb_quirks[] =
{
	{ USB_VENDOR_KYE,		USB_PRODUCT_KYE_NICHE,			0x100,
	{ UQ_NO_SET_PROTO }								},

	{ USB_VENDOR_INSIDEOUT,		USB_PRODUCT_INSIDEOUT_EDGEPORT4,	0x094,
	{ UQ_SWAP_UNICODE }								},

	{ USB_VENDOR_DALLAS,		USB_PRODUCT_DALLAS_J6502,		0x0A2,
	{ UQ_BAD_ADC }									},

	{ USB_VENDOR_DALLAS,		USB_PRODUCT_DALLAS_J6502,		0x0A2,
	{ UQ_AU_NO_XU }									},

	{ USB_VENDOR_ALTEC,		USB_PRODUCT_ALTEC_ADA70,		0x103,
	{ UQ_BAD_ADC }									},

	{ USB_VENDOR_ALTEC,		USB_PRODUCT_ALTEC_ASC495,		0x000,
	{ UQ_BAD_AUDIO }								},

	{ USB_VENDOR_QTRONIX,		USB_PRODUCT_QTRONIX_980N,		0x110,
	{ UQ_SPUR_BUT_UP }								},

	{ USB_VENDOR_ALCOR2,		USB_PRODUCT_ALCOR2_KBD_HUB,		0x001,
	{ UQ_SPUR_BUT_UP }								},

	{ USB_VENDOR_MCT,		USB_PRODUCT_MCT_HUB0100,		0x102,
	{ UQ_BUS_POWERED }								},

	{ USB_VENDOR_MCT,		USB_PRODUCT_MCT_USB232,			0x102,
	{ UQ_BUS_POWERED }								},

	{ USB_VENDOR_METRICOM,		USB_PRODUCT_METRICOM_RICOCHET_GS,	0x100,
	{ UQ_ASSUME_CM_OVER_DATA | UQ_NO_STRINGS }					},

	{ USB_VENDOR_SANYO,		USB_PRODUCT_SANYO_SCP4900,		0x000,
	{ UQ_ASSUME_CM_OVER_DATA | UQ_NO_STRINGS }					},

	{ USB_VENDOR_TI,		USB_PRODUCT_TI_UTUSB41,			0x110,
	{ UQ_POWER_CLAIM }								},

	{ USB_VENDOR_TELEX,		USB_PRODUCT_TELEX_MIC1,			0x009,
	{ UQ_AU_NO_FRAC }								},

	{ USB_VENDOR_SILICONPORTALS,	USB_PRODUCT_SILICONPORTALS_YAPPHONE,	0x100,
	{ UQ_AU_INP_ASYNC }								},

	{ USB_VENDOR_LOGITECH,		USB_PRODUCT_LOGITECH_UN53B,		ANY,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_CMOTECH,		USB_PRODUCT_CMOTECH_CDMAMODEM,		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	/* XXX These should have a revision number, but I don't know what they are */

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_895C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_880C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_815C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_810C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_830C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	{ USB_VENDOR_HP,		USB_PRODUCT_HP_1220C,			ANY,
	{ UQ_BROKEN_BIDIR }								},

	/* YAMAHA router's ucdDevice is the version of farmware and often changes. */

	{ USB_VENDOR_YAMAHA,		USB_PRODUCT_YAMAHA_RTA54I,		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_YAMAHA,		USB_PRODUCT_YAMAHA_RTA55I,		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_YAMAHA,		USB_PRODUCT_YAMAHA_RTW65B,		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_YAMAHA,		USB_PRODUCT_YAMAHA_RTW65I,		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_QUALCOMM,		USB_PRODUCT_QUALCOMM_CDMA_MSM, 		ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_QUALCOMM2,		USB_PRODUCT_QUALCOMM2_CDMA_MSM, 	ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_SUNTAC,		USB_PRODUCT_SUNTAC_AS64LX, 		0x100,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	{ USB_VENDOR_MOTOROLA2,		USB_PRODUCT_MOTOROLA2_A41XV32X, 	ANY,
	{ UQ_ASSUME_CM_OVER_DATA }							},

	 /* Devices which should be ignored by uhid */

	{ USB_VENDOR_APC,		USB_PRODUCT_APC_UPS, 			ANY,
	{ UQ_HID_IGNORE }								},

	{ USB_VENDOR_BELKIN,		USB_PRODUCT_BELKIN_F6C550AVR, 		ANY,
	{ UQ_HID_IGNORE }								},

	{ USB_VENDOR_DELORME,		USB_PRODUCT_DELORME_EARTHMATE, 		ANY,
	{ UQ_HID_IGNORE }								},

	{ USB_VENDOR_MGE,		USB_PRODUCT_MGE_UPS1, 			ANY,
	{ UQ_HID_IGNORE }								},

	{ USB_VENDOR_MGE,		USB_PRODUCT_MGE_UPS2, 			ANY,
	{ UQ_HID_IGNORE }								},

	{ 0,				0,					0,
	{ 0 }										}
};

const struct usbd_quirks usbd_no_quirk = { 0 };

/*
 ****************************************************************
 *	Consulta a tabela de dispositivos mal comportados	*
 ****************************************************************
 */
const struct usbd_quirks *
usbd_find_quirk (struct usb_device_descriptor *d)
{
	const struct usbd_quirk_entry	*t;
	ushort				vendor, product, revision;

	vendor   = UGETW (d->idVendor);
	product  = UGETW (d->idProduct);
	revision = UGETW (d->bcdDevice);

	for (t = usb_quirks; t->idVendor != 0; t++)
	{
		if
		(	t->idVendor  == vendor &&
			t->idProduct == product &&
			(t->bcdDevice == ANY || t->bcdDevice == revision)
		)
			break;
	}

	return (&t->quirks);

}	/* end usbd_find_quirk */

#if (0)	/*******************************************************/
	{ USB_VENDOR_BTC,		USB_PRODUCT_BTC_BTC7932,		0x100,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_ADS,		USB_PRODUCT_ADS_UBS10BT,		0x002,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_PERACOM,		USB_PRODUCT_PERACOM_SERIAL1,		0x101,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_WACOM,		USB_PRODUCT_WACOM_CT0405U,		0x101,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_ACERP,		USB_PRODUCT_ACERP_ACERSCAN_320U,	0x000,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_NEODIO,		USB_PRODUCT_NEODIO_ND5010,		0x100,
	{ UQ_NO_STRINGS }								},

	{ USB_VENDOR_LOGITECH,		USB_PRODUCT_LOGITECH_WMRPAD,		ANY,
	{ UQ_NO_STRINGS }								},

#endif	/*******************************************************/

