/*
 ****************************************************************
 *								*
 *			tblopcao.c				*
 *								*
 *	Trata as linhas de opção da tabela.			*
 *								*
 *	Versão	2.0.0, de 08.07.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtopcao					*
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
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Trata as opções.					*
 ****************************************************************
 */
void
trtopcao (void)
{
	char		buf[BLSZ];
	char		*cp;
	const char	*ptr;
	char		c;	
	BOOL		auxtab;
	BOOL		auxtit;

	strcpy (buf, bufin);
	if ((cp = strchr (buf, ';')) != NOSTR)
	{
		/*
		 *	Ultima linha de opcao.
		 */
		tbl = 3;
		*cp = NOCHAR;
	}

	cp = buf;
	while (!ESPACO (*cp)  &&  *cp != NOCHAR)
		cp++;
	c = *cp;
	*cp = NOCHAR;

	auxtab = FALSE;
	auxtit = FALSE;

	/*
	 *	Identifica a opcao.
	 */
	if (streq (buf, "center"))
		center = TRUE;

	else	if (streq (buf, "expand"))
			expand = TRUE;

	else	if (streq (buf, "box"))
			box = TRUE;

	else	if (streq (buf, "allbox"))
			allbox = TRUE;

	else	if (streq (buf, "doublebox"))
			doublebox = TRUE;

	else	if (streq (buf, "tab"))
			auxtab = TRUE;

	else	if (streq (buf, "title"))
			auxtit = TRUE;
		
	else
	{
		if (tbl == 1)
		{
			/*
			 *	Linha de formato.
			 */
			tbl = 3;
			trtfmt ();
		}
		else
			errcmd ();

		return;
	}

	if (tbl == 1)
		tbl = 2;

	if (auxtab)
	{
		/*
		 *	A opcao e' um 'tab'.
		 */
		if (!ESPACO (c)  ||  *++cp != '(')
		{
			errcmd ();
			return;
		}

		tab = *++cp;

		if (*++cp != ')')
		{
			errcmd ();
			return;
		}
		cp++;
	}
	else
	{
		if (auxtit)
		{
			if (tembloco  ||  !ESPACO (c))
			{
				errcmd ();
				return;
			}

			lintit = strtol (++cp, &ptr, 0) - 1;
			if (ptr == NOSTR) 
			{
				errcmd ();
				return;
			}

			cp = (char *)ptr;
		}
		else
			*cp = c;
	}

	while (ESPACO (*cp))
		cp++;

	if (*cp != NOCHAR)
	{
		errcmd ();
		return;
	}

}	/* end trtopcao */
