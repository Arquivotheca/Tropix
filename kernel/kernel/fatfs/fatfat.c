/*
 ****************************************************************
 *								*
 *			fatfat.c				*
 *								*
 *	Gerência da FAT						*
 *								*
 *	Versão	4.2.0, de 13.12.01				*
 *		4.2.0, de 05.10.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/fat.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
enum { CLUSVEC_DIR = 4, CLUSVEC_INC = 32 };

/*
 ******	Protótipos de funções ***********************************
 */
int		fat_attach_new_cluster (SB *unisp, clusno_t old_clusno);
int		fat_put_fat_value_in_one_fat (const SB *, int, clusno_t, off_t, int);
void		fat_cluster_clr (const SB *unisp, clusno_t clusno);
void		fat_put_fat_value (const SB *unisp, int value, clusno_t clusno);

/*
 ****************************************************************
 *	Mapeamento de endereços de blocos da FAT		*
 ****************************************************************
 */
int
fat_cluster_read_map (INODE *ip, clusno_t wanted_log_clusno)
{
	const FATSB	*sp;
	const clusno_t	*clusvec;

	/*
	 *	Retorna C_STD_EOF em caso de EOF/erro
	 *
	 *	Verifica se tem o vetor de CLUSTERs
	 */
	if (ip->i_clusvec == NULL)
		fat_creat_clusvec (ip);

	if ((clusvec = (clusno_t *)ip->i_clusvec) == NULL)
		{ printf ("%g: Vetor de CLUSTERs inexistente\n"); goto bad; }

	if ((unsigned)wanted_log_clusno < ip->i_clusvec_len)
		return (clusvec[wanted_log_clusno]);

	/*
	 *	Em case de erro, ...
	 */
    bad:
	sp = ip->i_sb->sb_ptr;

	return (C_STD_EOF);

}	/* end fat_block_read_map */

/*
 ****************************************************************
 *	Mapeamento de endereços de blocos da FAT		*
 ****************************************************************
 */
int
fat_cluster_write_map (INODE *ip, clusno_t wanted_log_clusno)
{
	const FATSB	*sp = ip->i_sb->sb_ptr;
	clusno_t	log_clusno, ph_clusno;
	clusno_t	*clusvec;

	/*
	 *	Retorna (-1) em caso de erro
	 *
	 *	Verifica se tem o vetor de CLUSTERs
	 */
	if (ip->i_clusvec == NULL)
		fat_creat_clusvec (ip);

	if ((clusvec = (clusno_t *)ip->i_clusvec) == NULL)
		{ printf ("%g: Vetor de CLUSTERs inexistente\n"); return (-1); }

	/*
	 *	Verifica se o CLUSTER já existe no vetor
	 */
	if ((unsigned)wanted_log_clusno < ip->i_clusvec_len)
		return (clusvec[wanted_log_clusno]);

	/*
	 *	Analisa o tamanho do vetor
	 */
	if (ip->i_clusvec_sz <= wanted_log_clusno)
	{
		ip->i_clusvec_sz = wanted_log_clusno + CLUSVEC_INC;

		if ((clusvec = realloc_byte (clusvec, ip->i_clusvec_sz * sizeof (clusno_t))) == NULL)
			{ printf ("%g: Memória esgotada\n"); return (-1); }
#ifdef	DEBUG
		printf ("%g: Aumentei para %d entradas, INODE %d\n", ip->i_clusvec_sz, ip->i_ino);
#endif	DEBUG
	   /***	ip->i_clusvec_sz  = ...;	/* Acima	***/
	   /***	ip->i_clusvec_len = ...;	/* Não altera	***/
		ip->i_clusvec	  = (int)clusvec;
	}

	/*
	 *	Verifica se o arquivo já tem pelo menos um "cluster"
	 */
	if (C_ISEOF (ip->i_first_clusno))
	{
		if ((ph_clusno = fat_attach_new_cluster (ip->i_sb, C_STD_EOF)) < 0)
			{ u.u_error = ENOSPC; return (-1); }

		fat_cluster_clr (ip->i_sb, ph_clusno);

		if (ip->i_clusvec_len != 0)
			printf ("%g: Tamanho devia ser 0\n");

		ip->i_first_clusno = ph_clusno;
		ip->i_clusvec_len  = 1;

		clusvec[0] = ph_clusno;
	}

	/*
	 *	Percorre a cadeia de CLUSTERs
	 */
	for (log_clusno = ip->i_clusvec_len - 1; log_clusno < wanted_log_clusno; log_clusno++)
	{
		if ((ph_clusno = fat_attach_new_cluster (ip->i_sb, clusvec[log_clusno])) < 0)
			{ u.u_error = ENOSPC; return (-1); }

		fat_cluster_clr (ip->i_sb, ph_clusno);

		clusvec[ip->i_clusvec_len++] = ph_clusno;
	}

	return (clusvec[wanted_log_clusno]);

}	/* end fat_cluster_write_map */

/*
 ****************************************************************
 *	Obtém e registra um "cluster" livre			*
 ****************************************************************
 */
int
fat_attach_new_cluster (SB *unisp, clusno_t old_clusno)
{
	FATSB		*sp = unisp->sb_ptr;
	BHEAD		*bp;
	daddr_t		blkno;
	const char	*cp, *end_cp;
	clusno_t	clusno, clus_val;
	int		clus_to_tst, clus_index, clus_last_index;
	int		count, cnt;
	int		*ptr, *end_ptr;
	void		*area, *area_ptr;
	ulong		l;

	/*
	 *	Preenche a lista livre do SB, se for o caso
	 */
	SLEEPLOCK (&sp->s_free_cluster_lock, PINODE);

	if (sp->s_free_cluster_ptr >= sp->s_free_cluster_end)
	{
		sp->s_free_cluster_ptr = ptr = sp->s_free_cluster_vector;
		end_ptr = ptr + FAT_FREE_CLUSTER_SZ;

		if (sp->s_fat_bits != 12)		/* FAT 16/32 */
		{
			clus_to_tst = sp->s_n_clusters; clus_last_index = clus_to_tst + 2;

			if ((clus_index = sp->s_free_cluster_index) == 0)
				clus_index = 2;

			blkno = sp->s_fat1_blkno;

			if (sp->s_fat_bits == 16)
				blkno += clus_index >> (BLSHIFT - 1);
			else		/* FAT 32 */
				blkno += clus_index >> (BLSHIFT - 2);

			do
			{
				bp = bread (unisp->sb_dev, blkno, 0);

				cp = bp->b_addr; end_cp = cp + bp->b_count;

				if   (sp->s_fat_bits == 16)
				{
					cp += (clus_index & 255) << 1;

					for (/* acima */; cp < end_cp; cp += 2)
					{
						clus_val = FAT_GET_SHORT (cp);

						if (C_ISFREE (clus_val))
						{
							*ptr++ = clus_index;

							if (ptr >= end_ptr)
								break;
						}

						if (++clus_index >= clus_last_index)
							break;

						if (--clus_to_tst <= 0)
							break;
					}
				}
				else	/* fat_bits == 32 */
				{
					cp += (clus_index & 127) << 2;

					for (/* acima */; cp < end_cp; cp += 4)
					{
						clus_val = FAT_GET_LONG (cp);

						if (C_ISFREE (clus_val))
						{
							*ptr++ = clus_index;

							if (ptr >= end_ptr)
								break;
						}

						if (++clus_index >= clus_last_index)
							break;

						if (--clus_to_tst <= 0)
							break;
					}
				}

				if (clus_index >= clus_last_index)
				{
					blkno = sp->s_fat1_blkno; clus_index = 2;
				}
				else
				{
					blkno += (bp->b_count >> BLSHIFT);
				}

				block_put (bp);
			}
			while (clus_to_tst > 0 && ptr < end_ptr);

			if ((sp->s_free_cluster_index = clus_index) > clus_last_index)
				sp->s_free_cluster_index = 2;
		}
		else				 	/* FAT 12 */
		{
			clus_index = 2; clus_last_index = sp->s_n_clusters + 2;

			blkno = sp->s_fat1_blkno;

			count = BLTOBY (sp->s_sectors_per_fat);

			if ((area_ptr = area = malloc_byte (count)) == NOVOID)
				{ u.u_error = ENOMEM; return (-1); }

			while (count > 0)
			{
				bp = bread (unisp->sb_dev, blkno, 0);

				if ((cnt = bp->b_count) > count)
					cnt = count;

				memmove (area_ptr, bp->b_addr, cnt);

				block_put (bp);

				blkno    += cnt >> BLSHIFT;

				area_ptr += cnt;
				count	 -= cnt;
			}

			for (cp = area + 3; /* abaixo */; cp += 3)
			{
				l = FAT_GET_LONG (cp);

				clus_val =  l	   & 0xFFF;

				if (C_ISFREE (clus_val))
				{
					*ptr++ = clus_index;

					if (ptr >= end_ptr)
						break;
				}

				if (++clus_index >= clus_last_index)
					break;

				clus_val = (l >> 12) & 0xFFF;

				if (C_ISFREE (clus_val))
				{
					*ptr++ = clus_index;

					if (ptr >= end_ptr)
						break;
				}

				if (++clus_index >= clus_last_index)
					break;
			}

			free_byte (area);
		}

		sp->s_free_cluster_end = ptr;

	}	/* if (lista livre vazia) */

	/*
	 *	Verifica agora, se há um "cluster" disponível
	 */
	if (sp->s_free_cluster_ptr >= sp->s_free_cluster_end)
	{
		SLEEPFREE (&sp->s_free_cluster_lock);

		printf ("%g: Sistema de arquivos cheio: \"%v\"\n", unisp->sb_dev);

		u.u_error = ENOSPC; return (-1);
	}

	/*
	 *	Utiliza e registra o "cluster" obtido
	 */
	clusno = *sp->s_free_cluster_ptr++;

	if (!C_ISBAD_OR_EOF (old_clusno))
		fat_put_fat_value (unisp, clusno, old_clusno);

	fat_put_fat_value (unisp, C_STD_EOF, clusno);

	sp->s_infree--; unisp->sb_sbmod = 1;

	SLEEPFREE (&sp->s_free_cluster_lock);

	return (clusno);

}	/* end fat_attach_new_cluster */

/*
 ****************************************************************
 *	Zero todos os blocos de um "cluster"			*
 ****************************************************************
 */
void
fat_cluster_clr (const SB *unisp, clusno_t clusno)
{
	const FATSB	*sp = unisp->sb_ptr;
	BHEAD		*bp;
	daddr_t		blkno, end_blkno;
	int		count, cnt;

	FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	for (count = sp->s_CLUSTER_SZ; count > 0; count -= cnt)
	{
		if (blkno + 1 < end_blkno)
			bp = breada (unisp->sb_dev, blkno, blkno + 1, 0);
		else
			bp = bread (unisp->sb_dev, blkno, 0);

		if ((cnt = bp->b_count) > count)
			cnt = count;

		memclr (bp->b_addr, cnt);

		bdwrite (bp);

		blkno += (cnt >> BLSHIFT);
	}

}	/* end fat_cluster_clr */

/*
 ****************************************************************
 *	Obtém uma entrada da FAT				*
 ****************************************************************
 */
int
fat_get_fat_value (const SB *unisp, clusno_t clusno)
{
	const FATSB	*sp = unisp->sb_ptr;
	BHEAD		*bp;
	off_t		offset;
	int		value;
	unsigned long	l;
	char		*area;

	/*
	 *	Pequena consistência
	 */
	if (clusno < 0 || clusno >= sp->s_n_clusters + 2)
		{ printf ("%g: No. de cluster inválido: %d\n", clusno); return (-1); }

	/*
	 *	Calcula o deslocamento no arquivo da FAT
	 */
	if   (sp->s_fat_bits == 12)
		offset = (clusno * 3) >> 1;
	elif (sp->s_fat_bits == 16)
		offset = clusno << 1;
	else	/* fat_bits == 32 */
		offset = clusno << 2;

	/*
	 *	Le a região da FAT do disco
	 */
#ifdef	DEBUG
	printf ("fat_get_fat_value: lendo bloco %d\n", sp->s_fat1_blkno + (offset >> BLSHIFT));
#endif	DEBUG

	bp = bread (unisp->sb_dev, sp->s_fat1_blkno + (offset >> BLSHIFT), 0);

	if (u.u_error)
		{ printf ("%g: Erro de leitura da FAT\n"); block_put (bp); return (-1); }

	area = bp->b_addr + (offset & BLMASK);

	l = FAT_GET_LONG (area);

	block_put (bp);

#ifdef	DEBUG
	printf ("fat_get_fat_value: offset = %d, l = %P\n", offset, l);
#endif	DEBUG

	/*
	 *	Verifica se por acaso não tem um pedaço no bloco seguinte
	 */
	if (sp->s_fat_bits == 12 && (offset & BLMASK) == 511)
	{
		bp = bread (unisp->sb_dev, sp->s_fat1_blkno + 1 + (offset >> BLSHIFT), 0);

		if (u.u_error)
			{ printf ("%g: Erro de leitura da FAT\n"); block_put (bp); return (-1); }

		area = bp->b_addr;

		l &= 0xFF; l |= (area[0] << 8);

		block_put (bp);
	}

	/*
	 *	Retira o conteúdo da FAT
	 */
	if (sp->s_fat_bits == 12)
	{
		if (clusno & 1)
			value = (l >> 4) & 0xFFF;
		else
			value =  l	 & 0xFFF;
	}
	elif (sp->s_fat_bits == 16)
	{
		value = l & 0xFFFF;
	}
	else	/* fat_bits == 32 */
	{
		value = l;
	}

	return (value);

}	/* end fat_get_fat_value */

/*
 ****************************************************************
 *	Armazena uma entrada da FAT				*
 ****************************************************************
 */
void
fat_put_fat_value (const SB *unisp, int value, clusno_t clusno)
{
	const FATSB	*sp = unisp->sb_ptr;
	off_t		offset;

	/*
	 *	Pequena consistência
	 */
	if (clusno < 0 || clusno >= sp->s_n_clusters + 2)
		{ printf ("%g: No. de cluster inválido: %d\n", clusno); return; }

	/*
	 *	Calcula o deslocamento no arquivo da FAT
	 */
	if   (sp->s_fat_bits == 12)
		offset = (clusno * 3) >> 1;
	elif (sp->s_fat_bits == 16)
		offset = clusno << 1;
	else	/* fat_bits == 32 */
		offset = clusno << 2;

	/*
	 *	Escreve a entrada na FAT 1 no disco
	 */
	if (fat_put_fat_value_in_one_fat (unisp, value, clusno, offset, sp->s_fat1_blkno) < 0)
		return;

	/*
	 *	Escreve a entrada na FAT 2 no disco
	 */
	if (sp->s_n_fats < 2)
		return;

	if (fat_put_fat_value_in_one_fat (unisp, value, clusno, offset, sp->s_fat2_blkno) < 0)
		return;

}	/* end fat_put_fat_value */

/*
 ****************************************************************
 *	Armazena uma entrada em uma das FATs			*
 ****************************************************************
 */
int
fat_put_fat_value_in_one_fat (const SB *unisp, int value, clusno_t clusno, off_t offset, int fat_blkno)
{
	const FATSB	*sp = unisp->sb_ptr;
	BHEAD		*bp;
	char		*area;

	/*
	 *	Le a região da FAT do disco
	 */
	bp = bread (unisp->sb_dev, fat_blkno + (offset >> BLSHIFT), 0);

	if (u.u_error)
		{ printf ("%g: Erro de leitura da FAT\n"); block_put (bp); return (-1); }

	area = bp->b_addr + (offset & BLMASK);

	/*
	 *	Insere o conteúdo na FAT
	 */
	if (sp->s_fat_bits == 12)
	{
		if (clusno & 1)
		{
			area[0] &= 0x0F; area[0] |= ((value & 0x0F) << 4);

			if ((offset & BLMASK) != 511)
				area[1] = value >> 4;
		}
		else
		{
			area[0] = value & 0xFF;

			if ((offset & BLMASK) != 511)
				area[1] &= 0xF0; area[1] |= (value >> 8) & 0x0F;
		}
	}
	elif (sp->s_fat_bits == 16)
	{
		FAT_PUT_SHORT (value, area);
	}
	else	/* fat_bits == 32 */
	{
		FAT_PUT_LONG (value, area);
	}

	bdwrite (bp);

	/*
	 *	Verifica se por acaso não tem um pedaço no bloco seguinte
	 */
	if (sp->s_fat_bits == 12 && (offset & BLMASK) == 511)
	{
		bp = bread (unisp->sb_dev, fat_blkno + 1 + (offset >> BLSHIFT), 0);

		if (u.u_error)
			{ printf ("%g: Erro de leitura da FAT\n"); block_put (bp); return (-1); }

		area = bp->b_addr;

		if (clusno & 1)
			area[0] = value >> 4;
		else
			{ area[0] &= 0xF0; area[0] |= (value >> 8) & 0x0F; }

		bdwrite (bp);
	}

	return (0);

}	/* end fat_put_fat_value_in_one_fat */

/*
 ****************************************************************
 *	Obtém todos os CLUSTERs de um arquivo			*
 ****************************************************************
 */
void
fat_creat_clusvec (INODE *ip)
{
	const SB	*unisp = ip->i_sb;
	const FATSB	*sp = unisp->sb_ptr;
	clusno_t	*clusvec;
	clusno_t	log_clusno, ph_clusno, value;
	off_t		offset;
	daddr_t		blkno = -1;
	BHEAD		*bp = NOBHEAD;
	unsigned long	l;
	char		*area;

	/*
	 *	Verifica se deve haver o vetor de CLUSTERs
	 */
	if (ip->i_clusvec != NULL)
		return;

	if ((ph_clusno = ip->i_first_clusno) == 0)
		return;

	/*
	 *	Aloca a memória para o vetor
	 */
	if (ip->i_size == 0)
		ip->i_clusvec_sz = CLUSVEC_DIR;
	else
		ip->i_clusvec_sz = (ip->i_size + sp->s_CLUSTER_MASK) >> sp->s_CLUSTER_SHIFT;

	if ((clusvec = malloc_byte (ip->i_clusvec_sz * sizeof (clusno_t))) == NULL)
		{ printf ("%g: Memória esgotada\n"); return; }

#ifdef	DEBUG
	printf ("%g: Aloquei %d entradas, INODE %d\n", ip->i_clusvec_sz, ip->i_ino);
#endif	DEBUG

   /***	ip->i_clusvec_sz  = ...;	/* Acima ***/
	ip->i_clusvec_len = 0;
	ip->i_clusvec	  = (int)clusvec;

	log_clusno	  = 0;
   /***	ph_clusno	  = ip->i_first_clusno;	***/

	/*
	 *	Malha preenchendo o vetor de CLUSTERs
	 */
	for (EVER)
	{
		if (C_ISBAD_OR_EOF (ph_clusno))
			goto out;

		/* Coloca um CLUSTER no vetor */

		if (ph_clusno < 0 || ph_clusno >= sp->s_n_clusters + 2)
			{ printf ("%g: No. de cluster inválido: %d\n", ph_clusno); goto out; }

		if (log_clusno >= ip->i_clusvec_sz)
		{
			ip->i_clusvec_sz += CLUSVEC_INC;

			if ((clusvec = realloc_byte (clusvec, ip->i_clusvec_sz * sizeof (clusno_t))) == NULL)
				{ printf ("%g: Memória esgotada\n"); goto out; }
#ifdef	DEBUG
			printf ("%g: Aumentei para %d entradas, INODE %d\n", ip->i_clusvec_sz, ip->i_ino);
#endif	DEBUG
		   /***	ip->i_clusvec_sz  = ...;	/* Acima	***/
		   /***	ip->i_clusvec_len = ...;	/* Não muda	***/
			ip->i_clusvec	  = (int)clusvec;
		}

		clusvec[log_clusno++] = ph_clusno;

		/* Obtém o novo CLUSTER: Calcula o deslocamento no arquivo da FAT */

		if   (sp->s_fat_bits == 12)
			offset = (ph_clusno * 3) >> 1;
		elif (sp->s_fat_bits == 16)
			offset = ph_clusno << 1;
		else	/* fat_bits == 32 */
			offset = ph_clusno << 2;

		/* Le a região da FAT do disco */

		blkno = sp->s_fat1_blkno + (offset >> BLSHIFT);

		if (bp == NOBHEAD || bp->b_blkno != BL4BASEBLK (blkno))
		{
			if (bp != NOBHEAD)
				block_put (bp);

			bp = bread (unisp->sb_dev, blkno, 0);

			if (u.u_error)
				{ printf ("%g: Erro de leitura da FAT\n"); goto out; }
		}

		area = bp->b_base_addr + BL4BASEOFF (blkno) + (offset & BLMASK);

		l = FAT_GET_LONG (area);

		/* Verifica se por acaso não tem um pedaço no bloco seguinte */

		if (sp->s_fat_bits == 12 && (offset & BLMASK) == 511)
		{
			BHEAD		*next_bp;
			unsigned char	c;

			if (BL4BASEBLK (blkno + 1) != BL4BASEBLK (blkno))
			{
				next_bp = bread (unisp->sb_dev, blkno + 1, 0);

				if (u.u_error)
				{
					printf ("%g: Erro de leitura da FAT\n");
					block_put (next_bp); goto out;
				}

				c = ((char *)next_bp->b_addr)[0];

				block_put (next_bp);
			}
			else
			{
				c = ((char *)bp->b_base_addr + BL4BASEOFF (blkno) + 512)[0];
			}

			l &= 0xFF; l |= (c << 8);
		}

		/* Retira o conteúdo da FAT */

		if (sp->s_fat_bits == 12)
		{
			if (ph_clusno & 1)
				value = (l >> 4) & 0xFFF;
			else
				value =  l	 & 0xFFF;
		}
		elif (sp->s_fat_bits == 16)
		{
			value = l & 0xFFFF;
		}
		else	/* fat_bits == 32 */
		{
			value = l;
		}

		ph_clusno = value;

	}	/* end for (EVER) */

	/*
	 *	Epílogo
	 */
    out:
	if (bp != NOBHEAD)
		block_put (bp);

	ip->i_clusvec_len = log_clusno;

	while (log_clusno < ip->i_clusvec_sz)
		clusvec[log_clusno++] = C_STD_EOF;

}	/* end fat_creat_clusvec */

/*
 ****************************************************************
 *	Libera o vetor de CLUSTERs				*
 ****************************************************************
 */
void
fat_free_clusvec (INODE *ip)
{
	if (ip->i_clusvec != NULL)
		free_byte ((void *)ip->i_clusvec);

   /***	ip->i_clusvec_sz  = 0;	***/
   /***	ip->i_clusvec_len = 0;	***/
	ip->i_clusvec	  = NULL;

}	/* end fat_free_clusvec */
