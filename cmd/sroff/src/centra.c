/*
 ****************************************************************
 *								*
 *			centra.c				*
 *								*
 *	Centra as linhas.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		centra	mtsub					*
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
 *	Trata as linhas a serem centradas.			*
 ****************************************************************
 */
void
centra (void)
{
	char		*cp;
	register int	nc;

	cp = bufin;
	cptrab = out1;
	nc = 0;

	/*
	 *	Copia a linha para bfout1.
	 */
	while (*cp != NOCHAR)
		nc += identcarac (&cp);

	if (nc == 0)
		return;

	*cptrab = NOCHAR;

	/*
	 *	Atualiza cptrab.
	 */
	cptrab = buftrab;

	/*
	 *	Calcula a indentacao.	
	 */
	if (nc < tamlin)
		nbrancos += ((tamlin - nc) / 2);

	if (tpchar == NEGRITO)
		out2 = bfout1;

	if (ul > 0)
		mtsub ();

	trtpag (0);
	atnbrancos ();
	ce--;

}	/* end centra */	

/*
 ****************************************************************
 *	Monta uma linha com '_' para sublinhar.			*
 ****************************************************************
 */
void
mtsub (void)
{
	register char	*cp,
			*cp3,
			*cpaux3 = NOSTR;

	cp3 = bfout3;
	cp = out1;

	/*
	 *	Coloca '_' nas posicoes que nao contem espaco.
	 */
	while (*cp != NOCHAR)
	{
		if (ESPACO (*cp))
			*cp3++ = *cp;
		else
		{
			*cp3++ = '_';
			cpaux3 = cp3;
		}

		cp++;
	}
	*cpaux3 = NOCHAR;

	out3 = bfout3;

}	/* end mtsub */
