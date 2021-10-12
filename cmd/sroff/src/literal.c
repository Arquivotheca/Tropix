/*
 ****************************************************************
 *								*
 *			literal.c				*
 *								*
 *	Trata as linhas que nao sao formatadas (no fill).	*
 *								*
 *	Versão	2.0.1, de 17.08.88				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		literal						*
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
 *	Trata as linhas dentro do comando literal (.nf ou .li).	*
 ****************************************************************
 */
void
literal (void)
{
	static	 int	vez = 0;
	static	 int	lsaux;
	char		*cp;

	if (bufin[0] == '.'  &&  bufin[1] == 'f'  &&  bufin[2] == 'i')
	{
		/*
		 *	Fim da nao formatacao.
		 */
		cp = bufin + 3;
		while (ESPACO (*cp))
			cp++;

		if (*cp != NOCHAR)
			errcmd ();

		if (vez == 1)
		{
			ls = lsaux;
			atnbrancos ();
			vez = 0;
			cptrab = buftrab;
		}
		lit = FALSE;
		return;
	}

	if (vez == 0)
	{
		lsaux = ls;
		ls = 1;
		nbrancos = po;
		vez = 1;
	}

	/*
	 *	Copia a linha para bfout1.
	 */
	cp = bufin;
	cptrab = bfout1;
	while (*cp != NOCHAR)
	{
		if (ESPACO (*cp))
			*cptrab++ = *cp++;
		else
			identcarac (&cp);
	}
	*cptrab = NOCHAR;

	if (tpchar == NEGRITO)
		out2 = bfout1;

	if (ul > 0)
		mtsub ();

	trtpag (0);

}	/* end literal */
