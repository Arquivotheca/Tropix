/*
 ****************************************************************
 *								*
 *			t1blk.c					*
 *								*
 *	Alocação/desalocação de blocos do disco			*
 *								*
 *	Versão	4.3.0, de 30.07.02				*
 *		4.6.0, de 19.09.04				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/t1.h"
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
daddr_t		t1_get_free_block_in_map (SB *sp, daddr_t map_bno, int hint_index, int type);
const char	*t1_edit_block_type (int type);

/*
 ****************************************************************
 *	Mapeamento do endereco do bloco da T1			*
 ****************************************************************
 */
daddr_t
t1_block_map (INODE *ip, daddr_t lbn, int rw, daddr_t *rabno_ptr)
{
	int		index;
	BHEAD		*bp;
	T1SB		*t1sp;
	int		level, shift, type;
	daddr_t 	pbn;
	daddr_t 	hint_pbn, default_hint_pbn, end_lbn;
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
	 *
	 *  Todos os números de blocos de 4 KB
	 */

	/*
	 *	Verifica se o No. do BLOCO é válido
	 */
	if (lbn < 0)
		{ u.u_error = EFBIG; return (NODADDR); }

	dev = ip->i_dev; t1sp = ip->i_sb->sb_ptr;

	*rabno_ptr = NODADDR;

	default_hint_pbn = T1_INOtoADDR (ip->i_ino);

	/*
	 *	BLOCOS 0 a T1_NADDR-4 são diretos
	 */
	if (lbn < T1_NADDR-3)
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

			if ((pbn = t1_block_alloc (ip->i_sb, hint_pbn, T1_MAP_DATA)) <= NODADDR)
				return (-1);

			*rabno_ptr = (daddr_t)-1;

#if (0)	/*******************************************************/
			bp = block_get (dev, T1_BL4toBL (pbn), 0);
			memclr (bp->b_addr, BL4SZ);
			bp->b_residual = 0;

			bdwrite (bp);
#endif	/*******************************************************/

			ip->i_addr[lbn] = pbn;
			inode_dirty_inc (ip);

			return (pbn);
		}

		/*
		 *	Calcula um BLOCO de "Lookahead"
		 */
		if (lbn < T1_NADDR-4)
			*rabno_ptr = ip->i_addr[lbn + 1];

		if (pbn <= T1_BMNO || pbn >= t1sp->s_fs_sz)
		{
			printf ("%g: Bloco %d inválido, dev = %v, ino = %d\n", pbn, dev, ip->i_ino);

			pbn = NODADDR;

			ip->i_addr[lbn] = NODADDR;
			inode_dirty_inc (ip);
		}

		return (pbn);
	}

	/*
	 *	Os Enderecos T1_NADDR-1, T1_NADDR-2 e T1_NADDR-3 são BLOCOS indiretos.
	 *
	 *	O Primeiro passo é determinar o nivel de indireção
	 */
	shift = 0; end_lbn = 1; lbn -= T1_NADDR-3;

	for (level = 1; /* abaixo */; level++)
	{
		if (level > 3)
			{ u.u_error = EFBIG; return (NODADDR); }

		shift += T1_INDIR_SHIFT; end_lbn <<= T1_INDIR_SHIFT;

		if (lbn < end_lbn)
			break;

		lbn -= end_lbn;
	}

	/*
	 *	Processa a RAIZ da árvore dos blocos indiretos
	 */
	if ((pbn = ip->i_addr[(T1_NADDR - 4) + level]) == NODADDR)
	{
		if (rw == B_READ)
			return (-1);

		if ((hint_pbn = ip->i_addr[(T1_NADDR - 4) + level - 1]) == NODADDR)
			hint_pbn = default_hint_pbn;

		if ((pbn = t1_block_alloc (ip->i_sb, hint_pbn, T1_MAP_INDIR)) <= NODADDR)
			return (-1);

		bp = block_get (dev, T1_BL4toBL (pbn), 0);
		memclr (bp->b_addr, BL4SZ);
		bp->b_residual = 0;

		bdwrite (bp);

		ip->i_addr[(T1_NADDR - 4) + level] = pbn;
		inode_dirty_inc (ip);
	}

	default_hint_pbn = pbn;

	/*
	 *	Calcula o No. do BLOCO, consultando os BLOCOS Indiretos
	 */
	for (/* acima */;  level >= 1;  level--)
	{
		daddr_t		*addr;

		bp = bread (dev, T1_BL4toBL (pbn), 0);

		if (bp->b_flags & B_ERROR)
			{ block_put (bp); return (NODADDR); }

		shift -= T1_INDIR_SHIFT; index = (lbn >> shift) & T1_INDIR_MASK;

		addr = bp->b_addr; pbn = addr[index];

		if (level == 1 && index < T1_INDIR_SZ - 1)
			*rabno_ptr = addr[index + 1];

		if (pbn == NODADDR)
		{
			if (rw == B_READ)
				{ block_put (bp); return (-1); }

			type = (level == 1) ? T1_MAP_DATA : T1_MAP_INDIR;

			hint_pbn = NODADDR;

			if (index > 0)
				hint_pbn = addr[index - 1];

			if (hint_pbn == NODADDR)
				hint_pbn = default_hint_pbn;

			if ((pbn = t1_block_alloc (ip->i_sb, hint_pbn, type)) <= NODADDR)
				{ block_put (bp); return (-1); }

			addr[index] = pbn;

			bdwrite (bp);

			if (level == 1)
			{
				*rabno_ptr = (daddr_t)-1;
			}
			else
			{
				bp = block_get (dev, T1_BL4toBL (pbn), 0);
				memclr (bp->b_addr, BL4SZ);
				bp->b_residual = 0;

				bdwrite (bp);
			}
		}
		else
		{
			if (pbn <= T1_BMNO || pbn >= t1sp->s_fs_sz)
			{
				printf ("%g: Bloco %d inválido, dev = %v, ino = %d\n", pbn, dev, ip->i_ino);

				addr[index] = NODADDR;

				bdwrite (bp);

				return (-1);
			}

			block_put (bp);
		}

		default_hint_pbn = pbn;
	}

	return (pbn);

}	/* end t1_block_map */

/*
 ****************************************************************
 *	Procura um BLOCO vago (perto de um dado)		*
 ****************************************************************
 */
daddr_t
t1_block_alloc (SB *sp, daddr_t hint_bno, int type)
{
	T1SB		*t1sp = sp->sb_ptr;
	daddr_t		hint_map_bno, map_bno;
	int		hint_index, delta, out;
	daddr_t		bno;

	/*
	 *	O bloco dado "hint_bno" já está sendo usado.
	 *
	 *	Pequena consistência
	 */
	if (hint_bno == 0)
		printf ("%g: Recebeu HINT == 0\n");

	/*
	 *	Procura em primeiro lugar na ZONA do bloco sugerido
	 */
	T1_ASSIGN_MAP_PARAM (hint_bno, hint_map_bno, hint_index);

	if ((bno = t1_get_free_block_in_map (sp, hint_map_bno, hint_index, type)) != NODADDR)
		return (bno);

	/*
	 *	Procura nas outras ZONAs
	 */
	for (out = 0, delta = T1_ZONE_SZ; /* abaixo */; delta += T1_ZONE_SZ)
	{
		if (out == 0x03)
			return (NODADDR);

		if ((out & 0x01) == 0)
		{
			if   ((map_bno = hint_map_bno - delta) < T1_BMNO)
				out |= 0x01;
			elif ((bno = t1_get_free_block_in_map (sp, map_bno, T1_ZONE_SZ, type)) != NODADDR)
				return (bno);
		}

		if ((out & 0x02) == 0)
		{
			if   ((map_bno = hint_map_bno + delta) >= t1sp->s_fs_sz)
				out |= 0x02;
			elif ((bno = t1_get_free_block_in_map (sp, map_bno, -1, type)) != NODADDR)
				return (bno);
		}
	}

}	/* end t1_block_alloc */

/*
 ****************************************************************
 *	Procura um BLOCO vago em um mapa de bits 		*
 ****************************************************************
 */
daddr_t
t1_get_free_block_in_map (SB *sp, daddr_t map_bno, int hint_index, int type)
{
	T1SB		*t1sp = sp->sb_ptr;
	BHEAD		*map_bp;
	int		index, delta, out;
	daddr_t		data_bno;
	char		*cp;

	/*
	 *	Obtém o mapa de bits dado
	 */
	map_bp = bread (sp->sb_dev, T1_BL4toBL (map_bno), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return (NODADDR); }

	cp = map_bp->b_addr;

if (T1_GET_MAP (cp, T1_BMNO) != T1_MAP_MAP)
{
	printf ("O Bloco %d de \"%v\" NÃO contém um mapa de bits\n", map_bno, sp->sb_dev);
	print_calls ();
}

	for (out = 0, delta = 1; /* abaixo */; delta++)
	{
		if (out == 0x03)
			{ block_put (map_bp); return (NODADDR); }

		if ((out & 0x01) == 0)
		{
			if   ((index = hint_index + delta) >= T1_ZONE_SZ)
				out |= 0x01;
			elif (T1_GET_MAP (cp, index) == T1_MAP_FREE)
				break;
		}

		if ((out & 0x02) == 0)
		{
			if   ((index = hint_index - delta) < 0)
				out |= 0x02;
			elif (T1_GET_MAP (cp, index) == T1_MAP_FREE)
				break;
		}
	}

	/*
	 *	Achou um bloco
	 */
	data_bno = (map_bno - T1_BMNO) + index;

	T1_PUT_MAP (cp, index, type);

	SPINLOCK (&sp->sb_lock);
	t1sp->s_busy_bl++; sp->sb_sbmod = 1;
	SPINFREE (&sp->sb_lock);

	bdwrite (map_bp);

	return (data_bno);

}	/* end t1_get_free_block_in_map */

/*
 ****************************************************************
 *	Libera blocos indiretos					*
 ****************************************************************
 */
void
t1_indir_block_release (SB *sp, daddr_t indir_bno, int level)
{
	int		index;
	BHEAD		*bp;
	daddr_t		bno;
	daddr_t		*addr;

	/*
	 *	Lê o bloco de endereços
	 */
	bp = bread (sp->sb_dev, T1_BL4toBL (indir_bno), 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return; }

	addr = bp->b_addr;

	/*
	 *	Percorre todos os enderecos deste bloco
	 */
	for (index = T1_INDIR_SZ - 1; index >= 0; index--)
	{
		if ((bno = addr[index]) == NODADDR)
			continue;

		if (level > 1)
			t1_indir_block_release (sp, bno, level - 1);
		else
			t1_block_release (sp, bno);
	}

	/*
	 *	Epílogo: libera o bloco dado.
	 */
	t1_block_release (sp, indir_bno);

	block_put (bp);

}	/* end t1_indir_block_release */

/*
 ****************************************************************
 *	Devolve um BLOCO à Lista Livre do Disco			*
 ****************************************************************
 */
void
t1_block_release (SB *sp, daddr_t bno)
{
	T1SB		*t1sp = sp->sb_ptr;
	daddr_t		map_bno;
	BHEAD		*map_bp;
	char		*cp;
	int		index, type;

	/*
	 *	Confere a validade do bloco
	 */
	if (bno <= T1_BMNO || bno >= t1sp->s_fs_sz)
	{
		printf ("%g: Tentando liberar o bloco %d inválido do dispositivo %v\n", bno, sp->sb_dev);
		return;
	}

	/*
	 *	Lê o bloco do mapa
	 */
	T1_ASSIGN_MAP_PARAM (bno, map_bno, index);

	map_bp = bread (sp->sb_dev, T1_BL4toBL (map_bno), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return; }

	cp = map_bp->b_addr;

if (T1_GET_MAP (cp, T1_BMNO) != T1_MAP_MAP)
{
	printf ("O Bloco %d de \"%v\" NÃO contém um mapa de bits\n", map_bno, sp->sb_dev);
	print_calls ();
}

	/*
	 *	Examina o estado atual do bloco
	 */
	switch (type = T1_GET_MAP (cp, index))
	{
	    case T1_MAP_DATA:
	    case T1_MAP_INDIR:
	    case T1_MAP_INODE_EMPTY:
		T1_PUT_MAP (cp, index, T1_MAP_FREE);

		SPINLOCK (&sp->sb_lock);
		t1sp->s_busy_bl--; sp->sb_sbmod = 1;
		SPINFREE (&sp->sb_lock);

		bdwrite (map_bp);
		break;

	    default:
		printf
		(	"%g: Tentando liberar o bloco %d de tipo %s do dispositivo %v\n",
			bno, t1_edit_block_type (type), sp->sb_dev
		);
		block_put (map_bp);

		break;
	}

}	/* end t1_block_release */

/*
 ****************************************************************
 *	Edita um tipo de bloco					*
 ****************************************************************
 */
const char *
t1_edit_block_type (int type)
{
	switch (type)
	{
	    case T1_MAP_FREE:
		return ("FREE");

	    case T1_MAP_MAP:
		return ("MAP");

	    case T1_MAP_DATA:
		return ("DATA");

	    case T1_MAP_INDIR:
		return ("INDIR");

	    case T1_MAP_INODE_EMPTY:
		return ("INODE_EMPTY");

	    case T1_MAP_INODE_FULL:
		return ("INODE_FULL");

	    case T1_MAP_BAD:
		return ("BAD");

	    case T1_MAP_OUT:
		return ("OUT");

	    default:
		return ("???");
	};

}	/* end t1_edit_block_type */
