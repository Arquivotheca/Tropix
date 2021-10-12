/*
 ****************************************************************
 *								*
 *			inflate.c				*
 *								*
 *	Inflate deflated (PKZIP's method 8 compressed) data	*
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
#include <stdlib.h>
#include <string.h>
#include "../h/gzip.h"

#define slide window

/*
 ****************************************************************
 *	Áreas e definições globais				*
 ****************************************************************
 */
typedef struct huft	HUFT;

struct huft
{
	char	e;	/* number of extra bits or operation */
	char	b;	/* number of bits in this code or subcode */
    union
    {
	ushort	n;	/* literal, length base, or distance base */
	HUFT	*t;	/* pointer to next level of table */
    } v;

};

/*
 ******	Protótipos de funções ***********************************
 */
int huft_build (unsigned *, unsigned, unsigned, ushort *, ushort *,
					HUFT **, int *);
int huft_free (HUFT *);
int inflate_codes (HUFT *, HUFT *, int, int);
int inflate_stored (void);
int inflate_fixed (void);
int inflate_dynamic (void);
int inflate_block (int *);
int inflate (void);


/* unsigned wp;			current position in slide */

#define wp outcnt

#define flush_output(w) (wp=(w),flush_window())

/*
 ****** Tables for deflate from PKZIP's appnote.txt *************
 */
static unsigned	border[] = 	/* Order of the bit length code lengths */
{
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

static ushort	cplens[] = 	/* Copy lengths for literal codes 257..285 */
{
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

static ushort	cplext[] = 	/* Extra bits for literal codes 257..285 */
{
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99  /* 99==invalid */
};

static ushort	cpdist[] = 	/* Copy offsets for distance codes 0..29 */
{
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
		8193, 12289, 16385, 24577
};

static ushort	cpdext[] = 	/* Extra bits for distance codes */
{
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
		12, 12, 13, 13
};

/*
 ******	Macros de entrada ***************************************
 */
ulong		bb;		/* bit buffer */

unsigned	bk;		/* bits in bit buffer */

ushort mask_bits[] =
{
	0x0000,
	0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

#define NEXTBYTE()	(char)get_byte()

#define NEEDBITS(n) {while(k<(n)){b|=((ulong)NEXTBYTE())<<k;k+=8;}}
#define DUMPBITS(n) {b>>=(n);k-=(n);}

/*
 ******	Árvores de Huffman **************************************
 */
int		lbits = 9;	/* bits in base literal/length lookup table */
int		dbits = 6;	/* bits in base distance lookup table */

/*
 *	If BMAX needs to be larger than 16, then h and x[] should be ulong
 */
#define BMAX 16		/* maximum bit length of any code (16 for explode) */
#define N_MAX 288	/* maximum number of codes in any set */

unsigned	hufts;	/* track memory usage */

/*
 ****************************************************************
 *	Make a set of tables to decode that set of codes	*
 ****************************************************************
 */
int
huft_build (unsigned *b,  unsigned n,  unsigned s,  ushort *d,  ushort *e, 
					HUFT **t,  int *m)
{
	unsigned	a;		/* counter for codes of length k */
	unsigned	c[BMAX+1];	/* bit length count table */
	unsigned	f;		/* i repeats in table every f entries */
	int		g;		/* maximum code length */
	int		h;		/* table level */
	register unsigned i;		/* counter, current code */
	register unsigned j;		/* counter */
	register int	k;		/* number of bits in current code */
	int		l;		/* bits per table (returned in m) */
	register unsigned *p;		/* pointer into c[], b[], or v[] */
	register HUFT	*q;	/* points to current table */
	HUFT		r;		/* table entry for structure assignment */
	HUFT		*u[BMAX];	/* table stack */
	unsigned	v[N_MAX];	/* values in order of bit length */
	register int	w;		/* bits before this table == (l * h) */
	unsigned	x[BMAX+1];	/* bit offsets, then code stack */
	unsigned	*xp;		/* pointer into x */
	int		y;		/* number of dummy codes added */
	unsigned	z;		/* number of entries in current table */

	/*
	 *	Generate counts for each bit length
	 */
	memzero (c, sizeof (c));
	p = b;	i = n;

	do
	{
		c[*p++]++;		/* assume all entries <= BMAX */
	}
	while (--i);

	if (c[0] == n)			/* null input--all zero length codes */
	{
		*t = (HUFT *)NULL;
		*m = 0;
		return 0;
	}

	/*
	 *	Find minimum and maximum length, bound *m by those
	 */
	l = *m;

	for (j = 1; j <= BMAX; j++)
	{
		if (c[j])
			break;
	}

	k = j;				/* minimum code length */

	if ((unsigned)l < j)
		l = j;

	for (i = BMAX; i; i--)
	{
		if (c[i])
			break;
	}

	g = i;				/* maximum code length */
	if ((unsigned)l > i)
	l = i;
	*m = l;

	/*
	 *	Adjust last length count to fill out codes, if needed
	 */
	for (y = 1 << j; j < i; j++, y <<= 1)
	{
		if ((y -= c[j]) < 0)
			return 2;	/* bad input: more codes than bits */
	}

	if ((y -= c[i]) < 0)
		return 2;
	c[i] += y;

	/*
	 *	Generate starting offsets into the value table for each length
	 */
	x[1] = j = 0;
	p = c + 1;	xp = x + 2;

	while (--i)
	{			/* note that i == g from above */
		*xp++ = (j += *p++);
	}

	/*
	 *	Make a table of values in order of bit lengths
	 */
	p = b;	i = 0;

	do
	{
		if ((j = *p++) != 0)
			v[x[j]++] = i;
	}
	while (++i < n);

	/*
	 *	Generate the Huffman codes and for each, make the table entries
	 */
	x[0] = i = 0;		/* first Huffman code is zero */
	p = v;			/* grab values in bit order */
	h = -1;			/* no tables yet--level -1 */
	w = -l;			/* bits decoded == (l * h) */
	u[0] = (HUFT *)NULL;	/* just to keep compilers happy */
	q = (HUFT *)NULL;	/* ditto */
	z = 0;				/* ditto */

	/*
	 *	go through the bit lengths (k already is bits in shortest code)
	 */
	for (; k <= g; k++)
	{
		a = c[k];

		/*
		 *	Here i is the Huffman code of length k bits for
	 	 *	value *p make tables up to required level
		 */
		while (a--)
		{
			while (k > w + l)
			{
				h++;
				w += l;	/* previous table always l bits */

				/*
				 *	compute minimum size table less
				 *	than or equal to l bits
				 */
				z = (z = g - w) > (unsigned)l ? l : z;

				/*
				 *	Try a k-w bit table
				 *	Too few codes for k-w bit table
				 */
				if ((f = 1 << (j = k - w)) > a + 1)
				{
					f -= a + 1;
					xp = c + k;

					while (++j < z)
					{
						if ((f <<= 1) <= *++xp)
							break;

						f -= *xp;			/* else deduct codes from patterns */
					}
				}

				/*
				 *	table entries for j-bit table
				 */
				z = 1 << j;

				/*
				 *	allocate and link in new table
				 */
				if ((q = malloc ((z + 1)*sizeof(HUFT))) == (HUFT *)NULL)
				{
					if (h)
					huft_free(u[0]);
					return 3;	/* not enough memory */
				}

				hufts += z + 1;		/* track memory usage */
				*t = q + 1;		/* link to list for huft_free() */
				*(t = &(q->v.t)) = (HUFT *)NULL;
				u[h] = ++q;		/* table starts after link */

				/*
				 *	connect to last table, if there is one
				 */
				if (h)
				{
					x[h] = i;		/* save pattern for backing up */
					r.b = (char)l;		/* bits to dump before this table */
					r.e = (char)(16 + j);	/* bits in this table */
					r.v.t = q;		/* pointer to this table */
					j = i >> (w - l);	/* (get around Turbo C bug) */
					u[h-1][j] = r;		/* connect to last table */
				}
			}

			/*
			 *	set up table entry in r
			 */
			r.b = (char)(k - w);

			if   (p >= v + n)
			{
				r.e = 99;	/* out of values--invalid code */
			}
			elif (*p < s)
			{
				r.e = (char)(*p < 256 ? 16 : 15);/* 256 is end-of-block code */
				r.v.n = *p++;			/* simple code is just the value */
			}
			else
			{
				r.e = (char)e[*p - s];	/* non-simple--look up in lists */
				r.v.n = d[*p++ - s];
			}

			/*
			 *	fill code-like entries with r
			 */
			f = 1 << (k - w);
			for (j = i >> w; j < z; j += f)
				q[j] = r;

			/*
			 *	backwards increment the k-bit code i
			 */
			for (j = 1 << (k - 1); i & j; j >>= 1)
				i ^= j;
			i ^= j;

			/*
			 *	backup over finished tables
			 */
			while ((i & ((1 << w) - 1)) != x[h])
			{
				h--;				/* don't need to update q */
				w -= l;
			}
		}
	}

	/*
	 *	Return true (1) if we were given an incomplete table
	 */
	return (y != 0 && g != 1);

}	/* end huft_build */

/*
 ****************************************************************
 *	Free the malloc'ed tables built by huft_build ()	*	
 ****************************************************************
 */
int
huft_free (HUFT *t)
{
	register HUFT	*p, *q;

	/*
	 *	Go through linked list, freeing from the
	 *	malloced (t[-1]) address
	 */
	p = t;

	while (p != (HUFT *)NULL)
	{
		q = (--p)->v.t;
		free(p);
		p = q;
	} 

	return 0;

}	/* end huft_free */

/*
 ****************************************************************
 *	Inflate (decompress) the codes in a deflated block	*
 ****************************************************************
 */
int
inflate_codes (register HUFT *tl, HUFT *td, int bl, int bd)
{
	register unsigned e;	/* table entry flag/number of extra bits */
	unsigned	n, d;	/* length and index for copy */
	unsigned	w;	/* current window position */
	register HUFT	*t;	/* pointer to table entry */
	register unsigned ml, md; /* masks for bl and bd bits */
	register ulong	b;	/* bit buffer */
	register unsigned k;	/* number of bits in bit buffer */

	/*
	 *	make local copies of globals
	 */
	b = bb;			/* initialize bit buffer */
	k = bk;
	w = wp;			/* initialize window position */

	/*
	 *	inflate the coded data
	 */
	ml = mask_bits[bl];	/* precompute masks for speed */
	md = mask_bits[bd];

	/*
	 *	do until end of block
	 */
	for (EVER)
	{
		NEEDBITS((unsigned)bl)

		if ((e = (t = tl + ((unsigned)b & ml))->e) > 16)
		{
			do
			{
				if (e == 99)
					return 1;

				DUMPBITS(t->b)
				e -= 16;
				NEEDBITS(e)

			}
			while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
		}

		DUMPBITS(t->b)

		if (e == 16)		/* then it's a literal */
		{
			slide[w++] = (char)t->v.n;

			if (w == WSIZE)
			{
				flush_output(w);
				w = 0;
			}
		}
		else			/* it's an EOB or a length */
		{
			/*
			 *	exit if end of block
			 */
			if (e == 15)
				break;

			/*
			 *	get length of block to copy
			 */
			NEEDBITS(e)
			n = t->v.n + ((unsigned)b & mask_bits[e]);
			DUMPBITS(e);

			/*
			 *	decode distance of block to copy
			 */
			NEEDBITS((unsigned)bd)

			if ((e = (t = td + ((unsigned)b & md))->e) > 16)
			{
				do
				{
					if (e == 99)
						return 1;

					DUMPBITS(t->b)
					e -= 16;
					NEEDBITS(e)

				}
				while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
			}

			DUMPBITS(t->b)
			NEEDBITS(e)
			d = w - t->v.n - ((unsigned)b & mask_bits[e]);
			DUMPBITS(e)

			/*
			 *	do the copy
			 */
			do
			{
				n -= (e = (e = WSIZE - ((d &= WSIZE-1) > w ? d : w)) > n ? n : e);

				if (w - d >= e)		/* (this test assumes unsigned comparison) */
				{
					memmove (slide + w, slide + d, e);
					w += e;
					d += e;
				}
				else						/* do it slow to avoid memmove() overlap */
				{
					do
					{
						slide[w++] = slide[d++];
					}
					while (--e);
				}

				if (w == WSIZE)
				{
					flush_output(w);
					w = 0;
				}
			}
			while (n);
		}
	}

	/*
	 *	restore the globals from the locals
	 */
	wp = w;		/* restore global window pointer */
	bb = b;		/* restore global bit buffer */
	bk = k;

	/*
	 *	done
	 */
	return 0;

}	/* end inflate_codes */

/*
 ****************************************************************
 *	"Decompress" an inflated type 0 (stored) block		*
 ****************************************************************
 */
int
inflate_stored (void)
{
	register unsigned n;	/* number of bytes in block */
	register unsigned w;	/* current window position */
	register ulong	b;	/* bit buffer */
	register unsigned k;	/* number of bits in bit buffer */

	/*
	 *	make local copies of globals
	 */
	b = bb;		/* initialize bit buffer */
	k = bk;
	w = wp;		/* initialize window position */

	/*
	 *	go to byte boundary
	 */
	n = k & 7;
	DUMPBITS(n);

	/*
	 *	get the length and its complement
	 */
	NEEDBITS(16)
	n = ((unsigned)b & 0xffff);
	DUMPBITS(16)
	NEEDBITS(16)

	if (n != (unsigned)((~b) & 0xffff))
		return 1;		/* error in compressed data */
	DUMPBITS(16)

	/*
	 *	read and output the compressed data
	 */
	while (n--)
	{
		NEEDBITS(8)
		slide[w++] = (char)b;

		if (w == WSIZE)
		{
			flush_output(w);
			w = 0;
		}

		DUMPBITS(8)
	}

	/*
	 *	restore the globals from the locals
	 */
	wp = w;		/* restore global window pointer */
	bb = b;		/* restore global bit buffer */
	bk = k;

	return (0);

}	/* end inflate_stored */

/*
 ****************************************************************
 *  Decompress an inflated type 1 (fixed Huffman codes) block	*
 ****************************************************************
 */
int
inflate_fixed (void)
{
	register int	i;		/* temporary variable */
	HUFT		*tl;		/* literal/length code table */
	HUFT		*td;		/* distance code table */
	int		bl;		/* lookup bits for tl */
	int		bd;		/* lookup bits for td */
	unsigned	l[288];		/* length list for huft_build */

	/*
	 *	set up literal table
	 */
	for (i = 0; i < 144; i++)
		l[i] = 8;

	for (; i < 256; i++)
		l[i] = 9;

	for (; i < 280; i++)
		l[i] = 7;

	for (; i < 288; i++)	/* make a complete, but wrong code set */
		l[i] = 8;

	bl = 7;

	if ((i = huft_build(l, 288, 257, cplens, cplext, &tl, &bl)) != 0)
		return i;

	/*
	 *	set up distance table
	 */
	for (i = 0; i < 30; i++)	/* make an incomplete code set */
		l[i] = 5;

	bd = 5;

	if ((i = huft_build(l, 30, 0, cpdist, cpdext, &td, &bd)) > 1)
	{
		huft_free(tl);
		return i;
	}

	/*
	 *	decompress until an end-of-block code
	 */
	if (inflate_codes (tl, td, bl, bd))
		return 1;

	/*
	 *	free the decoding tables, return
	 */
	huft_free (tl);
	huft_free (td);

	return (0);

}	/* end inflate_fixed */

/*
 ****************************************************************
 * Decompress an inflated type 2 (dynamic Huffman codes) block	*
 ****************************************************************
 */
int
inflate_dynamic (void)
{
	register int	i;		/* temporary variables */
	register unsigned j;
	register unsigned l;		/* last length */
	unsigned	m;		/* mask for bit lengths table */
	unsigned	n;		/* number of lengths to get */
	HUFT		*tl;		/* literal/length code table */
	HUFT		*td;		/* distance code table */
	int		bl;		/* lookup bits for tl */
	int		bd;		/* lookup bits for td */
	register unsigned nb;		/* number of bit length codes */
	unsigned	nl;		/* number of literal/length codes */
	unsigned	nd;		/* number of distance codes */
#ifdef PKZIP_BUG_WORKAROUND
	unsigned	ll[288+32];	/* literal/length and distance code lengths */
#else
	unsigned	ll[286+30];	/* literal/length and distance code lengths */
#endif
	register	ulong b;	/* bit buffer */
	register	unsigned k;	/* number of bits in bit buffer */

	/*
	 *	make local bit buffer
	 */
	b = bb;
	k = bk;

	/*
	 *	read in table lengths
	 */
	NEEDBITS(5)
	nl = 257 + ((unsigned)b & 0x1f);	/* number of literal/length codes */
	DUMPBITS(5)

	NEEDBITS(5)
	nd = 1 + ((unsigned)b & 0x1f);		/* number of distance codes */
	DUMPBITS(5)

	NEEDBITS(4)
	nb = 4 + ((unsigned)b & 0xf);		/* number of bit length codes */
	DUMPBITS(4)

#ifdef PKZIP_BUG_WORKAROUND
	if (nl > 288 || nd > 32)
#else
	if (nl > 286 || nd > 30)
#endif
		return (1);			/* bad lengths */

	/*
	 *	read in bit-length-code lengths
	 */
	for (j = 0; j < nb; j++)
	{
		NEEDBITS(3)
		ll[border[j]] = (unsigned)b & 7;
		DUMPBITS(3)
	}

	for (; j < 19; j++)
		ll[border[j]] = 0;

	/*
	 *	build decoding table for trees--single level, 7 bit lookup
	 */
	bl = 7;

	if ((i = huft_build(ll, 19, 19, (ushort *)NULL, (ushort *)NULL, &tl, &bl)) != 0)
	{
		if (i == 1)
			huft_free(tl);
		return i;		/* incomplete code set */
	}

	/*
	 *	read in literal and distance code lengths
	 */
	n = nl + nd;
	m = mask_bits[bl];
	i = l = 0;

	while ((unsigned)i < n)
	{
		NEEDBITS((unsigned)bl)
		j = (td = tl + ((unsigned)b & m))->b;
		DUMPBITS(j)
		j = td->v.n;

		if (j < 16)		/* length of code in bits (0..15) */
		{
			ll[i++] = l = j; /* save last length in l */
		}
		else if (j == 16)	/* repeat last length 3 to 6 times */
		{
			NEEDBITS(2)
			j = 3 + ((unsigned)b & 3);
			DUMPBITS(2)

			if ((unsigned)i + j > n)
				return 1;
			while (j--)
				ll[i++] = l;
		}
		else if (j == 17)	/* 3 to 10 zero length codes */
		{
			NEEDBITS(3)
			j = 3 + ((unsigned)b & 7);
			DUMPBITS(3)

			if ((unsigned)i + j > n)
				return 1;

			while (j--)
				ll[i++] = 0;
			l = 0;
		}
		else			/* j == 18: 11 to 138 zero length codes */
		{
			NEEDBITS(7)
			j = 11 + ((unsigned)b & 0x7f);
			DUMPBITS(7)

			if ((unsigned)i + j > n)
				return 1;

			while (j--)
				ll[i++] = 0;
			l = 0;
		}

	}	/* end while */


	/*
	 *	free decoding table for trees
	 */
	huft_free (tl);

	/*
	 *	restore the global bit buffer
	 */
	bb = b;
	bk = k;

	/*
	 *	build the decoding tables for literal/length and distance codes
	 */
	bl = lbits;

	if ((i = huft_build(ll, nl, 257, cplens, cplext, &tl, &bl)) != 0)
	{
		if (i == 1)
		{
			fprintf (stderr, " árvore literal incompleta\n");
			huft_free(tl);
		}

		return i;		/* incomplete code set */
	}

	bd = dbits;

	if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &td, &bd)) != 0)
	{
		if (i == 1)
		{
			fprintf(stderr, " árvore de distâncias incompleta\n");
#ifdef PKZIP_BUG_WORKAROUND
			i = 0;
		}
#else
			huft_free(td);
		}

		huft_free(tl);

		return i;	/* incomplete code set */
#endif
	}

	/*
	 *	decompress until an end-of-block code
	 */
	if (inflate_codes(tl, td, bl, bd))
		return 1;

	/*	
	 * 	free the decoding tables, return
	 */
	huft_free(tl);
	huft_free(td);

	return (0);

}	/* end inflate_dynamic */

/*
 ****************************************************************
 *	Decompress an inflated block				*
 ****************************************************************
 */
int
inflate_block (int *e)
{
	unsigned	t;	/* block type */
	register ulong	b;	/* bit buffer */
	register unsigned k;	/* number of bits in bit buffer */

	/*
	 *	make local bit buffer
	 */
	b = bb;
	k = bk;

	/*
	 *	read in last block bit
	 */
	NEEDBITS(1)
	*e = (int)b & 1;
	DUMPBITS(1)

	/*
	 *	read in block type
	 */
	NEEDBITS(2)
	t = (unsigned)b & 3;
	DUMPBITS(2)

	/*
	 *	restore the global bit buffer
	 */
	bb = b;
	bk = k;

	/*
	 *	inflate that block type
	 */
	if (t == 2)
		return inflate_dynamic();

	if (t == 0)
		return inflate_stored();

	if (t == 1)
		return inflate_fixed();

	/*
	 *	bad block type
	 */
	return (2);

}	/* end inflate_block */

/*
 ****************************************************************
 *	Decompress an inflated entry				*
 ****************************************************************
 */
int
inflate (void)
{
	int		e;	/* last block flag */
	register int	r;	/* result code */
	register unsigned h;	/* maximum HUFT's malloc'ed */

	/*
	 *	initialize window, bit buffer
	 */
	wp = 0;
	bk = 0;
	bb = 0;

	/*
	 *	decompress until the last block
	 */
	h = 0;

	do
	{
		hufts = 0;

		if ((r = inflate_block(&e)) != 0)
			return r;

		if (hufts > h)
			h = hufts;
	}
	while (!e);

	/*
	 *	Undo too much lookahead. The next read will be byte
	 *	aligned so we can discard unused bits in the last
	 *	meaningful byte
	 */
	while (bk >= 8)
	{
		bk -= 8;
		inptr--;
	}

	/*
	 *	flush out slide
	 */
	flush_output (wp);

	/*
	 *	return success
	 */
	return (0);

}	/* end inflate */
