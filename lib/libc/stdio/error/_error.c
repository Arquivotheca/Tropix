/*
 ****************************************************************
 *								*
 *			_error.c				*
 *								*
 *	Escreve mensagem de erro catastrófica			*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: _error						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <string.h>

/*
 ****************************************************************
 *	Imprime mensagem de erro catastrófica			*
 ****************************************************************
 */
void
_error (const char *s)
{
	static int	segundavez;

	if (segundavez++)
		_exit (0xFF);

	write (2, s, strlen (s));

	exit (0xFE);

}	/* end _error */
