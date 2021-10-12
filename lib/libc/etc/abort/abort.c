/*
 ****************************************************************
 *								*
 *			abort.c					*
 *								*
 *	Termina o programa anormalmente (com "core dump")	*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 06.06.95				*
 *								*
 *	Módulo: abort						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdlib.h>
#include <signal.h>

/*
 ******	Protótipos de funções ***********************************
 */
extern void	_cleanup (void);

/*
 ****************************************************************
 *	Termina o Processo Anormalmente				*
 ****************************************************************
 */
void
abort (void)
{
	void		(*oldstatus) (int, ...);

	if ((oldstatus = signal (SIGABRT, SIG_DFL)) == SIG_DFL)
	{
		_cleanup ();
		kill (getpid (), SIGABRT);
	}
	else
	{
		signal (SIGABRT, oldstatus);
		kill (getpid (), SIGABRT);
		_cleanup ();
	}

	_exit (0xFF);	/* Em todos os casos */

}	/* end abort */
