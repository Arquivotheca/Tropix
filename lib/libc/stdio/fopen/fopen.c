/*
 ****************************************************************
 *								*
 *			fopen.c					*
 *								*
 *	Abre um arquivo						*
 *								*
 *	Versão	1.0.0, de 17.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fopen						*
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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	MASK	0644
#define	_EOMEM	(char *)EOF

/*
 ****************************************************************
 *	Abre um arquivo						*
 ****************************************************************
 */
FILE *
fopen (register const char *nome, register const char *tipo)
{
	register FILE	*fp;
	register int 	fd;
	register char	rw;

	rw = (tipo[1] == '+') || (tipo[1] == 'b') && (tipo[2] == '+');

	for (fp = _ioblk; /* vazio */; fp++)
	{
		if (fp >= &_ioblk[_NFILE])
			return (NOFILE);

		if (fp->_flag == 0)
			break;
	}

	switch (tipo[0])
	{
	    case 'r':
		fd = open (nome, rw ? O_RDWR : O_RDONLY);
		break;

	    case 'w':
		if ((fd = creat (nome, MASK)) >= 0 && rw)
			{ close (fd); fd = open (nome, O_RDWR); }
		break;

	    case 'a':
		if ((fd = open (nome, rw ? O_RDWR : O_WRONLY)) < 0 && errno == ENOENT)
		{
			if ((fd = creat (nome, MASK)) >= 0 && rw)
				{ close (fd); fd = open (nome, O_RDWR); }
		}

		if (fd >= 0)
			lseek (fd, 0, L_END);
		break;

	    default:
		return (NOFILE);

	}	/* end switch (tipo[0])   */

	if (fd < 0)
		return (NOFILE);

	fp->_ptr = _EOMEM;
	fp->_flag = (rw ? _IORW : (tipo[0] == 'r' ? _IOREAD : _IOWRITE));
	fp->_file = fd;

	return (fp);

}	/* end fopen */
