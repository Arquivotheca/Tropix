/*
 ****************************************************************
 *								*
 *			fseek.c					*
 *								*
 *	Modifica a posição do ponteiro de arquivo		*
 *								*
 *	Versão	1.0.0, de 24.11.86				*
 *		3.0.0, de 09.07.95				*
 *								*
 *	Módulo: fseek						*
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
 *	Tipos e Definições Globais				*
 ****************************************************************
 */
#define	elif	else if
#define	NOSTR	(char *)NULL
#define	_EOMEM	(char *)EOF

/*
 ****************************************************************
 *	Modifica a posição do ponteiro de arquivo		*
 ****************************************************************
 */
int
fseek (FILE *fp, long offset, int whence)
{
	long		cnt, inc;
	int		r;

	fp->_flag &= ~_IOEOF;

	/*
	 *	Verifica o tipo de arquivo
	 */
	if   (fp->_flag & _IORW)
	{
		/* Arquivo de atualização */

		if
		(	fp->_flag & _IOREAD &&
			whence == SEEK_CUR &&
			(fp->_flag & _IONOBUF) == 0 &&
			fp->_base != NOSTR &&
			fp->_ptr != _EOMEM
		)
			offset -= (fp->_bend - fp->_ptr);

		fflush (fp);
		fp->_flag &= ~(_IOWRITE|_IOREAD);
		fp->_ptr = _EOMEM;
	}
	elif (fp->_flag & _IOREAD)
	{
		/* Arquivo de leitura */

		if
		(	whence != SEEK_END &&
			(fp->_flag & _IONOBUF) == 0 &&
			fp->_base != NOSTR &&
			fp->_ptr != _EOMEM
		)
		{
			cnt = fp->_bend - fp->_ptr;

			if   (whence == SEEK_SET)
			{
				inc = offset + cnt - lseek (fileno (fp), 0, SEEK_CUR);
			   /*** offset += 0; ****/
			}
			else /* whence == SEEK_CUR */
			{
				inc = offset;
				offset -= cnt;
			}

			/*
			 *	Verifica se a futura posição do ponteiro está na área
			 */
			if (inc <= cnt && inc >= fp->_base - fp->_ptr)
				{ fp->_ptr += inc; return (0); }
		}

		fp->_ptr = fp->_bend;
	}
	elif (fp->_flag & _IOWRITE)
	{
		/* Arquivo de escrita */

		fflush (fp); /*** fp->_ptr = fp->_base; ***/
	}

	/*
	 *	Executa o "seek"
	 */
	r = lseek (fileno (fp), offset, whence);

	return (r == -1 ? EOF : 0);

}	/*  end fseek */
