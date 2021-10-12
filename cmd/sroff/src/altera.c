/*
 ****************************************************************
 *								*
 *			altera.c				*
 *								*
 *	Atualiza valores de variaveis do sistema.		*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		attamlin	attampag			*
 *		atnbrancos	atlinflut			*
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
 *	Altera o numero de caracteres da linha.			*
 ****************************************************************
 */
void
attamlin (void)
{
	tamlin = ll - in - pp - ip - (2 * cit);

	if (tamlin <= 0)
	{
		fprintf
		(	stderr,
			"%s: Indentação ocupa toda a linha (linha %d)\n%s\n",
			pgname, linhalida, bufin
		);
	}

}	/* end attamlin */

/*
 ****************************************************************
 *	Altera o numero de linhas de texto na pagina.		*
 ****************************************************************
 */
void
attampag (void)
{
	register int	n1, n2;

	if (tp)
		n1 = n2 = 0;
	else
	{
		n1 = he ? (m2 + 1) : 0;
		n2 = fo ? (m3 + 1) : 0;
	}

	tampag = pl - m1 - m4 - n1 - n2 - linrod;

	if ((tampag + linrod) < 10)
	{
		fprintf (stderr, "O tamanho da página está muito pequeno\n");
		errcmd ();
		exit (0);
	}

}	/* end attampag */ 

/*
 ****************************************************************
 *	Altera o numero de brancos da indentacao.		*
 ****************************************************************
 */
void
atnbrancos (void)
{
	nbrancos = po + in + ip + pp + cit;

	if (nbrancos < 0)
	{
		errcmd ();
		nbrancos = 0;
	}

}	/* end atnbrancos */

/*
 ****************************************************************
 *	Altera o numero de linhas flutuantes na pagina.		*
 ****************************************************************
 */
void
atlinflut (void)
{
	register BLOCO	*blp;

	linflut = 0;
	blp = blflut;
	while (blp != (BLOCO *)NULL)
	{
		if (blp->b_nlinhas < (tampag - linhaescrita))
		{
			tampag -= blp->b_nlinhas;
			linflut += blp->b_nlinhas;
		}
		else
			if (blp->b_nlinhas == (tampag - linhaescrita))
			{
				linflut += blp->b_nlinhas;
				if (linhaescrita == 0)
					inicpag ();
				fimpag ();
				return;
			}

		blp = blp->b_blcprox;
	}

}	/* end atlinflut */
