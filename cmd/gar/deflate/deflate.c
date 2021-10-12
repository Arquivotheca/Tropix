/*
 ****************************************************************
 *								*
 *			deflate.c				*
 *								*
 *	Compress data using the deflation algorithm		*
 *								*
 *	Versão	3.0.00, de 05.06.93				*
 *		3.1.06, de 04.05.97				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários de compressão/descompressão		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no software "gzip" do GNU		*
 *								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>

#include "../h/common.h"
#include "../h/deflate.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
entry int	ZIP_LEVEL;	/* Os níveis podem ser de 1..9 */

#define HASH_BITS  15

/* HASH_SIZE and WSIZE must be powers of two */

#define HASH_SIZE	(unsigned)(1 << HASH_BITS)
#define HASH_MASK	(HASH_SIZE - 1)
#define WMASK		(WSIZE - 1)

#define NIL	0 		/* Tail of hash chains */

#define FAST 4 		/* speed options for the general purpose bit flag */
#define SLOW 2

/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */

#define TOO_FAR 4096

/*
 *	Local data used by the "longest match" routines.
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

/*
 ******	Global buffers ******************************************
 */
entry char	*inbuf;
entry char	*outbuf;
entry ushort	*d_buf;
entry char	*window;
entry ushort	*prev;

entry long	bytes_in;	/* number of input bytes */
entry long	bytes_out;	/* number of output bytes */

entry int	ifd;		/* input file descriptor */
entry int	ofd;	 	/* output file descriptor */

entry ZIPFILE	zipfile;	/* Estrutura para a saída ZIP */

/*
 * window size, 2*WSIZE except for MMAP or BIG_MEM, where it is the
 * input file length plus MIN_LOOKAHEAD.
 */
entry ulong	window_size = 2 * WSIZE;

/*
 * window position at the beginning of the current output block. Gets
 * negative when the window is moved backwards.
 */
entry long	block_start;

local unsigned	ins_h;  /* hash index of string to be inserted */

/*
 * Number of bits by which ins_h and del_h must be shifted at each
 * input step. It must be such that after MIN_MATCH steps, the oldest
 * byte no longer takes part in the hash key, that is:
 *	H_SHIFT * MIN_MATCH >= HASH_BITS
 */
#define H_SHIFT  ((HASH_BITS + MIN_MATCH - 1) / MIN_MATCH)

/*
 * Length of the best match at previous step. Matches not greater than this
 * are discarded. This is used in the lazy match evaluation.
 */
entry unsigned	prev_length;

entry unsigned	strstart;	/* start of string to insert */
entry unsigned	match_start;	/* start of matching string */
local int	eofile;		/* flag set at end of input file */
local unsigned	lookahead;	/* number of valid bytes ahead in window */

/*
 * To speed up deflation, hash chains are never searched beyond this length.
 * A higher limit improves compression ratio but degrades the speed.
 */
entry unsigned	max_chain_length;

/*
 * Attempt to find a better match only when the current match is strictly
 * smaller than this value.
 */
local unsigned	int max_lazy_match;

/* Use a faster search when the previous match is longer than this */
entry int	good_match;

/*
 ******	Tabela de configuração (em função do nível) *************
 */
typedef struct config
{
	ushort good_length; 	/* reduce lazy search above this match length */
	ushort max_lazy;	/* do not perform lazy search above this match length */
	ushort nice_length; 	/* quit search above this match length */
	ushort max_chain;

}	CONFIG;

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

/*
 * Note: the current code requires max_lazy >= MIN_MATCH and max_chain >= 4
 * but these restrictions can easily be removed at a small cost.
 */

#ifdef  FULL_SEARCH
# define nice_match MAX_MATCH
#else
entry int	nice_match; /* Stop searching when current match exceeds this */
#endif

/*
 ****************************************************************
 * Processes a new input file and return its compressed length	*
 ****************************************************************
 */
off_t
deflate (int file_fd, FILE *reduce_fp)
{
	ulong		hash_head;	/* head of hash chain */
	ulong		prev_match;	/* previous match */
	int		flush;		/* set if current block must be flushed */
	int		match_available = 0; /* set if previous match exists */
	unsigned	match_length = MIN_MATCH-1; /* length of best match */
	unsigned	j;
	const CONFIG	*gp;
	ZIPFILE		*zp1 = &zipfile;
	char		crc[14];
	char		*cp;

	/*
	 *	Aloca as áreas globais
	 */
	if (window == NOSTR)
	{
		if ((inbuf = malloc (INBUFSIZ + INBUF_EXTRA)) == NOSTR)
			error (NOSTR);

		if ((outbuf = malloc (OUTBUFSIZ + OUTBUF_EXTRA)) == NOSTR)
			error (NOSTR);

		if ((d_buf = malloc (DIST_BUFSIZE * sizeof (ushort))) == NOVOID)
			error (NOSTR);

		if ((window = malloc (2 * WSIZE)) == NOSTR)
			error (NOSTR);

		if ((prev = malloc (PREV_SZ * sizeof (ushort))) == NOVOID)
			error (NOSTR);
	}

	/*
	 *	Inicializa a saída ZIP
	 */
	ifd = file_fd;
	ofd = fileno (reduce_fp);
	bytes_out = 0;

	zp1->z_ptr  = outbuf; 
	zp1->z_base = outbuf; 
	zp1->z_bend = outbuf + OUTBUFSIZ; 

	crc_32 = updcrc (NOSTR, 0);

	bi_init ();
	ct_init ();

	/*
	 *	Initialize the hash table
	 */
	memsetl ((long *)head, 0, (HASH_SIZE * sizeof (*head)) / sizeof (long));

	/* prev will be initialized on the fly */

	/*
	 *	Atribui os valores de configuração
	 */
	gp = &configuration_table[ZIP_LEVEL];

	max_lazy_match	 = gp->max_lazy;
	good_match	 = gp->good_length;
#ifndef FULL_SEARCH
	nice_match	 = gp->nice_length;
#endif
	max_chain_length = gp->max_chain;

	/* ??? reduce max_chain_length for binary files */

	strstart = 0;
	block_start = 0;

	lookahead = file_read (window, 2 * WSIZE);

	if (lookahead == 0 || lookahead == EOF)
	{
		eofile = 1, lookahead = 0;
		return (-1);
	}

	eofile = 0;

	/*
	 *	Make sure that we always have enough lookahead.
	 *	This is important if input comes from a device such as a tty.
	 */
	while (lookahead < MIN_LOOKAHEAD && !eofile)
		fill_window ();

	/*
	 *	Coloca os primeiros caracteres na tabela HASH
	 */
	ins_h = 0;

	for (j = 0; j < MIN_MATCH - 1; j++)
		ins_h = ((ins_h << H_SHIFT) ^ window[j]) & HASH_MASK;

	/*
	 *	Processa o bloco de entrada
	 */
	while (lookahead != 0)
	{
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

				ins_h = ((ins_h << H_SHIFT) ^ window[strstart + MIN_MATCH-1]) & HASH_MASK;
				prev[strstart & WMASK] = hash_head = head[ins_h];
				head[ins_h] = strstart;
			}
			while (--prev_length != 0);

			match_available = 0;
			match_length = MIN_MATCH-1;
			strstart++;

			if (flush)
			{
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
			if (ct_tally (0, window[strstart-1]))
			{
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

		while (lookahead < MIN_LOOKAHEAD && !eofile)
			fill_window ();
	}

	if (match_available)
		ct_tally (0, window[strstart-1]);

	j = flush_block
	(	block_start >= 0L ? &window[block_start] : NOSTR,
		strstart - block_start,
		1
	);

	/*
	 *	Escreve o CRC do arquivo
	 */
	sprintf (crc, "%13ld", crc_32);

	for (cp = crc; cp < crc + 14; cp++)
		PUTZIP (*cp);

	/*
	 *	Terminou
	 */
	write_zip (0, zp1);

	return (bytes_out);

}	/* end deflate */

/*
 ****************************************************************
 *	Fill the window when the lookahead becomes insufficient	*
 ****************************************************************
 */
void
fill_window (void)
{
	unsigned	n, m;
	unsigned	more = (window_size - lookahead - strstart);

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

#if (0)	/*******************************************************/
#ifdef PC

#define	Assert(exp, msg)	if (!(exp)) printf (msg);

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
	unsigned	chain_length = max_chain_length; /* max hash chain length */
	char		*scan = window + strstart;	/* current string */
	char		*match;			/* matched string */
	int		len;			/* length of current match */
	int		best_len = prev_length;		 /* best match length so far */
	unsigned	limit = strstart > MAX_DIST ? strstart - MAX_DIST : NIL;
	char		*str_end = window + strstart + MAX_MATCH;
	char		scan_end1  = scan[best_len-1];
	char		scan_end	= scan[best_len];

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
	error: Code too clever
#endif

	/* Do not waste too much time if we already have a good match: */

	if (prev_length >= good_match)
		chain_length >>= 2;

	Assert (strstart <= window_size-MIN_LOOKAHEAD, "insufficient lookahead\n");

	do {
		Assert(cur_match < strstart, "no future\n");

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
			 scan < str_end
		)
			/* vazio */;

		len = MAX_MATCH - (str_end - scan);
		scan = str_end - MAX_MATCH;

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
#endif	PC
#endif	/*******************************************************/
