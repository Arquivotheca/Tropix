/*
 ****************************************************************
 *								*
 *			unlzw.c					*
 *								*
 *	Decompress files in LZW format				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../h/gzip.h"
#include "../h/lzw.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
typedef	char		char_type;
#if (0)	/*******************************************************/
typedef	unsigned char	char_type;
#endif	/*******************************************************/
typedef		 long	code_int;
typedef unsigned long 	count_int;
typedef unsigned short	count_short;
typedef unsigned long 	cmp_code_int;

#define MAXCODE(n)	(1L << (n))

#ifndef	REGISTERS
#	define	REGISTERS	2
#endif
#define	REG1
#define	REG2
#define	REG3
#define	REG4
#define	REG5
#define	REG6
#define	REG7
#define	REG8
#define	REG9
#define	REG10
#define	REG11
#define	REG12
#define	REG13
#define	REG14
#define	REG15
#define	REG16
#if REGISTERS >= 1
#	undef	REG1
#	define	REG1	register
#endif
#if REGISTERS >= 2
#	undef	REG2
#	define	REG2	register
#endif
#if REGISTERS >= 3
#	undef	REG3
#	define	REG3	register
#endif
#if REGISTERS >= 4
#	undef	REG4
#	define	REG4	register
#endif
#if REGISTERS >= 5
#	undef	REG5
#	define	REG5	register
#endif
#if REGISTERS >= 6
#	undef	REG6
#	define	REG6	register
#endif
#if REGISTERS >= 7
#	undef	REG7
#	define	REG7	register
#endif
#if REGISTERS >= 8
#	undef	REG8
#	define	REG8	register
#endif
#if REGISTERS >= 9
#	undef	REG9
#	define	REG9	register
#endif
#if REGISTERS >= 10
#	undef	REG10
#	define	REG10	register
#endif
#if REGISTERS >= 11
#	undef	REG11
#	define	REG11	register
#endif
#if REGISTERS >= 12
#	undef	REG12
#	define	REG12	register
#endif
#if REGISTERS >= 13
#	undef	REG13
#	define	REG13	register
#endif
#if REGISTERS >= 14
#	undef	REG14
#	define	REG14	register
#endif
#if REGISTERS >= 15
#	undef	REG15
#	define	REG15	register
#endif
#if REGISTERS >= 16
#	undef	REG16
#	define	REG16	register
#endif

#define	BYTEORDER	0000

#ifndef	NOALLIGN
#	define	NOALLIGN	0
#endif


union	bytes {
	long  word;
	struct {
#if BYTEORDER == 4321
	char_type	b1;
	char_type	b2;
	char_type	b3;
	char_type	b4;
#else
#if BYTEORDER == 1234
	char_type	b4;
	char_type	b3;
	char_type	b2;
	char_type	b1;
#else
#if (0)	/*******************************************************/
#	undef	BYTEORDER
#endif	/*******************************************************/
	int  dummy;
#endif
#endif
	} bytes;
};

#if BYTEORDER == 4321 && NOALLIGN == 1
#  define input(b,o,c,n,m){ \
	 (c) = (*(long *)(&(b)[(o)>>3])>>((o)&0x7))&(m); \
	 (o) += (n); \
	}
#else
#  define input(b,o,c,n,m){ \
	 REG1 char_type *p = &(b)[(o)>>3]; \
	 (c) = ((((long)(p[0]))|((long)(p[1])<<8)| \
	 ((long)(p[2])<<16))>>((o)&0x7))&(m); \
	 (o) += (n); \
	}
#endif

#ifndef MAXSEG_64K
	/* DECLARE(ushort, tab_prefix, (1<<BITS)); -- prefix code */
#  define tab_prefixof(i) tab_prefix[i]
#  define clear_tab_prefixof()	memzero(tab_prefix, 256);
#else
	/* DECLARE(ushort, tab_prefix0, (1<<(BITS-1)); -- prefix for even codes */
	/* DECLARE(ushort, tab_prefix1, (1<<(BITS-1)); -- prefix for odd  codes */
	ushort *tab_prefix[2];
#  define tab_prefixof(i) tab_prefix[(i)&1][(i)>>1]
#  define clear_tab_prefixof()	\
	  memzero(tab_prefix0, 128), \
	  memzero(tab_prefix1, 128);
#endif
#define de_stack		((char_type *)(&d_buf[DIST_BUFSIZE-1]))
#define tab_suffixof(i) tab_suffix[i]

int block_mode = BLOCK_MODE; /* block compress mode -C compatible with 2.0 */

/*
 ****************************************************************
 *	Decompress in to out					*
 ****************************************************************
 */
void
unlzw (int in, int out)
{
	REG2	char_type  *stackp;
	REG3	code_int	code;
	REG4	int		finchar;
	REG5	code_int	oldcode;
	REG6	code_int	incode;
	REG7	long		inbits;
	REG8	long		posbits;
	REG9	int		outpos;
/*  REG10  int		insize; (global) */
	REG11  unsigned	bitmask;
	REG12  code_int	free_ent;
	REG13  code_int	maxcode;
	REG14  code_int	maxmaxcode;
	REG15  int		n_bits;
	REG16  int		rsize;

	maxbits = get_byte ();
	block_mode = maxbits & BLOCK_MODE;

	if ((maxbits & LZW_RESERVED) != 0)
	{
		fprintf
		(	stderr,
			"%s: %s: Advertência, indicador 0x%X desconhecido\n",
			pgname, ifname, maxbits & LZW_RESERVED
		);
	}

	maxbits &= BIT_MASK;
	maxmaxcode = MAXCODE (maxbits);

	if (maxbits > BITS)
	{
		fprintf
		(	stderr,
			"%s: %s: comprimido com %d bits, só aceito até %d bits\n",
			pgname, ifname, maxbits, BITS
		);

		exit_code = ERROR;
		return;
	}

	rsize = insize;
	maxcode = MAXCODE (n_bits = INIT_BITS)-1;
	bitmask = (1<<n_bits) - 1;
	oldcode = -1;
	finchar = 0;
	outpos = 0;
	posbits = inptr << 3;

	free_ent = ((block_mode) ? FIRST : 256);

	/*
	 *	Initialize the first 256 entries in the table
	 */
	clear_tab_prefixof ();

	for (code = 255 ; code >= 0 ; --code)
		tab_suffixof(code) = (char_type)code;

	do
	{
		REG1 int	i;
		int		e;
		int		o;

	    resetbuf:
		e = insize - (o = (posbits >> 3));

		for (i = 0 ; i < e ; ++i)
			inbuf[i] = inbuf[i+o];

		insize = e;
		posbits = 0;

		if (insize < INBUF_EXTRA)
		{
			if ((rsize = read(in, inbuf+insize, INBUFSIZ)) == EOF)
				read_error ();

			insize += rsize;
		}

		inbits = ((rsize != 0) ? ((long)insize - insize%n_bits)<<3 :
					  ((long)insize<<3)-(n_bits-1));

		while (inbits > posbits)
		{
			if (free_ent > maxcode)
			{
				posbits = ((posbits-1) +
				((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
				++n_bits;

				if (n_bits == maxbits)
					maxcode = maxmaxcode;
				else
					maxcode = MAXCODE(n_bits)-1;

				bitmask = (1<<n_bits)-1;
				goto resetbuf;
			}

			input (inbuf, posbits, code, n_bits, bitmask);

			if (oldcode == -1)
			{
				outbuf[outpos++] = (char_type)(finchar = (int)(oldcode=code));
				continue;
			}

			if (code == CLEAR && block_mode)
			{
				clear_tab_prefixof();
				free_ent = FIRST - 1;
				posbits = ((posbits-1) +
					((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
				maxcode = MAXCODE(n_bits = INIT_BITS)-1;
				bitmask = (1<<n_bits)-1;
				goto resetbuf;
			}

			incode = code;
			stackp = de_stack;

			/*
			 *	Special case for KwKwK string
			 */
			if (code >= free_ent)
			{
				if (code > free_ent)
				{
					REG1 char_type	*p;

					posbits -= n_bits;
					p = &inbuf[posbits>>3];

					if (!tflag && outpos > 0)
						write_buf(out, outbuf, outpos);

					gzip_error ("Entrada corrompida. Use \"zcat\" para recuperar parte dos dados.");
				}

				*--stackp = (char_type)finchar;

				code = oldcode;
			}

			/*
			 *	Generate output characters in reverse order
			 */
			while ((cmp_code_int)code >= (cmp_code_int)256)
			{
				*--stackp = tab_suffixof(code);
				code = tab_prefixof(code);
			}

			*--stackp = (char_type)(finchar = tab_suffixof(code));

			/*
			 *	And put them out in forward order
			 */
			{
				REG1 int	k;

				if (outpos+(k = (de_stack-stackp)) >= OUTBUFSIZ)
				{
					do
					{
						if (k > OUTBUFSIZ-outpos)
							k = OUTBUFSIZ-outpos;

						if (k > 0)
						{
							memmove (outbuf+outpos, stackp, k);
							outpos += k;
						}

						if (outpos >= OUTBUFSIZ)
						{
							if (!tflag)
								write_buf (out, outbuf, outpos);
							outpos = 0;
						}

						stackp+= k;

					}
					while ((k = (de_stack-stackp)) > 0);

				}
				else
				{
					memmove(outbuf+outpos, stackp, k);
					outpos += k;
				}
			}

			/*
			 *	Generate the new entry
			 */
			if ((code = free_ent) < maxmaxcode)
			{
				tab_prefixof(code) = (unsigned short)oldcode;
				tab_suffixof(code) = (char_type)finchar;
				free_ent = code+1;
			}

			oldcode = incode;	/* Remember previous code */
		}

		bytes_in += rsize;

	}
	while (rsize != 0);

	if (!tflag && outpos > 0)
		write_buf (out, outbuf, outpos);

}	/* end unlzw */
