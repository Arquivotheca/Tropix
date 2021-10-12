/*
 ****************************************************************
 *								*
 *			raise.c					*
 *								*
 *	Envia um sinal para o próprio processo			*
 *								*
 *	Versão	2.3.0, de 03.03.90				*
 *		3.0.0, de 23.06.95				*
 *								*
 *	Módulo: raise						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <signal.h>

/*
 ****************************************************************
 *	Envia um sinal para o próprio processo			*
 ****************************************************************
 */
int
raise (int sig)
{
	return (kill (getpid (), sig));

}	/* end raise */
