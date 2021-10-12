/*
 ****************************************************************
 *								*
 *			trtcarac.c				*
 *								*
 *	Trata a mudanca de tipo de caracter.			*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtcarac					*
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
 *	Trata os comandos relativos ao tipo de caracter.	*
 ****************************************************************
 */
void
trtcarac (int cmd, char *arg)
{
	char	*cp,
		*palvr;
	TPCHAR	tpc;

	/*
	 *	Guarda tipo do caracter.
	 */
	tpc = tpchar;

	/*
	 *	Identifica o comando e muda o tipo de caracter.
	 */
	switch (cmd)
	{
	    case R:
		tpchar = ROMANO;
		break;

	    case I:
		tpchar = ITALICO;
		break;

	    case B:
		tpchar = NEGRITO;
		break;
	}

	if (arg == NOSTR)
		/*
		 *	Não ha' argumentos.
		 */
		return; 

	/*
	 *	Trata o argumento.
	 */
	palvr = lepalvr (arg, &cp);
	poepalvr (palvr, PALAVRA);

	/*
	 *	Retorna ao tipo de caracter inicial.
	 */
	tpchar = tpc;

	while (ESPACO (*cp))
		cp++;

	if (*cp != NOCHAR)
		errcmd ();

}	/* end trtcarac */
