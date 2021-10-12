/*
 ****************************************************************
 *								*
 *			ungetc.c				*
 *								*
 *	Devolve um caracter para a área de entrada		*
 *								*
 *	Versão	1.0.0, de 18.11.86				*
 *		3.0.0, de 03.01.95				*
 *								*
 *	Módulo: ungetc						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	_EOMEM	(char *)EOF

/*
 ****************************************************************
 *	Devolve um caracter para a área de entrada		*
 ****************************************************************
 */
int
ungetc (int c, register FILE *fp)
{
	if (c == EOF)
		return (EOF);

	if ((fp->_flag & _IOREAD) == 0)
		return (EOF);

	if (fp->_base == NOSTR)
		return (EOF);

	if ((fp->_flag & _IOSTRG) == 0 && fp->_bsize < 2)
		return (EOF);

	if (fp->_flag & _IOERR)
		return (EOF);

	if (fp->_flag & _IOEOF)
	{
		fp->_ptr = fp->_bend;
	}
	else
	{
		if (fp->_ptr <= fp->_base)
			return (EOF);
	}

	/*
	 *	É possível!
	 */
	*--fp->_ptr = c; fp->_flag &= ~_IOEOF; return (c);

}	/* end ungetc */
