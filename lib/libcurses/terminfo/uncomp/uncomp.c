/*
 ****************************************************************
 *								*
 *			uncomp.c				*
 *								*
 *	Descompilação de cadeias de parâmetros			*
 *								*
 *	Versão	2.0.0, de 07.05.87				*
 *		3.0.0, de 24.02.95				*
 *								*
 *	Modulo: parm						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

#include "../h/defs.h"
#include "../h/parm.h"

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
static char	*endcp;

/*
 ******	Protótipos de funções ***********************************
 */
static char	*putnumber (int, char *);
static void	putdigit (int);

/*
 ****************************************************************
 *	Descompilação de cadeias de parâmetros			*
 ****************************************************************
 */
char *
parmuncomp (const char *target)
{
	register char		*sp;	/* Source code pointer */
	register const char	*tp;	/* Target code pointer */
	register char		*source;

	/*
	 *	Inicializações
	 */
	if (_parmrtn == NOSTR && (_parmrtn = malloc (PARMSZ)) == NOSTR)
		return (NOSTR);

	source = _parmrtn; _parmerror = "";

	/*
	 *	Loop Principal
	 */
	for (tp = target, sp = source; *tp != T_END; tp++)
	{
		if (*tp == T_NORMAL)
		{
			*sp++ = *++tp;
		}
		elif (*tp == T_NULL)
		{
			*sp++ = 0x80;
		}
		else
		{
			/*
			 *	Escreve Caractere Especial
			 */
			*sp++ = S_SPECIAL;

			/*
			 *	Escreve Complemento
			 */
			switch (*tp)
			{
			    case T_SPECIAL:
				*sp++ = S_SPECIAL;
				break;

				/*
				 *	Saída Formatada
				 */
			    case TO_CHAR:		/* Caractere */
				*sp++ = SO_CHAR;
				break;

			    case TO_DEC:		/* Decimal */
				*sp++ = SO_DEC;
				break;

			    case TO_OCTAL:		/* Octal */
				*sp++ = SO_OCTAL;
				break;

			    case TO_HEX:		/* Hexa c/ Minúsculas */
				*sp++ = SO_HEX;
				break;

			    case TO_CHEX:		/* Hexa c/ Maiúsculas */
				*sp++ = SO_CHEX;
				break;

				/*
				 *	Push
				 */
			    case TP_PARM:		/* Parâmetro */
				*sp++ = SP_PARM;
				*sp++ = *++tp + '0';
				break;

			    case TP_CHAR:		/* Const. Caractere */
				*sp++ = SP_CHAR1;
				*sp++ = *++tp;
				*sp++ = SP_CHAR2;
				break;

			    case TP_DEC:		/* Const. Decimal */
				*sp++ = SP_DEC1;
				sp = putnumber (*++tp, sp);
				*sp++ = SP_DEC2;
				break;

				/*
				 *	Aritmética
				 */
			    case TA_ADD:
				*sp++ = SA_ADD;
				break;

			    case TA_MUL:
				*sp++ = SA_MUL;
				break;

			    case TA_SUB:
				*sp++ = SA_SUB;
				break;

			    case TA_DIV:
				*sp++ = SA_DIV;
				break;

			    case TA_MOD:
				*sp++ = SA_MOD;
				break;

				/*
				 *	Outras Instruções
				 */
			    case T_INC:
				*sp++ = S_INC;
				break;

				/*
				 *	NÃO IMPLEMENTADOS
				 */
			    default:
				fprintf
				(	stderr,
					"uncomp: caso impossível: %d\n",
					*tp
				);
			}
		}

	}	/* end for *sp  */

	*sp = '\0';

	return (source);

}	/* end parmuncomp */

/*
 ****************************************************************
 *	Escreve um Número em Decimal				*
 ****************************************************************
 */
static char *
putnumber (register int n, register char *cp)
{
	if (n < 0)
		{ *cp++ = '-'; n = - n; }

	endcp = cp; putdigit (n);

	return (endcp);

}	/* end putnumber */

/*
 ****************************************************************
 *	Escreve um Dígito Decimal				*
 ****************************************************************
 */
static void
putdigit (register int n)
{
	/*
	 *	Se este não é o último dígito, escreve os primeiros
	 */
	if (n >= 10)
		putdigit (n / 10);

	*endcp++ = n % 10 + '0';

}	/* end putnumber */
