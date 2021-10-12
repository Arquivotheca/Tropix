/*
 ****************************************************************
 *								*
 *			unzip.c					*
 *								*
 *	Decompress files in gzip or pkzip format		*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.0.0, de 17.06.93				*
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

#include <stdio.h>

#include "../h/gzip.h"

/*
 ****************************************************************
 *	Variáveis globais e definições				*
 ****************************************************************
 */
int	pkzip = 0;		/* set for a pkzip file */

int	extended = 0;		/* set if extended local header */

/*
 ****** PKZIP header definitions ********************************
 */
#define LOCSIG 0x04034B50	/* four-byte lead-in (lsb first) */
#define LOCFLG 6		/* offset of bit flag */
#define  CRPFLG 1		/*  bit for encrypted entry */
#define  EXTFLG 8		/*  bit for extended local header */
#define LOCHOW 8		/* offset of compression method */
#define LOCTIM 10		/* file mod time (for decryption) */
#define LOCCRC 14		/* offset of crc */
#define LOCSIZ 18		/* offset of compressed size */
#define LOCLEN 22		/* offset of uncompressed length */
#define LOCFIL 26		/* offset of file name field length */
#define LOCEXT 28		/* offset of extra field length */
#define LOCHDR 30		/* size of local header, including sig */
#define EXTHDR 16		/* size of extended local header, inc sig */

/*
 ****** Macros for getting two-byte and four-byte header values *
 */
#define SH(p) ((ushort)(char)((p)[0]) | ((ushort)(char)((p)[1]) << 8))
#define LG(p) ((ulong)(SH(p)) | ((ulong)(SH((p)+2)) << 16))

/*
 ****************************************************************
 *	Check zip file						*
 ****************************************************************
 */
int
check_zipfile (int in)
{
	char		*h;

	h = inbuf + inptr;	/* first local header */

	ifd = in;

	/*
	 *	Check validity of local header, and skip name and extra fields
	 */
	inptr += LOCHDR + SH (h + LOCFIL) + SH (h + LOCEXT);

	if (inptr > insize || LG (h) != LOCSIG)
		gzip_error ("A entrada não consiste de um arquivo vazio ou \"zip\"");

	method = h[LOCHOW];

	if (method != STORED && method != DEFLATED)
		gzip_error ("A primeira entrada não está deflacionada nem armazenada -- use \"unzip\"");

	/*
	 *	If entry encrypted, decrypt and validate encryption header
	 */
	if ((h[LOCFLG] & CRPFLG) != 0)
	{
		fprintf (stderr, "arquivo encriptado, ainda não suportado.\n");
		exit_code = ERROR;
		return -1;
	}

	/*
	 *	Save flags for unzip ()
	 */
	extended = (h[LOCFLG] & EXTFLG) != 0;
	pkzip = 1;

	/*
	 *	Get ofname and time stamp from local header (to be done)
	 */
	return (0);

}	/* end check_zipfile */

/*
 ****************************************************************
 *	Unzip in to out						*
 ****************************************************************
 */
void
unzip (int in, int out)
{
	ulong		orig_crc = 0;	/* original crc */
	ulong		orig_len = 0;	/* original uncompressed length */
	int		n;
	char		buf[EXTHDR];	/* extended local header */

	ifd = in; 	ofd = out;

	updcrc (NOSTR, 0);			/* initialize crc */

	/*
	 *	crc and length at the end otherwise
	 */
	if (pkzip && !extended)
	{
		orig_crc = LG (inbuf + LOCCRC);
		orig_len = LG (inbuf + LOCLEN);
	}

	/*
	 *	Decompress
	 */
	if (method == DEFLATED)
	{
		int		res = inflate ();

		if   (res == 3)
			gzip_error ("Memória esgotada");
		elif (res != 0)
			gzip_error ("Dado comprimido inválido -- violação de formato");

	}
	elif (pkzip && method == STORED)
	{
		register ulong	nn = LG (inbuf + LOCLEN);

		if (nn != LG (inbuf + LOCSIZ))
		{
			fprintf
			(	stderr,
				"len %ld, siz %ld\n",
				nn, LG (inbuf + LOCSIZ)
			);
			gzip_error ("Dado comprimido inválido -- violação de comprimento");
		}

		while (nn--)
		{
			char		c = (char)get_byte ();

			put_char (c);
		}

		flush_window ();
	}
	else
	{
		gzip_error ("Erro interno, método inválido");
	}

	/*
	 *	Get the crc and original length
	 */
	if (!pkzip)
	{
		/*
		 *	crc32  (see algorithm.doc)
		 *	uncompressed input size modulo 2^32
		 *	(may cause an error if EOF)
		 */
		for (n = 0; n < 8; n++)
			buf[n] = (char)get_byte();

		orig_crc = LG (buf);
		orig_len = LG (buf+4);

	}
	elif (extended)		/* If extended header, check it */
	{
		/*	signature - 4bytes: 0x50 0x4b 0x07 0x08
		 *	CRC-32 value
		 *	compressed size 4-bytes
		 *	uncompressed size 4-bytes
		 *	(may cause an error if EOF)
		 */
		for (n = 0; n < EXTHDR; n++)
			buf[n] = (char)get_byte ();

		orig_crc = LG (buf+4);
		orig_len = LG (buf+12);
	}

	/*
	 *	Validate decompression
	 */
	if (orig_crc != updcrc (outbuf, 0))
		gzip_error ("Dado comprimido inválido -- erro de CRC");

	if (orig_len != bytes_out)
		gzip_error ("Dado comprimido inválido -- erro de comprimento");

	/*
	 *	Check if there are more entries in a pkzip file
	 */
	if (pkzip && inptr + 4 < insize && LG (inbuf+inptr) == LOCSIG)
	{
		if (cflag)
		{
			fprintf
			(	stderr,
				  "Advertência: o arquivo ZIP tem mais de uma entrada -- ignorando o resto\n"
			);
		}
		else
		{
			/* Don't destroy the input zip file */

			gzip_error ("O arquivo ZIP tem mais de uma entrada");
		}
	}

	extended = pkzip = 0;	/* for next file */

}	/* end unzip */
