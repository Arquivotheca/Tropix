/*
 ****************************************************************
 *								*
 *			uhub.c					*
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

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Declaração do Driver ************************************
 */
int	uhub_probe  (struct device *);
int	uhub_attach (struct device *);
int	uhub_detach (struct device *);

const struct driver uhub_driver =
{
	"uhub",

	uhub_probe,
	uhub_attach,
   	uhub_detach
};

/*
 ****** Estrutura da parte específica ***************************
 */
struct uhub_softc
{
	struct device		*sc_dev;	/* base device */
	struct usbd_device	*sc_hub;	/* USB device */
	struct usbd_pipe	*sc_ipipe;	/* interrupt pipe */
	uchar			sc_status[1];	/* XXX more ports */
	uchar			sc_running;
};

/*
 ******	Protótipos de funções ***********************************
 */
void		uhub_intr (struct usbd_xfer *xfer, void *addr, int status);

#define UHUB_INTR_INTERVAL 255	/* ms */

/*
 ****************************************************************
 *	Função de "probe"					*
 ****************************************************************
 */
int
uhub_probe (struct device *dev)
{
	struct usb_attach_arg		*uaa = dev->ivars;
	struct usb_device_descriptor	*dd = &uaa->device->ddesc;

#ifdef	USB_MSG
	printf ("uhub_probe\n");
#endif	USB_MSG

	/* 
	 *	The subclass for hubs seems to be 0 for some and 1 for others,
	 *	so we just ignore the subclass.
	 */
	if (uaa->iface == NULL && dd->bDeviceClass == UDCLASS_HUB)
		return (UPROBE_DEVCLASS_DEVSUBCLASS);

	return (UPROBE_NONE);

}	/* end uhub_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
uhub_attach (struct device *dev)
{
        struct uhub_softc		*sc;
        struct usb_attach_arg		*uaa  = dev->ivars;
	struct usbd_device		*udev = uaa->device;
	struct usbd_hub			*hub  = NULL;
	struct usb_device_request	req;
	struct usb_hub_descriptor	*hubdesc = NULL;
	struct usbd_interface		*iface;
	struct usb_endpoint_descriptor	*ed;
	int				p, port, nports, nremov, pwrdly, err;

	/*
	 *	Aloca e zera a estrutura "softc"
	 */
	if ((sc = dev->softc = malloc_byte (sizeof (struct uhub_softc))) == NULL)
		return (-1);

	memclr (sc, sizeof (struct uhub_softc));

#if (0)	/*******************************************************/
{
	char				*devinfo;

	if ((devinfo = malloc_byte (1024)) == NULL)
		goto bad;

	memclr (devinfo, 1024);

	usbd_devinfo (udev, USBD_SHOW_INTERFACE_CLASS, devinfo);

	device_set_desc_copy (dev, devinfo);

	free_byte (devinfo);
}
#endif	/*******************************************************/

	sc->sc_hub = udev;
	sc->sc_dev = dev;

	if (err = usbd_set_config_index (udev, 0, 1))
	{
		printf ("uhub_attach (%s): erro na configuração (%s)\n", dev->nameunit, usbd_errstr (err));
		goto bad;
	}

	if (udev->depth > USB_HUB_MAX_DEPTH)
	{
		printf
		(	"uhub_attach (%s): profundidade máxima excedida (%d > %d), \"hub\" ignorado\n",
			dev->nameunit, udev->depth, USB_HUB_MAX_DEPTH
		);
		goto bad;
	}

	/* Get hub descriptor */

	if ((hubdesc = malloc_byte (sizeof (struct usb_hub_descriptor))) == NULL)
		{ err = USBD_NOMEM; goto bad; }

	req.bmRequestType	= UT_READ_CLASS_DEVICE;
	req.bRequest		= UR_GET_DESCRIPTOR;

	USETW2 (req.wValue, (udev->address > 1 ? UDESC_HUB : 0), 0);
	USETW  (req.wIndex, 0);
	USETW  (req.wLength, USB_HUB_DESCRIPTOR_SIZE);

	err = usbd_do_request (udev, &req, hubdesc);

	nports = hubdesc->bNbrPorts;

	if (err == 0 && nports > 7)
	{
		USETW (req.wLength, USB_HUB_DESCRIPTOR_SIZE + (nports + 1) / 8);

		err = usbd_do_request (udev, &req, hubdesc);
	}

	if (err)
	{
		printf
		(	"uhub_attach (%s): erro ao ler descritor do hub (%s)\n",
			dev->nameunit, usbd_errstr (err)
		);

		goto bad;
	}

	for (nremov = 0, port = 1; port <= nports; port++)
	{
		if (!UHD_NOT_REMOV (hubdesc, port))
			nremov++;
	}

	if (nports == 0)
		{ printf ("%s: hub sem portas ignorado\n", dev->nameunit); goto bad; }

	printf
	(	"%s: %d porta%s, %d removíve%s, %s energizadas\n",
		dev->nameunit,
		nports, nports != 1 ? "s" : "",
		nremov, nremov != 1 ? "is" : "l",
		udev->self_powered ? "auto" : "bus"
	);

	if ((hub = malloc_byte (sizeof (*hub) + (nports - 1) * sizeof (struct usbd_port))) == NULL)
		goto bad;

	memclr (hub, (sizeof (*hub) + (nports - 1) * sizeof (struct usbd_port)));

	udev->hub		= hub;
	udev->hub->hubsoftc	= sc;
	hub->explore		= uhub_explore;
	hub->hubdesc		= *hubdesc;

	free_byte (hubdesc); hubdesc = NULL;

#ifdef	USB_MSG
	printf
	(	"uhub_attach: selfpowered=%d, parent=%p, parent->selfpowered=%d\n",
		 dev->self_powered, dev->powersrc->parent,
		 dev->powersrc->parent ? dev->powersrc->parent->self_powered : 0
	);
#endif	USB_MSG

	if (!udev->self_powered && udev->powersrc->parent != NULL && !udev->powersrc->parent->self_powered)
	{
		printf
		(	"uhub_attach (%s): bus powered hub connected to bus powered hub, ignored\n",
			dev->nameunit
		);
		goto bad;
	}

	/* Set up interrupt pipe */

	if (usbd_device2interface_handle (udev, 0, &iface))
	{
		printf ("uhub_attach (%s): no interface handle\n", dev->nameunit);
		goto bad;
	}

	if ((ed = usbd_interface2endpoint_descriptor (iface, 0)) == NULL)
	{
		printf ("uhub_attach (%s): no endpoint descriptor\n", dev->nameunit);
		goto bad;
	}

	if ((ed->bmAttributes & UE_XFERTYPE) != UE_INTERRUPT)
	{
		printf ("uhub_attach (%s): bad interrupt endpoint\n", dev->nameunit);
		goto bad;
	}

	if
	(	usbd_open_pipe_intr
		(	iface, ed->bEndpointAddress,
			USBD_SHORT_XFER_OK, &sc->sc_ipipe, sc, sc->sc_status, 
			sizeof (sc->sc_status), uhub_intr, UHUB_INTR_INTERVAL
		)
	)
	{
		printf ("uhub_attach (%s): cannot open interrupt pipe\n", dev->nameunit);
		goto bad;
	}

	/* Wait with power off for a while */

	usbd_delay_ms (udev, USB_POWER_DOWN_TIME);

	/*
	 * To have the best chance of success we do things in the exact same
	 * order as Windoze98.  This should not be necessary, but some
	 * devices do not follow the USB specs to the letter.
	 *
	 * These are the events on the bus when a hub is attached:
	 *  Get device and config descriptors (see attach code)
	 *  Get hub descriptor (see above)
	 *  For all ports
	 *     turn on power
	 *     wait for power to become stable
	 * (all below happens in explore code)
	 *  For all ports
	 *     clear C_PORT_CONNECTION
	 *  For all ports
	 *     get port status
	 *     if device connected
	 *        wait 100 ms
	 *        turn on reset
	 *        wait
	 *        clear C_PORT_RESET
	 *        get port status
	 *        proceed with device attachment
	 */

	/* Set up data structures */

	for (p = 0; p < nports; p++)
	{
		struct usbd_port	*up = &hub->ports[p];

		up->device = NULL;
		up->parent = udev;
		up->portno = p+1;

		if (udev->self_powered) /* Self powered hub, give ports maximum current */
			up->power = USB_MAX_POWER;
		else
			up->power = USB_MIN_POWER;

		up->restartcnt = 0;
	}

	/* XXX should check for none, individual, or ganged power? */

	pwrdly = udev->hub->hubdesc.bPwrOn2PwrGood * UHD_PWRON_FACTOR + USB_EXTRA_POWER_UP_TIME;

	for (port = 1; port <= nports; port++)
	{
		/* Turn the power on */

		if (err = usbd_set_port_feature (udev, port, UHF_PORT_POWER))
		{
			printf
			(	"uhub_attach (%s): port %d power on failed, %s\n", 
				dev->nameunit, port, usbd_errstr (err)
			);
		}

#ifdef	USB_MSG
		printf ("uhub_attach: turn on port %d power\n", port);
#endif	USB_MSG

		/* Wait for stable power */

		usbd_delay_ms (udev, pwrdly);
	}

	/* The usual exploration will finish the setup */

	sc->sc_running = 1;

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (hubdesc != NULL)
		free_byte (hubdesc);

	if (hub != NULL)
		free_byte (hub);

	udev->hub = NULL;

	free_byte (sc);	dev->softc = NULL;

	return (-1);

}	/* end uhub_attach */

/*
 ****************************************************************
 *	Função de "detach"					*
 ****************************************************************
 */
int
uhub_detach (struct device *dev)
{
        struct uhub_softc	*sc  = dev->softc;
	struct usbd_hub		*hub;
	struct usbd_port	*rup;
	int			port, nports;

	if ((hub = sc->sc_hub->hub) == NULL)
		return (0);

	usbd_abort_pipe (sc->sc_ipipe);
	usbd_close_pipe (sc->sc_ipipe);

	nports = hub->hubdesc.bNbrPorts;

	for (port = 0; port < nports; port++)
	{
		rup = &hub->ports[port];

		if (rup->device)
			usb_disconnect_port (rup, dev);
	}

	free_byte (hub); sc->sc_hub->hub = NULL;

	return (0);

}	/* end uhub_detach */

/*
 ****************************************************************
 *	Varre as portas de uma unidade				*
 ****************************************************************
 */
int
uhub_explore (struct usbd_device *dev)
{
	struct usb_hub_descriptor	*hd = &dev->hub->hubdesc;
	struct uhub_softc		*sc = dev->hub->hubsoftc;
	struct usbd_port		*up;
	int				err, speed;
	int				port, change, status;

#ifdef	USB_MSG
	printf ("uhub_explore (%s)\n", sc->sc_dev->nameunit);
	printf ("uhub_explore: dev= %P addr = %d\n", dev, dev->address);
#endif	USB_MSG

	if (sc->sc_running == 0)
		return (USBD_NOT_STARTED);

	if (dev->depth > USB_HUB_MAX_DEPTH)
		return (USBD_TOO_DEEP);

	for (port = 1; port <= hd->bNbrPorts; port++)
	{
		up = &dev->hub->ports[port - 1];

		if (err = usbd_get_port_status (dev, port, &up->status))
		{
			printf
			(	"uhub_explore (%s): erro ao ler o estado da porta %d (%s)\n",
				sc->sc_dev->nameunit, port, usbd_errstr (err)
			);
			continue;
		}

		status = UGETW (up->status.wPortStatus);
		change = UGETW (up->status.wPortChange);

#ifdef	USB_MSG
		printf
		(	"uhub_explore (%s) porta %d status = 0x%04X change = 0x%04X\n",
			sc->sc_dev->nameunit, port, status, change
		);
#endif	USB_MSG

		if (change & UPS_C_PORT_ENABLED)
		{
#ifdef	USB_MSG
			printf ("uhub_explore: C_PORT_ENABLED\n");
#endif	USB_MSG

			usbd_clear_port_feature (dev, port, UHF_C_PORT_ENABLE);

			if   (change & UPS_C_CONNECT_STATUS)
			{
				/* Ignore the port error if the device vanished. */
#ifdef	USB_MSG
				printf
				(	"uhub_explore (%s): ignorando erro, porta %d\n",
					sc->sc_dev->nameunit, port
				);
#endif	USB_MSG
			}
			elif (status & UPS_PORT_ENABLED)
			{
				printf
				(	"uhub_explore (%s): mudança inválida de habilitação, porta %d\n",
					sc->sc_dev->nameunit, port
				);
			}
			else
			{
				/* Port error condition */

				if (up->restartcnt)		/* no message first time */
				{
					printf
					(	"uhub_explore (%s): inicializando a porta %d\n",
						sc->sc_dev->nameunit, port
					);
				}

				if (up->restartcnt++ < USBD_RESTART_MAX)
				{
					goto disco;
				}
				else
				{
					printf
					(	"uhub_explore (%s): desconsiderando a porta %d, após %d tentativas\n",
						sc->sc_dev->nameunit, port, up->restartcnt
					);
				}
			}
		}

		if ((change & UPS_C_CONNECT_STATUS) == 0)
		{
#ifdef	USB_MSG
			printf ("uhub_explore (%s): porta %d !C_CONNECT_STATUS\n", sc->sc_dev->nameunit, port);
#endif	USB_MSG
			/* No status change, just do recursive explore */

			if (up->device != NULL && up->device->hub != NULL)
				up->device->hub->explore (up->device);

#if (0)	/*******************************************************/
			if (up->device == NULL && (status & UPS_CURRENT_CONNECT_STATUS))
				printf ("uhub_explore (%s): connected, no device\n", sc->sc_dev->nameunit);
#endif	/*******************************************************/

			continue;
		}

		/* We have a connect status change, handle it */

#ifdef	USB_MSG
		printf
		(	"uhub_explore (%s): status change hub = %d port = %d\n",
			sc->sc_dev->nameunit, dev->address, port
		);
#endif	USB_MSG

		usbd_clear_port_feature (dev, port, UHF_C_PORT_CONNECTION);

		/*** usbd_clear_port_feature (dev, port, UHF_C_PORT_ENABLE);	***/

		/*
		 *	If there is already a device on the port the change status
		 *	must mean that is has disconnected.  Looking at the
		 *	current connect status is not enough to figure this out
		 *	since a new unit may have been connected before we handle
		 *	the disconnect.
		 */
	    disco:
		if (up->device != NULL)
		{
			/* Disconnected */

			printf ("%s: dispositivo desconectado da porta %d\n", sc->sc_dev->nameunit, port);

			usb_disconnect_port (up, sc->sc_dev);

			usbd_clear_port_feature (dev, port, UHF_C_PORT_CONNECTION);
		}

		if ((status & UPS_CURRENT_CONNECT_STATUS) == 0)
		{
			/* Nothing connected, just ignore it */

#ifdef	USB_MSG
			printf ("uhub_explore: port = %d NÃO possui dispositivo conectado\n", port);
#endif	USB_MSG
			continue;
		}

		/* Connected */

		if ((status & UPS_PORT_POWER) == 0)
		{
			printf
			(	"uhub_explore (%s): dispositivo sem energia na porta %d\n",
				sc->sc_dev->nameunit, port
			);
		}

		/* Wait for maximum device power up time */

		usbd_delay_ms (dev, USB_PORT_POWERUP_DELAY);

		/* Reset port, which implies enabling it */

		if (usbd_reset_port (dev, port, &up->status))
		{
			printf ("uhub_explore (%s): port %d reset failed\n", sc->sc_dev->nameunit, port);
			continue;
		}

		/* Get port status again, it might have changed during reset */

		if (err = usbd_get_port_status (dev, port, &up->status))
		{
			printf
			(	"uhub_explore (%s): erro ao ler o estado da porta %d (%s)\n",
				sc->sc_dev->nameunit, port, usbd_errstr (err)
			);
			continue;
		}

		status = UGETW (up->status.wPortStatus);
		change = UGETW (up->status.wPortChange);

		if ((status & UPS_CURRENT_CONNECT_STATUS) == 0)
		{
			/* Nothing connected, just ignore it */
#ifdef	USB_MSG
			printf
			(	"uhub_explore (%s): porta %d, o dispositivo sumiu após o \"reset\"\n",
				sc->sc_dev->nameunit, port
			);
#endif	USB_MSG
			continue;
		}

		printf ("%s: dispositivo recém-conectado na porta %d, ", sc->sc_dev->nameunit, port);

		/* Obtém a velocidade */

		if   (status & UPS_HIGH_SPEED)
			{ speed = USB_SPEED_HIGH;	printf ("USB 2.0\n"); /* USB 2.0, 480 Mbs */   }
		elif (status & UPS_LOW_SPEED)
			{ speed = USB_SPEED_LOW;	printf ("USB 1.0\n"); /* USB 1.0 */  }
		else
			{ speed = USB_SPEED_FULL;	printf ("USB 1.1\n"); /* USB 1.1, 24 Mbs */  }

		/* Anexa o dispositivo */

		err = usbd_new_device (sc->sc_dev, dev->bus, dev->depth + 1, speed, port, up);

		/* XXX retry a few times? */

		if (err)
		{
			printf
			(	"uhub_explore (%s): usbd_new_device failed, port = %d, error = %s\n",
				sc->sc_dev->nameunit, port, usbd_errstr (err)
			);

			/* Avoid addressing problems by disabling */

			/* usbd_reset_port (dev, port, &up->status); */

			/* 
			 *	The unit refused to accept a new address, or had
			 *	some other serious problem.  Since we cannot leave
			 *	at 0 we have to disable the port instead.
			 */
			printf
			(	"uhub_explore (%s): o dispositivo está com problemas, inabilitando a porta %d\n",
				sc->sc_dev->nameunit, port
			);

			usbd_clear_port_feature (dev, port, UHF_PORT_ENABLE);

		}
		else
		{
			/* Tudo correu bem */

			up->restartcnt = 0;

			if (up->device->hub)
				(*up->device->hub->explore) (up->device);
		}
	}

	return (USBD_NORMAL_COMPLETION);

}	/* end uhub_explore */

/*
 ****************************************************************
 *	Interrupção do HUB					*
 ****************************************************************
 *
 *	This an indication that some port has changed status.
 *	Notify the bus event handler thread that we need
 *	to be explored again.
 */
void
uhub_intr (struct usbd_xfer *xfer, void *addr, int status)
{
	struct uhub_softc	*sc = addr;

#if (0)	/*******************************************************/
printf ("%g: Chamada, status = %P\n", status);
#endif	/*******************************************************/

	if   (status == USBD_STALLED)
		usbd_clear_endpoint_stall_async (sc->sc_ipipe);
	elif (status == USBD_NORMAL_COMPLETION)
		usb_needs_explore (sc->sc_hub);

}	/* end uhub_intr */

#if (0)	/*******************************************************/
/* Called when a device has been detached from it */
Static void
uhub_child_detached(struct device *self, struct device *child)
{
       struct uhub_softc *sc = device_get_softc(self);
       struct usbd_device * devhub = sc->sc_hub;
       struct usbd_device * dev;
       int nports;
       int port;
       int i;

       if (!devhub->hub)  
               /* should never happen; children are only created after init */
               panic("hub not fully initialised, but child deleted?");

       nports = devhub->hub->hubdesc.bNbrPorts;
       for (port = 0; port < nports; port++) {
               dev = devhub->hub->ports[port].device;
               if (dev && dev->subdevs) {
                       for (i = 0; dev->subdevs[i]; i++) {
                               if (dev->subdevs[i] == child) {
                                       dev->subdevs[i] = NULL;
                                       return;
                               }
                       }
               }
       }
}
#endif	/*******************************************************/
