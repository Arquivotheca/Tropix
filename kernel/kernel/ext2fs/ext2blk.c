/*
 ****************************************************************
 *								*
 *			ext2blk.c				*
 *								*
 *	Alocação/desalocação de blocos do disco			*
 *								*
 *	Versão	4.3.0, de 30.07.02				*
 *		4.4.0, de 07.11.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ext2.h"
#include "../h/bhead.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
daddr_t		ext2_alloc_block (SB *sp, daddr_t hint_bno);
daddr_t		ext2_alloc_block_in_group (SB *sp, int group_index, int hint_index);

/*
 ****************************************************************
 *	Mapeamento do endereco do bloco da EXT2			*
 ****************************************************************
 */
daddr_t
ext2_block_map (INODE *ip, daddr_t lbn, int rw, daddr_t *rabno_ptr)
{
	int		index;
	BHEAD		*bp;
	EXT2SB		*ext2sp;
	int		level, shift, group_index;
	daddr_t 	pbn, end_lbn;
	daddr_t 	hint_pbn, default_hint_pbn;
	dev_t		dev;

	/*
	 *  Retorna:
	 *	< 0	=> READ:  O lbn não pertence ao Arquivo
	 *		   WRITE: Não conseguiu alocar um Bloco
	 *		   Se *rabno_ptr == (daddr_t)-1, foi alocado
	 *		   um bloco (que talvez deva ser zerado)
	 *	= 0	=> Erro (lbn inválido ou erro de I/O)
	 *		   u.u_error contém o código de erro
	 *	> 0	=> O pbn
	 */

	/*
	 *	Verifica se o No. do BLOCO é válido
	 */
	if (lbn < 0)
		{ u.u_error = EFBIG; return (NODADDR); }

	dev = ip->i_dev; ext2sp = ip->i_sb->sb_ptr;

	*rabno_ptr = NODADDR;

	group_index = (ip->i_ino - 1) / ext2sp->s_inodes_per_group;

	default_hint_pbn = group_index * ext2sp->s_blocks_per_group + ext2sp->s_first_data_block;

	/*
	 *	BLOCOS 0 a EXT2_NADDR-4 são diretos
	 */
	if (lbn < EXT2_NADDR-3)
	{
		/*
		 *	Verifica se este BLOCO já pertence ao Arquivo
		 */
		if ((pbn = ip->i_addr[lbn]) == NODADDR)
		{
			/*
			 *	O BLOCO não pertence ao Arquivo. Se ele deve
			 *	ser lido, ou se deve ser escrito e não há
			 *	blocos livres no disco entao retorna (-1)
			 */
			if (rw == B_READ)
				return (-1);

			if (lbn == 0 || (hint_pbn = ip->i_addr[lbn - 1]) == NODADDR)
				hint_pbn = default_hint_pbn;

			if ((pbn = ext2_alloc_block (ip->i_sb, hint_pbn)) == NODADDR)
				return (-1);

			ip->i_blocks += (ext2sp->s_BLOCK_SZ >> BLSHIFT);

			*rabno_ptr = (daddr_t)-1;

			ip->i_addr[lbn] = pbn;
			inode_dirty_inc (ip);

			return (pbn);
		}

		/*
		 *	Calcula um BLOCO de "Lookahead"
		 */
		if (lbn < EXT2_NADDR-4)
			*rabno_ptr = ip->i_addr[lbn + 1];

		return (pbn);
	}

	/*
	 *	Os Enderecos EXT2_NADDR-1, EXT2_NADDR-2 e EXT2_NADDR-3 são BLOCOS indiretos.
	 *
	 *	O Primeiro passo é determinar o nivel de indireção
	 */
	shift = 0; end_lbn = 1; lbn -= EXT2_NADDR-3;

	for (level = 1; /* abaixo */; level++)
	{
		if (level > 3)
			{ u.u_error = EFBIG; return (NODADDR); }

		shift += ext2sp->s_INDIR_SHIFT; end_lbn <<= ext2sp->s_INDIR_SHIFT;

		if (lbn < end_lbn)
			break;

		lbn -= end_lbn;
	}

	/*
	 *	Processa a RAIZ da árvore dos blocos indiretos
	 */
	if ((pbn = ip->i_addr[(EXT2_NADDR - 4) + level]) == NODADDR)
	{
		if (rw == B_READ)
			return (-1);

		if ((hint_pbn = ip->i_addr[(EXT2_NADDR - 4) + level - 1]) == NODADDR)
			hint_pbn = default_hint_pbn;

		if ((pbn = ext2_alloc_block (ip->i_sb, hint_pbn)) == NODADDR)
			return (-1);

		ip->i_blocks += (ext2sp->s_BLOCK_SZ >> BLSHIFT);

		bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);
		memclr (bp->b_addr, ext2sp->s_BLOCK_SZ);
		bp->b_residual = 0;

		bdwrite (bp);

		ip->i_addr[(EXT2_NADDR - 4) + level] = pbn;
		inode_dirty_inc (ip);
	}

	default_hint_pbn = pbn;

	/*
	 *	Calcula o No. do BLOCO, consultando os BLOCOS Indiretos
	 */
	for (/* acima */;  level >= 1;  level--)
	{
		daddr_t		*addr;

		bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);

		if (bp->b_flags & B_ERROR)
			{ block_put (bp); return (NODADDR); }

		shift -= ext2sp->s_INDIR_SHIFT; index = (lbn >> shift) & ext2sp->s_INDIR_MASK;

		addr = bp->b_addr; pbn = addr[index];

		if (level == 1 && index < ext2sp->s_INDIR_SZ - 1)
			*rabno_ptr = addr[index + 1];

		if (pbn == NODADDR)
		{
			if (rw == B_READ)
				{ block_put (bp); return (-1); }

			hint_pbn = NODADDR;

			if (index > 0)
				hint_pbn = addr[index - 1];

			if (hint_pbn == NODADDR)
				hint_pbn = default_hint_pbn;

			if ((pbn = ext2_alloc_block (ip->i_sb, hint_pbn)) == NODADDR)
				{ block_put (bp); return (-1); }

			ip->i_blocks += (ext2sp->s_BLOCK_SZ >> BLSHIFT);

			addr[index] = pbn;

			bdwrite (bp);

			if (level == 1)
			{
				*rabno_ptr = (daddr_t)-1;
			}
			else
			{
				bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);
				memclr (bp->b_addr, ext2sp->s_BLOCK_SZ);
				bp->b_residual = 0;

				bdwrite (bp);
			}
		}
		else
		{
			block_put (bp);
		}

		default_hint_pbn = pbn;
	}

	return (pbn);

}	/* end ext2_block_map */

/*
 ****************************************************************
 *	Procura um bloco vago para DADOs (perto de um dado)	*
 ****************************************************************
 */
daddr_t
ext2_alloc_block (SB *sp, daddr_t hint_bno)
{
	const EXT2SB	*ext2sp = sp->sb_ptr;
	int		central_group_index, group_index, hint_index;
	int		out, delta, end_index;
	daddr_t		bno;

	/*
	 *	Procura nos GRUPO dado
	 */
	hint_bno -= ext2sp->s_first_data_block;

	end_index	    = ext2sp->s_blocks_per_group;
	central_group_index = hint_bno / ext2sp->s_blocks_per_group;
	hint_index	    = hint_bno % ext2sp->s_blocks_per_group; 

	if ((bno = ext2_alloc_block_in_group (sp, central_group_index, hint_index)) != NODADDR)
		return (bno);

	/*
	 *	Procura nos outros GRUPOs
	 */
	for (out = 0, delta = 1; /* abaixo */; delta += 1)
	{
		if (out == 0x03)
			return (NODADDR);

		if ((out & 0x01) == 0)
		{
			if   ((group_index = central_group_index - delta) < 0)
				out |= 0x01;
			elif ((bno = ext2_alloc_block_in_group (sp, group_index, end_index)) != NODADDR)
				return (bno);
		}

		if ((out & 0x02) == 0)
		{
			if   ((group_index = central_group_index + delta) >= ext2sp->s_group_cnt)
				out |= 0x02;
			elif ((bno = ext2_alloc_block_in_group (sp, group_index, -1)) != NODADDR)
				return (bno);
		}
	}

}	/* end ext2_alloc_block */

/*
 ****************************************************************
 *	Procura um bloco vago para DADOs em um mapa de bits 	*
 ****************************************************************
 */
daddr_t
ext2_alloc_block_in_group (SB *sp, int group_index, int hint_index)
{
	BHEAD		*map_bp;
	EXT2SB		*ext2sp = sp->sb_ptr;
	EXT2BG		*bgp;
	int		index, out, delta;
	char		*cp;
	daddr_t		bno;

	/*
	 *	Obtém as informações do grupo
	 */
	bgp = &ext2sp->s_bg[group_index];

	if (bgp->bg_free_blocks_count == 0)
		return (NODADDR);

	/*
	 *	Obtém o mapa de bits dado
	 */
	map_bp = bread (sp->sb_dev, EXT2_BLOCKtoBL (bgp->bg_block_bitmap), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return (NODADDR); }

	/*
	 *	Procura um BLOCO livre
	 */
	for (cp = map_bp->b_addr, out = 0, delta = 1; /* abaixo */; delta++)
	{
		if (out == 0x03)
			{ block_put (map_bp); return (NODADDR); }

		if ((out & 0x01) == 0)
		{
			if   ((index = hint_index + delta) >= ext2sp->s_blocks_per_group)
				out |= 0x01;
			elif (EXT2_GET_MAP (cp, index) == 0)
				break;
		}

		if ((out & 0x02) == 0)
		{
			if   ((index = hint_index - delta) < 0)
				out |= 0x02;
			elif (EXT2_GET_MAP (cp, index) == 0)
				break;
		}
	}

	/*
	 *	Examina o valor obtido
	 */
	bno = group_index * ext2sp->s_blocks_per_group + ext2sp->s_first_data_block + index;

	if (bno >= ext2sp->s_blocks_count)
	{
		printf ("%g: Bloco %d inválido, dev = %v\n", bno, sp->sb_dev);
		block_put (map_bp); return (NODADDR);
	}

	/*
	 *	Aloca o BLOCO
	 */
	EXT2_SET_MAP (cp, index);

	bdwrite (map_bp);

	SPINLOCK (&sp->sb_lock);

	bgp->bg_free_blocks_count--;

	ext2sp->s_free_blocks_count--; sp->sb_sbmod = 1;

	SPINFREE (&sp->sb_lock);

#ifdef	DEBUG
	printf ("group_index = %d, index = %d, bno = %d\n", group_index, index, bno);
#endif	DEBUG

	return (bno);

}	/* end ext2_alloc_block_in_group */

/*
 ****************************************************************
 *	Libera blocos indiretos					*
 ****************************************************************
 */
void
ext2_indir_block_release (SB *sp, daddr_t indir_bno, int level)
{
	EXT2SB		*ext2sp = sp->sb_ptr;
	int		index;
	BHEAD		*bp;
	daddr_t		bno, *addr;

	/*
	 *	Lê o bloco de endereços
	 */
	bp = bread (sp->sb_dev, EXT2_BLOCKtoBL (indir_bno), 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return; }

	/*
	 *	Verifica se precisa copiar o bloco
	 */
	if (ext2sp->s_BLOCK_SZ != BL4SZ)
	{
		if ((addr = malloc_byte (ext2sp->s_BLOCK_SZ)) == NOVOID)
			{ printf ("%g: Não consegui memória\n"); block_put (bp); return; }

		memmove (addr, bp->b_addr, ext2sp->s_BLOCK_SZ); block_put (bp);
	}
	else
	{
		addr = bp->b_addr;
	}

	/*
	 *	Percorre todos os endereços deste bloco
	 */
	for (index = ext2sp->s_INDIR_SZ - 1; index >= 0; index--)
	{
		if ((bno = addr[index]) == NODADDR)
			continue;

		if (level > 1)
			ext2_indir_block_release (sp, bno, level - 1);
		else
			ext2_block_release (sp, bno);
	}

	/*
	 *	Epílogo: libera o bloco dado.
	 */
	ext2_block_release (sp, indir_bno);

	if (ext2sp->s_BLOCK_SZ == BL4SZ)
		block_put (bp);
	else
		free_byte (addr);

}	/* end ext2_indir_block_release */

/*
 ****************************************************************
 *	Devolve um BLOCO à Lista Livre do Disco			*
 ****************************************************************
 */
void
ext2_block_release (SB *sp, daddr_t bno)
{
	EXT2SB		*ext2sp = sp->sb_ptr;
	EXT2BG		*bgp;
	BHEAD		*map_bp;
	int		group_index, index;

	/*
	 *	Confere a validade do bloco
	 */
	if ((unsigned)bno >= ext2sp->s_blocks_count)
	{
		printf ("%g: Tentando liberar o bloco %d inválido do dispositivo %v\n", bno, sp->sb_dev);
		return;
	}

	/*
	 *	Obtém a localização do bloco
	 */
	group_index = (bno - ext2sp->s_first_data_block) / ext2sp->s_blocks_per_group;
	index	    = (bno - ext2sp->s_first_data_block) % ext2sp->s_blocks_per_group;

	/*
	 *	Obtém o mapa de bits
	 */
	bgp = &ext2sp->s_bg[group_index];

	map_bp = bread (sp->sb_dev, EXT2_BLOCKtoBL (bgp->bg_block_bitmap), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return; }

	/*
	 *	Desaloca o BLOCO
	 */
	EXT2_CLR_MAP (map_bp->b_addr, index);

	bdwrite (map_bp);

	SPINLOCK (&sp->sb_lock);

	bgp->bg_free_blocks_count++;

	ext2sp->s_free_blocks_count++; sp->sb_sbmod = 1;

	SPINFREE (&sp->sb_lock);

}	/* end ext2_block_release */
