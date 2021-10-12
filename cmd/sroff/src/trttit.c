/*
 ****************************************************************
 *								*
 *			trttit.c				*
 *								*
 *	Trata comandos de titulos.				*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trttit		codiftit			*
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
 *	Trata os comandos relativos a titulos.			*
 ****************************************************************
 */
void
trttit (int cmd, char *arg)
{
	/*
	 *	Identifica o titulo.
	 */
	switch (cmd)
	{
	    case HE:
		if (codiftit (arg, eh, oh))
		{
			he = TRUE;
			eh->t_temtit = oh->t_temtit = TRUE;
			attampag ();
		}
		break;

	    case EH:
		if (codiftit (arg, eh, (TITULO *)NULL))
		{
			he = TRUE;
			eh->t_temtit = TRUE;
			attampag ();
		}
		break;

	    case OH:
		if (codiftit (arg, oh, (TITULO *)NULL))
		{
			he = TRUE;
			oh->t_temtit = TRUE;
			attampag ();
		}
		break;

	    case FO:
		if (codiftit (arg, ef, of))
		{
			fo = TRUE;
			ef->t_temtit = of->t_temtit = TRUE;
			attampag ();
		}
		break;

	    case EF:
		if (codiftit (arg, ef, (TITULO *)NULL))
		{
			fo = TRUE;
			ef->t_temtit = TRUE;
			attampag ();
		}
		break;

	    case OF:
		if (codiftit (arg, of, (TITULO *)NULL))
		{
			fo = TRUE;
			of->t_temtit = TRUE;
			attampag ();
		}
		break;
	}

}	/* end trttit */

/*
 ****************************************************************
 *	Processa o titulo.					*
 ****************************************************************
 */
BOOL
codiftit (char *arg, TITULO *tit1, TITULO *tit2)
{
	register int	cont,
			i;
	char		*pont[3];

	/*
	 *	Copia o argumento para buftrab.
	 */
	strcpy (buftrab, arg);

	for (i = 0; i < 3; i++)
		pont[i] = NOSTR;

	/*
	 *	Identifica os elementos do titulo.
	 */
	cont = 0;
	while (cont < 3)
	{
		if (*++cptrab == '\'')
		{
			cont++;
			continue;
		}
		pont[cont] = cptrab;

		if ((cptrab = strchr (cptrab, '\'')) == NOSTR)
		{
			errcmd ();
			cptrab = buftrab;
			return (FALSE);
		}
		*cptrab = NOCHAR;

		cont++;
	}

	/*
	 *	Verifica erro de sintaxe no comando.
	 */ 
	if (strchr (++cptrab, '\'') != NOSTR)
	{
		errcmd ();
		cptrab = buftrab;
		return (FALSE);
	}

	while (ESPACO (*cptrab))
		cptrab++;

	if (*cptrab != NOCHAR)
	{
		errcmd ();
		cptrab = buftrab;
		return (FALSE);
	}

	/*
	 *	Preenche estrutura de titulo.
	 */
	while (cont-- > 0)
	{
		if (tit1->t_pont[cont] != NOSTR)
			free (tit1->t_pont[cont]);

		if (pont[cont] != NOSTR)
		{
			if ((tit1->t_pont[cont] =
				malloc (strlen (pont[cont]) + 1)) == NOSTR)
			{
				fprintf (stderr, no_mem_text, pgname);
				cptrab = buftrab;
				return (FALSE);
			}
			strcpy (tit1->t_pont[cont], pont[cont]);
		}
		else
			tit1->t_pont[cont] = NOSTR;
	}

	tit1->t_negrito = (tpchar == NEGRITO) ? TRUE : FALSE;
	tit1->t_italico = (tpchar == ITALICO) ? TRUE : FALSE;
	tit1->t_sublinhado = (ul > 0) ? TRUE : FALSE;

	if (tit2 != (TITULO *)NULL)
	{
		/*
		 *	Preenche segunda estrutura de titulo.
		 */
		while (++cont < 3)
		{
			if (tit2->t_pont[cont] != NOSTR)
				free (tit2->t_pont[cont]);

			if (pont[cont] != NOSTR)
			{
				if ((tit2->t_pont[cont] = malloc (strlen
						(pont[cont]) + 1)) == NOSTR)
				{
					fprintf (stderr, no_mem_text, pgname);
					cptrab = buftrab;
					return (FALSE);
				}
				strcpy (tit2->t_pont[cont], pont[cont]);
			}
			else
				tit2->t_pont[cont] = NOSTR;
		}

		tit2->t_negrito = tit1->t_negrito;
		tit2->t_sublinhado = tit1->t_sublinhado;
	}

	if (linhaescrita != 0)
	{
		if ((tit1 == eh  ||  tit1 == oh)  &&  !he)
			trtpag (-1);
		else
		{
			if ((tit1 == ef  ||  tit1 == of)  &&  !fo)
				trtpag (-1);
		}
	}

	cptrab = buftrab;
	return (TRUE);

}	/* end codiftit */
