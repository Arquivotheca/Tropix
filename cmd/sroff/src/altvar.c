/*
 ****************************************************************
 *								*
 *			altvar.c				*
 *								*
 *	Trata comandos relativos a alteracao de variaveis.	*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		altvar						*
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
 *	Trata os comandos de alteracao de constantes.		*
 ****************************************************************
 */
void
altvar (int cmd, char *arg)
{
	register char	*cp,
			*cpaux;
	register int	sp,
			lnaux,
			lsaux;

	if (arg == NOSTR)
	{
		/*
		 *	Não ha' argumento, a variavel recebe seu valor default.
		 */
		switch (cmd)
		{
		    case PL:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			pl = 75;
			attampag ();
			break;

		    case LN:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			ln = 6;
			break;

		    case M1:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m1 = 4;
			attampag ();
			break;

		    case M2:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m2 = 2;
			attampag ();
			break;

		    case M3:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m3 = 2;
			attampag ();
			break;

		    case M4:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m4 = 1;
			attampag ();
			break;

		    case LS:
			ls = 1;
			break;

		    case LL:
			ll = 67;
			attamlin ();
			break;

		    case PO:
			po = 0;
			atnbrancos ();
			break;

		    case IN:
			in = 0;
			atnbrancos ();
			attamlin ();
			break;

		    case BP:
			if (linhaescrita != 0)
				trtpag (-1);
			break;

		    case PN:
			pagcorr = 1;
			break;

		    case CE:
			ce = 1;
			break;

		    case UL:
			ul = 1;
			break;

		    case SP:
			lsaux = ls;
			ls = 1;
			trtpag (1);
			ls = lsaux;
			break;

		    case NR:
			indentauto = 5;
			break;
		}
		return;
	}

	/*
	 *	Le o argumento.
	 */
	cp = arg;
	if (*cp == '+'  ||  *cp == '-')
		cp++;

	while (*cp != NOCHAR  &&  !ESPACO (*cp))
	{
		if (*cp < '0'  ||  *cp > '9')
		{
			errcmd ();
			return;
		}
		cp++;
	}

	/*
	 *	Le a linha ate' o fim.
	 */
	if (*cp != NOCHAR)
	{
		cpaux = cp;
		while (ESPACO (*cpaux))
			cpaux++;

		if (*cpaux != NOCHAR)
		{
			errcmd ();
			return;
		}

		*cp = NOCHAR;
	}

	if (*arg == '+'  ||  *arg == '-')
	{
		/*
		 *	O argumento e' relativo.
		 */
		switch (cmd)
		{
		    case PL:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			pl += atoi (arg);
			attampag ();
			break;

		    case LN:
			errcmd ();
			break;

		    case M1:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m1 += atoi (arg);
			attampag ();
			break;

		    case M2:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m2 += atoi (arg);
			attampag ();
			break;

		    case M3:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m3 += atoi (arg);
			attampag ();
			break;

		    case M4:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m4 += atoi (arg);
			attampag ();
			break;

		    case LS:
			ls += atoi (arg);
			if (ls <= 0)
			{
				errcmd ();
				ls = 1;
			}
			break;

		    case LL:
			ll += atoi (arg);
			attamlin ();
			break;

		    case PO:
			po += atoi (arg);
			if (po < 0)
			{
				errcmd ();
				po = 0;
			}
			atnbrancos ();
			break;

		    case IN:
			in += atoi (arg);
			atnbrancos ();
			attamlin ();
			break;

		    case BP:
			if (linhaescrita != 0)
				trtpag (-1);
			pagcorr += (atoi (arg) - 1);
			if (pagcorr <= 0)
			{
				errcmd ();
				pagcorr = 1;
			}
			break;

		    case PN:
			pagcorr += atoi (arg);
			if (pagcorr <= 0)
			{
				errcmd ();
				pagcorr = 1;
			}
			break;

		    case CE:
			ce += atoi (arg);
			if (ce < 0)
			{
				errcmd ();
				ce = 0;
			}
			break;

		    case UL:
			ul += atoi (arg);
			if (ul < 0)
			{
				errcmd ();
				ul = 0;
			}
			break;

		    case SP:
			errcmd ();
			break;

		    case NR:
			indentauto += atoi (arg);
			if (indentauto < 0)
			{
				errcmd ();
				indentauto = 0;
			}
			break;
		}
	}
	else
	{
		/*
		 *	O argumento e' absoluto.
		 */
		switch (cmd)
		{
		    case PL:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			pl = atoi (arg);
			attampag ();
			break;

		    case LN:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);

			lnaux = atoi (arg);
			if (lnaux != 6  &&  lnaux != 8)
			{
				errcmd ();
				return;
			}
			ln = lnaux;
			break;

		    case M1:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m1 = atoi (arg);
			attampag ();
			break;

		    case M2:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m2 = atoi (arg);
			attampag ();
			break;

		    case M3:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m3 = atoi (arg);
			attampag ();
			break;

		    case M4:
			if (blflut != blrod)
			{
				errcmd ();
				return;
			}

			if (linhaescrita != 0)
				trtpag (-1);
			m4 = atoi (arg);
			attampag ();
			break;

		    case LS:
			ls = atoi (arg);
			if (ls <= 0)
			{
				errcmd ();
				ls = 1;
			}
			break;

		    case LL:
			ll = atoi (arg);
			attamlin ();
			break;

		    case PO:
			po = atoi (arg);
			if (po < 0)
			{
				errcmd ();
				po = 0;
			}
			atnbrancos ();
			break;

		    case IN:
			in = atoi (arg);
			attamlin ();
			atnbrancos ();
			break;

		    case BP:
			if (linhaescrita != 0)
				trtpag (-1);
			pagcorr = atoi (arg);
			if (pagcorr <= 0)
			{
				errcmd ();
				pagcorr = 1;
			}
			break;

		    case PN:
			pagcorr = atoi (arg);
			if (pagcorr <= 0)
			{
				errcmd ();
				pagcorr = 1;
			}
			break;

		    case CE:
			ce = atoi (arg);
			if (ce < 0)
			{
				errcmd ();
				ce = 0;
			}
			break;

		    case UL:
			ul = atoi (arg);
			if (ul < 0)
			{
				errcmd ();
				ul = 0;
			}
			break;

		    case SP:
			lsaux = ls;
			ls = 1;
			if ((sp = atoi (arg)) <= 0)
				errcmd ();
			else
				trtpag (sp);
			ls = lsaux;
			break;

		    case NR:
			indentauto = atoi (arg);
			if (indentauto < 0)
			{
				errcmd ();
				indentauto = 0;
			}
			break;
		}
	}

}	/* end altvar */
