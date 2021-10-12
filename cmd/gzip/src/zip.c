/*
 ****************************************************************
 *								*
 *			zip.c					*
 *								*
 *	Compress files to the gzip or pkzip format		*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.0.0, de 09.06.93				*
 *								*
 *	Módulo: GZIP						*
 *		Utilitários de compressão/descompressão		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <ctype.h>
#include <stdio.h>

#include "../h/gzip.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
local ulong	crc;		/* crc on uncompressed file data */
entry long	overhead;	/* number of bytes in gzip header */

entry ZIPFILE	zipfile;	/* Estrutura para a saída ZIP */

/*
 ****************************************************************
 *	Deflate in to out					*
 ****************************************************************
 */
/* IN assertions: the input and output buffers are cleared.
 *	The variables time_stamp and save_orig_name are initialized.
 */
void
zip (int in, int out)
{
	char		flags = 0;	/* general purpose bit flags */
	ushort		attr = 0;	/* ascii/binary flag */
	ushort		deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
	register ZIPFILE *zp1 = &zipfile;

	/*
	 *	Inicializa a saída ZIP
	 */
	zp1->z_ptr  = outbuf; 
	zp1->z_base = outbuf; 
	zp1->z_bend = outbuf + OUTBUFSIZ; 

	ifd = in;
	ofd = out;

#if (0)	/*************************************/
	outcnt = 0;
#endif	/*************************************/

	/* Write the header to the gzip file. See algorithm.doc for the format */

	method = DEFLATED;
	PUTZIP (GZIP_MAGIC[0]);	/* magic header */
	PUTZIP (GZIP_MAGIC[1]);
	PUTZIP (DEFLATED);		/* compression method */

	if (save_orig_name)
		flags |= ORIG_NAME;

	PUTZIP (flags);		/* general flags */
	putzip_long (time_stamp);

	/* Write deflated file to zip file */
	crc = updcrc (NOSTR, 0);

	bi_init (out);
	ct_init (&attr, &method);
	lm_init (level, &deflate_flags);

	PUTZIP ((char)deflate_flags);		/* extra flags */
	PUTZIP (OS_CODE);			/* OS identifier */

	if (save_orig_name)
	{
		char 	*p = basename (ifname); /* Don't save the directory part. */

		do
		{
			PUTZIP(*p);
		}
		while (*p++);
	}

	overhead = zp1->z_ptr - zp1->z_base;
#if (0)	/*************************************/
	overhead = outcnt;
#endif	/*************************************/

	deflate ();

	/*
	 *	Check input size
	 */
	if (ifile_size != -1 && isize != ifile_size)
	{
		Trace ((stderr, " actual=%ld, read=%ld ", ifile_size, isize));

		fprintf
		(	stderr,
			"%s: %s: O tamanho do arquivo mudou durante a compressão\n",
			pgname, ifname
		);
	}
#if (0)	/*************************************/
#endif	/*************************************/

	/* Write the crc and uncompressed size */
	putzip_long (crc);
	putzip_long (isize);

	write_zip (0, zp1);
#if (0)	/*************************************/
	flush_outbuf ();
#endif	/*************************************/

}	/* end zip */

/*
 ****************************************************************
 *	Escreve um longo no arquivo ZIP				*
 ****************************************************************
 */
void
putzip_long (register ulong l)
{
	register ZIPFILE *zp1 = &zipfile;
	register int	i;

	for (i = 4; i > 0; i--)
	{
		PUTZIP (l);	l >>= 8;
	}

}	/* end putzip_long */

/*
 ****************************************************************
 *	Lê uma área da entrada					*
 ****************************************************************
 */
/* ===========================================================================
 * Read a new buffer from the current input file, perform end-of-line
 * translation, and update the crc and input file size.
 * IN assertion: size >= 2 (for end-of-line translation)
 */
int
file_read (char *buf, unsigned size)
{
	unsigned	len;

	Assert (insize == 0, "inbuf not empty");

	len = read (ifd, buf, size);

	if (len == (unsigned)(-1) || len == 0)
		return len;

	crc = updcrc ((char *)buf, len);

	isize += len;

	return len;

}	/* end file_read */
