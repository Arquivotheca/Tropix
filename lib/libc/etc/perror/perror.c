/*
 ****************************************************************
 *								*
 *			perror.c				*
 *								*
 *	Imprime uma mensagem de erro do KERNEL			*
 *								*
 *	Versão	1.0.0, de 29.10.86				*
 *		3.0.0, de 23.06.95				*
 *								*
 *	Módulo: libc/etc					*
 *		Biblioteca padrao				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	elif	else if

/*
 ****************************************************************
 *	Imprime a mensagem de êrro				*
 ****************************************************************
 */
void
perror (const char *pgname)
{
	static char	*cp;
	register int 	n;

	if   (cp == NOSTR && (cp = malloc (80)) == NOSTR)
		cp = "Não consegui obter a mensagem de erro";
	elif (kcntl (K_ERRTOSTR, errno, cp) < 0)
		cp = "Não consegui obter a mensagem de erro";

	if (pgname != NOSTR && (n = strlen (pgname)) > 0)
	{
		write (2, pgname, n);
		write (2, ": ", 2);
	}

	write (2, cp, strlen (cp));
	write (2, "\n", 1);

}	/* end perror */
