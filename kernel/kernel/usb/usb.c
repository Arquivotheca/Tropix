/*
 ****************************************************************
 *								*
 *			usb.c					*
 *								*
 *	"Driver" para dispositivo USB				*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.6.0, de 06.10.04				*
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
#include "../h/scb.h"

#include "../h/signal.h"
#include "../h/region.h"
#include "../h/uproc.h"

#include "../h/usb.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Declaração do Driver ************************************
 */
int	usb_probe  (struct device *);
int	usb_attach (struct device *);
int	usb_detach (struct device *);

const struct driver usb_driver =
{
	"usb",

	usb_probe,
	usb_attach,
	usb_detach
};

/*
 ******	Estrutura da parte específica ***************************
 */
#define	EXPLORE_TIMEOUT		5

struct usb_softc
{
	struct device		*sc_dev;	/* base device */
	struct usbd_bus 	*sc_bus;	/* USB controller */
	struct usbd_port	sc_port;	/* dummy port for root hub */

	int			sc_dying;
};

/*
 ******	Informações sobre as Unidades ***************************
 */
entry struct usb_data		usb_data[NUSB];
entry struct usb_softc		*usb_softc_data[NUSB];

entry int			usb_unit = -1;
entry int			usb_active;

#ifdef	USB_POLLING
entry int			usb_cold;
#endif	USB_POLLING

entry EVENT			usb_explore_event;

void				usb_explorer (void);

/*
 ****************************************************************
 *	Função de "probe"					*
 ****************************************************************
 */
int
usb_probe (struct device *dev)
{
	return (UPROBE_GENERIC);

}	/* end usb_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
usb_attach (struct device *dev)
{
	struct usb_softc	*sc;
	struct usbd_device	*udev;
	int			speed;

	/*
	 *	Aloca a estrutura "softc"
	 */
	if ((sc = dev->softc = malloc_byte (sizeof (struct usb_softc))) == NULL)
		return (-1);

	memclr (sc, sizeof (struct usb_softc));

	sc->sc_dev		= dev;
	sc->sc_bus		= dev->ivars;
	sc->sc_bus->usbctl	= sc;
	sc->sc_port.power	= USB_MAX_POWER;

#ifdef	USB_MSG
	printf ("usb_attach: %s: USB revision ", dev->nameunit);
#endif	USB_MSG

	switch (sc->sc_bus->usbrev)
	{
	    case USBREV_1_0:
	    case USBREV_1_1:
		speed = USB_SPEED_FULL;
		break;

	    case USBREV_2_0:
		speed = USB_SPEED_HIGH;
		break;

	    default:
		goto bad;
	}

#ifdef	USB_POLLING
	if (usb_cold)
		sc->sc_bus->use_polling++;
#endif	USB_POLLING

	if (usbd_new_device (sc->sc_dev, sc->sc_bus, 0, speed, 0, &sc->sc_port) != 0)
		{ printf ("%s: HUB raiz defeituoso\n", dev->nameunit); goto bad; }

	udev = sc->sc_port.device;

	if (udev->hub == NULL)
		{ printf ("%s: Hub raiz NÃO encontrado\n", dev->nameunit); goto bad; }

	sc->sc_bus->root_hub = udev;

#if (0)	/*******************************************************/
	/*
	 *	Explora o "hub" raiz
	 */
	(*udev->hub->explore) (sc->sc_bus->root_hub);
#endif	/*******************************************************/

	/*
	 *	Inclui a unidade na lista de exploráveis
	 */
	usb_softc_data[usb_unit] = sc;	usb_active++;

#ifdef	USB_POLLING
	if (usb_cold)
		sc->sc_bus->use_polling--;
#endif	USB_POLLING

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	free_byte (sc);	dev->softc = NULL;
	return (-1);

}	/* end usb_attach */

/*
 ****************************************************************
 *	Função de "detach"					*
 ****************************************************************
 */
int
usb_detach (struct device *dev)
{
	return (-1);

}	/* end usb_detach */

/*
 ****************************************************************
 *	Varre todas as unidades, em busca de alterações		*
 ****************************************************************
 */
void	uhci_poll_hub (int unit);

void
usb_explorer (void)
{
	struct usb_softc	*sc;
	int			unit, nactive;

	/*
	 *	Nunca usará endereços virtuais de usuário
	 */
	u.u_no_user_mmu = 1;

	/*
	 *	Malha principal: varre periodicamente as unidades, explorando os HUB's
	 */
	do
	{
		EVENTWAIT  (&usb_explore_event, PBLKIO);
		EVENTCLEAR (&usb_explore_event);

		for (nactive = unit = 0; unit <= usb_unit; unit++)
		{
			if ((sc = usb_softc_data[unit]) == NULL)
				continue;

			if (usb_data[unit].usb_data_stolen != 0)
				continue;

			if (sc->sc_dying)
				{ usb_softc_data[unit] = NULL; continue; }

			/* Achou uma unidade a explorar */

			nactive++;
#ifdef	MSG
			if (CSWT (39))
				printf ("%s: iniciando exploração\n", sc->sc_dev->nameunit);
#endif	MSG
			sc->sc_bus->exploring_hub = 1;

			(*sc->sc_bus->root_hub->hub->explore) (sc->sc_bus->root_hub);

			sc->sc_bus->exploring_hub = 0;
		}

	}	while ((usb_active = nactive) > 0);

	/* Não há mais unidades ativas */

	printf ("usb: exploração periódica encerrada\n");

	kexit (0, 0, 0);

}	/* end usb_explorer */

#ifdef	USB_POLLING
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usb_hub_explore (int unit)
{
	struct usb_softc	*sc = usb_softc_data[unit];

	(*sc->sc_bus->root_hub->hub->explore) (sc->sc_bus->root_hub);

}	/* end usb_hub_explore */
#endif	USB_POLLING

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usb_needs_explore (struct usbd_device *udev)
{
	EVENTDONE (&usb_explore_event);

}	/* end usb_needs_explore */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usb_schedsoftintr (struct usbd_bus *bus)
{
	const struct usbd_bus_methods		*methods;
	void					(*soft_intr) (void *);

	if (bus != NULL && (methods = bus->methods) != NULL && (soft_intr = methods->soft_intr) != NULL)
		(*soft_intr) (bus);

} 	/* end usb_schedsoftintr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usb_str (struct usb_string_descriptor *p, int l, char *s)
{
	int	i;

	if (l == 0)
		return (0);

	p->bLength = 2 * strlen(s) + 2;

	if (l == 1)
		return (1);

	p->bDescriptorType = UDESC_STRING; l -= 2;

	for (i = 0; s[i] && l > 1; i++, l -= 2)
		USETW2 (p->bString[i], 0, s[i]);

	return (2 * i + 2);

}	/* end usb_str */
