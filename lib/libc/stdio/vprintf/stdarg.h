/*
 ****************************************************************
 *								*
 *			<stdarg.h>				*
 *								*
 *	Argumentos variáveis					*
 *								*
 *	Versão	2.3.0, de 28.02.90				*
 *		4.6.0, de 17.07.04				*
 *								*
 *	Módulo: stdarg.h					*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definição de tipos					*
 ****************************************************************
 */
typedef int	*va_list;	/* Lista para vfprintf */

/*
 ****************************************************************
 *	Definição de macros					*
 ****************************************************************
 */
#define _va_round_sizeof(mode)  ((sizeof (mode) + sizeof (int) - 1) / sizeof (int))

#define va_start(ap, parmN)	ap = ((int *)&parmN + 1)
#define va_first(parmN)		(void *)((int *)&parmN + 1)
#define va_arg(ap, mode)	((mode *)(ap += _va_round_sizeof (mode)))[-1]
#define va_end(ap)
