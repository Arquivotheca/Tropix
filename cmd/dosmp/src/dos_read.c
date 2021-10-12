/*
 ****************************************************************
 *								*
 *			dos_read.c				*
 *								*
 *	funções para leitura de arquivos (ou diretórios) DOS	*
 *								*
 *	Versão	3.0.0, de 16.11.93				*
 *		3.0.0, de 02.03.95				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *    Prepara para a leitura de um arquivo ou diretório DOS	*
 ****************************************************************
 */
int
_dos_open (DOSFILE *fp, const DOSSTAT *zp)
{
	const UNI	*up = &uni;

   	fp->f_code_conv = 0;	/* Ver abaixo */

   	fp->f_o_clusno = zp->z_cluster;
   	fp->f_clusno = fp->f_o_clusno;

   /***	fp->f_o_count = ...;	/* Abaixo */
   /***	fp->f_count = ...;	/* Abaixo */

   /***	fp->f_blkno = ...;	/* Abaixo */
   /***	fp->f_end_blkno = ...;	/* Abaixo */

   /***	fp->f_area = ...;	/* Inicializado pela macro */

	fp->f_end = fp->f_area + BLSZ;
	fp->f_ptr = fp->f_end;

	/*
	 *	Inicializa o tamanho
	 */
	if   (Z_ISREG (zp->z_mode))
	{
	   	if (fp->f_o_clusno == 0)
		   	fp->f_o_count = 0;
		else
		   	fp->f_o_count = GET_LONG (zp->z_size);

	   	if (iso_mode)
			fp->f_code_conv = 1;
	}
	elif (Z_ISDIR (zp->z_mode))
	{
	   	fp->f_o_count = BIG_SZ;	/* EOF pelos CLUSTERs */
	   /***	fp->f_code_conv = 0; ***/
	}
	else
	{
		error ("dos_open: Arquivo não é REG nem DIR");
		errno = EINVAL;
		return (-1);
	}

   	fp->f_count = fp->f_o_count;

	/*
	 *	Pequena consistência
	 */
	if ((unsigned)fp->f_clusno >= up->u_n_clusters + 2 || fp->f_clusno == 1)
	{
		error
		(	"dos_open_by_clus: No. de CLUSTER inválido: %d",
			fp->f_clusno
		);
		errno = EINVAL;
		return (-1);
	}

	/*
	 *	Prepara os parametros
	 */
	if (fp->f_clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (fp->f_blkno, fp->f_end_blkno);
	else
		FIRST_AND_LAST_BLK (fp->f_clusno, fp->f_blkno, fp->f_end_blkno);

	fp->f_blkno--;	/* É Incrementado antes da primeira leitura */

	return (0);

}	/* end dos_open */

/*
 ****************************************************************
 *    Prepara para a leitura de um arquivo ou diretório DOS	*
 ****************************************************************
 */
int
_dos_open_by_clus (DOSFILE *fp)
{
	const UNI	*up = &uni;

	/*
	 *	Só é apropriado para DIRETÓRIOS
	 */
   	fp->f_code_conv = 0;	/* ??? Provisório */

   /***	fp->f_o_clusno = ...;	/* Inicializado pela macro */
   	fp->f_clusno = fp->f_o_clusno;

   /***	fp->f_o_count = ...;	/* Inicializado pela macro */
   	fp->f_count = fp->f_o_count;

   /***	fp->f_blkno = ...;	/* Abaixo */
   /***	fp->f_end_blkno = ...;	/* Abaixo */

   /***	fp->f_area = ...;	/* Inicializado pela macro */

	fp->f_end = fp->f_area + BLSZ;
	fp->f_ptr = fp->f_end;

	/*
	 *	Pequena consistência
	 */
	if ((unsigned)fp->f_clusno >= up->u_n_clusters + 2)
	{
		error
		(	"dos_open_by_clus: No. de CLUSTER inválido: %d",
			fp->f_clusno
		);
		return (-1);
	}

	/*
	 *	Prepara os parametros
	 */
	if (fp->f_clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (fp->f_blkno, fp->f_end_blkno);
	else
		FIRST_AND_LAST_BLK (fp->f_clusno, fp->f_blkno, fp->f_end_blkno);

	fp->f_blkno--;	/* É Incrementado antes da primeira leitura */

	return (0);

}	/* end dos_open_by_clus */

/*
 ****************************************************************
 *    Prepara para a Re-leitura de um arquivo ou diretório DOS	*
 ****************************************************************
 */
int
dos_rewind (DOSFILE *fp)
{
	const UNI	*up = &uni;

   /***	fp->f_code_conv = 0;	/* Inicializado pelo "open" */

   /***	fp->f_o_clusno = ...;	/* Inicializado pelo "open" */
   	fp->f_clusno = fp->f_o_clusno;

   /***	fp->f_o_count = ...;	/* Inicializado pelo "open" */
   	fp->f_count = fp->f_o_count;

   /***	fp->f_blkno = ...;	/* Abaixo */
   /***	fp->f_end_blkno = ...;	/* Abaixo */

   /***	fp->f_area = ...;	/* Inicializado pelo "open" */

   /***	fp->f_end = fp->f_area + BLSZ; /*** Inicializado pelo "open" */
	fp->f_ptr = fp->f_end;

	/*
	 *	Prepara os parametros
	 */
	if (fp->f_clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (fp->f_blkno, fp->f_end_blkno);
	else
		FIRST_AND_LAST_BLK (fp->f_clusno, fp->f_blkno, fp->f_end_blkno);

	fp->f_blkno--;	/* É Incrementado antes da primeira leitura */

	return (0);

}	/* end dos_rewind */

/*
 ****************************************************************
 *	Le uma área de um arquivo ou diretório DOS		*
 ****************************************************************
 */
int
dos_read (DOSFILE *fp, void *AREA, int COUNT)
{
	const UNI	*up = &uni;
	int		count;
	int		total = 0;

	/*
	 *	Verifica se é necessário ler o CLUSTER seguinte
	 */
    again:
	if (fp->f_ptr >= fp->f_end)
	{
		if ((count = MIN (fp->f_count, BLSZ)) <= 0)	/* EOF */
			return (total);

		if (++fp->f_blkno >= fp->f_end_blkno)
		{
			if (fp->f_clusno == 0)		/* RAIZ */
				return (total);

			fp->f_clusno = get_fat_value (fp->f_clusno);

			if (C_ISBAD_OR_EOF (fp->f_clusno))
				return (total);

			FIRST_AND_LAST_BLK (fp->f_clusno, fp->f_blkno, fp->f_end_blkno);
		}

		bread (fp->f_blkno, fp->f_area);

		fp->f_ptr = fp->f_area;
		fp->f_end = fp->f_area + count;

		fp->f_count -= count;

	}	/* end (area vazia) */

	/*
	 *	Armazena em um dos dois modos
	 */
   	if (fp->f_code_conv == 0)	/* Armazena (modo binário) */
	{
		if ((count = fp->f_end - fp->f_ptr) > COUNT)
			count = COUNT;

		memmove (AREA, fp->f_ptr, count);

		fp->f_ptr += count;
	}
	else				/* Armazena (modo ISO) */
	{
		char	*op, *ep;
		char	c;

		for
		(	op = AREA, ep = op + COUNT;
			op < ep && fp->f_ptr < fp->f_end;
			/* abaixo */
		)
		{
			if ((c = *fp->f_ptr++) == 0x1A)	/* Ignora <^Z> */
				continue;
	
			if (c != '\r')			/* Ignora <cr> */
				*op++ = c;
		}

		count = op - (char *)AREA;
	}

	/*
	 *	Recalcula os totais e verifica se acabou
	 */
	total += count;

	AREA += count;

	if ((COUNT -= count) > 0)
		goto again;

	return (total);

}	/* end dos_read */
