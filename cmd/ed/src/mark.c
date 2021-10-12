/*
 ****************************************************************
 *								*
 *			src/mark.c				*
 *								*
 *	Marcação de posições					*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
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

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições deste módulo					*
 ****************************************************************
 */
static	BASE	*mark['z' - 'a' + 2];

#define	ID(c)	((c == ' ') ? 'z' - 'a' + 1 : c - 'a')

/*
 ****************************************************************
 *	Associa uma marca a uma base.				*
 ****************************************************************
 */
void	
mrk_define (int id, BASE *base)
{
	mark[ID(id)] = base;

}	/* end mrk_define */

/*
 ****************************************************************
 *	Informa base associada a uma marca.			*
 ****************************************************************
 */
BASE *
mrk_base (int id)
{
	BASE		*base, *b;
	
	if ((base = mark[ID(id)]) == NOBASE)
		return (NOBASE);

	/*
	 *	Procura base no Texto.
	 */
	b = Root;

	do
	{
		/*
		 *	Se base ainda esta no Texto, retorna base.
		 */
		if (b == base)
			return (base);
	}
	while ((b = b->pos) != Root);

	/*
	 *	Se base esta ausente do Texto,
	 *	desmarca base e retorna NOBASE.
	 */
	mark[ID(id)] = NOBASE;

	return (NOBASE);

}	/* end mrk_base */
