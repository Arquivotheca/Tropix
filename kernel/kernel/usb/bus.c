/*
 ****************************************************************
 *								*
 *			bus.c					*
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
 ******	Variáveis Globais ***************************************
 */
struct device		*usb_device_list;	/* Lista de dispositivos */
struct devclass		*usb_class_list;	/* Lista de classes */

/*
 ******	Protótipos de funções ***********************************
 */
struct driver	*device_get_best_driver (struct device *dev);
int		device_set_devclass (struct device *dev, const char *classname);
int		device_printf (struct device *dev, const char * fmt, const char *arg);
const char	*device_get_nameunit (struct device *dev);
int		device_detach (struct device *dev);
void		device_set_desc (struct device *dev, const char* desc);
int		add_device_to_class (struct devclass *dc, struct device *dev);
struct devclass	*devclass_find_or_create (const char *classname, int create);

/*
 ****************************************************************
 *	Cria as classes e associa os "drivers"			*
 ****************************************************************
 */
int
usb_create_classes (void)
{
	struct devclass		*dc;
	const char		*class_name;

	class_name = "usb_controller";

	if ((dc = devclass_find_or_create (class_name, TRUE)) == NULL)
		goto bad;

	add_driver_to_class (dc, &usb_driver);

	class_name = "usb";

	if ((dc = devclass_find_or_create (class_name, TRUE)) == NULL)
		goto bad;

	add_driver_to_class (dc, &uhub_driver);

	class_name = "uhub";

	if ((dc = devclass_find_or_create (class_name, TRUE)) == NULL)
		goto bad;

	add_driver_to_class (dc, &ums_driver);
	add_driver_to_class (dc, &ulp_driver);
	add_driver_to_class (dc, &ud_driver);
	add_driver_to_class (dc, &uhub_driver);

	return (0);

    bad:
	printf ("usb_create_classes: NÃO consegui criar a classe \"%s\"\n", class_name);
	return (-1);

}	/* end usb_create_classes */

/*
 ****************************************************************
 *	Cria uma "struct device", representando um dispositivo	*
 ****************************************************************
 */
struct device *
make_device (struct device *parent, const char *classname, int unit)
{
	struct device		*dev;
	struct devclass		*dc = NULL;

	if (classname != NOSTR && (dc = devclass_find_or_create (classname, TRUE)) == NULL)
	{
		printf ("make_device: NÃO consegui criar a classe \"%s\"\n", classname);
		return (NULL);
	}

	/*
	 *	Aloca memória e preenche a estrutura "device"
	 */
	if ((dev = malloc_byte (sizeof (struct device))) == NULL)
		return (NULL);

	dev->parent	= parent;
	dev->children	= NULL;
	dev->sibling	= NULL;

	dev->nameunit	= NULL;

	dev->devclass	= NULL;
	dev->unit	= unit;

	dev->driver	= NULL;

#if (0)	/*******************************************************/
	dev->desc	= NULL;
#endif	/*******************************************************/

	dev->state	= DS_NOTPRESENT;
	dev->flags	= DF_ENABLED;

	dev->ivars	= NULL;
	dev->softc	= NULL;

	if (unit == -1)
		dev->flags |= DF_WILDCARD;

	if (classname != NOSTR)
	{
		dev->flags |= DF_FIXEDCLASS;

		/* Já é conhecida a classe do dispositivo */

		if (add_device_to_class (dc, dev))
			return (NULL);
	}

	/* Insere na lista global de dispositivos */

	dev->next	= usb_device_list;
	usb_device_list = dev;

	return (dev);

}	/* end make_device */

/*
 ****************************************************************
 *	Tenta anexar um dispositivo				*
 ****************************************************************
 */
int
device_probe_and_attach (struct device *dev)
{
	int		error;
	int		hasclass = (dev->devclass != NULL);
	struct driver	*driver;

	/*
	 *	Obtém o melhor "driver" para o dispositivo (?)
	 */
	if ((driver = device_get_best_driver (dev)) == NULL)
		return (-1);

	/*
	 *	Chama a função de "attach" do driver
	 */
	if ((error = (*driver->attach) (dev)) == 0)
		{ dev->state = DS_ATTACHED; return (0); }

	printf ("device_probe_and_attach: erro em \"%s_attach\"\n", driver->name);

	if (!hasclass)
		device_set_devclass (dev, NOSTR);

	dev->state = DS_NOTPRESENT;

	return (error);

}	/* end device_probe_and_attach */

/*
 ****************************************************************
 *	Obtém um "driver" adequado a um dispositivo		*
 ****************************************************************
 */
struct driver *
device_get_best_driver (struct device *dev)
{
	struct device		*parent;
	struct devclass		*parent_class;
	struct driver		*dp;
	struct driver		*best_driver = NULL;
	int			result, pri = 0;

	if (dev->state == DS_ALIVE)
		return (dev->driver);

	if ((parent = dev->parent) == NULL)
		return (NULL);

	if ((parent_class = parent->devclass) == NULL)
		return (NULL);

	/*
	 *	Tenta os diversos drivers
	 */
	for (dp = parent_class->driver_list; dp != NULL; dp = dp->next)
	{
		if (dev->devclass != NULL && !streq (dp->name, dev->devclass->name))
			continue;

#ifdef	USB_MSG
		printf ("Tentando o driver \"%s\"\n", dp->name);
#endif	USB_MSG

		/*
		 *	Chama a função de "probe" do driver
		 */
		result = (*dp->probe) (dev);

		/*
		 *	Se o retorno é 0, não pode haver melhor
		 */
		if (result == 0)
			{ best_driver = dp; pri = 0; break; }

		/*
		 *	A função de "probe" retornou erro
		 */
		if (result > 0)
			continue;

		/*
		 *	Guarda o melhor "probe"
		 */
		if (best_driver == NULL || result > pri)
			{ best_driver = dp; pri = result; }
	}

	if (best_driver == NULL)
		return (NULL);

	/*
	 *	Obtido o melhor "driver", oficializa-o
	 */
	if (dev->devclass == NULL)
		device_set_devclass (dev, best_driver->name);

#if (0)	/*******************************************************/
	/*
	 *	Precisa ?
	 */
	if (pri < 0)
		(*best_driver->probe) (dev);
#endif	/*******************************************************/

	dev->state = DS_ALIVE;

	return (dev->driver = best_driver);

}	/* end device_get_best_driver */

/*
 ****************************************************************
 *	Busca por uma classe, possivelmente criando-a		*
 ****************************************************************
 */
struct devclass *
devclass_find_or_create (const char *classname, int create)
{
	struct devclass		*dc;

	/*
	 *	Se não foi dado o nome, ...
	 */
	if (classname == NOSTR)
		return (NULL);

	/*
	 *	Busca pela classe, através do nome
	 */	
	for (dc = usb_class_list; dc != NULL; dc = dc->next)
	{
		if (streq (dc->name, classname))
			return (dc);
	}

	/*
	 *	Não achou; se não deve criar, ...
	 */
	if (!create)
		return (NULL);

	/*
	 *	Cria a classe
	 */
	if ((dc = malloc_byte (sizeof (struct devclass) + strlen (classname) + 1)) == NULL)
		return (NULL);

	memclr (dc, sizeof (struct devclass));

	dc->name = (char *)(dc + 1);
	strcpy (dc->name, classname);

   /***	dc->drivers	= NULL; ***/

	/* Adiciona a nova classe à lista (global) de classes */

	dc->next	= usb_class_list;
	usb_class_list	= dc;

	return (dc);

}	/* end devclass_find_or_create */

/*
 ****************************************************************
 *	Adiciona um "driver" a uma classe			*
 ****************************************************************
 */
void
add_driver_to_class (struct devclass *dc, struct driver *driver)
{
	driver->next	= dc->driver_list;
	dc->driver_list	= driver;

}	/* end add_driver_to_class */

/*
 ****************************************************************
 *	Adiciona um dispositivo a uma classe			*
 ****************************************************************
 */
int
add_device_to_class (struct devclass *dc, struct device *dev)
{
	int		i, buflen;

	buflen = strlen (dc->name) + 8;

	if ((dev->nameunit = malloc_byte (buflen)) == NULL)
		return (-1);

	memclr (dev->nameunit, buflen);

	if (dev->unit < 0)
	{
		for (i = 0; /* abaixo */; i++)
		{
			if (i >= MAX_DEV_in_CLASS)
			{
				printf
				(	"add_device_to_class : a classe \"%s\""
					" NÃO comporta mais dispositivos\n",
					dc->name
				);

				return (-1);
			}

			if (dc->devices[i] == NULL)
				break;
		}

		dev->unit = i;
	}

	dc->devices[dev->unit] = dev;
	dev->devclass = dc;

	snprintf (dev->nameunit, buflen, "%s%d", dc->name, dev->unit);

	return (0);

}	/* end add_device_to_class */

/*
 ****************************************************************
 *	Remove um dispositivo de uma classe			*
 ****************************************************************
 */
int
device_delete_from_class (struct devclass *dc, struct device *dev)
{
	if (dc == NULL || dev == NULL)
		return (0);

	if (dev->devclass != dc || dc->devices[dev->unit] != dev)
	{
		printf
		(	"device_delete_from_class: inconsistência na classe do dispositivo \"%s\"\n",
			dev->nameunit
		);

		return (-1);
	}

	dc->devices[dev->unit] = NULL;

	if (dev->flags & DF_WILDCARD)
		dev->unit = -1;

	dev->devclass = NULL;

	free_byte (dev->nameunit); 	dev->nameunit = NULL;

	return (0);

}	/* end device_delete_from_class */

/*
 ****************************************************************
 *	Adiciona um dispositivo filho				*
 ****************************************************************
 */
struct device *
device_add_child (struct device *parent, const char *classname, int unit)
{
	struct device	*child;

	/*
	 *	Cria o dispositivo
	 */
	if ((child = make_device (parent, classname, unit)) == NULL)
		return (NULL);

	/*
	 *	Insere na lista de filhos de "parent"
	 */
	child->sibling	 = parent->children;
	parent->children = child;

   /***	child->parent	 = parent; ***/

	return (child);

}	/* end device_add_child */

/*
 ****************************************************************
 *	Remove um dispositivo filho				*
 ****************************************************************
 */
int
device_delete_child (struct device *parent, struct device *dev)
{
	int		error;
	struct device	*grandchild, **dp;

	/*
	 *	Primeiramente, remove os filhos
	 */
	while ((grandchild = dev->children) != NULL)
	{
		if (error = device_delete_child (dev, grandchild))
			return (error);
	}

	/*
	 *	Desanexa o dispositivo
	 */
	if (error = device_detach (dev))
		return (error);

	/*
	 *	Retira da classe
	 */
	if (dev->devclass)
		device_delete_from_class (dev->devclass, dev);

	/*
	 *	Remove da lista de filhos do dispositivo pai
	 */
	for (dp = &parent->children; /* abaixo */; dp = &(*dp)->sibling)
	{
		if (*dp == NULL)
		{
			printf
			(	"device_delete_child: o dispositivo \"%s\" NÃO consta da lista de filhos do pai\n",
				dev->nameunit
			);
			break;
		}

		if (*dp == dev)
			{ *dp = (*dp)->sibling; break; }
	}

	/*
	 *	Remove da lista global de dispositivos
	 */
	for (dp = &usb_device_list; /* abaixo */; dp = &(*dp)->next)
	{
		if (*dp == NULL)
		{
			printf
			(	"device_delete_child: o dispositivo \"%s\" NÃO consta da lista global\n",
				dev->nameunit
			);
			break;
		}

		if (*dp == dev)
			{ *dp = (*dp)->next; break; }
	}

#if (0)	/*******************************************************/
	device_set_desc (dev, NOSTR);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	if (dev->ivars != NOSTR)	/* Está na pilha */
		free_byte (dev->ivars);
#endif	/*******************************************************/

	free_byte (dev);

	return (0);

}	/* device_delete_child */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
device_print_prettyname (struct device *dev)
{
	const char	*name;

	name = (dev != NULL && dev->devclass != NULL) ? dev->devclass->name : NULL;

	if (name == NULL)
		printf ("???: ");
	else
		printf ("%s%d: ", name, dev->unit);

}	/* end device_print_prettyname */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
device_set_desc_internal (struct device *dev, const char *desc, int copy)
{
	if (dev->desc != NOSTR && (dev->flags & DF_DESCMALLOCED))
	{
		free_byte (dev->desc);	dev->desc = NULL;
		dev->flags &= ~DF_DESCMALLOCED;
	}

	if (copy && desc != NOSTR)
	{
		if (dev->desc = malloc_byte (strlen (desc) + 1))
		{
			strcpy (dev->desc, desc);
			dev->flags |= DF_DESCMALLOCED;
		}
	}
	else
	{
		dev->desc = (char *)desc;
	}

}	/* end device_set_desc_internal */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
device_set_desc (struct device *dev, const char* desc)
{
	device_set_desc_internal (dev, desc, FALSE);

}	/* end device_set_desc */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
device_set_desc_copy (struct device *dev, const char* desc)
{
	device_set_desc_internal (dev, desc, TRUE);

}	/* end device_set_desc_copy */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Estabelece a classe de um dispositivo			*
 ****************************************************************
 */
int
device_set_devclass (struct device *dev, const char *classname)
{
	struct devclass		*dc;

	if (classname == NOSTR)
	{
		if (dev->devclass != NULL)
			device_delete_from_class (dev->devclass, dev);

		return (0);
	}

	if (dev->devclass != NULL)
	{
		printf
		(	"device_set_devclass: o dispositivo \"%s\" já possui classe \"%s\"\n",
			dev->nameunit, dev->devclass->name
		);

		return (-1);
	}

	if ((dc = devclass_find_or_create (classname, TRUE)) == NULL)
		return (-1);

	return (add_device_to_class (dc, dev));

}	/* end device_set_devclass */

/*
 ****************************************************************
 *	Desanexa um dispositivo					*
 ****************************************************************
 */
int
device_detach (struct device *dev)
{
	int		error;

	if (dev->state == DS_BUSY)
		return (-1);

	if (dev->state != DS_ATTACHED)
		return (0);

	if ((error = (*dev->driver->detach) (dev)) != 0)
		return (error);

#if (0)	/*******************************************************/
	device_print_prettyname (dev); 	printf (" removido\n");

	if (dev->parent)
		BUS_CHILD_DETACHED (dev->parent, dev);
#endif	/*******************************************************/

	if ((dev->flags & DF_FIXEDCLASS) == 0)
		device_delete_from_class (dev->devclass, dev);

	dev->state  = DS_NOTPRESENT;
	dev->driver = NULL;

	if (dev->softc != NULL)
		{ free_byte (dev->softc); dev->softc = NULL; }

	return (0);

}	/* end device_detach */

#if (0)	/*******************************************************/
int
bus_generic_attach(struct device *dev)
{
	struct device *child;

	TAILQ_FOREACH(child, &dev->children, link) {
		device_probe_and_attach(child);
	}

	return (0);
}

int
bus_generic_detach(struct device *dev)
{
	struct device *child;
	int error;

	if (dev->state != DS_ATTACHED)
		return (EBUSY);

	TAILQ_FOREACH(child, &dev->children, link) {
		if ((error = device_detach(child)) != 0)
			return (error);
	}

	return (0);
}

int
bus_generic_shutdown(struct device *dev)
{
	struct device *child;

	TAILQ_FOREACH(child, &dev->children, link) {
		device_shutdown(child);
	}

	return (0);
}

int
bus_generic_suspend(struct device *dev)
{
	int		error;
	struct device	*child, child2;

	TAILQ_FOREACH(child, &dev->children, link) {
		error = DEVICE_SUSPEND(child);
		if (error) {
			for (child2 = TAILQ_FIRST(&dev->children);
			     child2 && child2 != child;
			     child2 = TAILQ_NEXT(child2, link))
				DEVICE_RESUME(child2);
			return (error);
		}
	}
	return (0);
}

int
bus_generic_resume(struct device *dev)
{
	struct device	*child;

	TAILQ_FOREACH(child, &dev->children, link) {
		DEVICE_RESUME(child);
		/* if resume fails, there's nothing we can usefully do... */
	}
	return (0);
}

void
bus_generic_driver_added(struct device *dev, struct driver *driver)
{
	struct device *child;

	DEVICE_IDENTIFY(driver, dev);

	TAILQ_FOREACH(child, &dev->children, link)
	{
		if (child->state == DS_NOTPRESENT)
			device_probe_and_attach(child);
	}
}
#endif	/*******************************************************/
