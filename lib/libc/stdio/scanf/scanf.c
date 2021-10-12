/*
 ****************************************************************
 *								*
 *			scanf.c					*
 *								*
 *	Leitura de dados formatados da entrada padrão		*
 *								*
 *	Versão	1.0.0, de 19.11.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: scanf						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdarg.h>

/*
 ******	Protótipos de funções ***********************************
 */
extern int	_doscan (FILE *, const char *, void **);

/*
 ****************************************************************
 *	Ponto de Entrada da Rotina "scanf"			*
 ****************************************************************
 */
int
scanf (const char *fmt, ...)
{
	return (_doscan (stdin, fmt, va_first (fmt)));

}	/* end scanf */
