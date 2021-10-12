/*
 ****************************************************************
 *								*
 *			secnum.c				*
 *								*
 *	Trata numeracao de secoes.				*
 *								*
 *	Versão	2.0.0, de 03.06.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		secnum						*
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
 *	Trata o comando.					*
 ****************************************************************
 */
void
secnum (char *arg)
{
	char		c;
	char		*cp;
	char		*tit;
	int		nv;
	int		nvaux[4];
	register int	i;
	register char	*cpout;

	if (arg == NOSTR)
	{
		/*
		 *	Não tem argumentos,
		 *	inicializa numeracao.
		 */
		for (i = 1; i <= ultnivel; i++)
			nivel[i] = 0;
		ultnivel = 0;
		return;
	}

	cp = arg + 1;
	if (*arg < '1'  ||  *arg > '3'  ||  (!ESPACO (*cp)  &&  *cp != NOCHAR))
	{
		errcmd ();
		return;
	}

	c = *cp;
	*cp = NOCHAR;
	nv = atoi (arg);
	*cp = c;

	while (ESPACO (*cp)  &&  *cp != NOCHAR)
		cp++;

	if (*cp == NOCHAR)
		tit = NOSTR;
	else
	{
		/*
		 *	Le o titulo.
		 */
		tit = lepalvr (cp, &cp);

		if (*cp != NOCHAR)
		{
			/*
			 *	Identifica numeracao absoluta.
			 */
			while (ESPACO (*cp))
				cp++;

			for (i = 1; i <= nv; i++)
				nvaux[i] = 0;

			i = 0;
			while (*cp != NOCHAR)
			{
				if (i == nv)
				{
					errcmd ();
					return;
				}

				arg = cp;

				while (!ESPACO (*cp)  &&  *cp != NOCHAR)
				{
					if (*cp < '0'  ||  *cp > '9')
					{
						errcmd ();
						return;
					}
					cp++;
				}

				c = *cp;
				*cp = NOCHAR;
				nvaux[++i] = atoi (arg);
				*cp = c;

				while (ESPACO (*cp))
					cp++;
			}
	

			if (i != nv)
			{
				errcmd ();
				return;
			}

			for (i = 1; i <= nv; i++)
				nivel[i] = nvaux[i];
			ultnivel = nv;
			nv = 0;
		}
	}

	if (nv != 0)
	{
		/*
		 *	Atualiza numeracao.
		 */
		if (nv > ultnivel)
		{
			for (i = ultnivel + 1; i <= nv; i++)
				nivel[i]++;
			ultnivel = nv;
		}
		else
		{
			if (nv == ultnivel)
				nivel[nv]++;
			else
			{
				nivel[nv]++;

				for (i = nv + 1; i <= ultnivel; i++)
				{
					if (nivel[i] != 0)
						nivel[i] = 0;
				}
				ultnivel = nv;
			}
		}
	}

	if (indentauto != 0)
		/*
		 *	Atualiza indentacao.
		 */
		nbrancos += (indentauto * (ultnivel - 1));

	cpout = bfout1;
	/*
	 *	Coloca a numeracao.
	 */
	for (i = 1; i <= ultnivel; i++)
	{
		sprintf (cpout, "%d.", nivel[i]);
		cpout = strchr (cpout, '.') + 1;
	}

	*--cpout = ' ';
	cpout++;

	if (tit != NOSTR)
		/*
		 *	Coloca o titulo.
		 */
		strcpy (cpout, tit);
	else
		*cpout = NOCHAR;

	if (tpchar == NEGRITO)
		out2 = bfout1;

	if (ul > 0)
		mtsub ();

	trtpag (0);

	atnbrancos ();

}	/* end secnum */

		

