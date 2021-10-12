/*
 ****************************************************************
 *								*
 *			poepalvr.c				*
 *								*
 *	Trata as palavras.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		poepalvr	identcarac			*
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
 *	Prepara a linha para formatacao.			*
 ****************************************************************
 */
char *
poepalvr (char *palvr, int opt)
{
	char		*cp;
	register int	nc,
			cont;

	/*
	 *	O proximo ponteiro de buflin aponta para a palavra.
	 */
	buflin[contpalvr].p_palavra = cptrab;

	if (tpchar == NEGRITO)
	{
		buflin[contpalvr].p_negrito = TRUE;
		rpt2++;
	}
	else
		buflin[contpalvr].p_negrito = FALSE;

	if (tpchar == ITALICO)
		buflin[contpalvr].p_italico = TRUE;
	else
		buflin[contpalvr].p_italico = FALSE;
	
	if (ul > 0)
	{
		buflin[contpalvr].p_sublinhado = TRUE;
		rpt3++;
	}
	else
		buflin[contpalvr].p_sublinhado = FALSE;

	cp = palvr;
	nc = 0;

	/*
	 *	Coloca a palavra em buftrab.
	 */
	if (opt == CADEIA)
	{
		while (*cp != NOCHAR  &&  !ESPACO (*cp))
			nc += identcarac (&cp);
	}
	else	/* opt == PALAVRA */
	{
		while (*cp != NOCHAR)
			nc += identcarac (&cp);
	}
	*cptrab++ = NOCHAR;

	/*
	 *	Verifica fim de linha.
	 */
	cont = contcarac + nc;
	if (cont < tamlin)
	{
		/*
		 *	Meio da linha.
		 */
		contcarac += (nc + ((iptit  &&  contpalvr == 0) ? 0 : 1));
		contpalvr++;
	}
	else
	{
		if (cont == tamlin  ||  contpalvr == 0)
		{
			/*
			 *	Palavra maior que a linha, ou
			 *	nao precisa formatar.
			 */
			contpalvr++;
			trtlin (QUEBRA);
		}
		else
		{
			/*
			 *	Ultima palavra nao cabe na linha.
			 */
			if (tpchar == NEGRITO)
				rpt2--;

			if (ul > 0)
				rpt3--;

			trtlin (FORMATA);
			strcpy (buftrab, buflin[contpalvr].p_palavra);
			if (buflin[0].p_negrito = buflin[contpalvr].p_negrito)
				rpt2++;

			if (buflin[0].p_sublinhado =
					buflin[contpalvr].p_sublinhado)
				rpt3++;

			cptrab = strend (buftrab) + 1;
			contcarac = nc + 1;
			contpalvr = 1;
		}
	}
	return (cp);

}	/* end poepalvr */

/*
 ****************************************************************
 *	Identifica o caracter a ser colocado em buftrab.	*
 ****************************************************************
 */
int
identcarac (char **cpp)
{
	register char	*cp, *cpaux;
	register int	nc;
	char		npag[20];

	nc = 0;
	cp = *cpp;

	switch (*cp)
	{
	    case '\\':
		if (*(cp + 1) == '%')
		{
			*cptrab++ = *++cp;
			cp++;
		}
		else
			*cptrab++ = *cp++;

		nc++;
		break;

	    case '%':
		sprintf (npag, "%d", pagcorr);
		cpaux = npag;
		while (*cpaux != NOCHAR)
		{
			*cptrab++ = *cpaux++;
			nc++;
		}
		cp++;
		break;

	    case '\b':
		*cptrab++ = *cp++;
		nc--;
		break;

	    case '\f':
	    case '\t':
	    case ' ':
		*cptrab++ = ' ';
		cp++;
		nc++;
		break;

	    default:
		*cptrab++ = *cp++;
		nc++;
		break;
	}
	*cpp = cp;

	return (nc);

}	/* end identcarac */
