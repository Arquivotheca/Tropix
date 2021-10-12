/*
 ****************************************************************
 *								*
 *			parag.c					*
 *								*
 *	Trata os paragrafos.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		parag		iparag				*
 *		contacarac 					*
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
 *	Trata os paragrafos nao indentados.			*
 ****************************************************************
 */
void
parag (int cmd)
{
	/*
	 *	Pula uma linha no texto formatado.
	 */
	trtpag (1);

	/*
	 *	Atualiza indentacao.
	 */
	if (cmd == PP)
	{
		pp = 5;
	}
	else
	{
		if (pp > 0)
			pp = 0;
	}

	if (ip > 0)
		ip = 0;

	attamlin ();
	atnbrancos ();

}	/* end parag */

/*
 ****************************************************************
 *	Trata os paragrafos indentados.				*
 ****************************************************************
 */
void
iparag (int cmd, char *arg)
{
	char	*tit;
	char	*cp,
		*cpaux;
	int	i,
		tamtit;

	/*
	 *	Zera o paragrafo padrao.
	 */
	if (pp > 0)
		pp = 0;

	/*
	 *	Pula uma linha no texto formatado.
	 */
	if (cmd == IP)
		trtpag (1);

	if (arg == NOSTR)
	{
		/*
		 *	Não tem titulo.
		 */
		ip = 5;
		attamlin ();
		atnbrancos ();
		return;
	}

	ip = 0;
	atnbrancos ();
	attamlin ();

	/*
	 *	Le o titulo.
	 */
	tit = lepalvr (arg, &cp);

	/*
	 *	Procura a indentacao.
	 */
	while (ESPACO (*cp))
		cp++;

	if (*cp == NOCHAR)
		ip = 5;
	else
	{
		/*
		 *	Le a indentacao.
		 */
		arg = cp;
		while (*cp != NOCHAR  &&  !ESPACO (*cp))
		{
			if (*cp < '0'  ||  *cp > '9')
			{
				errcmd ();
				*cp = NOCHAR;
				break;
			}
			cp++;
		}
		if (*cp != NOCHAR)
		{
			cpaux = cp;
			while (ESPACO (*cpaux))
				cpaux++;

			if (*cpaux != NOCHAR)
				errcmd ();

			*cp = NOCHAR;
		}

		if (cp == arg)
			ip = 5;
		else
			ip = atoi (arg);
	}

	tamtit = contacarac (tit);

	if (ip < tamtit)
	{
		/*
		 *	Titulo maior que a indentacao.
		 */
		poepalvr (tit, PALAVRA);
		trtlin (QUEBRA);
		atnbrancos ();
		attamlin ();
		return;
	}

	/*
	 *	Indica paragrafo indentado com titulo.
	 */
	iptit = TRUE;

	/*
	 *	Completa o titulo com brancos.
	 */
	cp = strend (tit);
	for (i = tamtit; i < ip; i++)
		*cp++ = ' ';
	*cp = NOCHAR;

	/*
	 *	Coloca o titulo na linha.
	 */
	poepalvr (tit, PALAVRA);

}	/* end iparag */

/*
 ****************************************************************
 *	Conta o número de caracteres.				*
 ****************************************************************
 */
int
contacarac (char *cp)
{
	char		npag[6];
	register int	nc;

	nc = 0;
	while (*cp != NOCHAR)
	{
		switch (*cp)
		{
		    case '%':
			if (*(cp - 1) != '\\')
			{
				sprintf (npag, "%d", pagcorr);
				nc += strlen (npag);
			}
			break;

		    case '\b':
			nc--; 
			break;

		    default:
			nc++;
			break;
		}

		cp++;
	}

	return (nc);

}	/* end contacarac */
