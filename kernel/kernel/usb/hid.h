/*
 ****************************************************************
 *								*
 *			hid.h					*
 *								*
 *	Definições acerca de "Human Interface Devices"		*
 *								*
 *	Versão	4.3.0, de 07.10.03				*
 *		4.5.0, de 01.01.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

enum hid_kind
{ 
	hid_input, hid_output, hid_feature, hid_collection, hid_endcollection
};

struct hid_location
{
	ulong		size;
	ulong		count;
	ulong		pos;
};

struct hid_item
{
	/* Global */

	long		_usage_page;
	long		logical_minimum;
	long		logical_maximum;
	long		physical_minimum;
	long		physical_maximum;
	long		unit_exponent;
	long		unit;
	long		report_ID;

	/* Local */

	long		usage;
	long		usage_minimum;
	long		usage_maximum;
	long		designator_index;
	long		designator_minimum;
	long		designator_maximum;
	long		string_index;
	long		string_minimum;
	long		string_maximum;
	long		set_delimiter;

	/* Misc */

	long		collection;
	int		collevel;
	enum hid_kind	kind;
	ulong		flags;

	/* Location */

	struct hid_location loc;

	/* */

	struct hid_item	*next;
};

#define HIO_VARIABLE	0x002

struct hid_data	*hid_start_parse (void *d, int len, int kindset);
void		hid_end_parse (struct hid_data *s);
int		hid_get_item (struct hid_data *s, struct hid_item *h);
int		hid_report_size (void *buf, int len, enum hid_kind k, uchar *id);
int		hid_locate (void *desc, int size, ulong usage, 
				enum hid_kind kind, struct hid_location *loc, ulong *flags);
ulong		hid_get_data (uchar *buf, struct hid_location *loc);
int		hid_is_collection (void *desc, int size, ulong usage);
