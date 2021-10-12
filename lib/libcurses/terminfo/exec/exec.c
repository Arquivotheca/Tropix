/*
 ****************************************************************
 *								*
 *			exec.c					*
 *								*
 *	Interpretação de cadeias de parâmetros			*
 *								*
 *	Versão	2.0.0, de 07.05.87				*
 *		3.0.0, de 26.02.95				*
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
#include <stdarg.h>
#include <string.h>		/* PROVISÓRIO */

#include "../h/defs.h"
#include "../h/parm.h"

/*
 ****************************************************************
 *	Variáveis externas					*
 ****************************************************************
 */

/*
 ******	Definições da PILHA: sem teste de "overflow" ************
 */
#define	STACKSZ		20	/* Tamanho da Pilha */
#define	STACKSAVE	10	/* Parte abaixo do início (segurança) */

/*
 ****************************************************************
 *	Interpretação de cadeias de parâmetros			*
 ****************************************************************
 */
char *
parmexec (const char *code, ...)
{
	register const char	*cp;
	register char		*rp;
	register int		*param, *sp;
	va_list			args;
	int			stack[STACKSZ+STACKSAVE];

	/*
	 *	Pequeno prólogo
	 */
	va_start (args, code); param = (int *)args - 1;

	if (_parmrtn == NOSTR && (_parmrtn = malloc (PARMSZ)) == NOSTR)
		return (NOSTR);

	sp = &stack[STACKSZ]; rp = _parmrtn;

	/*
	 *	Execução do Código
	 */
	for (cp = code; *cp != T_END; cp++)
	{
		switch (*cp)
		{
		    case T_NULL:
			*rp++ = 0x80;
			break;

		    case T_NORMAL:
			*rp++ = *++cp;
			break;

		    case TO_CHAR:
			*rp++ = *sp++;
			break;

		    case TO_DEC:
			sprintf (rp, "%d", *sp++);
			rp = strend (_parmrtn);		/* PROVISÓRIO */
#if (0)	/*************************************/
			rp = putnumber (*sp++, rp);
#endif	/*************************************/
			break;

		    case TP_PARM:
			*--sp = param[*++cp];
			break;

		    case TP_CHAR:
			*--sp = *++cp;
			break;

		    case TP_DEC:
			*--sp = *++cp;
			break;

		    case TA_ADD:
			sp++;
			sp[0] = sp[0] + sp[-1];
			break;

		    case TA_SUB:
			sp++;
			sp[0] = sp[0] - sp[-1];
			break;

		    case TA_MUL:
			sp++;
			sp[0] = sp[0] * sp[-1];
			break;

		    case TA_DIV:
			sp++;
			sp[0] = sp[0] / sp[-1];
			break;

		    case TA_MOD:
			sp++;
			sp[0] = sp[0] % sp[-1];
			break;

		    case T_INC:
			param[1] += 1;
			param[2] += 1;
			break;

		}	/* switch *cp */

	}	/* for *cp != T_END */

	/*
	 *	Epílogo
	 */
	*rp = '\0';

	va_end (args);

	return (_parmrtn);

}	/* end parmexec */

#if (0)	/*************************************/
static char	*endcp;

/*
 ******	Protótipos de funções ***********************************
 */
static char	*putnumber (int, char *);
static void	putdigit (int);

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
#endif	/*************************************/
