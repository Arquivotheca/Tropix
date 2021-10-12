/*
 ****************************************************************
 *								*
 *			deflate.c				*
 *								*
 *	Compress data using the deflation algorithm		*
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

#include <stdio.h>
#include <string.h>

#include "../h/gzip.h"
#include "../h/lzw.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define HASH_BITS  15

/* To save space (see unlzw.c), we overlay prev+head with tab_prefix and
 * window with tab_suffix. Check that we can do this:
 */
#if WSIZE<<1 > 1<<BITS
#	error: cannot overlay window with tab_suffix and prev with tab_prefix0
#endif

#if HASH_BITS > BITS-1
#	error: cannot overlay head with tab_prefix1
#endif

/* HASH_SIZE and WSIZE must be powers of two */

#define HASH_SIZE	(unsigned)(1 << HASH_BITS)
#define HASH_MASK	(HASH_SIZE - 1)
#define WMASK		(WSIZE - 1)

#define NIL	0 		/* Tail of hash chains */

#define FAST 4 		/* speed options for the general purpose bit flag */
#define SLOW 2

/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */

#define TOO_FAR 4096

/* ===========================================================================
 * Local data used by the "longest match" routines.
 */

/* DECLARE(char, window, 2L*WSIZE); */

/* Sliding window. Input bytes are read into the second half of the window,
 * and move to the first half later to keep a dictionary of at least WSIZE
 * bytes. With this organization, matches are limited to a distance of
 * WSIZE-MAX_MATCH bytes, but this ensures that IO is always
 * performed with a length multiple of the block size. Also, it limits
 * the window size to 64K, which is quite useful on MSDOS.
 * To do: limit the window size to WSIZE+BSZ if SMALL_MEM (the code would
 * be less efficient).
 */

/* DECLARE(ushort, prev, WSIZE); */
/* Link to older string with same hash index. To limit the size of this
 * array to 64K, this link is maintained only for the last 32K strings.
 * An index in this array is thus a window index modulo 32K.
 */

/* DECLARE(ushort, head, 1<<HASH_BITS); */
/* Heads of the hash chains or NIL. */

ulong		window_size = 2 * WSIZE;

/* window size, 2*WSIZE except for MMAP or BIG_MEM, where it is the
 * input file length plus MIN_LOOKAHEAD.
 */

long		block_start;
/* window position at the beginning of the current output block. Gets
 * negative when the window is moved backwards.
 */

local unsigned	ins_h;  /* hash index of string to be inserted */

#define H_SHIFT  ((HASH_BITS + MIN_MATCH - 1) / MIN_MATCH)

/* Number of bits by which ins_h and del_h must be shifted at each
 * input step. It must be such that after MIN_MATCH steps, the oldest
 * byte no longer takes part in the hash key, that is:
 *	H_SHIFT * MIN_MATCH >= HASH_BITS
 */

unsigned int	prev_length;
/* Length of the best match at previous step. Matches not greater than this
 * are discarded. This is used in the lazy match evaluation.
 */

unsigned	strstart;	/* start of string to insert */
unsigned	match_start;	/* start of matching string */
local int	eofile;		/* flag set at end of input file */
local unsigned	lookahead;	/* number of valid bytes ahead in window */

unsigned	max_chain_length;
/* To speed up deflation, hash chains are never searched beyond this length.
 * A higher limit improves compression ratio but degrades the speed.
 */

local unsigned	int max_lazy_match;
/* Attempt to find a better match only when the current match is strictly
 * smaller than this value.
 */

int		good_match;
/* Use a faster search when the previous match is longer than this */


/* Values for max_lazy_match, good_match and max_chain_length, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
typedef struct config
{
	ushort good_length; 	/* reduce lazy search above this match length */
	ushort max_lazy;	/* do not perform lazy search above this match length */
	ushort nice_length; 	/* quit search above this match length */
	ushort max_chain;

}	CONFIG;

#ifdef  FULL_SEARCH
# define nice_match MAX_MATCH
#else
  int	 nice_match; /* Stop searching when current match exceeds this */
#endif

/*
 ******	Tabela de configuração (em função do nível) *************
 */
local const CONFIG	configuration_table[10] =
{
	/*	      good    lazy	nice	chain */

	/* 0 */		0,	0,	0,	0,	/* store only */
	/* 1 */ 	4,	4,	16,	16,	/* maximum speed */
	/* 2 */ 	6,	8,	16,	16,
	/* 3 */ 	8,	16,	32,	32,
	/* 4 */ 	8,	16,	64,	64,
	/* 5 */ 	8,	16,	128,	128,
	/* 6 */ 	8,	32,	128,	256,
	/* 7 */ 	8,	64,	128,	512,
	/* 8 */ 	32,	128,	258,	1024,
	/* 9 */ 	32,	258,	258,	4096	/* maximum compression */

};

/* Note: the current code requires max_lazy >= MIN_MATCH and max_chain >= 4
 * but these restrictions can easily be removed at a small cost.
 */

/*
 ******	Protótipos de funções ***********************************
 */
local void	fill_window (void);
int		longest_match (ulong cur_match);

/* ===========================================================================
 * Update a hash value with the given input byte
 * IN  assertion: all calls to to UPDATE_HASH are made with consecutive
 *	input characters, so that a running hash key can be computed from the
 *	previous key instead of complete recalculation each time.
 */
#if (0)	/*************************************/
#define UPDATE_HASH(h, c) (h = (((h) << H_SHIFT) ^ (c)) & HASH_MASK)
#endif	/*************************************/

/* ===========================================================================
 * Insert string s in the dictionary and set match_head to the previous head
 * of the hash chain (the most recent string with same hash key). Return
 * the previous length of the hash chain.
 * IN  assertion: all calls to to INSERT_STRING are made with consecutive
 *	input characters and the first MIN_MATCH bytes of s are valid
 *	(except for the last MIN_MATCH-1 bytes of the input file).
 */
#if (0)	/*************************************/
#define INSERT_STRING(s, match_head) \
	(UPDATE_HASH(ins_h, window[(s) + MIN_MATCH-1]), \
	prev[(s) & WMASK] = match_head = head[ins_h], \
	head[ins_h] = (s))
#endif	/*************************************/

/*
 ****************************************************************
 *	Initialize the "longest match" routines for a new file	*
 ****************************************************************
 */
void
lm_init (int pack_level, ushort *flags)
{
	register unsigned	j;

	if (pack_level < 1 || pack_level > 9)
		gzip_error ("bad pack level");

	/* Initialize the hash table. */

	memsetl ((long *)head, 0, (HASH_SIZE * sizeof (*head)) / sizeof (long));

#if (0)	/*************************************/
printf ("%d\n", (HASH_SIZE * sizeof (*head)) / sizeof (long));

	memzero (head, HASH_SIZE * sizeof (*head));
#endif	/*************************************/

	/* prev will be initialized on the fly */

	/*
	 *	Atribui os valores de configuração
	 */
	max_lazy_match	 = configuration_table[pack_level].max_lazy;
	good_match	 = configuration_table[pack_level].good_length;
#ifndef FULL_SEARCH
	nice_match	 = configuration_table[pack_level].nice_length;
#endif
	max_chain_length = configuration_table[pack_level].max_chain;

	if   (pack_level == 1)
		*flags |= FAST;
	elif (pack_level == 9)
		*flags |= SLOW;

	/* ??? reduce max_chain_length for binary files */

	strstart = 0;
	block_start = 0;

	lookahead = file_read (window, 2 * WSIZE);

	if (lookahead == 0 || lookahead == EOF)
	{
		eofile = 1, lookahead = 0;
		return;
	}

	eofile = 0;

	/*
	 *	Make sure that we always have enough lookahead.
	 *	This is important if input comes from a device such as a tty.
	 */
	while (lookahead < MIN_LOOKAHEAD && !eofile)
		fill_window ();

	ins_h = 0;

	for (j = 0; j < MIN_MATCH - 1; j++)
		ins_h = ((ins_h << H_SHIFT) ^ window[j]) & HASH_MASK;

#if (0)	/*************************************/
	for (j = 0; j < MIN_MATCH-1; j++)
		UPDATE_HASH (ins_h, window[j]);
#endif	/*************************************/

	/*
	 *	If lookahead < MIN_MATCH, ins_h is garbage, but this is
	 *	not important since only literal bytes will be emitted.
	 */

}	/* end lm_init */

#ifndef ASMV
/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
/* ===========================================================================
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 * string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 */
int
longest_match (unsigned long cur_match) /* current match */
{
	unsigned chain_length = max_chain_length; /* max hash chain length */
	register char *scan = window + strstart;	/* current string */
	register char *match;			/* matched string */
	register int len;			/* length of current match */
	int best_len = prev_length;		 /* best match length so far */
	unsigned limit = strstart > MAX_DIST ? strstart - MAX_DIST : NIL;
	register char *strend = window + strstart + MAX_MATCH;
	register char scan_end1  = scan[best_len-1];
	register char scan_end	= scan[best_len];


	/*
	 *	Stop when cur_match becomes <= limit. To simplify the code,
	 *	we prevent matches with the string of window index 0.
	 */

	/*
	 *	The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2
	 *	multiple of 16.
	 *	It is easy to get rid of this optimization if necessary.
	 */

#if HASH_BITS < 8 || MAX_MATCH != 258
#	error: Code too clever
#endif

	/* Do not waste too much time if we already have a good match: */

	if (prev_length >= good_match)
		chain_length >>= 2;

	Assert (strstart <= window_size-MIN_LOOKAHEAD, "insufficient lookahead");

	do {
		Assert(cur_match < strstart, "no future");

		match = window + cur_match;

		/*
		 *	Skip to next match if the match length
		 *	cannot increase or if the match length is less than 2:
		 */
		if
		(	match[best_len]	!= scan_end  ||
			match[best_len-1] != scan_end1 ||
			*match	!= *scan ||
			*++match  != scan[1]
		)
			continue;

		/*
		 *	The check at best_len-1 can be removed because
		 *	it will be made again later. (This heuristic is
		 *	not always a win.) It is not necessary to compare
		 *	scan[2] and match[2] since they are always equal
		 *	when the other bytes match, given that the hash
		 *	keys are equal and that HASH_BITS >= 8.
		 */
		scan += 2, match++;

		/*
		 *	We check for insufficient lookahead only
		 *	every 8th comparison; the 256th check will
		 *	be made at strstart+258.
		 */
		while
		(	*++scan == *++match && *++scan == *++match &&
			 *++scan == *++match && *++scan == *++match &&
			 *++scan == *++match && *++scan == *++match &&
			 *++scan == *++match && *++scan == *++match &&
			 scan < strend
		)
			/* vazio */;

		len = MAX_MATCH - (strend - scan);
		scan = strend - MAX_MATCH;

		if (len > best_len)
		{
			match_start = cur_match;
			best_len = len;

			if (len >= nice_match)
				break;

			scan_end1  = scan[best_len-1];
			scan_end	= scan[best_len];
		}
	}
	while ((cur_match = prev[cur_match & WMASK]) > limit  && --chain_length != 0);

	return (best_len);

}	/* end longest_match */
#endif /* ASMV */

/*
 ****************************************************************
 *	Fill the window when the lookahead becomes insufficient	*
 ****************************************************************
 */
/* ===========================================================================
 * Fill the window when the lookahead becomes insufficient.
 * Updates strstart and lookahead, and sets eofile if end of input file.
 * IN assertion: lookahead < MIN_LOOKAHEAD && strstart + lookahead > 0
 * OUT assertions: at least one byte has been read, or eofile is set;
 *	file reads are performed for at least two bytes (required for the
 *	translate_eol option).
 */
local void
fill_window (void)
{
	register unsigned	n, m;
	register unsigned	more = (window_size - lookahead - strstart);
	/* Amount of free space at the end of the window. */

	/* If the window is almost full and there is insufficient lookahead,
	 * move the upper half to the lower one to make room in the upper half.
	 */
	if (more == EOF)
	{
		/*
		 *	Very unlikely, but possible on 16 bit machine
		 *	if strstart == 0 and lookahead == 1 (input done
		 *	one byte at time)
		 */
		more--;
	}
	elif (strstart >= WSIZE+MAX_DIST)
	{
		/*
		 *	By the IN assertion, the window is not empty so
		 *	we can't confuse more == 0 with more == 64K
		 *	on a 16 bit machine.
		 */
		Assert (window_size == 2 * WSIZE, "no sliding with BIG_MEM");

		memmove (window, window + WSIZE, WSIZE);
		match_start -= WSIZE;
		strstart -= WSIZE; /* we now have strstart >= MAX_DIST: */

		block_start -= WSIZE;

		for (n = 0; n < HASH_SIZE; n++)
		{
			m = head[n];

			head[n] = (m >= WSIZE ? m - WSIZE : NIL);
		}

		for (n = 0; n < WSIZE; n++)
		{
			m = prev[n];

			prev[n] = (m >= WSIZE ? m - WSIZE : NIL);
			/*
			 *	If n is not on any hash chain, prev[n]
			 *	is garbage but its value will never be used.
			 */
		}

		more += WSIZE;
	}

	/* At this point, more >= 2 */

	if (!eofile)
	{
		n = file_read (window + strstart + lookahead, more);

		if (n == 0 || n == EOF)
			eofile = 1;
		else
			lookahead += n;
	}

}	/* end fill_window */

/* ===========================================================================
 * Flush the current block, with given end-of-file flag.
 * IN assertion: strstart is set to the end of the current match.
 */
#if (0)	/*************************************/
#define FLUSH_BLOCK(eof) \
	flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : \
		(char*)NULL, (long)strstart - block_start, (eof))
#endif	/*************************************/

/*
 ****************************************************************
 * Processes a new input file and return its compressed length	*
 ****************************************************************
 */
ulong
deflate (void)
{
	register ulong	hash_head;	/* head of hash chain */
	register ulong	prev_match;	/* previous match */
	register int	flush;		/* set if current block must be flushed */
	register int	match_available = 0; /* set if previous match exists */
	register unsigned match_length = MIN_MATCH-1; /* length of best match */
#ifdef DEBUG
	extern long	isize;		/* byte length of input file, for debug only */
#endif

	/*
	 *	Processa o bloco de entrada
	 */
	while (lookahead != 0)
	{
		/*
		 *	Insert the string window[strstart .. strstart+2]
		 *	in the dictionary, and set hash_head to the head
		 *	of the hash chain:
		 */
#if (0)	/*************************************/
		INSERT_STRING (strstart, hash_head);
		UPDATE_HASH (ins_h, window[strstart + MIN_MATCH-1]);
#endif	/*************************************/

		ins_h = ((ins_h << H_SHIFT) ^ window[strstart + MIN_MATCH-1]) & HASH_MASK;
		prev[strstart & WMASK] = hash_head = head[ins_h];
		head[ins_h] = strstart;

		/*
		 *	Find the longest match, discarding those <= prev_length.
		 */
		prev_length = match_length, prev_match = match_start;
		match_length = MIN_MATCH-1;

		if
		(	hash_head != NIL && prev_length < max_lazy_match &&
			strstart - hash_head <= MAX_DIST
		)
		{
			/*
			 *	To simplify the code, we prevent matches
			 *	with the string of window index 0
			 *	(in particular we have to avoid a match
			 *	of the string with itself at the start of
			 *	the input file).
			 */
			match_length = longest_match (hash_head);

			/* longest_match () sets match_start */

			if (match_length > lookahead)
				match_length = lookahead;

			/* Ignore a length 3 match if it is too distant: */

			if
			(	match_length == MIN_MATCH &&
				strstart-match_start > TOO_FAR
			)
			{
				/*
				 *	If prev_match is also MIN_MATCH,
				 *	match_start is garbage but we will
				 *	ignore the current match anyway.
				 */
				match_length--;
			}
		}

		/*
		 *	If there was a match at the previous step and
		 *	the current match is not better, output the
		 *	previous match:
		 */
		if   (prev_length >= MIN_MATCH && match_length <= prev_length)
		{
			flush = ct_tally (strstart-1-prev_match, prev_length - MIN_MATCH);

			/*
			 *	Insert in hash table all strings up to
			 *	the end of the match. strstart-1 and strstart
			 *	are already inserted.
			 */
			lookahead -= prev_length - 1;

			prev_length -= 2;

			do
			{
				strstart++;

#if (0)	/*************************************/
				INSERT_STRING (strstart, hash_head);
				UPDATE_HASH (ins_h, window[strstart + MIN_MATCH-1]);
#endif	/*************************************/

				ins_h = ((ins_h << H_SHIFT) ^ window[strstart + MIN_MATCH-1]) & HASH_MASK;
				prev[strstart & WMASK] = hash_head = head[ins_h];
				head[ins_h] = strstart;

				/*
				 *	strstart never exceeds WSIZE-MAX_MATCH,
				 *	so there are always MIN_MATCH bytes
				 *	ahead. If lookahead < MIN_MATCH
				 *	these bytes are garbage, but it does
				 *	not matter since the next lookahead
				 *	bytes will always be emitted as
				 *	literals.
				 */
			}
			while (--prev_length != 0);

			match_available = 0;
			match_length = MIN_MATCH-1;
			strstart++;

			if (flush)
			{
#if (0)	/*************************************/
				FLUSH_BLOCK (0), block_start = strstart;
#endif	/*************************************/

				flush_block
				(	block_start >= 0 ? &window[block_start] : NOSTR,
					strstart - block_start,
					0
				);
				block_start = strstart;
			}

		}
		elif (match_available)
		{
			/*
			 *	If there was no match at the previous position,
			 *	output a single literal. If there was a match
			 *	but the current match is longer, truncate the
			 *	previous match to a single literal.
			 */
			Tracevv((stderr,"%c",window[strstart-1]));

			if (ct_tally (0, window[strstart-1]))
			{
#if (0)	/*************************************/
				FLUSH_BLOCK (0), block_start = strstart;
#endif	/*************************************/
				flush_block
				(	block_start >= 0 ? &window[block_start] : NOSTR,
					strstart - block_start,
					0
				);
				block_start = strstart;
			}

			strstart++;
			lookahead--;
		}
		else
		{
			/*
			 *	There is no previous match to compare
			 *	with, wait for the next step to decide.
			 */
			match_available = 1;
			strstart++;
			lookahead--;
		}

		Assert (strstart <= isize && lookahead <= isize, "a bit too far");

		/*
		 *	Make sure that we always have enough lookahead,
		 *	except at the end of the input file. We need
		 *	MAX_MATCH bytes for the next match, plus MIN_MATCH
		 *	bytes to insert the string following the next match.
		 */
		while (lookahead < MIN_LOOKAHEAD && !eofile)
			fill_window ();
	}

	if (match_available)
		ct_tally (0, window[strstart-1]);

#if (0)	/*************************************/
	return FLUSH_BLOCK (1); /* eof */
#endif	/*************************************/
	return
	(	flush_block
		(	block_start >= 0L ? &window[block_start] : NOSTR,
			strstart - block_start,
			1
		)
	);

}	/* end deflate */
