/*
 ****************************************************************
 *								*
 *			setbuf.c				*
 *								*
 *	Define a área de entrada/saída de um arquivo		*
 *								*
 *	Versão	1.0.0, de 17.11.86				*
 *		4.4.0, de 02.11.02				*
 *								*
 *	Módulo: setbuf						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ******	Definições globais **************************************
 */
#define	_EOMEM	(char *)EOF
#define	elif	else if

/*
 ******	Definição de variáveis externas *************************
 */
extern char	_sio1buf[], _sio2buf[];
extern int	_stdbuf;

/*
 ****************************************************************
 *	Define a área de entrada/saída				*
 ****************************************************************
 */
void
setbuf (FILE *fp, char *buf)
{
	if (fp->_flag == 0)
		return;

	/*
	 *	Libera a área anterior
	 */
	if (fp->_base != NOSTR)
	{
		if   (fp->_flag & _IOMYBUF)
			free (fp->_base);
		elif (fp->_base == _sio1buf)
			_stdbuf |= 0x1;
		elif (fp->_base == _sio2buf)
			_stdbuf |= 0x2;

		fp->_bend = NOSTR;
		fp->_lend = NOSTR;
	}

	fp->_flag &= ~(_IOMYBUF|_IONOBUF|_IOLNBUF);

	if ((fp->_base = buf) == NOSTR)
	{
		fp->_bsize = 1;
		fp->_flag |= _IONOBUF;
	   /*** fp->_base = NOSTR; ***/
	}
	else
	{
		fp->_bsize = BUFSIZ;

		if (fp->_flag & _IOWRITE && isatty (fileno (fp)))
		{
			fp->_flag |= _IOLNBUF;
			fp->_lend = buf + BUFSIZ;
		}
		else
		{
			fp->_bend = buf + BUFSIZ;
		}
	}

	fp->_ptr = _EOMEM;

}	/* end setbuf */
