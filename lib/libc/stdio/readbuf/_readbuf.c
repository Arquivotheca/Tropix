/*
 ****************************************************************
 *								*
 *			_readbuf.c				*
 *								*
 *	Preenche a área da stdio				*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		4.4.0, de 02.11.02				*
 *								*
 *	Módulo: getc						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ******	Definições globais **************************************
 */
#define	elif	else if
#define	_EOMEM	(char *)EOF

/*
 ******	Definição de variáveis externas *************************
 */
extern char	_sio1buf[], _sio2buf[];
extern int	_stdbuf;

/*
 ******	Protótipos de funções ***********************************
 */
extern void	_error (const char *);

/*
 ****************************************************************
 *	Preenche a área da stdio				*
 ****************************************************************
 */
int
_readbuf (FILE *fp)
{
	static char	noreadbuf[_NFILE];
	int		count;

	/*
	 *	Verifica se o arquivo é de atualização
	 */
	if (fp->_flag & _IORW)
		fp->_flag |= _IOREAD;

	/*
	 *	Verifica se o arquivo é de leitura
	 */
	if ((fp->_flag & (_IOREAD|_IOWRITE)) != _IOREAD)
		_error ("Lendo arquivo de saida\n");

	/*
	 *	Verifica se o arquivo é uma cadeia
	 */
	if (fp->_flag & _IOSTRG)
		{ fp->_flag |= _IOEOF; return (EOF); }

	/*
	 *	Verifica se é a primeira utilização da getc
	 */
	if (fp->_base == NOSTR)
	{
		if   (fp->_flag & _IONOBUF)
		{
		    noarea:
			fp->_base  = &noreadbuf[fileno(fp)];
			fp->_bsize = 1;
			fp->_flag |= _IONOBUF;
		}
		elif (isatty (fileno (fp)))
		{
			fp->_bsize = BLSZ;

			if   (_stdbuf & 0x01)
				{ fp->_base = _sio1buf; _stdbuf &= ~0x01;  }
			elif (_stdbuf & 0x02)
				{ fp->_base = _sio2buf; _stdbuf &= ~0x02;  }
			elif ((fp->_base = malloc (fp->_bsize)) != NOSTR)
				fp->_flag |= _IOMYBUF;
			else
				goto noarea;
		}
		else
		{
			fp->_bsize = BUFSIZ;

			if ((fp->_base = malloc (fp->_bsize)) != NOSTR)
				fp->_flag |= _IOMYBUF;
			else
				goto noarea;
		}

	}	/* end if (fp->_base == NOSTR) */

	/*
	 *	Verifica se a leitura é do stdin
	 */
	if (fp == stdin)
	{
		if (stdout->_flag & _IOLNBUF)
			fflush (stdout);

		if (stderr->_flag & _IOLNBUF)
			fflush (stderr);
	}

	/*
	 *	Le um registro
	 */
	count = read (fileno (fp), fp->_base, fp->_bsize);

	/*
	 *	Verifica se houve erro na leitura
	 */
	if (count <= 0)
	{
		if (count == 0)		/* Fim de arquivo */
		{
			fp->_flag |= _IOEOF;

			if (fp->_flag & _IORW)
				fp->_flag &= ~_IOREAD;
		}
		else	/* count < 0 =>	   Erro de leitura */
		{
			fp->_flag |= _IOERR;
		}

		fp->_ptr = _EOMEM;

		return (EOF);

	}	/* end if (count <= 0) */

	fp->_ptr  = fp->_base;
	fp->_bend = fp->_base + count;

	return (*fp->_ptr++);

}	/* end _readbuf */
