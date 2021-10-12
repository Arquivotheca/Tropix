/*
 ****************************************************************
 *								*
 *			remove.c				*
 *								*
 *	Remove um arquivo					*
 *								*
 *	Versão	2.3.0, de 03.03.90				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: remove						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>

/*
 ****************************************************************
 *	Remove um arquivo					*
 ****************************************************************
 */
int
remove (const char *filenm)
{
	return (unlink (filenm));

}	/* end remove */
