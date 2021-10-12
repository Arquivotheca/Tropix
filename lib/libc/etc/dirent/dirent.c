/*
 ****************************************************************
 *								*
 *			dirent.c				*
 *								*
 *   Leitura de diretórios independente de sistema de arquivos	*
 *								*
 *	Versão	4.0.0, de 03.08.01				*
 *		4.8.0, de 05.11.05				*
 *								*
 *	Módulo: dirent						*
 *		libc/etc 					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stat.h>
#include <fcntl.h>
#include <errno.h>

#define	elif	else if

/*
 ******	Protótipos de funções ***********************************
 */
static DIR	*_opendir (int fd);

/*
 ****************************************************************
 *	Abre um diretório para leitura				*
 ****************************************************************
 */
DIR *
opendir (const char *file_nm)
{
	int		fd;

	if ((fd = open (file_nm, O_RDONLY)) < 0)
		return (NODIR);

	return (_opendir (fd));

} 	/* end opendir */

/*
 ****************************************************************
 *	Abre um diretório para leitura (dado o nó-índice)	*
 ****************************************************************
 */
DIR *
inopendir (int dev, int ino)
{
	int		fd;

	if ((fd = inopen (dev, ino)) < 0)
		return (NODIR);

	return (_opendir (fd));

} 	/* end opendir */

/*
 ****************************************************************
 *	Interface interna de "opendir"				*
 ****************************************************************
 */
static DIR *
_opendir (int fd)
{
	DIR		*dp;
	STAT		s;

	if (fstat (fd, &s) < 0)
		return (NODIR);

	if (!S_ISDIR (s.st_mode))
		{ errno = ENOTDIR; close (fd); return (NODIR); }

	if ((dp = malloc (sizeof (DIR))) == NODIR)
		{ errno = ENOMEM; close (fd); return (NODIR); }

	dp->d_fd    = fd;
	dp->d_begin = NODIRENT;
	dp->d_end   = NODIRENT;
	dp->d_eof   = 0;

	return (dp);

}	/* end _opendir */

/*
 ****************************************************************
 *	Obtém a entrada seguinte do diretório			*
 ****************************************************************
 */
DIRENT *
readdir (DIR *dp)
{
	int		n;

	/*
	 *	Verifica se há entrada disponível
	 */
	if (dp->d_begin >= dp->d_end)
	{
		if (dp->d_eof)
			return (NODIRENT);

		if ((n = getdirent (dp->d_fd, dp->d_area, sizeof (dp->d_area), &dp->d_eof)) <= 0)
			return (NODIRENT);

		dp->d_begin = dp->d_area;
		dp->d_end   = (DIRENT *)((int)dp->d_area + n);
	}

	/*
	 *	Fornece uma entrada
	 */
	return (dp->d_begin++);

}	/* end readdir */

/*
 ****************************************************************
 *	Obtém o deslocamento no diretório			*
 ****************************************************************
 */
off_t
telldir (const DIR *dp)
{
	if   (dp->d_begin == NODIRENT)
		return (0);
	elif (dp->d_begin == dp->d_end)
		return (lseek (dp->d_fd, 0, L_CUR));
	else
		return (dp->d_begin->d_offset);

}	/* end telldir */

/*
 ****************************************************************
 *	Atribui o deslocamento do diretório			*
 ****************************************************************
 */
int
seekdir (DIR *dp, off_t offset)
{
	dp->d_begin = NODIRENT;
	dp->d_end   = NODIRENT;
	dp->d_eof   = 0;

	return (lseek (dp->d_fd, offset, L_SET));

}	/* end seekdir */

/*
 ****************************************************************
 *	Encerra o uso do diretório				*
 ****************************************************************
 */
int
closedir (DIR *dp)
{
	int	status;

	status = close (dp->d_fd);

	free (dp);

	return (status);

}	/* end closedir */
