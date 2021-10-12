/*
 ****************************************************************
 *								*
 *			trees.c					*
 *								*
 *	Output deflated data using Huffman coding		*
 *								*
 *	Versão	3.0.00, de 05.06.93				*
 *		3.1.06, de 04.05.97				*
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#include "../h/common.h"
#include "../h/deflate.h"

/*
 ****************************************************************
 *	Áreas e definições globais				*
 ****************************************************************
 */
#define MAX_BITS 15	/* All codes must not exceed MAX_BITS bits */

#define MAX_BL_BITS 7	/* Bit length codes must not exceed MAX_BL_BITS bits */

#define LENGTH_CODES 29 /* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256	/* number of literal bytes 0..255 */

#define END_BLOCK 256	/* end of block literal code */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES	30	/* number of distance codes */

#define BL_CODES  19	/* number of codes used to transfer the bit lengths */

/*
 *	Index within the heap array of least frequent node in the Huffman tree
 */
#define SMALLEST	1

/*
 *	extra bits for each length code
 */
const int	extra_lbits[LENGTH_CODES] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 0
};

/*
 *	extra bits for each distance code
 */
const int	extra_dbits[D_CODES] =
{
	0,  0,  0,  0,  1,  1,  2,  2,
	3,  3,  4,  4,  5,  5,  6,  6,
	7,  7,  8,  8,  9,  9, 10,  10,
	11, 11, 12, 12, 13, 13
};

/*
 *	extra bits for each bit length code
 */
const int	extra_blbits[BL_CODES] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 3, 7
};

/*
 * The three kinds of block type
 */
#define STORED_BLOCK	0
#define STATIC_TREES	1
#define DYN_TREES	2

#define LIT_BUFSIZE  0x8000

#ifndef DIST_BUFSIZE
#  define DIST_BUFSIZE  LIT_BUFSIZE
#endif

#if LIT_BUFSIZE > INBUFSIZ
	error cannot overlay l_buf and inbuf
#endif

#define REP_3_6		16
/* repeat previous bit length 3-6 times (2 bits of repeat count) */

#define REPZ_3_10	17
/* repeat a zero length 3-10 times  (3 bits of repeat count) */

#define REPZ_11_138	18
/* repeat a zero length 11-138 times  (7 bits of repeat count) */

/*
 ******	Um valor e seu código ***********************************
 */
typedef struct ct_data
{
	union
	{
		ushort  freq;		/* frequency count */
		ushort  code;		/* bit string */
	} fc;

	union
	{
		ushort  dad;		/* father node in Huffman tree */
		ushort  len;		/* length of bit string */
	} dl;

}	CT_DATA;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

#define HEAP_SIZE	(2 * L_CODES + 1)	/* maximum heap size */

entry CT_DATA	*dyn_ltree;		/* literal and length tree */
entry CT_DATA	*dyn_dtree;		/* distance tree */

/*
 *	The static literal tree. Since the bit lengths are imposed,
 *	there is no need for the L_CODES extra codes used during heap
 *	construction. However the codes 286 and 287 are needed to build
 *	a canonical tree (see ct_init below).
 */
entry CT_DATA	*static_ltree;

/*
 *	The static distance tree.
 *	(Actually a trivial tree since all codes use 5 bits.)
 */
entry CT_DATA	*static_dtree;

/*
 *	Huffman tree for the bit lengths
 */
entry CT_DATA	*bl_tree;

typedef struct tree_desc
{
	CT_DATA		*dyn_tree;	/* the dynamic tree */
	CT_DATA		*static_tree;	/* corresponding static tree or NULL */
	const int	*extra_bits;	/* extra bits for each code or NULL */
	int		extra_base;	/* base index for extra_bits */
	int		elems;		/* max number of elements in the tree */
	int		max_length;	/* max bit length for the codes */
	int		max_code;	/* largest code with non zero frequency */

}	TREE_DESC;

entry TREE_DESC	l_desc =
{
	NOVOID, 	/*** == dyn_ltree ***/
	NOVOID,		/*** == static_ltree ***/
	extra_lbits,
	LITERALS+1,
	L_CODES,
	MAX_BITS,
	0
};

entry TREE_DESC	d_desc =
{
	NOVOID, 	/*** == dyn_dtree ***/
	NOVOID,		/*** == static_dtree ***/
	extra_dbits,
	0,
	D_CODES,
	MAX_BITS,
	0
};

entry TREE_DESC	bl_desc =
{
	NOVOID,		/*** == bl_tree ***/
	(CT_DATA *)0,
	extra_blbits,
	0,
	BL_CODES,
	MAX_BL_BITS,
	0
};

/* number of codes at each bit length for an optimal tree */
entry ushort	bl_count[MAX_BITS + 1];

/*
 *	The lengths of the bit length codes are sent in order of decreasing
 *	probability, to avoid transmitting the lengths for unused bit
 *	length codes.
 */
const char	bl_order[BL_CODES] =
{
	16, 17, 18, 0,  8, 7,  9, 6,
	10,  5, 11, 4, 12, 3, 13, 2,
	14,  1, 15
};

/*
 *	The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
 *	The same heap array is used to build all trees.
 */
entry int	*heap;			/* heap used to build the Huffman trees */
entry int	heap_len;		/* number of elements in the heap */
entry int	heap_max;		/* element of largest frequency */

/*
 *	Depth of each subtree used as tie breaker for trees of equal frequency
 */
entry char	*depth;

/*
 *	length code for each normalized match length (0 == MIN_MATCH)
 */
entry char	*length_code;

/*
 * distance codes. The first 256 values correspond to the distances
 * 3 .. 258, the last 256 values correspond to the top 8 bits of
 * the 15 bit distances.
 */
entry char	*dist_code;

/*
 *	First normalized length for each code (0 = MIN_MATCH)
 */
entry int	base_length[LENGTH_CODES];

/*
 *	First normalized distance for each code (0 = distance of 1)
 */
entry int	base_dist[D_CODES];

/*
 *	buffer for literals or lengths
 */
/* DECLARE (char, l_buf, LIT_BUFSIZE); */

#define l_buf	inbuf

/*
 *	buffer for distances
 */
/* DECLARE (ushort, d_buf, DIST_BUFSIZE); */

/*
 *	flag_buf is a bit array distinguishing literals from lengths in
 *	l_buf, thus indicating the presence or absence of a distance.
 */
entry char	*flag_buf;

/*
 *	bits are filled in new_flags starting at bit 0 (least significant).
 *	Note: these flags are overkill in the current code since we don't
 *	take advantage of DIST_BUFSIZE == LIT_BUFSIZE.
 */
entry unsigned	last_lit;	/* running index in l_buf */
entry unsigned	last_dist;	/* running index in d_buf */
entry unsigned	last_flags;	/* running index in flag_buf */
entry char	new_flags;	/* current flags not yet saved in flag_buf */
entry char	flag_bit;	/* current bit used in flags */

entry ulong	opt_len;	/* bit length of current block with optimal trees */
entry ulong	static_len;	/* bit length of current block with static trees */

entry ulong	compressed_len;	/* total bit length of compressed file */

/*
 *	input_len is for debugging only since we can get it by other means.
 */
entry ulong	input_len;	/* total byte length of input file */

/*
 ******	Protótipos de funções ***********************************
 */
void		init_block (void);
void		pqdownheap (CT_DATA *tree, int k);
void		gen_bitlen (TREE_DESC *desc);
void		gen_codes (CT_DATA *tree, int max_code);
void		build_tree (TREE_DESC *desc);
void		scan_tree (CT_DATA *tree, int max_code);
void		send_tree (CT_DATA *tree, int max_code);
int 		build_bl_tree (void);
void		send_all_trees (int lcodes, int dcodes, int blcodes);
void		compress_block (CT_DATA *ltree, CT_DATA *dtree);

/*
 *	Send a code of the given tree. c and tree must not have side effects
 */
#define send_code(c, tree) send_bits (tree[c].Code, tree[c].Len)

/*
 *	Mapping from a distance to a distance code. dist is the distance - 1
 *	and must not have side effects. dist_code[256] and dist_code[257]
 *	are never used.
 */
#define d_code(dist)	\
	((dist) < 256 ? dist_code[dist] : dist_code[256 + ((dist) >> 7)])

/*
 *	the arguments must not have side effects
 */
#define MAX(a,b) (a >= b ? a : b)

/*
 ****************************************************************
 *	Allocate the match buffer				*
 ****************************************************************
 */
void
ct_init (void)
{
	int		n;		/* iterates over tree elements */
	int		bits;	 	/* bit counter */
	int		length;		/* length value */
	int		code;	 	/* code value */
	int		dist;		/* distance index */

	compressed_len = input_len = 0;
		
	/*
	 *	ct_init already called?
	 */
	if (static_dtree != NOVOID)
		return;

	/*
	 *	Aloca as áreas necessárias
	 */
	if ((dyn_ltree = malloc (HEAP_SIZE * sizeof (CT_DATA))) == NOVOID)
		error (NOSTR);

	if ((dyn_dtree = malloc ((2 * D_CODES + 1) * sizeof (CT_DATA))) == NOVOID)
		error (NOSTR);

	if ((static_ltree = malloc ((L_CODES + 2) * sizeof (CT_DATA))) == NOVOID)
		error (NOSTR);

	if ((static_dtree = malloc (D_CODES * sizeof (CT_DATA))) == NOVOID)
		error (NOSTR);

	if ((bl_tree = malloc ((2 * BL_CODES + 1) * sizeof (CT_DATA))) == NOVOID)
		error (NOSTR);

	if ((heap = malloc ((2 * L_CODES + 1) * sizeof (int))) == NOVOID)
		error (NOSTR);

	if ((depth = malloc ((2 * L_CODES + 1) * sizeof (char))) == NOSTR)
		error (NOSTR);

	if ((length_code = malloc (MAX_MATCH - MIN_MATCH + 1)) == NOSTR)
		error (NOSTR);

	if ((dist_code = malloc (512)) == NOSTR)
		error (NOSTR);

	if ((flag_buf = malloc (LIT_BUFSIZE / 8)) == NOSTR)
		error (NOSTR);

	/*
	 *	Não esquece de inicializar os ponteiros
	 */
	l_desc.dyn_tree = dyn_ltree;
	l_desc.static_tree = static_ltree;

	d_desc.dyn_tree = dyn_dtree;
	d_desc.static_tree = static_dtree;

	bl_desc.dyn_tree = bl_tree;

	/*
	 *	Initialize the mapping length (0..255) -> length code (0..28)
	 */
	for (length = 0, code = 0; code < LENGTH_CODES-1; code++)
	{
		base_length[code] = length;

		for (n = 0; n < (1 << extra_lbits[code]); n++)
			length_code[length++] = code;
	}

	/*
	 *	Note that the length 255 (match length 258) can be represented
	 *	in two different ways: code 284 + 5 bits or code 285, so we
	 *	overwrite length_code[255] to use the best encoding:
	 */
	length_code[length-1] = code;

	/*
	 *	Initialize the mapping dist (0..32K) -> dist code (0..29)
	 */
	for (dist = 0, code = 0 ; code < 16; code++)
	{
		base_dist[code] = dist;

		for (n = 0; n < (1 << extra_dbits[code]); n++)
			dist_code[dist++] = code;
	}

	/*
	 *	from now on, all distances are divided by 128
	 */
	dist >>= 7;

	for (/* vazio */; code < D_CODES; code++)
	{
		base_dist[code] = dist << 7;

		for (n = 0; n < (1 << (extra_dbits[code]-7)); n++)
			dist_code[256 + dist++] = code;
	}

	/*
	 *	Construct the codes of the static literal tree
	 */
	for (bits = 0; bits <= MAX_BITS; bits++)
		bl_count[bits] = 0;

	n = 0;

	while (n <= 143)
		static_ltree[n++].Len = 8, bl_count[8]++;

	while (n <= 255)
		static_ltree[n++].Len = 9, bl_count[9]++;

	while (n <= 279)
		static_ltree[n++].Len = 7, bl_count[7]++;

	while (n <= 287)
		static_ltree[n++].Len = 8, bl_count[8]++;

	/*
	 *	Codes 286 and 287 do not exist, but we must include them in the
	 *	tree construction to get a canonical Huffman tree (longest code
	 *	all ones)
	 */
	gen_codes (static_ltree, L_CODES + 1);

	/*
	 *	The static distance tree is trivial:
	 */
	for (n = 0; n < D_CODES; n++)
	{
		static_dtree[n].Len = 5;
		static_dtree[n].Code = bi_reverse (n, 5);
	}

	/*
	 *	Initialize the first block of the first file:
	 */
	init_block ();

}	/* end ct_init */

/*
 ****************************************************************
 *	Initialize a new block					*
 ****************************************************************
 */
void
init_block (void)
{
	int		 n;	/* iterates over tree elements */

	/*
	 *	Initialize the trees.
	 */
	for (n = 0; n < L_CODES;  n++)
		dyn_ltree[n].Freq = 0;

	for (n = 0; n < D_CODES;  n++)
		dyn_dtree[n].Freq = 0;

	for (n = 0; n < BL_CODES; n++)
		bl_tree[n].Freq = 0;

	dyn_ltree[END_BLOCK].Freq = 1;
	opt_len = static_len = 0;
	last_lit = last_dist = last_flags = 0;
	new_flags = 0; flag_bit = 1;

}	/* end init_block */

/*
 ****************************************************************
 *	Restore the heap					*
 ****************************************************************
 */
void
pqdownheap (CT_DATA *tree, int k)
{
	int		v = heap[k];
	int		j = k << 1;	  /* left son of k */

	while (j <= heap_len)
	{
		/* Set j to the smallest of the two sons: */

		if
		(	j < heap_len &&
			(tree[heap[j+1]].Freq < tree[heap[j]].Freq ||
			(tree[heap[j+1]].Freq == tree[heap[j]].Freq &&
			depth[heap[j+1]] <= depth[heap[j]]))
		)
			j++;

		/* Exit if v is smaller than both sons */

		if
		(	(tree[v].Freq < tree[heap[j]].Freq ||
			(tree[v].Freq == tree[heap[j]].Freq &&
			depth[v] <= depth[heap[j]]))
		)
			break;

		/* Exchange v with the smallest son */

		heap[k] = heap[j];	k = j;

		/* And continue down the tree, setting j to the left son of k */

		j <<= 1;
	}

	heap[k] = v;

}	/* end pqdownheap */

/*
 ****************************************************************
 *	Compute the optimal bit lengths for a tree		*
 ****************************************************************
 */
void
gen_bitlen (TREE_DESC *desc)
{
	CT_DATA		*tree = desc->dyn_tree;
	const int	*extra = desc->extra_bits;
	int		base = desc->extra_base;
	int		max_code = desc->max_code;
	int		max_length = desc->max_length;
	CT_DATA		*stree = desc->static_tree;
	int		h;	/* heap index */
	int		n, m;	/* iterate over the tree elements */
	int		bits;	/* bit length */
	int		xbits; 	/* extra bits */
	ulong		f;	/* frequency */
	int		overflow = 0; /* number of elements with bit length too large */

	for (bits = 0; bits <= MAX_BITS; bits++)
		bl_count[bits] = 0;

	/*
	 *	In a first pass, compute the optimal bit lengths (which may
	 *	overflow in the case of the bit length tree).
	 */
	tree[heap[heap_max]].Len = 0; /* root of the heap */

	for (h = heap_max+1; h < HEAP_SIZE; h++)
	{
		n = heap[h];
		bits = tree[tree[n].Dad].Len + 1;

		if (bits > max_length)
			{ bits = max_length; overflow++; }

		tree[n].Len = bits;

		/* We overwrite tree[n].Dad which is no longer needed */

		if (n > max_code)	/* not a leaf node */
			continue;

		bl_count[bits]++;
		xbits = 0;

		if (n >= base)
			xbits = extra[n-base];

		f = tree[n].Freq;
		opt_len += f * (bits + xbits);

		if (stree)
			static_len += f * (stree[n].Len + xbits);
	}

	if (overflow == 0)
		return;

	/* This happens for example on obj2 and pic of the Calgary corpus */

	/*
	 *	Find the first bit length which could increase:
	 */
	do
	{
		bits = max_length-1;

		while (bl_count[bits] == 0)
			bits--;

		bl_count[bits]--; 	/* move one leaf down the tree */
		bl_count[bits+1] += 2;	/* move one overflow item as its brother */
		bl_count[max_length]--;

		/*
		 *	The brother of the overflow item also moves one step up,
		 *	but this does not affect bl_count[max_length]
		 */
		overflow -= 2;

	}
	while (overflow > 0);

	/*
	 *	Now recompute all bit lengths, scanning in increasing frequency.
	 *	h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
	 *	lengths instead of fixing only the wrong ones. This idea is
	 *	taken from 'ar' written by Haruhiko Okumura.)
	 */
	for (bits = max_length; bits != 0; bits--)
	{
		n = bl_count[bits];

		while (n != 0)
		{
			m = heap[--h];

			if (m > max_code)
				continue;

			if (tree[m].Len != bits)
			{
				opt_len += (bits - tree[m].Len) * tree[m].Freq;

				tree[m].Len = bits;
			}

			n--;
		}
	}

}	/* end gen_bitlen */

/*
 ****************************************************************
 *	Generate the codes for a given tree			*
 ****************************************************************
 */
void
gen_codes (CT_DATA *tree, int max_code)
{
	ushort		next_code[MAX_BITS+1]; /* next code value for each bit length */
	ushort		code = 0;	 /* running code value */
	int		bits;		 /* bit index */
	int		n;		 /* code index */

	/*
	 *	The distribution counts are first used to generate the
	 *	code values without bit reversal.
	 */
	for (bits = 1; bits <= MAX_BITS; bits++)
		next_code[bits] = code = (code + bl_count[bits - 1]) << 1;

	/*
	 *	Check that the bit counts in bl_count are consistent.
	 *	The last code must be all ones.
	 */
	for (n = 0;  n <= max_code; n++)
	{
		int len = tree[n].Len;

		if (len == 0)
			continue;

		/* Now reverse the bits */

		tree[n].Code = bi_reverse (next_code[len]++, len);

	}

}	/* end gen_codes */

/*
 ****************************************************************
 *	Construct one Huffman tree				*
 ****************************************************************
 */
void
build_tree (TREE_DESC *desc)
{
	CT_DATA 	*tree	= desc->dyn_tree;
	CT_DATA		*stree = desc->static_tree;
	int		elems	= desc->elems;
	int		n, m;		/* iterate over heap elements */
	int		max_code = -1;	/* largest code with non zero frequency */
	int		node = elems;	/* next internal node of the tree */

	/*
	 *	Construct the initial heap, with least frequent element in
	 *	heap[SMALLEST]. The sons of heap[n] are heap[2*n] and
	 *	heap[2*n+1] heap[0] is not used.
	 */
	heap_len = 0, heap_max = HEAP_SIZE;

	for (n = 0; n < elems; n++)
	{
		if (tree[n].Freq != 0)
		{
			heap[++heap_len] = max_code = n;
			depth[n] = 0;
		}
		else
		{
			tree[n].Len = 0;
		}
	}

	/*
	 *	The pkzip format requires that at least one distance code
	 *	exists, and that at least one bit should be sent even if
	 *	there is only one possible code. So to avoid special checks
	 *	later on we force at least two codes of non zero frequency.
	 */
	while (heap_len < 2)
	{
		int new = heap[++heap_len] = (max_code < 2 ? ++max_code : 0);
		tree[new].Freq = 1;
		depth[new] = 0;
		opt_len--;

		if (stree)
			static_len -= stree[new].Len;

		/* new is 0 or 1 so it does not have extra bits */
	}

	desc->max_code = max_code;

	/*
	 *	The elements heap[heap_len/2+1 .. heap_len] are leaves
	 *	of the tree, establish sub-heaps of increasing lengths:
	 */
	for (n = heap_len/2; n >= 1; n--)
		pqdownheap (tree, n);

	/*
	 *	Construct the Huffman tree by repeatedly combining
	 *	the least two frequent nodes.
	 */
	do
	{
		n = heap[SMALLEST];	/* n = node of least frequency */
		heap[SMALLEST] = heap[heap_len--];
		pqdownheap (tree, SMALLEST);

		m = heap[SMALLEST];  /* m = node of next least frequency */

		heap[--heap_max] = n; /* keep the nodes sorted by frequency */
		heap[--heap_max] = m;

		/* Create a new node father of n and m */

		tree[node].Freq = tree[n].Freq + tree[m].Freq;
		depth[node] = (char) (MAX(depth[n], depth[m]) + 1);
		tree[n].Dad = tree[m].Dad = node;

		/* and insert the new node in the heap */

		heap[SMALLEST] = node++;
		pqdownheap(tree, SMALLEST);

	}
	while (heap_len >= 2);

	heap[--heap_max] = heap[SMALLEST];

	/*
	 *	At this point, the fields freq and dad are set. We can now
	 *	generate the bit lengths.
	 */
	gen_bitlen (desc);

	/* The field len is now set, we can generate the bit codes */

	gen_codes (tree, max_code);

}	/* end build_tree */

/*
 ****************************************************************
 *	Scan a literal or distance tree				*
 ****************************************************************
 */
void
scan_tree (CT_DATA *tree, int max_code)
{
	int		n;			/* iterates over all tree elements */
	int		prevlen = -1;		/* last emitted length */
	int		curlen;			/* length of current code */
	int		nextlen = tree[0].Len;	/* length of next code */
	int		count = 0;		/* repeat count of the current code */
	int		max_count = 7;		/* max repeat count */
	int		min_count = 4;		/* min repeat count */

	if (nextlen == 0)
		{ max_count = 138; min_count = 3; }

	tree[max_code+1].Len = 0xFFFF;	/* guard */

	for (n = 0; n <= max_code; n++)
	{
		curlen = nextlen; nextlen = tree[n+1].Len;

		if   (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		elif (count < min_count)
		{
			bl_tree[curlen].Freq += count;
		}
		elif (curlen != 0)
		{
			if (curlen != prevlen)
				bl_tree[curlen].Freq++;

			bl_tree[REP_3_6].Freq++;
		}
		elif (count <= 10)
		{
			bl_tree[REPZ_3_10].Freq++;
		}
		else
		{
			bl_tree[REPZ_11_138].Freq++;
		}

		count = 0; prevlen = curlen;

		if   (nextlen == 0)
		{
			max_count = 138, min_count = 3;
		}
		elif (curlen == nextlen)
		{
			max_count = 6, min_count = 3;
		}
		else
		{
			max_count = 7, min_count = 4;
		}
	}

}	/* end scan_tree */

/*
 ****************************************************************
 *	Send a literal or distance tree in compressed form	*
 ****************************************************************
 */
void
send_tree (CT_DATA *tree, int max_code)
{
	int		n;			/* iterates over all tree elements */
	int		prevlen = -1;		/* last emitted length */
	int		curlen;			/* length of current code */
	int		nextlen = tree[0].Len;	/* length of next code */
	int		count = 0;		/* repeat count of the current code */
	int		max_count = 7;		/* max repeat count */
	int		min_count = 4;		/* min repeat count */

   /*** tree[max_code+1].Len = -1; ***/  /* guard already set */

	if (nextlen == 0)
		{ max_count = 138; min_count = 3; }

	for (n = 0; n <= max_code; n++)
	{
		curlen = nextlen; nextlen = tree[n+1].Len;

		if   (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		elif (count < min_count)
		{
			do
			{	send_code (curlen, bl_tree);
			}
			while (--count != 0);
		}
		elif (curlen != 0)
		{
			if (curlen != prevlen)
				{ send_code (curlen, bl_tree); count--; }

			send_code (REP_3_6, bl_tree); send_bits (count-3, 2);

		}
		elif (count <= 10)
		{
			send_code (REPZ_3_10, bl_tree); send_bits (count-3, 3);
		}
		else
		{
			send_code(REPZ_11_138, bl_tree); send_bits (count-11, 7);
		}

		count = 0; prevlen = curlen;

		if   (nextlen == 0)
		{
			max_count = 138; min_count = 3;
		}
		elif (curlen == nextlen)
		{
			max_count = 6; min_count = 3;
		}
		else
		{
			max_count = 7; min_count = 4;
		}
	}

}	/* end send_tree */

/*
 ****************************************************************
 *	Construct the Huffman tree for the bit lengths		*
 ****************************************************************
 */
int
build_bl_tree (void)
{
	int		max_blindex;  /* index of last bit length code of non zero freq */

	/*
	 *	Determine the bit length frequencies for literal
	 *	and distance trees
	 */
	scan_tree (dyn_ltree, l_desc.max_code);
	scan_tree (dyn_dtree, d_desc.max_code);

	/*
	 *	Build the bit length tree:
	 */
	build_tree (&bl_desc);

	/*
	 *	opt_len now includes the length of the tree representations,
	 *	except the lengths of the bit lengths codes and the 5+5+4
	 *	bits for the counts.
	 */

	/*
	 *	Determine the number of bit length codes to send.
	 *	The pkzip format requires that at least 4 bit length codes
	 *	be sent. (appnote.txt says 3 but the actual value used is 4.)
	 */
	for (max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex--)
	{
		if (bl_tree[bl_order[max_blindex]].Len != 0)
			break;
	}

	/* Update opt_len to include the bit length tree and counts */

	opt_len += 3 * (max_blindex + 1) + 5 + 5 + 4;

	return (max_blindex);

}	/* end build_bl_tree */

/*
 ****************************************************************
 *	Send the header for a block using dynamic Huffman trees	*
 ****************************************************************
 */
void
send_all_trees (int lcodes, int dcodes, int blcodes)
{
	int		rank;		/* index in bl_order */

	send_bits (lcodes - 257, 5); /* not +255 as stated in appnote.txt */
	send_bits (dcodes-1, 5);
	send_bits (blcodes - 4,  4); /* not -3 as stated in appnote.txt */

	for (rank = 0; rank < blcodes; rank++)
	{
		send_bits (bl_tree[bl_order[rank]].Len, 3);
	}

	/*
	 *	send the literal tree
	 */
	send_tree (dyn_ltree, lcodes-1);

	/*
	 *	send the distance tree
	 */
	send_tree (dyn_dtree, dcodes-1);

}	/* end send_all_trees */

/*
 ****************************************************************
 *	Determine the best encoding for the current block	*
 ****************************************************************
 */
ulong
flush_block (char *buf, ulong stored_len, int eof)
{
	ulong		opt_lenb;	/* opt_len in bytes */
	ulong		static_lenb;	/* static_len in bytes */
	int		max_blindex;	/* index of last bit length code of non zero freq */

	flag_buf[last_flags] = new_flags; /* Save the flags for the last 8 items */

	/*
	 *	Construct the literal and distance trees
	 */
	build_tree (&l_desc);
	build_tree (&d_desc);

	/*
	 *	At this point, opt_len and static_len are the total
	 *	bit lengths of the compressed block data, excluding
	 *	the tree representations.
	 */

	/*
	 *	Build the bit length tree for the above two trees,
	 *	and get the index in bl_order of the last bit length
	 *	code to send.
	 */
	max_blindex = build_bl_tree ();

	/*
	 *	Determine the best encoding. Compute first the block
	 *	length in bytes
	 */
	opt_lenb = (opt_len + 3 + 7) >> 3;
	static_lenb = (static_len + 3 + 7) >> 3;

	input_len += stored_len;	/* for debugging only */

	if (static_lenb <= opt_lenb)
		opt_lenb = static_lenb;

	/*
	 *	4: two words for the lengths
	 */
	if (stored_len + 4 <= opt_lenb && buf != NOSTR)
	{
		/*
		 *	The test buf != NULL is only necessary if
		 *	LIT_BUFSIZE > WSIZE. Otherwise we can't have
		 *	processed more than WSIZE input bytes since
		 *	the last block flush, because compression would
		 *	have been successful. If LIT_BUFSIZE <= WSIZE,
		 *	it is never too late to transform a block into
		 *	a stored block.
		 */
		send_bits ((STORED_BLOCK << 1) + eof, 3);  /* send block type */
		compressed_len = (compressed_len + 3 + 7) & ~7;
		compressed_len += (stored_len + 4) << 3;

		copy_block (buf, (unsigned)stored_len, 1); /* with header */
	}
	elif (static_lenb == opt_lenb)
	{
		send_bits ((STATIC_TREES << 1) + eof, 3);
		compress_block (static_ltree, static_dtree);
		compressed_len += 3 + static_len;
	}
	else
	{
		send_bits ((DYN_TREES << 1) + eof, 3);
		send_all_trees (l_desc.max_code + 1, d_desc.max_code + 1, max_blindex + 1);
		compress_block (dyn_ltree, dyn_dtree);
		compressed_len += 3 + opt_len;
	}

	init_block ();

	if (eof)
	{
		bi_windup ();
		compressed_len += 7;  /* align on byte boundary */
	}

	return (compressed_len >> 3);

}	/* end flush_block */
 
/*
 ****************************************************************
 *	Save the match info and tally the frequency counts	*
 ****************************************************************
 */
int
ct_tally (int dist, int lc)
{
	l_buf[last_lit++] = lc;

	if (dist == 0)
	{
		/* lc is the unmatched char */

		dyn_ltree[lc].Freq++;
	}
	else
	{
		/* Here, lc is the match length - MIN_MATCH */

		dist--;			 /* dist = match distance - 1 */

		dyn_ltree[length_code[lc] + LITERALS + 1].Freq++;
		dyn_dtree[d_code (dist)].Freq++;

		d_buf[last_dist++] = dist;
		new_flags |= flag_bit;
	}

	flag_bit <<= 1;

	/*
	 *	Output the flags if they fill a byte:
	 */
	if ((last_lit & 7) == 0)
	{
		flag_buf[last_flags++] = new_flags;
		new_flags = 0; flag_bit = 1;
	}

	/*
	 *	Try to guess if it is profitable to stop the current block here
	 */
	if (ZIP_LEVEL > 2 && (last_lit & 0xFFF) == 0)
	{
		/* Compute an upper bound for the compressed length */

		ulong	out_length = last_lit * 8;
		ulong	in_length = strstart - block_start;
		int	dcode;

		for (dcode = 0; dcode < D_CODES; dcode++)
		{
			out_length += dyn_dtree[dcode].Freq * (5 + extra_dbits[dcode]);
		}

		out_length >>= 3;

		if (last_dist < last_lit/2 && out_length < in_length/2)
			return 1;
	}

	return (last_lit == LIT_BUFSIZE-1 || last_dist == DIST_BUFSIZE);

	/*
	 *	We avoid equality with LIT_BUFSIZE because of wraparound at 64K
	 *	on 16 bit machines and because stored blocks are restricted to
	 *	64K-1 bytes.
	 */

}	/* end ct_tally */

/*
 ****************************************************************
 *	Send the block data compressed using the Huffman trees	*
 ****************************************************************
 */
void
compress_block (CT_DATA *ltree, CT_DATA *dtree)
{
	unsigned	dist;		/* distance of matched string */
	int		lc;		/* match length or unmatched char (if dist == 0) */
	unsigned	lx = 0;	/* running index in l_buf */
	unsigned	dx = 0;	/* running index in d_buf */
	unsigned	fx = 0;	/* running index in flag_buf */
	char		flag = 0;	/* current flags */
	unsigned	code;		/* the code to send */
	int		extra;		/* number of extra bits to send */

	if (last_lit != 0) do
	{
		if ((lx & 7) == 0)
			flag = flag_buf[fx++];

		lc = l_buf[lx++];

		if ((flag & 1) == 0)
		{
			send_code (lc, ltree); /* send a literal byte */
		}
		else	/* Here, lc is the match length - MIN_MATCH */
		{
			code = length_code[lc];
			send_code (code + LITERALS + 1, ltree); /* send the length code */
			extra = extra_lbits[code];

			if (extra != 0)
			{
				lc -= base_length[code];
				send_bits (lc, extra);	/* send the extra length bits */
			}

			dist = d_buf[dx++];

			/* Here, dist is the match distance - 1 */

			code = d_code (dist);

			send_code (code, dtree);	/* send the distance code */
			extra = extra_dbits[code];

			if (extra != 0)
			{
				dist -= base_dist[code];
				send_bits (dist, extra);	/* send the extra distance bits */
			}

		}	/* end literal or match pair ? */

		flag >>= 1;

	}
	while (lx < last_lit);

	send_code (END_BLOCK, ltree);

}	/* end compress_block */
