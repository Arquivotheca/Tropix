/*
 ****************************************************************
 *								*
 *			fstatus.c				*
 *								*
 *	Imprime o estado do um FILE				*
 *								*
 *	Versão	1.0.0, de 14.11.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: status						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
extern char	_sio1buf[], _sio2buf[];
extern int	_stdbuf;

/*
 ****************************************************************
 *	Imprime o estado do um FILE				*
 ****************************************************************
 */
void
fstatus (register const FILE *fp, const char *msg)
{
	register short	flag;

	if (msg != NOSTR)
		fprintf (stderr, "%s", msg);

	fprintf (stderr, "fd = %d, ", fp->_file);

	fprintf (stderr, "bsize = %d, ", fp->_bsize);

	fprintf (stderr, "flags =");

	flag = fp->_flag;

	if (flag & _IOREAD)
		fprintf (stderr, " IOREAD");

	if (flag & _IOWRITE)
		fprintf (stderr, " IOWRITE");

	if (flag & _IORW)
		fprintf (stderr, " IORW");

	if (flag & _IONOBUF)
		fprintf (stderr, " IONOBUF");

	if (flag & _IOMYBUF)
		fprintf (stderr, " IOMYBUF");

	if (flag & _IOLNBUF)
		fprintf (stderr, " IOLNBUF");

	if (flag & _IOEOF)
		fprintf (stderr, " IOEOF");

	if (flag & _IOERR)
		fprintf (stderr, " IOERR");

	if (flag & _IOSTRG)
		fprintf (stderr, " IOSTRG");

	fprintf (stderr, ", ptr = %P", fp->_ptr);

	if (fp->_ptr == (char *)EOF)
		fprintf (stderr, " (EOMEM)");

	fprintf (stderr, "\nbase = %P", fp->_base);

	if (fp->_base == _sio1buf)
		fprintf (stderr, " (sio1buf)"); 

	if (fp->_base == _sio2buf)
		fprintf (stderr, " (sio2buf)"); 

	fprintf (stderr, ", bend = %P", fp->_bend);

	fprintf (stderr, ", lend = %P\n", fp->_lend);

}	/* end fstatus */
