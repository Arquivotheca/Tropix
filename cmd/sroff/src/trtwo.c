/*
 ****************************************************************
 *								*
 *			trtwo.c					*
 *								*
 *	Trata comando "wo".					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtwo	lepalvr					*
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
 *	Trata o comando word (.wo).				*
 ****************************************************************
 */
void
trtwo (char *arg)
{
	char	*cp,
		*palvr;

	/*
	 *	Le a palavra.
	 */
	palvr = lepalvr (arg, &cp);

	/*
	 *	Coloca a palavra na linha.
	 */
	poepalvr (palvr, PALAVRA);

	while (ESPACO (*cp))
		cp++;

	if (*cp != NOCHAR)
		errcmd ();

}	/* end trtwo */

/*
 ****************************************************************
 *	Le uma palavra.						*
 ****************************************************************
 */
char *
lepalvr (char *arg, char **cpp)
{
	register char	*cp;
	register char	c;
	static	 char	palvr[BLSZ];

	cp = palvr;
	if ((c = *arg) != '"'  &&  c != '\'')
	{
		while (!ESPACO (*arg)  &&  *arg != NOCHAR)
			*cp++ = *arg++;
	}
	else
	{
		/*
		 *	A palavra esta' entre aspas ou plics.
		 */
		while (*++arg != c  &&  *arg != NOCHAR)
		{
			if (*arg == '\\'  &&  *(arg + 1) == c)
				*cp++ = *++arg;
			else
				*cp++ = *arg;
		}
		if (*arg == c)
			arg++;
		else
			errcmd ();
	}

	*cp = NOCHAR;
	*cpp = arg;
	return (palvr);

}	/* end lepalvr */	
