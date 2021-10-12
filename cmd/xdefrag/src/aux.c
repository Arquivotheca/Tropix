/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Examina e desfragmenta o sistema de arquivos		*
 *								*
 *	Versão	4.3.0, de 25.10.02				*
 *		4.5.0, de 31.10.03				*
 *								*
 *	Módulo: xdefrag						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/disktb.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "../h/xdefrag.h"

/*
 ****************************************************************
 *	Calcula o no. de blocos de endereços de um arquivo	*
 ****************************************************************
 */
int
compute_no_of_indir_blocks (daddr_t n_blocks)
{
	int		n;

	/*
	 *	Desconta os blocos diretos
	 */
	if ((n_blocks -= (T1_NADDR - 3)) <= 0)
		return (0);

	/*
	 *	Computa os blocos indiretos de nível 1
	 */
	if ((n_blocks -= T1_INDIR_SZ) <= 0)
		return (1);

	/*
	 *	Computa os blocos indiretos de nível 2
	 */
	if (n_blocks <= T1_INDIR_SZ * T1_INDIR_SZ)
		return (((n_blocks + T1_INDIR_MASK) / T1_INDIR_SZ) + 1 + 1);

	n_blocks -= T1_INDIR_SZ * T1_INDIR_SZ;

	/*
	 *	Computa os blocos indiretos de nível 3
	 */
	n = (n_blocks + T1_INDIR_MASK) / T1_INDIR_SZ;

	n += (n + T1_INDIR_MASK) / T1_INDIR_SZ;

	return ((n + 1) + (T1_INDIR_SZ + 1) + 1);

}	/* end compute_no_of_indir_blocks */

/*
 ****************************************************************
 *	Escreve na última linha das mensagens			*
 ****************************************************************
 */
void
draw_action_line (const char *format, ...)
{
	va_list		args;
	int		old_len = 0, new_len;
	char		text[80];

	va_start (args, format);

	vsnprintf (text, sizeof (text), format, args);

	if (msg_text[3] != NOSTR)
		{ old_len = strlen (msg_text[3]); free (msg_text[3]); }

	new_len = strlen (text);

	if (new_len < old_len)
		clear_msg_window ();

	if ((msg_text[3] = malloc (new_len + 1)) == NOSTR)
		{ xerror (NOSTR); return; }

	strcpy (msg_text[3], text);

	draw_msg_win (); do_XFlush ();

	va_end (args);

}	/* end draw_action_line */

/*
 ****************************************************************
 *	Lê um bloco do dispositivo				*
 ****************************************************************
 */
int
bread (void *area, daddr_t bno, int count)
{
	if   (llseek (dev_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET) < 0)
	{
		xerror ("*Erro de posicionamento para o bloco %d", bno);
	}
	elif (read (dev_fd, area, count) != count)
	{
		xerror ("*Erro de leitura do bloco %d", bno);
	}
	else
	{
		return (0);
	}

	return (-1);

}	/* end bread */

/*
 ****************************************************************
 *	Escreve um Bloco 					*
 ****************************************************************
 */
int
bwrite (const void *area, daddr_t bno, int count)
{
	if (rflag)
		return (0);

	if   (llseek (dev_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET) < 0)
	{
		xerror ("*Erro de posicionamento para o bloco %d", bno);
	}
	elif (write (dev_fd, area, count) != count)
	{
		xerror ("*Erro de escrita no bloco %d", bno);
	}
	else
	{
		return (0);
	}

	return (-1);

}	/* end bwrite */

/*
 ****************************************************************
 *	Prepara para criar um bloco novo			*
 ****************************************************************
 */
void *
bclear (CACHE *bp, daddr_t bno)
{
	if (bp->b_bno != bno)
	{
		bflush (bp);

		bp->b_bno = bno;
	}

	memsetl (bp->b_area, 0, BL4SZ / sizeof (long));

	return (bp->b_area);

}	/* end bclear */

/*
 ****************************************************************
 *	Aloca um bloco do cache					*
 ****************************************************************
 */
void *
bget (CACHE *bp, daddr_t bno)
{
	if (bp->b_bno != bno)
	{
		bflush (bp);

		bread (bp->b_area, bno, BL4SZ);

		bp->b_bno = bno;

		cache_miss++;
	}
	else
	{
		cache_hit++;
	}

	return (bp->b_area);

}	/* end bget */

/*
 ****************************************************************
 *	Descarrega um buffer do cache				*
 ****************************************************************
 */
void
bflush (CACHE *bp)
{
	if (bp->b_dirty)
	{
		bwrite (bp->b_area, bp->b_bno, BL4SZ);

		bp->b_dirty = 0;
	}

}	/* end bflush */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);

	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */
