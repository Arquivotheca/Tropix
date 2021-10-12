/*
 ****************************************************************
 *								*
 *			usbdi_util.c				*
 *								*
 *	"Driver" para dispositivo USB				*
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

#include "../usb/usbhid.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_config_desc (struct usbd_device *udev, int confidx, struct usb_config_descriptor *d)
{
	int		err;

#ifdef	USB_MSG
	printf ("usbd_get_config_desc: confidx=%d\n", confidx);
#endif	USB_MSG

	if (err = usbd_get_desc (udev, UDESC_CONFIG, confidx, USB_CONFIG_DESCRIPTOR_SIZE, d))
		return (err);

	if (d->bDescriptorType != UDESC_CONFIG)
	{
		printf
		(	"usbd_get_config_desc: descritor inválido tam = %d :: %d tipo = %d :: %d\n",
			d->bLength, USB_CONFIG_DESCRIPTOR_SIZE, d->bDescriptorType, UDESC_CONFIG
		);

		return (USBD_INVAL);
	}

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_get_config_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_device_desc (struct usbd_device *udev, struct usb_device_descriptor *d)
{
	return (usbd_get_desc (udev, UDESC_DEVICE, 0, USB_DEVICE_DESCRIPTOR_SIZE, d));

}	/* end usbd_get_device_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_device_status (struct usbd_device *udev, struct usb_status *st)
{
	struct usb_device_request	req;

	req.bmRequestType	= UT_READ_DEVICE;
	req.bRequest		= UR_GET_STATUS;

	USETW (req.wValue, 0);
	USETW (req.wIndex, 0);
	USETW (req.wLength, sizeof (struct usb_status));

	return (usbd_do_request (udev, &req, st));

}	/* end usbd_get_device_status */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_port_status (struct usbd_device *udev, int port, struct usb_port_status *ps)
{
	struct usb_device_request	req;

	req.bmRequestType	= UT_READ_CLASS_OTHER;
	req.bRequest		= UR_GET_STATUS;

	USETW (req.wValue, 0);
	USETW (req.wIndex, port);
	USETW (req.wLength, sizeof *ps);

	return (usbd_do_request (udev, &req, ps));

}	/* end usbd_get_port_status */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_clear_port_feature (struct usbd_device *udev, int port, int sel)
{
	struct usb_device_request	req;

	req.bmRequestType	= UT_WRITE_CLASS_OTHER;
	req.bRequest		= UR_CLEAR_FEATURE;

	USETW (req.wValue, sel);
	USETW (req.wIndex, port);
	USETW (req.wLength, 0);

	return (usbd_do_request (udev, &req, 0));

}	/* end usbd_clear_port_feature */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_set_port_feature (struct usbd_device *udev, int port, int sel)
{
	struct usb_device_request	req;

	req.bmRequestType	= UT_WRITE_CLASS_OTHER;
	req.bRequest		= UR_SET_FEATURE;

	USETW (req.wValue, sel);
	USETW (req.wIndex, port);
	USETW (req.wLength, 0);

	return (usbd_do_request (udev, &req, 0));

}	/* end usbd_set_port_feature */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_report_descriptor (struct usbd_device *udev, int ifcno, int size, void *d)
{
	struct usb_device_request	req;

	req.bmRequestType	= UT_READ_INTERFACE;
	req.bRequest		= UR_GET_DESCRIPTOR;

	USETW2 (req.wValue, UDESC_REPORT, 0);	/* report id should be 0 */
	USETW  (req.wIndex, ifcno);
	USETW  (req.wLength, size);

	return (usbd_do_request (udev, &req, d));

}	/* end usbd_get_report_descriptor */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct usb_hid_descriptor *
usbd_get_hid_descriptor (struct usbd_interface *ifc)
{
	struct usb_interface_descriptor	*idesc = ifc->idesc;
	struct usbd_device		*udev;
	struct usb_config_descriptor	*cdesc;
	struct usb_hid_descriptor	*hd;
	char				*p, *End;

	if (idesc == NULL)
		return (0);

	udev   = ifc->device;
	cdesc = udev->cdesc;

	p   = (char *)idesc + idesc->bLength;
	End = (char *)cdesc + UGETW (cdesc->wTotalLength);

	for (/* acima */; p < End; p += hd->bLength)
	{
		hd = (struct usb_hid_descriptor *)p;

		if (p + hd->bLength <= End && hd->bDescriptorType == UDESC_HID)
			return (hd);

		if (hd->bDescriptorType == UDESC_INTERFACE)
			break;
	}

	return (NULL);

}	/* end usbd_get_hid_descriptor */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_read_report_desc (struct usbd_interface *ifc, void **descp, int *sizep)
{
	struct usb_interface_descriptor	*id;
	struct usb_hid_descriptor	*hid;
	struct usbd_device		*udev;
	int	 			err;

	udev = ifc->device;

	if ((id = ifc->idesc) == NULL)
		return (USBD_INVAL);

	if ((hid = usbd_get_hid_descriptor (ifc)) == NULL)
		return (USBD_IOERROR);

	*sizep = UGETW(hid->descrs[0].wDescriptorLength);

	if ((*descp = malloc_byte (*sizep)) == NULL)
		return (USBD_NOMEM);

	if (err = usbd_get_report_descriptor (udev, id->bInterfaceNumber, *sizep, *descp))
		{ free_byte (*descp); *descp = NULL; return (err); }

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_read_report_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_get_desc (struct usbd_device *udev, int type, int index, int len, void *desc)
{
	struct usb_device_request	req;

	req.bmRequestType = UT_READ_DEVICE;
	req.bRequest	  = UR_GET_DESCRIPTOR;

	USETW2 (req.wValue, type, index);
	USETW  (req.wIndex, 0);
	USETW  (req.wLength, len);

	return (usbd_do_request (udev, &req, desc));

}	/* end usbd_get_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_set_address (struct usbd_device *udev, int addr)
{
	struct usb_device_request req;

	req.bmRequestType	= UT_WRITE_DEVICE;
	req.bRequest		= UR_SET_ADDRESS;

	USETW (req.wValue, addr);
	USETW (req.wIndex, 0);
	USETW (req.wLength, 0);

	return (usbd_do_request (udev, &req, 0));

}	/* end usbd_set_address */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_bulk_transfer_cb (struct usbd_xfer *xfer, void *priv, int status)
{
	EVENTDONE (&xfer->done);

}	/* end usbd_bulk_transfer_cb */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_bulk_transfer (struct usbd_xfer *xfer, struct usbd_pipe *pipe_ptr,
		   ushort flags, ulong timeout, void *buf, ulong *size, char *lbl)
{
	int		err;

	usbd_setup_xfer (xfer, pipe_ptr, 0, buf, *size, flags, timeout, usbd_bulk_transfer_cb);

	printf ("usbd_bulk_transfer: start transfer %d bytes\n", *size);

#if (0)	/*******************************************************/
	s = splusb ();		/* don't want callback until tsleep() */

	if ((err = usbd_transfer (xfer)) != USBD_IN_PROGRESS)
		{ splx(s); return (err); }

	error = tsleep (xfer, PZERO | PCATCH, lbl, 0);

	splx (s);

	if (error)
	{
		printf ("usbd_bulk_transfer: tsleep=%d\n", error);
		usbd_abort_pipe (pipe_ptr);

		return (USBD_INTERRUPTED);
	}
#endif	/*******************************************************/

	EVENTCLEAR (&xfer->done);

	if ((err = usbd_transfer (xfer)) != USBD_IN_PROGRESS)
		{ return (err); }

	EVENTWAIT (&xfer->done, PBLKIO);

	usbd_get_xfer_status (xfer, NULL, NULL, size, &err);

	printf ("usbd_bulk_transfer: transferred %d\n", *size);

	if (err)
	{
		printf ("usbd_bulk_transfer: error=%d\n", err);

		usbd_clear_endpoint_stall (pipe_ptr);
	}

	return (err);

}	/* end usbd_bulk_transfer */
