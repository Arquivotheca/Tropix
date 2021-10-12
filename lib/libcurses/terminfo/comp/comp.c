/*
 ****************************************************************
 *								*
 *			comp.c					*
 *								*
 *	Compilação de Capacidades String Paramétricas		*
 *								*
 *	Versão	2.0.0, de 07.05.87				*
 *		3.0.0, de 22.02.95				*
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
#include <ctype.h>

#include "../h/defs.h"
#include "../h/parm.h"

/*
 ****************************************************************
 *	Variáveis Externas					*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
static char	*getnumber (char *, char, char *);

/*
 ****************************************************************
 *	Compilação						*
 ****************************************************************
 */
char *
parmcomp (char *source)
{
	register char	*sp;	/* Source code pointer */
	register char	*tp;	/* Target code pointer */
	register char	*target;

	/*
	 *	Inicializações
	 */
	if (_parmrtn == NOSTR && (_parmrtn = malloc (PARMSZ)) == NOSTR)
		return (NOSTR);

	target = _parmrtn; _parmerror = NOSTR;

	/*
	 *	Malha principal
	 */
	for (sp = source, tp = target; *sp != '\0'; sp++)
	{
		if (*sp == S_SPECIAL)
		{
			switch (*++sp)
			{
			    case S_SPECIAL:
				*tp++ = T_SPECIAL;
				break;

				/*
			 	 *	Saída Formatada
			 	 */
			    case SO_CHAR:		/* Caractere */
				*tp++ = TO_CHAR;
				break;

			    case SO_DEC:		/* Decimal */
				*tp++ = TO_DEC;
				break;

			    case SO_OCTAL:		/* Octal */
				*tp++ = TO_OCTAL;
				break;

			    case SO_HEX:		/* Hexa c/ Minúsculas */
				*tp++ = TO_HEX;
				break;

			    case SO_CHEX:		/* Hexa c/ Maiúsculas */
				*tp++ = TO_CHEX;
				break;

				/*
			 	 *	Push
			 	 */
			    case SP_PARM:		/* Parâmetro */
				*tp++ = TP_PARM;

				if (*++sp >= '1' && *sp <= '9')
					*tp++ = *sp - '0';
				else
					{ _parmptr = sp; return (NOSTR); }
				break;

			    case SP_CHAR1:		/* Const. Caractere */
				*tp++ = TP_CHAR;

				if (*++sp != '\0')
					*tp++ = *sp;
				else
					{ _parmptr = sp; return (NOSTR); }

				if (*++sp != SP_CHAR2)
					{ _parmptr = sp; return (NOSTR); }
				break;

			    case SP_DEC1:	/* Push Const. Decimal */
				*tp++ = TP_DEC;
				_parmptr = sp;		

				if ((sp = getnumber (sp + 1, SP_DEC2, tp)) == NOSTR)
					return (NOSTR);
				tp++;
				break;

				/*
			 	 *	Aritmética
			 	 */
			    case SA_ADD:
				*tp++ = TA_ADD;
				break;

			    case SA_MUL:
				*tp++ = TA_MUL;
				break;

			    case SA_SUB:
				*tp++ = TA_SUB;
				break;

			    case SA_DIV:
				*tp++ = TA_DIV;
				break;

			    case SA_MOD:
				*tp++ = TA_MOD;
				break;

				/*
			 	 *	Outras Instruções
			 	 */
			    case S_INC:
				*tp++ = T_INC;
				break;

				/*
			 	 *	NÃO IMPLEMENTADOS
			 	 */
			    default:
				_parmptr = sp;
printf ("comp: caso não implementado: %d\n", *sp);
				return (NOSTR);
			}
		}
		elif (*sp == 0x80)
		{
			*tp = T_NULL;
		}
		else
		{
			*tp++ = T_NORMAL; *tp++ = *sp;
		}

	}	/* end for *sp  */

	*tp = T_END;

	return (target);

}	/* end parmcomp */

/*
 ****************************************************************
 *	Leitura de um Número (de um byte).			*
 ****************************************************************
 */
static char *
getnumber (register char *sp, register char End, register char *np)
{
	register int	n;
	register int	neg;

	/*
	 *	Pula espaços e sinal
	 */
	while (isspace (*sp))
		sp++;

	if (neg = (*sp == '-') || *sp == '+')
		sp++;

	/*
	 *	Calcula o Número
	 */
	for (n = 0 ; isdigit (*sp) ; sp++)
		n = 10 * n + *sp - '0';

	if (neg)
		n = - n;

	/*
	 *	Verifica Terminação Correta
	 */
	if (*sp != End)
	{
		_parmerror = "Espero \"x\" ";
		_parmerror[8] = End;
		_parmptr = sp;
		return (NOSTR);
	}

	/*
	 *	Verifica Limite Máximo
	 */
	if (n > 127 || n < -128)
	{
		_parmerror = "Número muito grande";
		_parmptr = sp;
		return (NOSTR);
	}

	/*
	 *	Copia o número
	 */
	*np = (char)n;

	/*
	 *	Retorna endereço de continuação
	 */
	return (sp);

}	/* end getnumber */
