/*
 ****************************************************************
 *								*
 *			trtlin.c				*
 *								*
 *	Formata uma linha.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtlin	preenche				*
 *		poebr						*
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
#include <string.h>

#include "../h/sroff.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis globais					*
 ****************************************************************
 */
entry int	tam, np, nb, nb1, nb2, ne, ne1, ne2;

/*
 ****************************************************************
 *	Passa a linha formatada para bfout1 (2, 3).		*
 ****************************************************************
 */
void
trtlin (int opt)
{
	register char	*cp1,
			*cp2,
			*cp3;
	register int	nc;
	char		*cpaux2 = NOSTR, *cpaux3 = NOSTR;

	if (contpalvr <= 0)
		return;

	np = 0;
	cp1 = bfout1;
	cp2 = bfout2;
	cp3 = bfout3;

	if (rpt2 >= contpalvr)
	{
		/*
		 *	A linha toda e' em negrito.
		 */
		rpt2 = 0;
		out2 = bfout1;
	}
	else
	{
		if (rpt2 > 0)
			/*
			 *	Tem palavra em negrito.
			 */
			out2 = bfout2;
	}

	if (rpt3 > 0)
		/*
		 *	Tem palavra sublinhada.
		 */
		out3 = bfout3;

	if (opt == QUEBRA)
	{
		ne1 = ne2 = 0;
	}
	else
	{
		/*
		 *	Calcula a formatacao.
		 */
		nb = tamlin - (contcarac - 1);
		ne = contpalvr - 1 - (iptit ? 1 : 0);

		if (ne > 0)
		{
			if (distb == INICIO)
			{
				distb = FIM;
				nb1 = nb / ne;
				if ((ne2 = nb % ne) != 0)
					nb2 = nb1 + 1;
				ne1 = ne - ne2;
			}
			else
			{
				distb = INICIO;
				nb2 = nb / ne;
				if ((ne1 = nb % ne) != 0)
					nb1 = nb2 + 1;
				ne2 = ne - ne1;
			}
		}
		else
			ne1 = ne2 = 0;
	}

	while (np < contpalvr)
	{
		/*
		 *	Monta a linha.
		 */
		cp1 = preenche (cp1);

		if (rpt2 > 0)
		{
			if (buflin[np].p_negrito)
			{
				strcpy (cp2, buflin[np].p_palavra);
				cp2 += tam;
				cpaux2 = cp2;
				cp2 = poebr (nb, cp2);
			}
			else
				cp2 = poebr (tam + nb, cp2);
		}

		if (rpt3 > 0)
		{

			if (buflin[np].p_sublinhado)
			{
				nc = tam;
				while (nc-- > 0)
					*cp3++ = '_';
				cpaux3 = cp3;
				cp3 = poebr (nb, cp3);
			}
			else
				cp3 = poebr (tam + nb, cp3);
		}

		np++;
	}
	*cp1 = NOCHAR;

	if (rpt2 > 0)
		*cpaux2 = NOCHAR;

	if (rpt3 > 0)
		*cpaux3 = NOCHAR;

	trtpag (0);

	if (opt == QUEBRA)
	{
		/*
		 *	Reinicializa as variaveis.
		 */
		contpalvr = contcarac = 0;
		cptrab = buftrab;
	}

}	/* end trtlin */

/*
 ****************************************************************
 *	Arruma as palavras em bfout1.				*
 ****************************************************************
 */
char *
preenche (char *cp)
{
	if (laser  &&  buflin[np].p_italico)
	{
		strcpy (cp, LITON);
		cp = strend (cp);
	}
	strcpy (cp, buflin[np].p_palavra);
	tam = strlen (buflin[np].p_palavra);
	cp = strend (cp);
	if (laser  &&  buflin[np].p_italico)
	{
		strcpy (cp, LITOFF);
		cp = strend (cp);
	}

	if (!(iptit  &&  np == 0))
	{
		if (ne1)
		{
			cp = poebr (nb1, cp);
			ne1--;
			nb = nb1 + 1;
		}
		else
		{
			if (ne2)
			{
				cp = poebr (nb2, cp);
				ne2--;
				nb = nb2 + 1;
			}
			else
			{
				if ((np + 1) < contpalvr)
					nb = 1;
			}
		}
		if ((np + 1) < contpalvr)
			*cp++ = ' ';
	}
	else
	{
		nb = 0;
	}

	return (cp);

}	/* end preenche */

/*
 ****************************************************************
 *	Coloca brancos na regiao indicada.			*
 ****************************************************************
 */
char *
poebr (int n, char *cp)
{
	while (n-- > 0)
		*cp++ = ' ';

	return (cp);

}	/* end poebr */

