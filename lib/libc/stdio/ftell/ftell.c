/*
 ****************************************************************
 *								*
 *			ftell.c					*
 *								*
 *	Fornece o ponteiro de posição do arquivo		*
 *								*
 *	Versão	1.0.0, de 24.11.86				*
 *		3.0.0, de 09.07.95				*
 *								*
 *	Módulo: ftell						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <unistd.h>

/*
 ****************************************************************
 *	Variaveis e Definicoes Globais				*
 ****************************************************************
 */
#define	_EOMEM	(char *)EOF
#define	NOSTR	(char *)NULL
#define	elif	else if

/*
 ****************************************************************
 *	Fornece o ponteiro de posição do arquivo		*
 ****************************************************************
 */
long
ftell (register FILE *fp)
{
	register int	ajuste;
	long		distancia;

	if (fp->_ptr == _EOMEM || fp->_flag & _IONOBUF || fp->_base == NOSTR)
		ajuste = 0;
	elif (fp->_flag & _IOREAD)
		ajuste = fp->_ptr - fp->_bend;
	elif (fp->_flag & _IOWRITE)
		ajuste = fp->_ptr - fp->_base;
	else
		return (EOF);

	distancia = lseek (fileno (fp), 0, SEEK_CUR);

	return (distancia < 0 ? EOF : distancia + ajuste);

}	/* end ftell */
