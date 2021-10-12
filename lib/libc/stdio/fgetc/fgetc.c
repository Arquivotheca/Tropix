/*
 ****************************************************************
 *								*
 *			fgetc.c					*
 *								*
 *	Le de um arquivo de entrada (função)			*
 *								*
 *	Versão	1.0.0, de 18.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fgetc						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<stdio.h>

/*
 ****************************************************************
 *	Lê um caractere de um arquivo				*
 ****************************************************************
 */
int
fgetc (FILE *fp)
{
	return (getc (fp));

}	/* end fgetc   */
