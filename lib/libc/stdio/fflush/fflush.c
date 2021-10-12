/*
 ****************************************************************
 *								*
 *			fflush.c				*
 *								*
 *	Atualiza o arquivo no disco				*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fflush						*
 *		libc/stdio					*
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
 *	Definições e áreas globais				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL
#define	_EOMEM	(char *)EOF

/*
 ****************************************************************
 *	Atualiza o conteúdo do arquivo				*
 ****************************************************************
 */
int
fflush (register FILE *fp)
{
	register int		n, r = 0;
	register const FILE	*end_fp;

	/*
	 *	Acerta os limites da malha
	 */
	if (fp != NOFILE)
		{ /*** fp = fp; ***/ end_fp = fp; }
	else
		{ fp = &_ioblk[0]; end_fp = &_ioblk[_NFILE-1]; }

	/*
	 *	Executa a malha
	 */
	for (/* acima */; fp <= end_fp; fp++)
	{
		if ((fp->_flag & (_IONOBUF|_IOWRITE)) != _IOWRITE)
			continue;

		if (fp->_base == NOSTR)
			continue;

		if (fp->_ptr == _EOMEM || (n = fp->_ptr - fp->_base) == 0)
			continue;

		fp->_ptr = fp->_base;

		if (write (fileno (fp), fp->_base, n) != n)
			{ fp->_flag |= _IOERR; r |= EOF; }

	}	/* end for (arquivos) */

	return (r);

}	/* end fflush */
