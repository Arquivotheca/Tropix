/*
 ****************************************************************
 *								*
 *			assert.c				*
 *								*
 *	Imprime mensagens de erro para assertivas falsas	*
 *								*
 *	Versão	2.3.0, de 26.02.90				*
 *		3.0.0, de 08.06.95				*
 *								*
 *	Módulo: assert						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
_assert (const char *expr, const char *file, int line)
{
	fprintf
	(	stderr,
		"Assertiva falsa: \"%s\", arquivo \"%s\", linha %d\n",
		expr, file, line
	);

	abort ();

}	/* end assert */
