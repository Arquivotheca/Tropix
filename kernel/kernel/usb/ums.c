/*
 ****************************************************************
 *								*
 *			ums.c					*
 *								*
 *	"Driver" para o camundongo USB				*
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
int	ums_probe (struct device *);
int	ums_attach (struct device *);
int	ums_detach (struct device *);

const struct driver ums_driver =
{
	"ums",

	ums_probe,
	ums_attach,
   	ums_detach
};

/*
 ****** Informações sobre uma unidade ***************************
 */
#define UMS_Z			0x01			/* Z direction available */
#define UMS_SPUR_BUT_UP		0x02			/* spurious button up events */

#define	UMS_ASLEEP		0x01			/* readFromDevice is waiting */
#define	UMS_SELECT		0x02			/* select is waiting */
#define MAX_BUTTONS		7			/* chosen because sc_buttons is uchar */

#define MOUSE_FLAGS_MASK	(HIO_CONST|HIO_RELATIVE)
#define MOUSE_FLAGS		(HIO_RELATIVE)

struct ums_softc
{
	struct device		*sc_dev;		/* base device */
	struct usbd_interface	*sc_iface;		/* interface */
	struct usbd_pipe	*sc_intrpipe;		/* interrupt pipe */
	int			sc_ep_addr;

	uchar			*sc_ibuf;
	uchar			sc_iid;
	int			sc_isize;

	struct hid_location	sc_loc_x,
				sc_loc_y,
				sc_loc_z;

	struct hid_location	*sc_loc_btn;

	char			sc_enabled;
	char			sc_disconnected;	/* device is gone */
	char			flags;			/* device configuration */
	char			nbuttons;

	mousehw_t		hw;
	mousemode_t		mode;
	int			buttons_state;

	TTY			sc_tty;
};

struct devclass	*ums_devclass;

/*
 ******	Protótipos de funções ***********************************
 */
void		ums_intr (struct usbd_xfer * xfer, void * priv, int status);
int		ums_enable (struct ums_softc *);
void		ums_disable (struct ums_softc *);
void		umsstart (TTY *tp, int flag);

/*
 ****************************************************************
 *	Função de "probe"					*
 ****************************************************************
 */
int
ums_probe (struct device *dev)
{
	struct usb_attach_arg		*uaa = dev->ivars;
	struct usb_interface_descriptor *id;
	int				size, ret, err;
	void				*desc;

#ifdef	USB_MSG
	printf ("ums_probe\n");
#endif	USB_MSG

	if (uaa == NULL || uaa->iface == NULL || (id = uaa->iface->idesc) == NULL)
		return (UPROBE_NONE);

	if (id->bInterfaceClass != UICLASS_HID)
		return (UPROBE_NONE);

	if (err = usbd_read_report_desc (uaa->iface, &desc, &size))
		return (UPROBE_NONE);

	if (hid_is_collection (desc, size, HID_USAGE2 (HUP_GENERIC_DESKTOP, HUG_MOUSE)))
		ret = UPROBE_IFACECLASS;
	else
		ret = UPROBE_NONE;

	free_byte (desc);

	return (ret);

}	/* end ums_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
ums_attach (struct device *dev)
{
        struct ums_softc		*sc;
        struct usb_attach_arg		*uaa   = dev->ivars;
	struct usbd_interface		*iface = uaa->iface;
	struct usb_interface_descriptor	*id    = iface->idesc;
	struct usb_endpoint_descriptor	*ed;
	int				size;
	void				*desc;
	char				*devinfo;
	ulong				flags;
	int				i;
	struct hid_location		loc_btn;

	/*
	 *	Aloca a estrutura "softc"
	 */
	if ((sc = dev->softc = malloc_byte (sizeof (struct ums_softc))) == NULL)
		return (-1);

	memclr (sc, sizeof (struct ums_softc));

	sc->sc_disconnected	= 1;
	sc->sc_iface		= iface;
	sc->sc_dev		= dev;

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
	printf ("\e[0m");

	free_byte (devinfo);

	if ((ed = usbd_interface2endpoint_descriptor (iface, 0)) == NULL)
	{
		printf ("%s: could not read endpoint descriptor\n", dev->nameunit);
		goto bad;
	}

#ifdef	USB_MSG
	printf
	(	"ums_attach: bLength=%d bDescriptorType=%d "
		"bEndpointAddress=%d-%s bmAttributes=%d wMaxPacketSize=%d bInterval=%d\n",
		ed->bLength, ed->bDescriptorType, 
		UE_GET_ADDR (ed->bEndpointAddress),
		UE_GET_DIR (ed->bEndpointAddress) == UE_DIR_IN ? "in":"out",
		UE_GET_XFERTYPE (ed->bmAttributes),
		UGETW (ed->wMaxPacketSize), ed->bInterval
	);
#endif	USB_MSG

	if (UE_GET_DIR (ed->bEndpointAddress) != UE_DIR_IN || UE_GET_XFERTYPE (ed->bmAttributes) != UE_INTERRUPT)
	{
		printf ("%s: unexpected endpoint\n", dev->nameunit);
		goto bad;
	}

	if (usbd_read_report_desc (uaa->iface, &desc, &size))
		goto bad;

	if (!hid_locate (desc, size, HID_USAGE2 (HUP_GENERIC_DESKTOP, HUG_X), hid_input, &sc->sc_loc_x, &flags))
	{
		printf ("%s: mouse has no X report\n", dev->nameunit);
		goto bad;
	}

	if ((flags & MOUSE_FLAGS_MASK) != MOUSE_FLAGS)
	{
		printf ("%s: X report 0x%04x not supported\n", dev->nameunit, flags);
		goto bad;
	}

	if (!hid_locate (desc, size, HID_USAGE2 (HUP_GENERIC_DESKTOP, HUG_Y), hid_input, &sc->sc_loc_y, &flags))
	{
		printf ("%s: mouse has no Y report\n", dev->nameunit);
		goto bad;
	}

	if ((flags & MOUSE_FLAGS_MASK) != MOUSE_FLAGS)
	{
		printf ("%s: Y report 0x%04x not supported\n", dev->nameunit, flags);
		goto bad;
	}

	/* try to guess the Z activator: first check Z, then WHEEL */

	if
	(	hid_locate (desc, size, HID_USAGE2 (HUP_GENERIC_DESKTOP, HUG_Z),     hid_input, &sc->sc_loc_z, &flags) ||
		hid_locate (desc, size, HID_USAGE2 (HUP_GENERIC_DESKTOP, HUG_WHEEL), hid_input, &sc->sc_loc_z, &flags)
	)
	{
		if ((flags & MOUSE_FLAGS_MASK) != MOUSE_FLAGS)
			sc->sc_loc_z.size = 0;	/* Bad Z coord, ignore it */
		else
			sc->flags |= UMS_Z;
	}

	/* figure out the number of buttons */

	for (i = 1; i <= MAX_BUTTONS; i++)
	{
		if (!hid_locate (desc, size, HID_USAGE2 (HUP_BUTTON, i), hid_input, &loc_btn, 0))
			break;
	}

	sc->nbuttons = i - 1;

	if ((sc->sc_loc_btn = malloc_byte (sizeof (struct hid_location) * sc->nbuttons)) == NULL)
	{
		printf ("%s: memória esgotada\n", dev->nameunit);
		goto bad;
	}

	printf ("\e[31m");
	printf
	(	"%s: %d botões%s\n",
		dev->nameunit, sc->nbuttons,
		(sc->flags & UMS_Z) ? " e direção Z" : ""
	);
	printf ("\e[0m");

	for (i = 1; i <= sc->nbuttons; i++)
	{
		hid_locate (desc, size, HID_USAGE2(HUP_BUTTON, i), hid_input, &sc->sc_loc_btn[i-1], 0);
	}

	sc->sc_isize = hid_report_size (desc, size, hid_input, &sc->sc_iid);

	if ((sc->sc_ibuf = malloc_byte (sc->sc_isize)) == NULL)
	{
		printf ("%s: memória esgotada\n", dev->nameunit);
		free_byte (sc->sc_loc_btn);
		goto bad;
	}

	sc->sc_ep_addr		= ed->bEndpointAddress;
	sc->sc_disconnected	= 0;

	free_byte (desc);

	if (sc->nbuttons > MOUSE_SYS_MAXBUTTON)
		sc->hw.buttons	= MOUSE_SYS_MAXBUTTON;
	else
		sc->hw.buttons	= sc->nbuttons;

	sc->hw.iftype		= MOUSE_IF_USB;
	sc->hw.type		= MOUSE_MOUSE;
	sc->hw.model		= MOUSE_MODEL_GENERIC;
	sc->hw.hwid		= 0;

	sc->mode.protocol	= MOUSE_PROTO_SYSMOUSE;
	sc->mode.rate		= -1;
	sc->mode.resolution	= MOUSE_RES_UNKNOWN;
	sc->mode.accelfactor	= 0;
	sc->mode.level		= 1;
	sc->mode.packetsize	= MOUSE_SYS_PACKETSIZE;
	sc->mode.syncmask[0]	= MOUSE_SYS_SYNCMASK;
	sc->mode.syncmask[1]	= MOUSE_SYS_SYNC;
	sc->buttons_state	= 0;

#if (0)	/*******************************************************/
	if (uaa->device->quirks->uq_flags & UQ_SPUR_BUT_UP)
	{
		printf ("%s: Spurious button up events\n", dev->nameunit);
		sc->flags |= UMS_SPUR_BUT_UP;
	}
#endif	/*******************************************************/

	ums_devclass = dev->devclass;

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	free_byte (sc);	dev->softc = NULL;

	return (-1);

}	/* end ums_attach */

/*
 ****************************************************************
 *	Função de "detach"					*
 ****************************************************************
 */
int
ums_detach (struct device *dev)
{
	struct ums_softc	*sc;
	TTY			*tp;

	if ((sc = dev->softc) == NULL)
		return (-1);

	if (sc->sc_enabled)
		ums_disable (sc);

	free_byte (sc->sc_loc_btn);
	free_byte (sc->sc_ibuf);

	/*
	 *	Se houver algum processo esperando dados, sinaliza-o
	 */
	tp = &sc->sc_tty;

	if (tp->t_attention_set && tp->t_uproc->u_attention_index < 0)
	{
		tp->t_attention_set = 0;
		tp->t_uproc->u_attention_index = tp->t_index; 

		EVENTDONE (&tp->t_uproc->u_attention_event);
	}

	return (0);

}	/* end ums_detach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
umsopen (dev_t dev, int oflag)
{
	struct ums_softc	*sc;
	struct device		*device;
	TTY			*tp;
	int			unit;

	/*
	 *	Prólogo
	 */
	if (ums_devclass == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((unit = MINOR (dev)) >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ums_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if (ums_enable (sc) < 0)
		return (-1);

	/*
	 *	Prepara a estrutura TTY
	 */
	tp = &sc->sc_tty;

	if ((tp->t_state & ISOPEN) == 0)
	{
		tp->t_state |= ONEINIT;

		tp->t_oproc  = umsstart;

		tp->t_iflag  = 0;
		tp->t_oflag  = 0;
		tp->t_cflag  = 0;
		tp->t_lflag  = ICOMM;

		tp->t_min    = sc->mode.packetsize;
	}

	ttyopen (dev, tp, oflag);

	return (0);

}	/* end umsopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
int
umsclose (dev_t dev, int flag)
{
	int			unit;
	struct device		*device;
	struct ums_softc	*sc;
	TTY			*tp;

	/*
	 *	Prólogo
	 */
	if ((unit = MINOR (dev)) >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ums_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	tp = &sc->sc_tty;

	if (flag == 0)
		ttyclose (tp);

	if (sc->sc_enabled)
		ums_disable (sc);

	return (0);

}	/* end umsclose */

/*
 ****************************************************************
 *	Função de "read"					*
 ****************************************************************
 */
void
umsread (IOREQ *iop)
{
	int			unit;
	struct device		*device;
	struct ums_softc	*sc;

	if ((unit = MINOR (iop->io_dev)) >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return; }

	if ((device = ums_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return; }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return; }

	ttyread (iop, &sc->sc_tty);

}	/* end umsread */

/*
 ****************************************************************
 *	Função de "ioctl"					*
 ****************************************************************
 */
int
umsctl (dev_t dev, int cmd, void *arg, int flag)
{
	int			unit;
	struct device		*device;
	struct ums_softc	*sc;
	struct usbd_bus		*bus;
	TTY			*tp;

	if ((unit = MINOR (dev)) >= MAX_DEV_in_CLASS)
		{ u.u_error = ENXIO; return (-1); }

	if ((device = ums_devclass->devices[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	if ((sc = device->softc) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	bus = sc->sc_intrpipe->device->bus;

	tp = &sc->sc_tty;

	/*
	 *	Tratamento especial para certos comandos
	 */
	switch (cmd)
	{
		/*
		 *	Processa/prepara o ATTENTION
		 */
	    case U_ATTENTION:
		splx (irq_pl_vec[bus->irq]);

		if (EVENTTEST (&tp->t_inqnempty) == 0)
			{ spl0 (); return (1); }

		tp->t_uproc  = u.u_myself;
		tp->t_index = (int)arg;

		tp->t_attention_set = 1;
		*(char **)flag	= &tp->t_attention_set;

		spl0 ();

		return (0);

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
	return (ttyctl (tp, cmd, arg));

}	/* end umsctl */

/*
 ****************************************************************
 *	Função de Interrupção					*
 ****************************************************************
 */
void
ums_intr (struct usbd_xfer *xfer, void *addr, int status)
{
	struct ums_softc	*sc = addr;
	uchar			*ibuf;
	int			dx, dy, dz;
	uchar			buttons = 0;
	int			i;

#define UMS_BUT(i) ((i) < 3 ? (((i) + 2) % 3) : (i))

#ifdef	USB_MSG
	printf ("ums_intr: sc = %P status = %d\n", sc, status);
	printf
	(	"ums_intr: data = %02X %02X %02X %02X %02X\n",
		sc->sc_ibuf[0], sc->sc_ibuf[1], sc->sc_ibuf[2], sc->sc_ibuf[3], sc->sc_ibuf[4]
	);
#endif	USB_MSG

	if (status == USBD_CANCELLED)
		return;

	if (status != USBD_NORMAL_COMPLETION)
	{
#if (0)	/*******************************************************/
		printf ("ums_intr: status = %d\n", status);
#endif	/*******************************************************/

		if (status == USBD_STALLED && sc->sc_intrpipe != NULL)
			usbd_clear_endpoint_stall_async (sc->sc_intrpipe);

		return;
	}

	ibuf = sc->sc_ibuf;

	if (sc->sc_iid)
	{
		if (*ibuf++ != sc->sc_iid)
			return;
	}

	dx =  hid_get_data (ibuf, &sc->sc_loc_x);
	dy = -hid_get_data (ibuf, &sc->sc_loc_y);
	dz = -hid_get_data (ibuf, &sc->sc_loc_z);

	for (i = 0; i < sc->nbuttons; i++)
	{
		if (hid_get_data (ibuf, &sc->sc_loc_btn[i]))
			buttons |= (1 << UMS_BUT (i));
	}

	if (dx || dy || dz || (sc->flags & UMS_Z) || buttons != sc->buttons_state)
	{
		TTY	*tp = &sc->sc_tty;

		sc->buttons_state  = buttons;

		if (dx >  254)	dx =  254;
		if (dx < -256)	dx = -256;
		if (dy >  254)	dy =  254;
		if (dy < -256)	dy = -256;
		if (dz >  126)	dz =  126;
		if (dz < -128)	dz = -128;

		/* Insere as informações na CLIST */

		ttyin (sc->mode.syncmask[1] | (~buttons & MOUSE_MSC_BUTTONS), tp);
		ttyin (dx >> 1, tp);
		ttyin (dy >> 1, tp);
		ttyin (dx - (dx >> 1), tp);
		ttyin (dy - (dy >> 1), tp);

		if (sc->mode.level == 1)
		{
			ttyin (dz >> 1, tp);
			ttyin (dz - (dz >> 1), tp);
			ttyin (((~buttons >> 3) & MOUSE_SYS_EXTBUTTONS), tp);
		}
	}

}	/* end ums_intr */

/*
 ****************************************************************
 *	Habilita o camundongo					*
 ****************************************************************
 */
int
ums_enable (struct ums_softc *sc)
{
	int		err;

	if (sc->sc_enabled)
		{ u.u_error = EBUSY; return (-1); }

	/* Set up interrupt pipe */

	err = usbd_open_pipe_intr
	(	sc->sc_iface, sc->sc_ep_addr, 
		USBD_SHORT_XFER_OK, &sc->sc_intrpipe, sc, 
		sc->sc_ibuf, sc->sc_isize, ums_intr,
		USBD_DEFAULT_INTERVAL
	);

	if (err)
	{
		printf ("ums_enable: usbd_open_pipe_intr failed, error = %d\n", err);
		u.u_error = EIO; return (-1);
	}

	sc->buttons_state = 0;
	sc->sc_enabled    = 1;

	return (0);

}	/* end ums_enable */

/*
 ****************************************************************
 *	Desabilita o camundongo					*
 ****************************************************************
 */
void
ums_disable (struct ums_softc *sc)
{
	usbd_abort_pipe (sc->sc_intrpipe);
	usbd_close_pipe (sc->sc_intrpipe);

	sc->sc_intrpipe = NULL;
	sc->sc_enabled  = 0;

}	/* end ums_disable */

/*
 ****************************************************************
 *	Inicia a escrita (?)					*
 ****************************************************************
 */
void
umsstart (TTY *tp, int flag)
{
	int		c;

	SPINLOCK (&tp->t_outq.c_lock);

	while ((c = cget (&tp->t_outq)) >= 0)
		/* vazio */;

	SPINFREE (&tp->t_outq.c_lock);

   /***	CLEAR (&tp->t_obusy);	***/

	EVENTDONE (&tp->t_outqempty);

}	/* end umsstart */
