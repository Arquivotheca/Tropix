/*
 ****************************************************************
 *								*
 *			vprintf.c				*
 *								*
 *	Converte e formata a saida com lista variável de args	*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 18.07.95				*
 *								*
 *	Módulo: vprintf						*
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
 *	Protótipos						*
 ****************************************************************
 */
extern int	_doprnt (const char *, void *, FILE *);

/*
 ****************************************************************
 *	Escreve em "stdout"					*
 ****************************************************************
 */
int
vprintf (const char *fmt, va_list args)
{
	return (_doprnt (fmt, args, stdout));

}	/* end vprintf */
