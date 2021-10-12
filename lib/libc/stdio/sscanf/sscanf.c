/*
 ****************************************************************
 *								*
 *			sscanf.c				*
 *								*
 *	Leitura de dados formatados de uma cadeia		*
 *								*
 *	Versão	1.0.0, de 19.11.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: sscanf						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*
 ******	Protótipos de funções ***********************************
 */
int	 _doscan (FILE *, const char *, void **);

/*
 ****************************************************************
 *	Leitura de dados formatados de uma cadeia		*
 ****************************************************************
 */
int
sscanf (register const char *str, const char *fmt, ...)
{
	FILE		_strbuf;

	_strbuf._flag = _IOREAD|_IOSTRG;
	_strbuf._ptr  = (char *)str;
	_strbuf._base = (char *)str;
	_strbuf._bend = strend (str);

	return (_doscan (&_strbuf, fmt, va_first (fmt)));

}	/* end sscanf */
