/*
 ****************************************************************
 *								*
 *			strerror.c				*
 *								*
 *	Obtem uma mensagem de erro do KERNEL			*
 *								*
 *	Versão	1.0.0, de 30.10.86				*
 *		3.0.0, de 08.08.95				*
 *								*
 *	Módulo: strerror					*
 *		libc/etc 					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Obtem a mensagem de erro				*
 ****************************************************************
 */
char *
strerror (int err)
{
	static char	*cp;

	if (cp == NOSTR && (cp = malloc (80)) == NOSTR)
		return ("Não consegui obter a mensagem de erro");

	if (kcntl (K_ERRTOSTR, err, cp) < 0)
		return ("Não consegui obter a mensagem de erro");

	return (cp);

}	/* end strerror */
