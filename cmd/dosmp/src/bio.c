/*
 ****************************************************************
 *								*
 *			bio.c					*
 *								*
 *	Interface de entrada/saída				*
 *								*
 *	Versão	3.0.0, de 10.11.93				*
 *		4.4.0, de 29.10.02				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Le um bloco do disco para o endereco 			*
 ****************************************************************
 */
void *
bread (int blk_no, void *area)
{
	int		n;

	for (EVER)
	{
		llseek (dev_fd, (loff_t)blk_no << BLSHIFT, NULL, L_SET);

		if ((n = read (dev_fd, area, BLSZ)) != BLSZ)
		{
			if (n < 0 && errno == EINTR)
				continue;

			fprintf
			(	stderr,
				"%s: Erro de leitura do bloco %d, %P (%s)\n",
				pgname, blk_no, area, strerror (errno)
			);
			quit (1);
		}

		return (area);
	}

}	/* end bread */

/*
 ****************************************************************
 *	Escreve um bloco do disco				*
 ****************************************************************
 */
const void *
bwrite (int blk_no, const void *area)
{
	int		n;

	for (EVER)
	{
		llseek (dev_fd, (loff_t)blk_no << BLSHIFT, NULL, L_SET);

		if ((n = write (dev_fd, area, BLSZ)) != BLSZ)
		{
			if (n < 0 && errno == EINTR)
				continue;

			fprintf
			(	stderr,
				"%s: Erro de escrita do bloco %d %P (%s)\n",
				pgname, blk_no, area, strerror (errno)
			);
			quit (1);
		}

		return (area);
	}

}	/* end bwrite */

/*
 ****************************************************************
 *	Escreve uma entrada do diretório			*
 ****************************************************************
 */
void
dos_dir_write (DOSSTAT *zp)
{
	UNI		*up = &uni;
	int		n;

	if (zp->z_blkno < up->u_root_dir_blkno || zp->z_blkno >= up->u_end_fs_blkno)
	{
		error ("dos_dir_write: blkno inválido: %d", zp->z_blkno);
		return;
	}

	if ((unsigned)zp->z_offset > BLSZ - sizeof (DOSDIR))
	{
		error ("dos_dir_write: offset inválido: %d", zp->z_offset);
		return;
	}

	for (EVER)
	{
		llseek (dev_fd, ((loff_t)zp->z_blkno << BLSHIFT) + zp->z_offset, NULL, L_SET);

		if ((n = write (dev_fd, &zp->z_d, sizeof (DOSDIR))) != sizeof (DOSDIR))
		{
			if (n < 0 && errno == EINTR)
				continue;

			fprintf
			(	stderr,
				"%s: Erro de escrita do bloco %d (%s)\n",
				pgname, zp->z_blkno, strerror (errno)
			);
			quit (1);
		}

		return;
	}

}	/* end dos_dir_write */

/*
 ****************************************************************
 *	Zera os blocos de um CLUSTER				*
 ****************************************************************
 */
void
cluster_block_clr (int clusno)
{
	UNI		*up = &uni;
	int		blkno, end_blkno;
	char		area[BLSZ];

	memsetl (area, 0, sizeof (area) / sizeof (long));

	if (clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	for (/* acima */; blkno < end_blkno; blkno++)
		bwrite (blkno, area);

}	/* end cluster_block_clr */

/*
 ****************************************************************
 *	Lê CLUSTERs						*
 ****************************************************************
 */
void
clus_read (int clusno, void *area, int count)
{
	UNI		*up = &uni;
	int		blk_no, n;

	if ((unsigned)count > CLUSZ)
		{ error ("clus_read: count (%d) inválido", count); return; }

	blk_no = FIRST_BLK (clusno);

	for (EVER)
	{
		llseek (dev_fd, (loff_t)blk_no << BLSHIFT, NULL, L_SET);

		if ((n = read (dev_fd, area, count)) != count)
		{
			if (n < 0 && errno == EINTR)
				continue;

			fprintf
			(	stderr,
				"%s: Erro de leitura do bloco %d (%s)\n",
				pgname, blk_no, strerror (errno)
			);
			quit (1);
		}

		return;
	}

}	/* end clus_read */

/*
 ****************************************************************
 *	Escreve CLUSTERs					*
 ****************************************************************
 */
void
clus_write (int clusno, const void *area, int count)
{
	UNI		*up = &uni;
	int		blk_no, n;

	if ((unsigned)count > CLUSZ)
		{ error ("clus_write: count (%d) inválido", count); return; }

	blk_no = FIRST_BLK (clusno);

	for (EVER)
	{
		llseek (dev_fd, (loff_t)blk_no << BLSHIFT, NULL, L_SET);

		if ((n = write (dev_fd, area, count)) != count)
		{
			if (n < 0 && errno == EINTR)
				continue;

			fprintf
			(	stderr,
				"%s: Erro de escrita do bloco %d (%s)\n",
				pgname, blk_no, strerror (errno)
			);
			quit (1);
		}

		return;
	}

}	/* end clus_write */
