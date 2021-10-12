/*
 ****************************************************************
 *								*
 *			unpack.c				*
 *								*
 *	Decompress files in pack format				*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.0.0, de 18.06.93				*
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
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

/*
 *	Maximum length of Huffman codes. (Minor modifications to the code
 *	would be needed to support 32 bits codes, but pack never generates
 *	more than 24 bits anyway.)
 */
#define MAX_BITLEN 25

/*
 *	Number of literals, excluding the End of Block (EOB) code
 */
#define LITERALS 256

/*
 *	Maximum number of 'peek' bits used to optimize traversal of the
 *	Huffman tree.
 */
#define MAX_PEEK 12

local ulong	orig_len;		/* original uncompressed length */
local int	max_len;		/* maximum bit length of Huffman codes */

/*
 *	The literal bytes present in the Huffman tree.
 *	The EOB code is not represented.
 */
local char	literal[LITERALS];

/*
 *	All literals of a given bit length are contiguous in literal[] and
 *	have contiguous codes. literal[code+lit_base[len]] is the literal
 *	for a code of len bits.
 */
local int	lit_base[MAX_BITLEN+1];

local int	leaves [MAX_BITLEN+1]; /* Number of leaves for each bit length */
local int	parents[MAX_BITLEN+1]; /* Number of parents for each bit length */

local int	peek_bits; /* Number of peek bits currently used */

/*
 *	For each bit pattern b of peek_bits bits, prefix_len[b] is the length
 *	of the Huffman code starting with a prefix of b (upper bits), or 0
 *	if all codes of prefix b have more than peek_bits bits. It is not
 *	necessary to have a huge table (large MAX_PEEK) because most of the
 *	codes encountered in the input stream are short codes (by construction).
 *	So for most codes a single lookup will be necessary.
 */
local char	prefix_len[1 << MAX_PEEK];

/*
 *	Bits are added on the low part of bitbuf and read from the high part
 */
local ulong	bitbuf;

/*
 *	all bits above the last valid bit are always zero
 */
local int	valid;		/* number of valid bits in bitbuf */

/*
 *	Set code to the next 'bits' input bits without skipping them. code
 *	must be the name of a simple variable and bits must not have side effects.
 *	IN assertions: bits <= 25 (so that we still have room for an extra byte
 *	when valid is only 24), and mask = (1<<bits)-1.
 */
#define look_bits(code,bits,mask) \
{ \
  while (valid < (bits)) bitbuf = (bitbuf<<8) | (ulong)get_byte(), valid += 8; \
  code = (bitbuf >> (valid-(bits))) & (mask); \
}

/*
 *	Skip the given number of bits (after having peeked at them):
 */
#define skip_bits(bits)  (valid -= (bits))

#define clear_bitbuf() (valid = 0, bitbuf = 0)

/*
 *	Local functions
 */
local void	read_tree  OF((void));
local void	build_tree OF((void));

/*
 ****************************************************************
 *	Read the Huffman tree					*
 ****************************************************************
 */
local void
read_tree (void)
{
	int		len;	/* bit length */
	int		base;	/* base offset for a sequence of leaves */
	int		n;

	/*
	 *	Read the original input size, MSB first
	 */
	orig_len = 0;

	for (n = 1; n <= 4; n++)
		orig_len = (orig_len << 8) | (ulong)get_byte();

	/*
	 *	maximum bit length of Huffman codes
	 */
	max_len = (int)get_byte();

	if (max_len > MAX_BITLEN)
		gzip_error ("Dados comprimidos inválidos -- código de Huffman > 32 bits");

	/*
	 *	Get the number of leaves at each bit length
	 */
	n = 0;

	for (len = 1; len <= max_len; len++)
	{
		leaves[len] = (int)get_byte();
		n += leaves[len];
	}

	if (n > LITERALS)
		gzip_error ("Níveis excessivos na árvore de Huffman");

	/*
	 * There are at least 2 and at most 256 leaves of length max_len.
	 * (Pack arbitrarily rejects empty files and files consisting of
	 * a single byte even repeated.) To fit the last leaf count in a
	 * byte, it is offset by 2. However, the last literal is the EOB
	 * code, and is not transmitted explicitly in the tree, so we must
	 * adjust here by one only.
	 */
	leaves[max_len]++;

	/*
	 *	Now read the leaves themselves
	 */
	base = 0;

	/*
	 *	Remember where the literals of this length start in literal[]
	 */
	for (len = 1; len <= max_len; len++)
	{
		lit_base[len] = base;

		/* And read the literals: */

		for (n = leaves[len]; n > 0; n--)
			literal[base++] = (char)get_byte();
	}

	leaves[max_len]++; /* Now include the EOB code in the Huffman tree */

}	/* end read_tree */

/*
 ****************************************************************
 *	Build the Huffman tree and the prefix table		*
 ****************************************************************
 */
local void
build_tree (void)
{
	int	 nodes = 0;	/* number of nodes (parents+leaves) at current bit length */
	int	 len;		/* current bit length */
	char	 *prefixp;	/* pointer in prefix_len */

	for (len = max_len; len >= 1; len--)
	{
		/*
		 * The number of parent nodes at this level is half the total
		 * number of nodes at parent level:
		 */
		nodes >>= 1;
		parents[len] = nodes;

		/*
		 * Update lit_base by the appropriate bias to skip the parent nodes
		 * (which are not represented in the literal array):
		 */
		lit_base[len] -= nodes;

		/* Restore nodes to be parents+leaves: */

		nodes += leaves[len];
	}

	/*
	 * Construct the prefix table, from shortest leaves to longest ones.
	 * The shortest code is all ones, so we start at the end of the table.
	 */
	peek_bits = MIN(max_len, MAX_PEEK);
	prefixp = &prefix_len[1<<peek_bits];

	for (len = 1; len <= peek_bits; len++)
	{
		int prefixes = leaves[len] << (peek_bits-len); /* may be 0 */

		while (prefixes--) *--prefixp = (char)len;
	}

	/* The length of all other codes is unknown: */

	while (prefixp > prefix_len)
		*--prefixp = 0;

}	/* end build_tree */

/*
 ****************************************************************
 *	Unpack in to out					*
 ****************************************************************
 */
void
unpack (int in, int out)
{
	int		 len;	/* Bit length of current code */
	unsigned	 eob;	/* End Of Block code */
	register unsigned peek; /* lookahead bits */
	unsigned	 peek_mask;	/* Mask for peek_bits bits */

	ifd = in;
	ofd = out;

	/*
	 *	Read the Huffman tree
	 */
	read_tree ();

	/*
	 *	Build the prefix table
	 */
	build_tree ();

	/*
	 *	Initialize bit input
	 */
	clear_bitbuf (); 
	peek_mask = (1<<peek_bits)-1;

	/*
	 *	The eob code is the largest code among all leaves
	 *	of maximal length:
	 */
	eob = leaves[max_len]-1;

	/*
	 *	Decode the input data:
	 */
	for (EVER)
	{
		/*
		 * Since eob is the longest code and not shorter than max_len,
		 * we can peek at max_len bits without having the risk of reading
		 * beyond the end of file.
		 */
		look_bits(peek, peek_bits, peek_mask);
		len = prefix_len[peek];

		if (len > 0)
		{
			peek >>= peek_bits - len; /* discard the extra bits */
		}
		else
		{
			/* Code of more than peek_bits bits, we must traverse the tree */

			ulong mask = peek_mask;
			len = peek_bits;

			do
			{
				len++, mask = (mask<<1)+1;
				look_bits(peek, len, mask);
			}
			while (peek < parents[len]);

			/* loop as long as peek is a parent node */
		}

		/*
		 *	At this point, peek is the next complete code,
		 *	of len bits
		 */
		if (peek == eob && len == max_len) /* end of file? */
			break;

		put_char(literal[peek+lit_base[len]]);

		skip_bits(len);

	}	/* for (EVER) */

	flush_window ();

	if (orig_len != bytes_out)
		gzip_error ("Dado comprimidos inválidos -- erro de tamanho");

}	/* end unpack */
