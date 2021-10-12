/*
 ****************************************************************
 *								*
 *			lista.c					*
 *								*
 *	Trata as linhas dentro de uma lista.			*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		lista						*
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
 *	Trata as linhas dentro do comando lista (.(l).		*
 ****************************************************************
 */
void
lista (void)
{
	static	 int	vez = 0;
	static	 char	*cpout;
	static	 int	lsaux;
	char		*cp;

	if (bufin[0] == '.'  &&  bufin[1] == ')'  &&  bufin[2] == 'l')
	{
		/*
		 *	Fim da lista.
		 */
		cp = bufin + 3;
		while (ESPACO (*cp))
			cp++;

		if (*cp != NOCHAR)
			errcmd ();

		if (vez == 1)
		{
			atnbrancos ();
			vez = 0;
			cptrab = buftrab;
			ls = lsaux;
		}
		lst = FALSE;
		return;
	}

	if (vez == 0)
	{
		nbrancos = po;
		lsaux = ls;
		ls = 1;
		vez = 1;
		/*
		 *	Coloca 5 brancos em bfout1.
		 */
		cpout = poebr (5, bfout1);
	}

	/*
	 *	Copia a linha para bfout1.
	 */
	cp = bufin;
	cptrab = cpout;
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

}	/* end lista */
