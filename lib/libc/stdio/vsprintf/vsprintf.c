/*
 ****************************************************************
 *								*
 *			vsprintf.c				*
 *								*
 *   Converte e formata uma cadeia com lista variável de args	*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 18.07.95				*
 *								*
 *	Módulo: vsprintf					*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

/*
 ****************************************************************
 *	Definições						*
 ****************************************************************
 */
#define	_EOMEM	(char *)EOF

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern int	_doprnt (const char *, void *, FILE *);

/*
 ****************************************************************
 *	Escreve na string "str"					*
 ****************************************************************
 */
int
vsprintf (char *str, const char *fmt, va_list args)
{
	register int	n;
	FILE		_strbuf;

	_strbuf._flag = _IOWRITE|_IOSTRG;
	_strbuf._ptr = (char *)str;
	_strbuf._bend = _EOMEM;

	n = _doprnt (fmt, args, &_strbuf);

	putc ('\0', &_strbuf);

	return (n);

}	/* end vsprintf */
