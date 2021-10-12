/*
 ****************************************************************
 *								*
 *			vfprintf.c				*
 *								*
 *	Converte e formata a saida com lista variável de args	*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 14.01.95				*
 *								*
 *	Módulo: vfprintf					*
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
 ******	Protótipos de funções ***********************************
 */
extern int	_doprnt (const char *, void *, FILE *);

/*
 ****************************************************************
 *	Escreve em um arquivo determinado por "fp"		*
 ****************************************************************
 */
int
vfprintf (FILE *fp, const char *fmt, va_list args)
{
	return (_doprnt (fmt, args, fp));

}	/* end vfprintf */
