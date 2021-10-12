/*
 ****************************************************************
 *								*
 *			hid.c					*
 *								*
 *	"Driver" para dispositivo USB				*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.5.0, de 01.01.04				*
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
#include "../usb/hid.h"
#include "../usb/usbhid.h"

#include "../h/extern.h"
#include "../h/proto.h"

#define MAXUSAGE	100

struct hid_data
{
	uchar			*start;
	uchar			*end;
	uchar			*p;
	struct hid_item		cur;
	long			usages[MAXUSAGE];
	int			nu;
	int			minset;
	int			multi;
	int			multimax;
	int			kindset;
};

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
hid_clear_local (struct hid_item *c)
{
	c->usage		= 0;
	c->usage_minimum	= 0;
	c->usage_maximum	= 0;
	c->designator_index	= 0;
	c->designator_minimum	= 0;
	c->designator_maximum	= 0;
	c->string_index		= 0;
	c->string_minimum	= 0;
	c->string_maximum	= 0;
	c->set_delimiter	= 0;

}	/* end hid_clear_local */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct hid_data *
hid_start_parse (void *d, int len, int kindset)
{
	struct hid_data		*s;

	s = malloc_byte (sizeof (*s));

	memclr (s, sizeof (*s));

	s->start	= s->p = d;
	s->end		= (uchar *)d + len;
	s->kindset	= kindset;

	return (s);

}	/* end hid_start_parse */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
hid_end_parse (struct hid_data *s)
{
	while (s->cur.next != NULL)
	{
		struct hid_item	*hi = s->cur.next->next;

		free_byte (s->cur.next);

		s->cur.next = hi;
	}

	free_byte (s);

}	/* end hid_end_parse */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
hid_get_item (struct hid_data *s, struct hid_item *h)
{
	struct hid_item		*c = &s->cur;
	unsigned int		bTag, bType, bSize;
	ulong			oldpos;
	uchar			*data;
	long			dval;
	uchar			*p;
	struct hid_item		*hi;
	int			i;

    top:
	if (s->multimax != 0)
	{
		if (s->multi < s->multimax)
		{
			c->usage = s->usages[MIN (s->multi, s->nu - 1)];
			s->multi++;
			*h = *c;
			c->loc.pos += c->loc.size;
			h->next = 0;
			return (1);
		}
		else
		{
			c->loc.count = s->multimax;
			s->multimax  = 0;
			s->nu        = 0;

			hid_clear_local (c);
		}
	}

	for (EVER)
	{
		p = s->p;

		if (p >= s->end)
			return (0);

		bSize = *p++;

		if (bSize == 0xfe)
		{
			/* long item */

			bSize  = *p++;
			bSize |= *p++ << 8;
			bTag   = *p++;
			data   = p;
			p     += bSize;
			bType  = 0xff; /* XXX what should it be */
		}
		else
		{
			/* short item */
			bTag   = bSize >> 4;
			bType  = (bSize >> 2) & 3;
			bSize &= 3;

			if (bSize == 3)
				bSize = 4;

			data   = p;
			p     += bSize;
		}

		s->p = p;

		switch (bSize)
		{
		    case 0:
			dval = 0;
			break;

		    case 1:
			dval = *data++;
			break;

		    case 2:
			dval = *data++;
			dval |= *data++ << 8;
			dval = dval;
			break;

		    case 4:
			dval = *data++;
			dval |= *data++ << 8;
			dval |= *data++ << 16;
			dval |= *data++ << 24;
			break;

		    default:
			printf("BAD LENGTH %d\n", bSize);
			continue;
		}
		
		switch (bType)
		{
		    case 0:			/* Main */
			switch (bTag)
			{
			    case 8:		/* Input */
				if (!(s->kindset & (1 << hid_input)))
					continue;
				c->kind = hid_input;
				c->flags = dval;

			    ret:
				if (c->flags & HIO_VARIABLE)
				{
					s->multimax  = c->loc.count;
					s->multi     = 0;
					c->loc.count = 1;

					if (s->minset)
					{
						for (i = c->usage_minimum; i <= c->usage_maximum; i++)
						{
							s->usages[s->nu] = i;

							if (s->nu < MAXUSAGE - 1)
								s->nu++;
						}

						s->minset = 0;
					}

					goto top;

				}
				else
				{
					*h          = *c;
					h->next     = 0;
					c->loc.pos += c->loc.size * c->loc.count;

					hid_clear_local (c);

					s->minset = 0;
					return (1);
				}

			    case 9:		/* Output */
				if (!(s->kindset & (1 << hid_output)))
					continue;

				c->kind = hid_output;
				c->flags = dval;
				goto ret;

			    case 10:	/* Collection */
				c->kind = hid_collection;
				c->collection = dval;
				c->collevel++;
				*h = *c;

				hid_clear_local (c);

				s->nu = 0;
				return (1);

			    case 11:	/* Feature */
				if ((s->kindset & (1 << hid_feature)) == 0)
					continue;

				c->kind = hid_feature;
				c->flags = dval;
				goto ret;

			    case 12:	/* End collection */
				c->kind = hid_endcollection;
				c->collevel--;
				*h = *c;

				hid_clear_local (c);

				s->nu = 0;
				return (1);

			    default:
				printf ("Main bTag=%d\n", bTag);
				break;
			}

			break;

		    case 1:		/* Global */
			switch (bTag)
			{
			    case 0:
				c->_usage_page = dval << 16;
				break;

			    case 1:
				c->logical_minimum = dval;
				break;

			    case 2:
				c->logical_maximum = dval;
				break;

			    case 3:
				c->physical_maximum = dval;
				break;

			    case 4:
				c->physical_maximum = dval;
				break;

			    case 5:
				c->unit_exponent = dval;
				break;

			    case 6:
				c->unit = dval;
				break;

			    case 7:
				c->loc.size = dval;
				break;

			    case 8:
				c->report_ID = dval;
				break;

			    case 9:
				c->loc.count = dval;
				break;

			    case 10: /* Push */
				hi = malloc_byte (sizeof (*hi));
				*hi = s->cur;
				c->next = hi;
				break;

			    case 11: /* Pop */
				hi = c->next;
				oldpos = c->loc.pos;
				s->cur = *hi;
				c->loc.pos = oldpos;
				free_byte (hi);
				break;

			    default:
				printf ("Global bTag=%d\n", bTag);
				break;
			}
			break;

		    case 2:		/* Local */
			switch (bTag)
			{
			    case 0:
				if (bSize == 1) 
					dval = c->_usage_page | (dval&0xff);
				else if (bSize == 2) 
					dval = c->_usage_page | (dval&0xffff);

				c->usage = dval;

				if (s->nu < MAXUSAGE)
					s->usages[s->nu++] = dval;
				/* else XXX */
				break;

			    case 1:
				s->minset = 1;

				if (bSize == 1) 
					dval = c->_usage_page | (dval&0xff);
				else if (bSize == 2) 
					dval = c->_usage_page | (dval&0xffff);

				c->usage_minimum = dval;
				break;

			    case 2:
				if (bSize == 1) 
					dval = c->_usage_page | (dval&0xff);
				else if (bSize == 2) 
					dval = c->_usage_page | (dval&0xffff);

				c->usage_maximum = dval;
				break;

			    case 3:
				c->designator_index = dval;
				break;

			    case 4:
				c->designator_minimum = dval;
				break;

			    case 5:
				c->designator_maximum = dval;
				break;

			    case 7:
				c->string_index = dval;
				break;

			    case 8:
				c->string_minimum = dval;
				break;

			    case 9:
				c->string_maximum = dval;
				break;

			    case 10:
				c->set_delimiter = dval;
				break;

			    default:
				printf("Local bTag=%d\n", bTag);
				break;
			}
			break;

		    default:
			printf("default bType=%d\n", bType);
			break;
		}
	}

}	/* end hid_get_item */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
hid_report_size (void *buf, int len, enum hid_kind k, uchar *idp)
{
	struct hid_data		*d;
	struct hid_item		h;
	int			size, id;

	id = 0;

	for (d = hid_start_parse (buf, len, 1 << k); hid_get_item (d, &h); /* vazio */)
	{
		if (h.report_ID != 0)
			id = h.report_ID;
	}

	hid_end_parse (d);

	size = h.loc.pos;

	if (id != 0)
	{
		size += 8;
		*idp = id;	/* XXX wrong */
	}
	else
	{
		*idp = 0;
	}

	return ((size + 7) / 8);

}	/* end hid_report_size */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
hid_locate (void *desc, int size, ulong usage, enum hid_kind k, struct hid_location *loc, ulong *flags)
{
	struct hid_data		*d;
	struct hid_item		h;

	for (d = hid_start_parse (desc, size, 1 << k); hid_get_item (d, &h); /* vazio */)
	{
		if (h.kind == k && !(h.flags & HIO_CONST) && h.usage == usage)
		{
			if (loc != NULL)
				*loc = h.loc;

			if (flags != NULL)
				*flags = h.flags;

			hid_end_parse (d);

			return (1);
		}
	}

	hid_end_parse (d);

	loc->size = 0;

	return (0);

}	/* end hid_locate */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
ulong
hid_get_data (uchar *buf, struct hid_location *loc)
{
	unsigned	hpos = loc->pos;
	unsigned	hsize = loc->size;
	ulong		data;
	int		i, s;

#ifdef	USB_MSG
	printf ("hid_get_data: loc %d/%d\n", hpos, hsize);
#endif	USB_MSG

	if (hsize == 0)
		return (0);

	data = 0;
	s    = hpos / 8; 

	for (i = hpos; i < hpos + hsize; i += 8)
		data |= buf[i / 8] << ((i / 8 - s) * 8);

	data >>= hpos % 8;
	data  &= (1 << hsize) - 1;
	hsize  = 32 - hsize;

	/* Sign extend */

	data = ((long)data << hsize) >> hsize;

#ifdef	USB_MSG
	printf ("hid_get_data: loc %d/%d = %u\n", loc->pos, loc->size, data);
#endif	USB_MSG

	return (data);

}	/* end hid_get_data */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
hid_is_collection (void *desc, int size, ulong usage)
{
	struct hid_data		*hd;
	struct hid_item		hi;
	int			err;

	if ((hd = hid_start_parse (desc, size, hid_input)) == NULL)
		return (0);

	err = hid_get_item (hd, &hi) && hi.kind == hid_collection && hi.usage == usage;

	hid_end_parse (hd);

	return (err);

}	/* end hid_is_collection */
