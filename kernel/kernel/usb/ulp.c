/*
 ****************************************************************
 *								*
 *			ulp.c					*
 *								*
 *	"Driver" para impressoras USB				*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.9.0, de 30.05.06				*
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
#include "../h/region.h"

#include "../h/tty.h"

#include "../h/usb.h"

#include "../h/inode.h"
#include "../h/ioctl.h"

#include "../usb/hid.h"
#include "../usb/mouse.h"
#include "../usb/usbhid.h"

#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Declaração do Driver ************************************
 */
int	ulp_probe (struct device *);
int	ulp_attach (struct device *);
int	ulp_detach (struct device *);

const struct driver ulp_driver =
{
	"ulp",

	ulp_probe,
	ulp_attach,
   	ulp_detach
};

/*
 ****** Informações sobre uma unidade ***************************
 */
#define	TIMEOUT			(16 * 4)
#define	STEP			1

#define UR_GET_DEVICE_ID	0
#define UR_GET_PORT_STATUS	1
#define UR_SOFT_RESET		2

#define	ULPT_BSIZE		16384

#define	ULPT_OPEN		0x01	/* device is open */
#define	ULPT_OBUSY		0x02	/* printer is busy doing output */
#define	ULPT_INIT		0x04	/* waiting to initialize for open */

#define	ULPT_NOPRIME		0x40	/* don't prime on open */

#define	LPS_NERR		0x08	/* printer no error */
#define	LPS_SELECT		0x10	/* printer selected */
#define	LPS_NOPAPER		0x20	/* printer out of paper */

#define LPS_INVERT		(LPS_SELECT|LPS_NERR)
#define LPS_MASK		(LPS_SELECT|LPS_NERR|LPS_NOPAPER)

struct ulp_softc
{
	struct device		*sc_dev;		/* base device */
	struct usbd_device	*sc_udev;
	struct usbd_interface	*sc_iface;		/* interface */

	int			sc_ifaceno;

	int			sc_out;
	struct usbd_pipe	*sc_out_pipe;		/* bulk out pipe */

	int			sc_in;
	struct usbd_pipe	*sc_in_pipe;		/* bulk in pipe */

	struct usbd_xfer	*sc_in_xfer1;
	struct usbd_xfer	*sc_in_xfer2;

	char			sc_area[64];		/* somewhere to dump input */

	char			sc_state;
	char			sc_flags;
	char			sc_laststatus;
	char			sc_dying;

	int			sc_refcnt;
};

struct devclass	*ulp_devclass;

/*
 ******	Protótipos de funções ***********************************
 */
void		ulp_reset (struct ulp_softc *sc);
void		ulp_input (struct usbd_xfer *xfer, void *priv, int status);
int		ulp_status (struct ulp_softc *sc);
void		ulp_do_write (struct ulp_softc *sc, IOREQ *iop);
void		ulp_print_status_msg (int status, struct ulp_softc *sc);

/*
 ****************************************************************
 *	Função de "probe"					*
 ****************************************************************
 */
int
ulp_probe (struct device *dev)
{
	struct usb_attach_arg		*uaa = dev->ivars;
	struct usb_interface_descriptor *id;

#ifdef	USB_MSG
	printf ("ulp_probe\n");
#endif	USB_MSG

	if (uaa == NULL || uaa->iface == NULL || (id = uaa->iface->idesc) == NULL)
		return (UPROBE_NONE);

	if (id->bInterfaceClass == UICLASS_PRINTER && id->bInterfaceSubClass == UISUBCLASS_PRINTER)
	{
		switch (id->bInterfaceProtocol)
		{
		    case UIPROTO_PRINTER_UNI:
		    case UIPROTO_PRINTER_BI:
		    case UIPROTO_PRINTER_1284:
			return (UPROBE_IFACECLASS_IFACESUBCLASS_IFACEPROTO);

		    default:
			break;
		}
	}

	return (UPROBE_NONE);

}	/* end ulp_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
ulp_attach (struct device *dev)
{
        struct ulp_softc		*sc;
        struct usb_attach_arg		*uaa   = dev->ivars;
	struct usbd_device		*udev  = uaa->device;
	struct usbd_interface		*iface = uaa->iface;
	struct usb_interface_descriptor	*ifcd  = iface->idesc;
	struct usb_interface_descriptor	*id, *iend;
	struct usb_endpoint_descriptor	*ed;
	struct usb_config_descriptor	*cdesc;
	char				*devinfo;
	int				i, err, altno;

	/*
	 *	Aloca a estrutura "softc"
	 */
	if ((sc = dev->softc = malloc_byte (sizeof (struct ulp_softc))) == NULL)
		return (-1);

	memclr (sc, sizeof (struct ulp_softc));

	sc->sc_dev = dev;

	if ((devinfo = malloc_byte (1024)) == NULL)
		{ printf ("%s: memória esgotada\n", dev->nameunit); goto bad; }

	usbd_devinfo (udev, 0, devinfo);

#if (0)	/*******************************************************/
	device_set_desc_copy (dev, devinfo);
#endif	/*******************************************************/

	printf ("\e[31m");
	printf
	(	"%s: %s, iclass %d/%d\n", dev->nameunit,
		devinfo, ifcd->bInterfaceClass, ifcd->bInterfaceSubClass
	);
	printf ("\e[0m");

	free_byte (devinfo);

	if ((cdesc = udev->cdesc) == NULL)
	{
		printf ("%s: failed to get configuration descriptor\n", dev->nameunit);
		goto bad;
	}

	iend = (struct usb_interface_descriptor *)((char *)cdesc + UGETW (cdesc->wTotalLength));

	/* Step through all the descriptors looking for bidir mode */

	for (id = ifcd, altno = 0; /* abaixo */; id = (void *)((char *)id + id->bLength))
	{
		if (id >= iend)
			{ id = ifcd; break; }

		if (id->bDescriptorType == UDESC_INTERFACE && id->bInterfaceNumber == ifcd->bInterfaceNumber)
		{
			if
			(	id->bInterfaceClass    == UICLASS_PRINTER    &&
				id->bInterfaceSubClass == UISUBCLASS_PRINTER &&
				id->bInterfaceProtocol == UIPROTO_PRINTER_BI
			)
				break;

			altno++;
		}
	}

	if (id != ifcd)
	{
		/* Found a new bidir setting */

		printf ("ulpt_attach: set altno = %d\n", altno);

		if (err = usbd_set_interface (iface, altno))
		{
			printf ("%s: setting alternate interface failed\n", dev->nameunit);
			goto bad;
		}
	}

#if (0)	/*******************************************************/
	uchar				epcount;
	epcount = 0;	usbd_endpoint_count (iface, &epcount);
	epcount = iface->idesc->bNumEndpoints;
#endif	/*******************************************************/

	sc->sc_in = sc->sc_out = -1;

	for (i = 0; i < iface->idesc->bNumEndpoints; i++)
	{
		int	direction, xfertype;

		if ((ed = usbd_interface2endpoint_descriptor (iface, i)) == NULL)
			{ printf ("%s: couldn't get ep %d\n", dev->nameunit, i); goto bad; }

		direction = UE_GET_DIR (ed->bEndpointAddress);
		xfertype  = UE_GET_XFERTYPE (ed->bmAttributes);

		if   (direction == UE_DIR_IN && xfertype == UE_BULK)
			sc->sc_in = ed->bEndpointAddress;
		elif (direction == UE_DIR_OUT && xfertype == UE_BULK)
			sc->sc_out = ed->bEndpointAddress;
	}

	if (sc->sc_out == -1)
		{ printf ("%s: NÃO achei o \"endpoint\" de escrita\n", dev->nameunit); goto bad; }

	if (udev->quirks && (udev->quirks->uq_flags & UQ_BROKEN_BIDIR))
		sc->sc_in = -1;		/* This device doesn't handle reading properly */

	printf ("\e[31m");
	printf ("%s: usando modo %sdirecional\n", dev->nameunit, sc->sc_in >= 0 ? "bi" : "uni");
	printf ("\e[0m");

	sc->sc_iface	= iface;
	sc->sc_ifaceno	= id->bInterfaceNumber;
	sc->sc_udev	= udev;

	ulp_devclass	= dev->devclass;

	return (0);

    bad:
	free_byte (sc);	dev->softc = NULL;
	return (-1);

}	/* end ulp_attach */

int
ulp_detach (struct device *dev)
{
	struct ulp_softc	*sc = dev->softc;

	sc->sc_dying = 1;

	if (sc->sc_out_pipe != NULL)
		usbd_abort_pipe (sc->sc_out_pipe);

	if (sc->sc_in_pipe != NULL)
		usbd_abort_pipe (sc->sc_in_pipe);

#if (0)	/*******************************************************/
	int			s;
	s = splusb();

	if (--sc->sc_refcnt >= 0)
	{
		/* There is noone to wake, aborting the pipe is enough */
		/* Wait for processes to go away. */
		usb_detach_wait(USBDEV(sc->sc_dev));
	}
	splx(s);

#if defined(__NetBSD__) || defined(__OpenBSD__)
	/* locate the major number */
	for (maj = 0; maj < nchrdev; maj++)
		if (cdevsw[maj].d_open == ulptopen)
			break;

	/* Nuke the vnodes for any open instances (calls close). */
	mn = self->dv_unit;
	vdevgone(maj, mn, mn, VCHR);
#elif defined(__FreeBSD__)
	vp = SLIST_FIRST(&sc->dev->si_hlist);
	if (vp)
		VOP_REVOKE(vp, REVOKEALL);
	vp = SLIST_FIRST(&sc->dev_noprime->si_hlist);
	if (vp)
		VOP_REVOKE(vp, REVOKEALL);

	destroy_dev(sc->dev);
	destroy_dev(sc->dev_noprime);
#endif
#endif	/*******************************************************/

	return (0);

}	/* end ulp_detach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
ulpopen (dev_t dev, int oflag)
{
	struct ulp_softc	*sc;
	struct device		*device;
	int			unit, count;

	/*
	 *	Prólogo
	 */
	if (ulp_devclass == NULL)
		{ u.u_error = ENXIO; return (-1); }

	unit = MINOR (dev);

	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ulp_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (sc->sc_state)
		{ u.u_error = EBUSY; return (-1); }

	sc->sc_state = ULPT_INIT;

#if (0)	/*******************************************************/
	sc->sc_flags = flags;

	printf ("ulptopen: flags=0x%x\n", flags);

	/* Ignoring these flags might not be a good idea */
	if ((flags & ~ULPT_NOPRIME) != 0)
		printf("ulptopen: flags ignored: %b\n", flags,
			"\20\3POS_INIT\4POS_ACK\6PRIME_OPEN\7AUTOLF\10BYPASS");

	sc->sc_refcnt++;

	if ((flags & ULPT_NOPRIME) == 0)
		ulp_reset (sc);
#endif	/*******************************************************/

	ulp_reset (sc);

	for (count = 0; (ulp_status (sc) & LPS_SELECT) == 0; count += STEP)
	{
		printf ("ulpopen: waiting a while\n");

		if (count >= TIMEOUT)
			{ u.u_error = EBUSY; sc->sc_state = 0; goto bad; }

		DELAY (250000);

#if (0)	/*******************************************************/
		/* wait 1/4 second, give up if we get a signal */
		error = tsleep((caddr_t)sc, LPTPRI | PCATCH, "ulpop", STEP);

		if (error != EWOULDBLOCK)
		{
			sc->sc_state = 0;
			goto bad;
		}

		if (sc->sc_dying)
		{
			u.u_error = ENXIO;
			sc->sc_state = 0;
			goto bad;
		}
#endif	/*******************************************************/
	}

	if (usbd_open_pipe (sc->sc_iface, sc->sc_out, 0, &sc->sc_out_pipe))
	{
		sc->sc_state = 0;
		u.u_error = EIO;
		goto bad;
	}

	if (sc->sc_in != -1)
	{
		printf ("ulpopen: open input pipe\n");

		if (usbd_open_pipe (sc->sc_iface, sc->sc_in,0,&sc->sc_in_pipe))
		{
			u.u_error = EIO;
			usbd_close_pipe (sc->sc_out_pipe);
			sc->sc_out_pipe = NULL;
			sc->sc_state = 0;
			goto bad;
		}

		sc->sc_in_xfer1 = usbd_alloc_xfer (sc->sc_udev);
		sc->sc_in_xfer2 = usbd_alloc_xfer (sc->sc_udev);

		if (sc->sc_in_xfer1 == NULL || sc->sc_in_xfer2 == NULL)
		{
			u.u_error = ENOMEM;

			if (sc->sc_in_xfer1 != NULL)
				{ usbd_free_xfer (sc->sc_in_xfer1); sc->sc_in_xfer1 = NULL; }

			if (sc->sc_in_xfer2 != NULL)
				{ usbd_free_xfer (sc->sc_in_xfer2); sc->sc_in_xfer2 = NULL; }

			usbd_close_pipe (sc->sc_out_pipe); sc->sc_out_pipe = NULL;
			usbd_close_pipe (sc->sc_in_pipe);  sc->sc_in_pipe = NULL;

			sc->sc_state = 0;
			goto bad;
		}

		usbd_setup_xfer
		(	sc->sc_in_xfer1, sc->sc_in_pipe, sc,
			sc->sc_area, sizeof (sc->sc_area), USBD_SHORT_XFER_OK,
			USBD_NO_TIMEOUT, ulp_input
		);

		usbd_setup_xfer
		(	sc->sc_in_xfer2, sc->sc_in_pipe, sc,
			sc->sc_area, sizeof (sc->sc_area), USBD_SHORT_XFER_OK,
			USBD_NO_TIMEOUT, ulp_input
		);

		usbd_transfer (sc->sc_in_xfer1); /* ignore failed start */
	}

	sc->sc_state = ULPT_OPEN;

#if (0)	/*******************************************************/
	if (--sc->sc_refcnt < 0)
		usb_detach_wakeup (sc->sc_dev);
#endif	/*******************************************************/

	return (0);

    bad:
#if (0)	/*******************************************************/
	if (--sc->sc_refcnt < 0)
		usb_detach_wakeup (sc->sc_dev);
#endif	/*******************************************************/

	return (-1);

}	/* end ulpopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
int
ulpclose (dev_t dev, int flag)
{
	int			unit = MINOR (dev);
	struct ulp_softc	*sc;

	/*
	 *	Prólogo
	 */
	if (unit >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = ulp_devclass->devices[unit]->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (sc->sc_state != ULPT_OPEN)
		return (0);	/* We are being forced to close before the open completed. */

	if (sc->sc_out_pipe != NULL)
		{ usbd_close_pipe (sc->sc_out_pipe); sc->sc_out_pipe = NULL; }

	if (sc->sc_in_pipe != NULL)
	{
		usbd_abort_pipe (sc->sc_in_pipe);
		usbd_close_pipe (sc->sc_in_pipe);

		sc->sc_in_pipe = NULL;

		if (sc->sc_in_xfer1 != NULL)
			{ usbd_free_xfer(sc->sc_in_xfer1); sc->sc_in_xfer1 = NULL; }

		if (sc->sc_in_xfer2 != NULL)
			{ usbd_free_xfer(sc->sc_in_xfer2); sc->sc_in_xfer2 = NULL; }
	}

	sc->sc_state = 0;

	return (0);

}	/* end ulpclose */

/*
 ****************************************************************
 *	Função de "write"					*
 ****************************************************************
 */
void
ulpwrite (IOREQ *iop)
{
	struct ulp_softc	*sc;

	/*
	 *	Prólogo
	 */
	if ((sc = ulp_devclass->devices[MINOR (iop->io_dev)]->softc) == NULL)
		{ u.u_error = ENXIO; return; }

	if (sc->sc_dying)
		{ u.u_error = EIO; return; }

#if (0)	/*******************************************************/
	sc->sc_refcnt++;
#endif	/*******************************************************/

	ulp_do_write (sc, iop);

#if (0)	/*******************************************************/
	if (--sc->sc_refcnt < 0)
		usb_detach_wakeup (sc->sc_dev);
#endif	/*******************************************************/

}	/* end ulpwrite */

/*
 ****************************************************************
 *	Inicializa a impressora					*
 ****************************************************************
 *
 *	There was a mistake in the USB printer 1.0 spec that gave the
 *	request type as UT_WRITE_CLASS_OTHER; it should have been
 *	UT_WRITE_CLASS_INTERFACE.  Many printers use the old one so we try both.
 */
void
ulp_reset (struct ulp_softc *sc)
{
	struct usb_device_request	req;

	req.bRequest		= UR_SOFT_RESET;
	req.bmRequestType	= UT_WRITE_CLASS_OTHER;

	USETW (req.wValue, 0);
	USETW (req.wIndex, sc->sc_ifaceno);
	USETW (req.wLength, 0);

	if (usbd_do_request (sc->sc_udev, &req, 0))	/* 1.0 */
	{
		req.bmRequestType = UT_WRITE_CLASS_INTERFACE;

		usbd_do_request (sc->sc_udev, &req, 0); /* 1.1 */
	}

}	/* end ulp_reset */

/*
 ****************************************************************
 *	Obtém dados da impressora				*
 ****************************************************************
 */
void
ulp_input (struct usbd_xfer *xfer, void *priv, int status)
{
	struct ulp_softc	*sc = priv;
	ulong			count;

	/* Don't loop on errors or 0-length input. */

	usbd_get_xfer_status (xfer, NULL, NULL, &count, NULL);

	if (status != USBD_NORMAL_COMPLETION || count == 0)
		return;

	printf ("ulp_input: got some data\n");

	/* Do it again */

	if (xfer == sc->sc_in_xfer1)
		usbd_transfer (sc->sc_in_xfer2);
	else
		usbd_transfer (sc->sc_in_xfer1);

}	/* end ulp_input */

/*
 ****************************************************************
 *	Obtém o estado da impressora				*
 ****************************************************************
 */
int
ulp_status (struct ulp_softc *sc)
{
	struct usb_device_request	req;
	int				err;

	req.bmRequestType	= UT_READ_CLASS_INTERFACE;
	req.bRequest		= UR_GET_PORT_STATUS;

	USETW (req.wValue, 0);
	USETW (req.wIndex, sc->sc_ifaceno);
	USETW (req.wLength, 1);

	err = usbd_do_request (sc->sc_udev, &req, &sc->sc_area[0]);

	printf ("ulp_status: status=0x%02x err=%d\n", sc->sc_area[0], err);

	return (err == 0 ? sc->sc_area[0] : 0);

}	/* end ulp_status */

/*
 ****************************************************************
 *	Envia, efetivamente, dados para a impressora		*
 ****************************************************************
 */
void
ulp_do_write (struct ulp_softc *sc, IOREQ *iop)
{
	ulong			n;
	void			*bufp;
	struct usbd_xfer	*xfer;

	if ((xfer = usbd_alloc_xfer (sc->sc_udev)) == NULL)
		{ u.u_error = ENOMEM; return; }

	if ((bufp = usbd_alloc_buffer (xfer, ULPT_BSIZE)) == NULL)
		{ usbd_free_xfer (xfer); u.u_error = ENOMEM; return; }

	while ((n = MIN (ULPT_BSIZE, iop->io_count)) > 0)
	{
		ulp_print_status_msg (ulp_status (sc), sc);

		if (unimove (bufp, iop->io_area, n, US) < 0)
			{ u.u_error = EFAULT; break; }

		printf ("ulpwrite: enviando %d bytes\n", n);

		if (usbd_bulk_transfer (xfer, sc->sc_out_pipe, USBD_NO_COPY, USBD_NO_TIMEOUT, bufp, &n, "ulpwr"))
			{ u.u_error = EIO; break; }

		iop->io_count -= n;
		iop->io_area  += n;
	}

	usbd_free_xfer (xfer);

}	/* end ulp_do_write */

/*
 ****************************************************************
 *	Imprime o estado					*
 ****************************************************************
 */
void
ulp_print_status_msg (int status, struct ulp_softc *sc)
{
	int		new;
	const char	*msg = NOSTR;

	status			= (status ^ LPS_INVERT) & LPS_MASK;
	new			= status & ~sc->sc_laststatus;
	sc->sc_laststatus	= status;

	if   (new & LPS_SELECT)
		msg = "fora de linha";
	elif (new & LPS_NOPAPER)
		msg = "sem papel";
	elif (new & LPS_NERR)
		msg = "erro de escrita";

	if (msg != NOSTR)
		printf ("%s: %s\n", sc->sc_dev->nameunit, msg);

}	/* end ulp_print_status_msg */

#if (0)	/*******************************************************/
#define	TIMEOUT		hz*16	/* wait up to 16 seconds for a ready */
#define	STEP		hz/4

#define	LPTPRI		(PZERO+8)
#define	ULPT_BSIZE	16384

#define UR_GET_DEVICE_ID 0
#define UR_GET_PORT_STATUS 1
#define UR_SOFT_RESET 2

#define	LPS_NERR		0x08	/* printer no error */
#define	LPS_SELECT		0x10	/* printer selected */
#define	LPS_NOPAPER		0x20	/* printer out of paper */
#define LPS_INVERT      (LPS_SELECT|LPS_NERR)
#define LPS_MASK        (LPS_SELECT|LPS_NERR|LPS_NOPAPER)

struct ulpt_softc {
	USBBASEDEVICE sc_dev;
	usbd_device_handle sc_udev;	/* device */
	usbd_interface_handle sc_iface;	/* interface */
	int sc_ifaceno;

	int sc_out;
	usbd_pipe_handle sc_out_pipe;	/* bulk out pipe */

	int sc_in;
	usbd_pipe_handle sc_in_pipe;	/* bulk in pipe */
	usbd_xfer_handle sc_in_xfer1;
	usbd_xfer_handle sc_in_xfer2;
	u_char sc_junk[64];	/* somewhere to dump input */

	u_char sc_state;
#define	ULPT_OPEN	0x01	/* device is open */
#define	ULPT_OBUSY	0x02	/* printer is busy doing output */
#define	ULPT_INIT	0x04	/* waiting to initialize for open */
	u_char sc_flags;
#define	ULPT_NOPRIME	0x40	/* don't prime on open */
	u_char sc_laststatus;

	int sc_refcnt;
	u_char sc_dying;

#if defined(__FreeBSD__)
	dev_t dev;
	dev_t dev_noprime;
#endif
};

void ulpt_disco(void *);

int ulpt_do_write(struct ulpt_softc *, struct uio *uio, int);
int ulpt_status(struct ulpt_softc *);
void ulpt_reset(struct ulpt_softc *);
int ulpt_statusmsg(u_char, struct ulpt_softc *);

#if 0
void ieee1284_print_id(char *);
#endif

#define	ULPTUNIT(s)	(minor(s) & 0x1f)
#define	ULPTFLAGS(s)	(minor(s) & 0xe0)


USB_DECLARE_DRIVER(ulpt);

USB_PROBE(ulpt)
{
	USB_PROBE_START(ulpt, uaa);
	usb_interface_descriptor_t *id;

	DPRINTFN(10,("ulpt_probe\n"));
	if (uaa->iface == NULL)
		return (UPROBE_NONE);
	id = usbd_get_interface_descriptor(uaa->iface);
	if (id != NULL &&
	    id->bInterfaceClass == UICLASS_PRINTER &&
	    id->bInterfaceSubClass == UISUBCLASS_PRINTER &&
	    (id->bInterfaceProtocol == UIPROTO_PRINTER_UNI ||
	     id->bInterfaceProtocol == UIPROTO_PRINTER_BI ||
	     id->bInterfaceProtocol == UIPROTO_PRINTER_1284))
		return (UPROBE_IFACECLASS_IFACESUBCLASS_IFACEPROTO);
	return (UPROBE_NONE);
}

USB_ATTACH(ulpt)
{
	USB_ATTACH_START(ulpt, sc, uaa);
	usbd_device_handle dev = uaa->device;
	usbd_interface_handle iface = uaa->iface;
	usb_interface_descriptor_t *ifcd = usbd_get_interface_descriptor(iface);
	usb_interface_descriptor_t *id, *iend;
	usb_config_descriptor_t *cdesc;
	usbd_status err;
	char devinfo[1024];
	usb_endpoint_descriptor_t *ed;
	u_int8_t epcount;
	int i, altno;

	DPRINTFN(10,("ulpt_attach: sc=%p\n", sc));
	usbd_devinfo(dev, 0, devinfo);
	USB_ATTACH_SETUP;
	printf("%s: %s, iclass %d/%d\n", USBDEVNAME(sc->sc_dev),
	       devinfo, ifcd->bInterfaceClass, ifcd->bInterfaceSubClass);

	/* XXX
	 * Stepping through the alternate settings needs to be abstracted out.
	 */
	cdesc = usbd_get_config_descriptor(dev);
	if (cdesc == NULL) {
		printf("%s: failed to get configuration descriptor\n",
		       USBDEVNAME(sc->sc_dev));
		USB_ATTACH_ERROR_RETURN;
	}
	iend = (usb_interface_descriptor_t *)
		   ((char *)cdesc + UGETW(cdesc->wTotalLength));
#ifdef DIAGNOSTIC
	if (ifcd < (usb_interface_descriptor_t *)cdesc ||
	    ifcd >= iend)
		panic("ulpt: iface desc out of range\n");
#endif
	/* Step through all the descriptors looking for bidir mode */
	for (id = ifcd, altno = 0;
	     id < iend;
	     id = (void *)((char *)id + id->bLength)) {
		if (id->bDescriptorType == UDESC_INTERFACE &&
		    id->bInterfaceNumber == ifcd->bInterfaceNumber) {
			if (id->bInterfaceClass == UICLASS_PRINTER &&
			    id->bInterfaceSubClass == UISUBCLASS_PRINTER &&
			    (id->bInterfaceProtocol == UIPROTO_PRINTER_BI /* ||
			     id->bInterfaceProtocol == UIPROTO_PRINTER_1284 */))
				goto found;
			altno++;
		}
	}
	id = ifcd;		/* not found, use original */
 found:
	if (id != ifcd) {
		/* Found a new bidir setting */
		DPRINTF(("ulpt_attach: set altno = %d\n", altno));
		err = usbd_set_interface(iface, altno);
		if (err) {
			printf("%s: setting alternate interface failed\n",
			       USBDEVNAME(sc->sc_dev));
			sc->sc_dying = 1;
			USB_ATTACH_ERROR_RETURN;
		}
	}

	epcount = 0;
	(void)usbd_endpoint_count(iface, &epcount);

	sc->sc_in = -1;
	sc->sc_out = -1;
	for (i = 0; i < epcount; i++) {
		ed = usbd_interface2endpoint_descriptor(iface, i);
		if (ed == NULL) {
			printf("%s: couldn't get ep %d\n",
			    USBDEVNAME(sc->sc_dev), i);
			USB_ATTACH_ERROR_RETURN;
		}
		if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN &&
		    UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK) {
			sc->sc_in = ed->bEndpointAddress;
		} else if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_OUT &&
			   UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK) {
			sc->sc_out = ed->bEndpointAddress;
		}
	}
	if (sc->sc_out == -1) {
		printf("%s: could not find bulk out endpoint\n",
		    USBDEVNAME(sc->sc_dev));
		sc->sc_dying = 1;
		USB_ATTACH_ERROR_RETURN;
	}

	if (usbd_get_quirks(dev)->uq_flags & UQ_BROKEN_BIDIR) {
		/* This device doesn't handle reading properly. */
		sc->sc_in = -1;
	}

	printf("%s: using %s-directional mode\n", USBDEVNAME(sc->sc_dev),
	       sc->sc_in >= 0 ? "bi" : "uni");

	DPRINTFN(10, ("ulpt_attach: bulk=%d\n", sc->sc_out));

	sc->sc_iface = iface;
	sc->sc_ifaceno = id->bInterfaceNumber;
	sc->sc_udev = dev;

#if 0
/*
 * This code is disabled because for some mysterious reason it causes
 * printing not to work.  But only sometimes, and mostly with
 * UHCI and less often with OHCI.  *sigh*
 */
	{
	usb_config_descriptor_t *cd = usbd_get_config_descriptor(dev);
	usb_device_request_t req;
	int len, alen;

	req.bmRequestType = UT_READ_CLASS_INTERFACE;
	req.bRequest = UR_GET_DEVICE_ID;
	USETW(req.wValue, cd->bConfigurationValue);
	USETW2(req.wIndex, id->bInterfaceNumber, id->bAlternateSetting);
	USETW(req.wLength, sizeof devinfo - 1);
	err = usbd_do_request_flags(dev, &req, devinfo, USBD_SHORT_XFER_OK,
		  &alen, USBD_DEFAULT_TIMEOUT);
	if (err) {
		printf("%s: cannot get device id\n", USBDEVNAME(sc->sc_dev));
	} else if (alen <= 2) {
		printf("%s: empty device id, no printer connected?\n",
		       USBDEVNAME(sc->sc_dev));
	} else {
		/* devinfo now contains an IEEE-1284 device ID */
		len = ((devinfo[0] & 0xff) << 8) | (devinfo[1] & 0xff);
		if (len > sizeof devinfo - 3)
			len = sizeof devinfo - 3;
		devinfo[len] = 0;
		printf("%s: device id <", USBDEVNAME(sc->sc_dev));
		ieee1284_print_id(devinfo+2);
		printf(">\n");
	}
	}
#endif

#if defined(__FreeBSD__)
	sc->dev = make_dev(&ulpt_cdevsw, device_get_unit(self),
		UID_ROOT, GID_OPERATOR, 0644, "ulpt%d", device_get_unit(self));
	sc->dev_noprime = make_dev(&ulpt_cdevsw,
		device_get_unit(self)|ULPT_NOPRIME,
		UID_ROOT, GID_OPERATOR, 0644, "unlpt%d", device_get_unit(self));
#endif

	USB_ATTACH_SUCCESS_RETURN;
}

#if defined(__NetBSD__) || defined(__OpenBSD__)
int
ulpt_activate(device_ptr_t self, enum devact act)
{
	struct ulpt_softc *sc = (struct ulpt_softc *)self;

	switch (act) {
	case DVACT_ACTIVATE:
		return (EOPNOTSUPP);
		break;

	case DVACT_DEACTIVATE:
		sc->sc_dying = 1;
		break;
	}
	return (0);
}
#endif

USB_DETACH(ulpt)
{
	USB_DETACH_START(ulpt, sc);
	int s;
#if defined(__NetBSD__) || defined(__OpenBSD__)
	int maj, mn;
#elif defined(__FreeBSD__)
	struct vnode *vp;
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
	DPRINTF(("ulpt_detach: sc=%p flags=%d\n", sc, flags));
#elif defined(__FreeBSD__)
	DPRINTF(("ulpt_detach: sc=%p\n", sc));
#endif

	sc->sc_dying = 1;
	if (sc->sc_out_pipe != NULL)
		usbd_abort_pipe(sc->sc_out_pipe);
	if (sc->sc_in_pipe != NULL)
		usbd_abort_pipe(sc->sc_in_pipe);

	s = splusb();
	if (--sc->sc_refcnt >= 0) {
		/* There is noone to wake, aborting the pipe is enough */
		/* Wait for processes to go away. */
		usb_detach_wait(USBDEV(sc->sc_dev));
	}
	splx(s);

#if defined(__NetBSD__) || defined(__OpenBSD__)
	/* locate the major number */
	for (maj = 0; maj < nchrdev; maj++)
		if (cdevsw[maj].d_open == ulptopen)
			break;

	/* Nuke the vnodes for any open instances (calls close). */
	mn = self->dv_unit;
	vdevgone(maj, mn, mn, VCHR);
#elif defined(__FreeBSD__)
	vp = SLIST_FIRST(&sc->dev->si_hlist);
	if (vp)
		VOP_REVOKE(vp, REVOKEALL);
	vp = SLIST_FIRST(&sc->dev_noprime->si_hlist);
	if (vp)
		VOP_REVOKE(vp, REVOKEALL);

	destroy_dev(sc->dev);
	destroy_dev(sc->dev_noprime);
#endif

	return (0);
}

int
ulpt_status(struct ulpt_softc *sc)
{
	usb_device_request_t req;
	usbd_status err;
	u_char status;

	req.bmRequestType = UT_READ_CLASS_INTERFACE;
	req.bRequest = UR_GET_PORT_STATUS;
	USETW(req.wValue, 0);
	USETW(req.wIndex, sc->sc_ifaceno);
	USETW(req.wLength, 1);
	err = usbd_do_request(sc->sc_udev, &req, &status);
	DPRINTFN(1, ("ulpt_status: status=0x%02x err=%d\n", status, err));
	if (!err)
		return (status);
	else
		return (0);
}

void
ulpt_reset(struct ulpt_softc *sc)
{
	usb_device_request_t req;

	DPRINTFN(1, ("ulpt_reset\n"));
	req.bRequest = UR_SOFT_RESET;
	USETW(req.wValue, 0);
	USETW(req.wIndex, sc->sc_ifaceno);
	USETW(req.wLength, 0);

	/*
	 * There was a mistake in the USB printer 1.0 spec that gave the
	 * request type as UT_WRITE_CLASS_OTHER; it should have been
	 * UT_WRITE_CLASS_INTERFACE.  Many printers use the old one,
	 * so we try both.
	 */
	req.bmRequestType = UT_WRITE_CLASS_OTHER;
	if (usbd_do_request(sc->sc_udev, &req, 0)) {	/* 1.0 */
		req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
		(void)usbd_do_request(sc->sc_udev, &req, 0); /* 1.1 */
	}
}

static void
ulpt_input(usbd_xfer_handle xfer, usbd_private_handle priv, usbd_status status)
{
	struct ulpt_softc *sc = priv;
	u_int32_t count;

	/* Don't loop on errors or 0-length input. */
	usbd_get_xfer_status(xfer, NULL, NULL, &count, NULL);
	if (status != USBD_NORMAL_COMPLETION || count == 0)
		return;

	DPRINTFN(2,("ulpt_input: got some data\n"));
	/* Do it again. */
	if (xfer == sc->sc_in_xfer1)
		usbd_transfer(sc->sc_in_xfer2);
	else
		usbd_transfer(sc->sc_in_xfer1);
}

int ulptusein = 1;

/*
 * Reset the printer, then wait until it's selected and not busy.
 */
int
ulptopen(dev_t dev, int flag, int mode, usb_proc_ptr p)
{
	u_char flags = ULPTFLAGS(dev);
	struct ulpt_softc *sc;
	usbd_status err;
	int spin, error;

	USB_GET_SC_OPEN(ulpt, ULPTUNIT(dev), sc);

	if (sc == NULL || sc->sc_iface == NULL || sc->sc_dying)
		return (ENXIO);

	if (sc->sc_state)
		return (EBUSY);

	sc->sc_state = ULPT_INIT;
	sc->sc_flags = flags;
	DPRINTF(("ulptopen: flags=0x%x\n", (unsigned)flags));

#if defined(USB_DEBUG) && defined(__FreeBSD__)
	/* Ignoring these flags might not be a good idea */
	if ((flags & ~ULPT_NOPRIME) != 0)
		printf("ulptopen: flags ignored: %b\n", flags,
			"\20\3POS_INIT\4POS_ACK\6PRIME_OPEN\7AUTOLF\10BYPASS");
#endif


	error = 0;
	sc->sc_refcnt++;

	if ((flags & ULPT_NOPRIME) == 0)
		ulpt_reset(sc);

	for (spin = 0; (ulpt_status(sc) & LPS_SELECT) == 0; spin += STEP) {
		DPRINTF(("ulpt_open: waiting a while\n"));
		if (spin >= TIMEOUT) {
			error = EBUSY;
			sc->sc_state = 0;
			goto done;
		}

		/* wait 1/4 second, give up if we get a signal */
		error = tsleep((caddr_t)sc, LPTPRI | PCATCH, "ulptop", STEP);
		if (error != EWOULDBLOCK) {
			sc->sc_state = 0;
			goto done;
		}

		if (sc->sc_dying) {
			error = ENXIO;
			sc->sc_state = 0;
			goto done;
		}
	}

	err = usbd_open_pipe(sc->sc_iface, sc->sc_out, 0, &sc->sc_out_pipe);
	if (err) {
		sc->sc_state = 0;
		error = EIO;
		goto done;
	}
	if (ulptusein && sc->sc_in != -1) {
		DPRINTF(("ulpt_open: open input pipe\n"));
		err = usbd_open_pipe(sc->sc_iface, sc->sc_in,0,&sc->sc_in_pipe);
		if (err) {
			error = EIO;
			usbd_close_pipe(sc->sc_out_pipe);
			sc->sc_out_pipe = NULL;
			sc->sc_state = 0;
			goto done;
		}
		sc->sc_in_xfer1 = usbd_alloc_xfer(sc->sc_udev);
		sc->sc_in_xfer2 = usbd_alloc_xfer(sc->sc_udev);
		if (sc->sc_in_xfer1 == NULL || sc->sc_in_xfer2 == NULL) {
			error = ENOMEM;
			if (sc->sc_in_xfer1 != NULL) {
				usbd_free_xfer(sc->sc_in_xfer1);
				sc->sc_in_xfer1 = NULL;
			}
			if (sc->sc_in_xfer2 != NULL) {
				usbd_free_xfer(sc->sc_in_xfer2);
				sc->sc_in_xfer2 = NULL;
			}
			usbd_close_pipe(sc->sc_out_pipe);
			sc->sc_out_pipe = NULL;
			usbd_close_pipe(sc->sc_in_pipe);
			sc->sc_in_pipe = NULL;
			sc->sc_state = 0;
			goto done;
		}
		usbd_setup_xfer(sc->sc_in_xfer1, sc->sc_in_pipe, sc,
		    sc->sc_junk, sizeof sc->sc_junk, USBD_SHORT_XFER_OK,
		    USBD_NO_TIMEOUT, ulpt_input);
		usbd_setup_xfer(sc->sc_in_xfer2, sc->sc_in_pipe, sc,
		    sc->sc_junk, sizeof sc->sc_junk, USBD_SHORT_XFER_OK,
		    USBD_NO_TIMEOUT, ulpt_input);
		usbd_transfer(sc->sc_in_xfer1); /* ignore failed start */
	}

	sc->sc_state = ULPT_OPEN;

 done:
	if (--sc->sc_refcnt < 0)
		usb_detach_wakeup(USBDEV(sc->sc_dev));

	DPRINTF(("ulptopen: done, error=%d\n", error));
	return (error);
}

int
ulpt_statusmsg(u_char status, struct ulpt_softc *sc)
{
	u_char new;

	status = (status ^ LPS_INVERT) & LPS_MASK;
	new = status & ~sc->sc_laststatus;
	sc->sc_laststatus = status;

	if (new & LPS_SELECT)
		log(LOG_NOTICE, "%s: offline\n", USBDEVNAME(sc->sc_dev));
	else if (new & LPS_NOPAPER)
		log(LOG_NOTICE, "%s: out of paper\n", USBDEVNAME(sc->sc_dev));
	else if (new & LPS_NERR)
		log(LOG_NOTICE, "%s: output error\n", USBDEVNAME(sc->sc_dev));

	return (status);
}

int
ulptclose(dev_t dev, int flag, int mode, usb_proc_ptr p)
{
	struct ulpt_softc *sc;

	USB_GET_SC(ulpt, ULPTUNIT(dev), sc);

	if (sc->sc_state != ULPT_OPEN)
		/* We are being forced to close before the open completed. */
		return (0);

	if (sc->sc_out_pipe != NULL) {
		usbd_close_pipe(sc->sc_out_pipe);
		sc->sc_out_pipe = NULL;
	}
	if (sc->sc_in_pipe != NULL) {
		usbd_abort_pipe(sc->sc_in_pipe);
		usbd_close_pipe(sc->sc_in_pipe);
		sc->sc_in_pipe = NULL;
		if (sc->sc_in_xfer1 != NULL) {
			usbd_free_xfer(sc->sc_in_xfer1);
			sc->sc_in_xfer1 = NULL;
		}
		if (sc->sc_in_xfer2 != NULL) {
			usbd_free_xfer(sc->sc_in_xfer2);
			sc->sc_in_xfer2 = NULL;
		}
	}

	sc->sc_state = 0;

	DPRINTF(("ulptclose: closed\n"));
	return (0);
}

int
ulpt_do_write(struct ulpt_softc *sc, struct uio *uio, int flags)
{
	u_int32_t n;
	int error = 0;
	void *bufp;
	usbd_xfer_handle xfer;
	usbd_status err;

	DPRINTF(("ulptwrite\n"));
	xfer = usbd_alloc_xfer(sc->sc_udev);
	if (xfer == NULL)
		return (ENOMEM);
	bufp = usbd_alloc_buffer(xfer, ULPT_BSIZE);
	if (bufp == NULL) {
		usbd_free_xfer(xfer);
		return (ENOMEM);
	}
	while ((n = min(ULPT_BSIZE, uio->uio_resid)) != 0) {
		ulpt_statusmsg(ulpt_status(sc), sc);
		error = uiomove(bufp, n, uio);
		if (error)
			break;
		DPRINTFN(1, ("ulptwrite: transfer %d bytes\n", n));
		err = usbd_bulk_transfer(xfer, sc->sc_out_pipe, USBD_NO_COPY,
			  USBD_NO_TIMEOUT, bufp, &n, "ulptwr");
		if (err) {
			DPRINTF(("ulptwrite: error=%d\n", err));
			error = EIO;
			break;
		}
	}
	usbd_free_xfer(xfer);

	return (error);
}

int
ulptwrite(dev_t dev, struct uio *uio, int flags)
{
	struct ulpt_softc *sc;
	int error;

	USB_GET_SC(ulpt, ULPTUNIT(dev), sc);

	if (sc->sc_dying)
		return (EIO);

	sc->sc_refcnt++;
	error = ulpt_do_write(sc, uio, flags);
	if (--sc->sc_refcnt < 0)
		usb_detach_wakeup(USBDEV(sc->sc_dev));
	return (error);
}

int
ulptioctl(dev_t dev, u_long cmd, caddr_t data, int flag, usb_proc_ptr p)
{
	int error = 0;

	switch (cmd) {
	default:
		error = ENODEV;
	}

	return (error);
}

#endif	/*******************************************************/
