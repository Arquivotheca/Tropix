/*
 ****************************************************************
 *								*
 *			printw.c				*
 *								*
 *	Converte e formata cadeias em janelas 			*
 *								*
 *	Versão	1.0 0, de 17.03.88				*
 *		3.0.0, de 10.03.95				*
 *								*
 *	Modulo: printw						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include <curses.h>

#include "../../terminfo/h/defs.h"

/*
 ******	Definições globais **************************************
 */
#define	_EOMEM	(char *)EOF

/*
 ******	Protótipos de funções ***********************************
 */
extern int	_doprnt (const char *, void *, FILE *);

/*
 ****************************************************************
 *	Converte e formata cadeias na janela padrão		*
 ****************************************************************
 */
int
printw (const char *fmt, ...)
{
	register va_list args;
	FILE		_strbuf;
	char		str[256];

	va_start (args, fmt);

	_strbuf._flag = _IOWRITE|_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._bend = _EOMEM;
	_doprnt (fmt, args, &_strbuf);

	putc ('\0', &_strbuf);

	va_end (args);

	return (waddstr (stdwin, str));

}	/* end printw */

/*
 ****************************************************************
 *	Converte e formata cadeias em uma janela		*
 ****************************************************************
 */
int
wprintw (WINDOW *win, const char *fmt, ...)
{
	register va_list args;
	FILE		_strbuf;
	char		str[256];

	va_start (args, fmt);

	_strbuf._flag = _IOWRITE|_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._bend = _EOMEM;
	_doprnt (fmt, args, &_strbuf);

	putc ('\0', &_strbuf);

	va_end (args);

	return (waddstr (win, str));

}	/* end printw */
