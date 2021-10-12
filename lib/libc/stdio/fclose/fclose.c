/*
 ****************************************************************
 *								*
 *			fclose.c				*
 *								*
 *	Fecha um arquivo da "stdio"				*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fclose						*
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
#include <stdlib.h>

/*
 ****************************************************************
 *	Constantes e tipos					*
 ****************************************************************
 */
#define	elif	else if

/*
 ****************************************************************
 *	Definição de variáveis externas				*
 ****************************************************************
 */
extern char	_sio1buf[], _sio2buf[];
extern int	_stdbuf;

/*
 ****************************************************************
 *	Fecha um arquivo da "stdio"				*
 ****************************************************************
 */
int
fclose (register FILE *fp)
{
	register int	n;

	if (fp == NOFILE || fp->_flag == 0)
		return (EOF);

	n = fflush (fp);

	/*
	 *	Libera o buffer
	 */
	if   (fp->_flag & _IOMYBUF)
	{
		free (fp->_base);
	}
	elif ((fp->_flag & _IONOBUF) == 0)
	{
		if   (fp->_base == _sio1buf)
			_stdbuf |= 0x1;
		elif (fp->_base == _sio2buf)
			_stdbuf |= 0x2;
	}

	if (close (fileno (fp)) < 0)
		n = EOF;

	fp->_ptr   = NOSTR;
	fp->_base  = NOSTR;
	fp->_bend  = NOSTR;
	fp->_lend  = NOSTR;
	fp->_bsize = 0;
	fp->_flag  = 0;
	fp->_file  = 0;

	return (n);

}	/* end fclose */
