/*
 ****************************************************************
 *								*
 *			citacao.c				*
 *								*
 *	Trata os comandos de inicio e fim de citacao. 		*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		iniciocit	fimcit				*
 *								*
 *	Modulo: SROFF						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/sroff.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Inicia a citacao.					*
 ****************************************************************
 */
void
iniciocit (void)
{
	cit = 5;
	attamlin ();
	atnbrancos ();
	lscit = ls;
	ls = 1;

}	/* end iniciocit */

/*
 ****************************************************************
 *	Termina a citacao.					*
 ****************************************************************
 */
void
fimcit (void)
{
	cit = 0;
	attamlin ();
	atnbrancos ();
	ls = lscit;

}	/* end iniciocit */
