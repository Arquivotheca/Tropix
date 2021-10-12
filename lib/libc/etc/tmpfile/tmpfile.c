/*
 ****************************************************************
 *								*
 *			tmpfile.c				*
 *								*
 *	Cria um arquivo temporário				*
 *								*
 *	Versão	2.3.0, de 03.03.90				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: tmpfile						*
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
 *	Cria um arquivo temporário				*
 ****************************************************************
 */
FILE *
tmpfile (void)
{
	FILE		*fp;
	const char	*filenm;

	filenm = tmpnam (NOSTR);

	if ((fp = fopen (filenm, "w+")) == NOFILE)
		return (NOFILE);

	unlink (filenm);

	return (fp);

}	/* end tmpfile */
