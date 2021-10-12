/*
 ****************************************************************
 *								*
 *			src/line.c				*
 *								*
 *	Edições em linhas de Texto				*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições deste módulo					*
 ****************************************************************
 */
#define	DEL	127
#define	DELCH	'?'

/*
 ****************************************************************
 *	Append							*
 ****************************************************************
 */
bool
append (BASE *anterior)
{
	BASE		*base;

	base = anterior;

	for (EVER)
	{
		fngets (temp_buffer, LSIZE, stdin);

		if (streq (temp_buffer, "."))
			break;

		if (!txt_insert (base, temp_buffer))
			return (false);

		base = (base != NOBASE) ? base->pos : Root;
	}

	return (true);

}	/* end append */

/*
 ****************************************************************
 *	Mostra uma linha.					*
 ****************************************************************
 */
void
ln_print (char *line)
{
	char		c;

	while ((c = *line++))
	{
		if (isgraph (c) || c == ' ' || c == '\t')
		{
			putchar (c);
		}
		elif (c == DEL)
		{
			putchar ('^');
			putchar (DELCH);
		}
		else
		{
			putchar ('^');
			putchar (c | 0100);
		}
	}

	putchar ('\n');

}	/* end ln_print */

/*
 ****************************************************************
 *	Lista uma linha.					*
 ****************************************************************
 */
void
ln_list (const char *line)
{
	char		c;
	int		col;

	col = 1;

	while ((c = *line++))
	{
		if (isgraph (c) || c == ' ')
		{
			putchar (c);
		}
		elif (c == '\t')
		{
			putchar ('>');
		}
		elif (c == DEL)
		{
			putchar ('^');
			putchar (DELCH);
			col++;
		}
		else
		{
			putchar ('^');
			putchar (c | 0100);
			col++;
		}

		col++;

		if (col >= 72)
		{
			putchar ('\\');
			putchar ('\n');
			col = 0;
		}
	}

	putchar ('$'); putchar ('\n');

}	/* end ln_list */
