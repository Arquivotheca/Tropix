/*
 ****************************************************************
 *								*
 *			_cleanup.c				*
 *								*
 *	Executa o "atexit" e fecha todos os arquivos		*
 *								*
 *	Versão	1.0.0, de 14.10.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: _cleanup					*
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
 ******	Protótipos de funções ***********************************
 */
extern void	_atexit_exec (void);

/*
 ****************************************************************
 *	Executa o "atexit" e fecha todos os arquivos		*
 ****************************************************************
 */
void
_cleanup (void)
{
	register FILE *fp;

	_atexit_exec ();

	for (fp = &_ioblk[0]; fp < &_ioblk[_NFILE]; fp++)
		fclose (fp);

}	/* end _cleanup */
