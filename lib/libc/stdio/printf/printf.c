/*
 ****************************************************************
 *								*
 *			printf.c				*
 *								*
 *	Converte e formata a saida				*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: printf						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdarg.h>
#include <stdio.h>

/*
 ******	Protótipos de funções ***********************************
 */
int	_doprnt (const char *, const int *, FILE *);

/*
 ****************************************************************
 *	Converte e formata a saida				*
 ****************************************************************
 */
int
printf (const char *fmt, ...)
{
	return (_doprnt (fmt, va_first (fmt), stdout));

}	/* end printf */
