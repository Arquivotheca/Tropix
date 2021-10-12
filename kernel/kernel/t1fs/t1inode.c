/*
 ****************************************************************
 *								*
 *			t1inode.c				*
 *								*
 *	Alocação e desalocação de INODEs no disco		*
 *								*
 *	Versão	4.3.0, de 29.07.02				*
 *		4.5.0, de 03.10.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/t1.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
ino_t		t1_alloc_inode_in_dev (SB *sp, daddr_t hint_bno);
ino_t		t1_alloc_inode_in_map (SB *sp, daddr_t map_bno, int hint_index);

/*
 ****************************************************************
 *	Lê e converte um INODE T1FS do disco para a memória	*
 ****************************************************************
 */
int
t1_read_inode (INODE *ip)
{
	BHEAD		*bp;
	T1DINO		*dp;
	ulong		ifmt;
#define	SB_USER_UID
#ifdef	SB_USER_UID
	const SB	*sp;
#endif	SB_USER_UID

	/*
	 *	Le o bloco correspondente
	 */
	bp = bread (ip->i_dev, T1_BL4toBL (T1_INOtoADDR (ip->i_ino)), 0);

	if (bp->b_flags & B_ERROR)
	{
		printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
		block_put (bp); return (-1);
	}

	dp = (T1DINO *)bp->b_addr + T1_INOtoINDEX (ip->i_ino);

	/*
	 *	Copia diversos campos em comum
	 */
	ip->i_mode	= dp->n_mode;

#ifdef	SB_USER_UID
	sp = ip->i_sb;

	if (sp->sb_mntent.me_flags & SB_USER)
	{
		ip->i_uid = sp->sb_uid;
		ip->i_gid = sp->sb_gid;
	}
	else
	{
		ip->i_uid = dp->n_uid;
		ip->i_gid = dp->n_gid;
	}
#else
	ip->i_uid	= dp->n_uid;
	ip->i_gid	= dp->n_gid;
#endif	SB_USER_UID

	ip->i_nlink	= dp->n_nlink;
	ip->i_size	= dp->n_size;
   /***	ip->i_high_size	= dp->n_high_size; ***/

	ip->i_atime	= dp->n_atime;
	ip->i_mtime	= dp->n_mtime;
	ip->i_ctime	= dp->n_ctime;

	memmove (ip->i_addr, dp->n_addr, sizeof (dp->n_addr));

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = ip->i_mode & IFMT) == IFCHR)
	{
		ip->i_fsp  = &fstab[FS_CHR];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	elif (ifmt == IFBLK)
	{
		ip->i_fsp  = &fstab[FS_BLK];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	else
	{
		ip->i_fsp  = &fstab[FS_T1];
		ip->i_rdev = 0;
	}

	block_put (bp);

	return (0);

}	/* end t1_read_inode */

/*
 ****************************************************************
 *	Converte o formato do INODE: memória para disco		*
 ****************************************************************
 */
void
t1_write_inode (INODE *ip)
{
	T1DINO		*dp;
	BHEAD		*bp;
	ulong		ifmt;

	/*
	 *	Verifica se não houve modificação no INODE
	 *	ou entao ele está montado somente para leituras
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

	/*
	 *	Le o bloco contendo o INODE do disco
	 */
	bp = bread (ip->i_dev, T1_BL4toBL (T1_INOtoADDR (ip->i_ino)), 0);

	if (bp->b_flags & B_ERROR)
	{
		printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
		block_put (bp); return;
	}

	/*
	 *	Copia diversos campos em comum
	 */
	dp = (T1DINO *)bp->b_addr + T1_INOtoINDEX (ip->i_ino);

	dp->n_mode	= ip->i_mode;
	dp->n_uid	= ip->i_uid;
	dp->n_gid	= ip->i_gid;
	dp->n_nlink	= ip->i_nlink;
	dp->n_size	= ip->i_size;
   /***	dp->n_high_size	= ip->i_high_size; ***/

	dp->n_atime	= ip->i_atime;
	dp->n_mtime	= ip->i_mtime;
	dp->n_ctime	= ip->i_ctime;

	if ((ifmt = ip->i_mode & IFMT) == IFBLK || ifmt == IFCHR)
		ip->i_addr[0] = (daddr_t)ip->i_rdev;

	memmove (dp->n_addr, ip->i_addr, sizeof (dp->n_addr));

	bdwrite (bp);

#ifdef	MSG
	if (CSWT (6))
		printf ("%g: Atualizando o INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
#endif	MSG

}	/* end t1_write_inode */

/*
 ****************************************************************
 *	Cria um arquivo novo					*
 ****************************************************************
 */
INODE *
t1_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip;
	int		ifmt;
	daddr_t		hint_bno;

	/*
	 *	Tenta obter um INODE no Dispositivo do pai
	 */
	hint_bno = T1_INOtoADDR (iop->io_ip->i_ino);

	if ((ip = t1_alloc_inode (iop->io_ip->i_sb, hint_bno)) == NOINODE)
		{ iput (iop->io_ip); return (NOINODE); }

	/*
	 *	Prepara o INODE
	 */
	if ((mode & IFMT) == 0)
		mode |= IFREG;

	ip->i_mode	= mode & ~u.u_cmask;
	ip->i_nlink	= 1;
	ip->i_uid	= u.u_euid;
	ip->i_gid	= u.u_egid;

	ip->i_rdev	= 0;
	ip->i_size	= 0;

	memclr (ip->i_addr, sizeof (ip->i_addr));

	ip->i_atime	= time;
	ip->i_mtime	= time;
	ip->i_ctime	= time;

	inode_dirty_inc (ip);

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = mode & IFMT) == IFCHR)
		ip->i_fsp  = &fstab[FS_CHR];
	elif (ifmt == IFBLK)
		ip->i_fsp  = &fstab[FS_BLK];
	else
		ip->i_fsp  = &fstab[FS_T1];

	/*
	 *	Escreve o nome do novo arquivo no Diretorio pai
	 */
	dep->d_ino = ip->i_ino;

	t1_write_dir (iop, dep, ip, 0 /* NO rename */);

	return (ip);

}	/* end t1_make_inode */

/*
 ****************************************************************
 *	Libera todos os blocos de um arquivo			*
 ****************************************************************
 */
void
t1_trunc_inode (INODE *ip)
{
	int		index;
	daddr_t		bno;

	/*
	 *	Examina o tipo do arquivo
	 */
	switch (ip->i_mode & IFMT)
	{
#if (0)	/*******************************************************/
	    case IFBLK:
	    case IFCHR:
		ip->i_rdev = 0;
		return;
#endif	/*******************************************************/

	    case IFLNK:
		if (ip->i_size <= T1_NADDR * sizeof (daddr_t) - 1)
		{
			memclr (ip->i_addr, sizeof (ip->i_addr));
			break;
		}
	    case IFREG:
	    case IFDIR:
		for (index = T1_NADDR - 1; index >= 0; index--)
		{
			/* Percorre os diversos niveis de indireção */

			if ((bno = ip->i_addr[index]) == NODADDR)
				continue;
	
			ip->i_addr[index] = NODADDR;
	
			/*
			 *	Verifica qual o nivel de indireção
			 */
			if (index < T1_NADDR - 3)
				t1_block_release (ip->i_sb, bno);
			else
				t1_indir_block_release (ip->i_sb, bno, index - (T1_NADDR - 4));
		}

		break;

	    default:
		return;

	}	/* end switch (mode) */

	/*
	 *	Epilogo
	 */
	ip->i_size = 0;

	ip->i_mtime = time;
	inode_dirty_inc (ip);

}	/* end t1_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
t1_free_inode (const INODE *ip)
{
	SB		*sp = ip->i_sb;
	T1SB		*t1sp = sp->sb_ptr;
	daddr_t		inode_bno, map_bno;
	BHEAD		*inode_bp, *map_bp;
	T1DINO		*np;
	char		*cp;
	int		map_index, ino_index;
	int		type, new_type, inode_count;

	/*
	 *	Em primeiro lugar, lê o bloco do mapa
	 */
	inode_bno = T1_INOtoADDR (ip->i_ino);

	T1_ASSIGN_MAP_PARAM (inode_bno, map_bno, map_index);

	map_bp = bread (sp->sb_dev, T1_BL4toBL (map_bno), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return; }

#define	SAVE
#ifdef	SAVE
	if (T1_GET_MAP (((char *)map_bp->b_addr), T1_BMNO) != T1_MAP_MAP)
	{
		printf ("O Bloco %d de \"%v\" NÃO contém um mapa de bits\n", map_bno, sp->sb_dev);
		print_calls ();
	}
#endif	SAVE

	/*
	 *	Em seguida, lê o bloco do INODE
	 */
	inode_bp = bread (sp->sb_dev, T1_BL4toBL (inode_bno), 0);

	if (inode_bp->b_flags & B_ERROR)
		{ block_put (inode_bp); block_put (map_bp); return; }

	/*
	 *	Conta o número de INODEs e obtém o novo tipo
	 */
	inode_count = 0;

	for (np = inode_bp->b_addr, ino_index = 0; ino_index < T1_INOinBL4; ino_index++, np++)
	{
		if (np->n_magic != T1_SBMAGIC)
		{
			printf ("%g: INODE %d de %v NÃO contém o número MÁGICO\n", ip->i_ino, sp->sb_dev);
			np->n_magic = T1_SBMAGIC;
		}

		if (np->n_mode != 0)
			inode_count++;
	}

	/*
	 *	Analisa o tipo antigo
	 */
	cp = map_bp->b_addr;

	switch (type = T1_GET_MAP (cp, map_index))
	{
	    case T1_MAP_INODE_EMPTY:
		if (inode_count < 1 || inode_count == T1_INOinBL4)
		{
			printf
			(	"%g: Código inválido (%s :: %s) do bloco de INODEs do dispositivo %v\n",
				t1_edit_block_type (type), t1_edit_block_type (T1_MAP_INODE_FULL), sp->sb_dev
			);
		}

		break;


	    case T1_MAP_INODE_FULL:
		if (inode_count != T1_INOinBL4)
		{
			printf
			(	"%g: Código inválido (%s :: %s) do bloco de INODEs do dispositivo %v\n",
				t1_edit_block_type (type), t1_edit_block_type (T1_MAP_INODE_EMPTY), sp->sb_dev
			);
		}
		break;

	    default:
		printf
		(	"%g: Código inválido %s do bloco de INODEs do dispositivo %v\n",
			t1_edit_block_type (type), sp->sb_dev
		);
		break;
	}

	/*
	 *	Zera a entrada
	 */
	np = (T1DINO *)inode_bp->b_addr + T1_INOtoINDEX (ip->i_ino);

	if (np->n_mode == 0)
		printf ("%g: INODE %d já estava ZERADO %v\n", ip->i_ino, sp->sb_dev);

	if (inode_count > 1)
	{
		memclr (np, sizeof (T1DINO)); np->n_magic = T1_SBMAGIC;

		new_type = T1_MAP_INODE_EMPTY;
	}
	else
	{
		memclr (inode_bp->b_addr, BL4SZ);

		new_type = T1_MAP_FREE;
	}

	/*
	 *	Atera o código e escreve o mapa
	 */
	T1_PUT_MAP (cp, map_index, new_type);

	bdwrite (inode_bp);

	bdwrite (map_bp);

	/*
	 *	Atualiza o SB
	 */
	SPINLOCK (&sp->sb_lock);

	t1sp->s_busy_ino--;

	if (new_type == T1_MAP_FREE)
		t1sp->s_busy_bl--;

	sp->sb_sbmod = 1;

	SPINFREE (&sp->sb_lock);

}	/* end t1_free_inode */

/*
 ****************************************************************
 *	Aloca um INODE no disco (e na memória)			*
 ****************************************************************
 */
INODE *
t1_alloc_inode (SB *sp, daddr_t hint_bno)
{
	T1SB		*t1sp = sp->sb_ptr;
	INODE		*ip;
	ino_t		ino;

	/*
	 *	Obtém um INODE do disco
	 */
	if ((ino = t1_alloc_inode_in_dev (sp, hint_bno)) == 0)
	{
		printf ("%g: O sistema de arquivos \"%v\" não tem mais INODEs livres\n", sp->sb_dev);

		u.u_error = ENOSPC; return (NOINODE);
	}

	/*
	 *	Achou um INODE livre
	 */
	SPINLOCK (&sp->sb_lock);
	t1sp->s_busy_ino++; sp->sb_sbmod = 1;
	SPINFREE (&sp->sb_lock);

	if ((ip = iget (sp->sb_dev, ino, 0)) == NOINODE)
	{
		printf ("%g: Não obtive o INODE (%v, %d)\n", sp->sb_dev, ino);

		return (NOINODE);
	}

	return (ip);

}	/* end t1_alloc_inode */

/*
 ****************************************************************
 *	Procura um bloco vago para INODEs perto de um dado	*
 ****************************************************************
 */
ino_t
t1_alloc_inode_in_dev (SB *sp, daddr_t hint_bno)
{
	T1SB		*t1sp = sp->sb_ptr;
	daddr_t		hint_map_bno, map_bno;
	int		hint_index, delta, out;
	ino_t		ino;

	/*
	 *	O Bloco "hint_bno" contém possivelmente ainda um INODE livre
	 *
	 *	Procura em primeiro lugar perto do bloco dado
	 */
	if (hint_bno == 0)
		printf ("%g: Recebeu HINT = 0\n");

	T1_ASSIGN_MAP_PARAM (hint_bno, hint_map_bno, hint_index);

	if ((ino = t1_alloc_inode_in_map (sp, hint_map_bno, hint_index)) != NOINO)
		return (ino);

	/*
	 *	Procura nas outras zonas
	 */
	for (out = 0, delta = T1_ZONE_SZ; /* abaixo */; delta += T1_ZONE_SZ)
	{
		if (out == 0x03)
			return (NOINO);

		if ((out & 0x01) == 0)
		{
			if   ((map_bno = hint_map_bno - delta) < T1_BMNO)
				out |= 0x01;
			elif ((ino = t1_alloc_inode_in_map (sp, map_bno, T1_ZONE_SZ - 1)) != NOINO)
				return (ino);
		}

		if ((out & 0x02) == 0)
		{
			if   ((map_bno = hint_map_bno + delta) >= t1sp->s_fs_sz)
				out |= 0x02;
			elif ((ino = t1_alloc_inode_in_map (sp, map_bno, 0)) != NOINO)
				return (ino);
		}
	}

}	/* end t1_alloc_inode_in_dev */

/*
 ****************************************************************
 *	Procura um bloco vago para INODEs em um mapa de bits 	*
 ****************************************************************
 */
ino_t
t1_alloc_inode_in_map (SB *sp, daddr_t map_bno, int hint_index)
{
	T1SB		*t1sp = sp->sb_ptr;
	BHEAD		*map_bp, *inode_bp;
	int		map_index, ino_index, free_ino_index, type, delta, inode_count, out;
	daddr_t		inode_bno;
	T1DINO		*np, *free_np;
	char		*cp;

	/*
	 *	Obtém o mapa de bits dado
	 */
	map_bp = bread (sp->sb_dev, T1_BL4toBL (map_bno), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return (NOINO); }

	cp = map_bp->b_addr;

#ifdef	SAVE
	if (T1_GET_MAP (cp, T1_BMNO) != T1_MAP_MAP)
	{
		printf ("O Bloco %d de \"%v\" NÃO contém um mapa de bits\n", map_bno, sp->sb_dev);
		print_calls ();
	}
#endif	SAVE

	/*
	 *	Procura um bloco de INODEs, e se não encontrar, um bloco livre
	 */
	type = T1_MAP_INODE_EMPTY;

	for (/* acima */; /* abaixo */; type = (type == T1_MAP_INODE_EMPTY) ? T1_MAP_FREE : -1)
	{
		if (type < 0)
			{ block_put (map_bp); return (NOINO); }

		for (out = 0, delta = 0; /* abaixo */; delta += 1)
		{
			if (out == 0x03)
				break;

			if ((out & 0x01) == 0)
			{
				if   ((map_index = hint_index - delta) < 0)
					out |= 0x01;
				elif (T1_GET_MAP (cp, map_index) == type)
					goto found;
			}

			if ((out & 0x02) == 0)
			{
				if   ((map_index = hint_index + delta) >= T1_ZONE_SZ)
					out |= 0x02;
				elif (T1_GET_MAP (cp, map_index) == type)
					goto found;
			}
		}
	}

    found:
	inode_bno = (map_bno - T1_BMNO) + map_index;

	/*
	 *	Achou um bloco LIVRE
	 */
	if (type == T1_MAP_FREE)
	{
		inode_bp = block_get (sp->sb_dev, T1_BL4toBL (inode_bno), 0);

		memclr (inode_bp->b_addr, BL4SZ);

		np = inode_bp->b_addr; np->n_mode = IFREG;

		for (ino_index = 0; ino_index < T1_INOinBL4; ino_index++, np++)
			np->n_magic = T1_SBMAGIC;

		bdwrite (inode_bp);

		T1_PUT_MAP (cp, map_index, T1_MAP_INODE_EMPTY);

		bdwrite (map_bp);

		SPINLOCK (&sp->sb_lock);
		t1sp->s_busy_bl++; sp->sb_sbmod = 1;
		SPINFREE (&sp->sb_lock);

		return (T1_MAKE_INO (inode_bno, 0));
	}

	/*
	 *	Insere mais um INODE em bloco antigo
	 */
	inode_bp = bread (sp->sb_dev, T1_BL4toBL (inode_bno), 0);

	inode_count = 0; free_np = NOT1DINO; free_ino_index = 0;

	for (np = inode_bp->b_addr, ino_index = 0; ino_index < T1_INOinBL4; ino_index++, np++)
	{
		if (np->n_magic != T1_SBMAGIC)
		{
			printf
			(	"%g: INODE %d de %v NÃO contém o mágico\n",
				T1_MAKE_INO (inode_bno, ino_index), sp->sb_dev
			);

			np->n_magic = T1_SBMAGIC;
		}

		if   (np->n_mode != 0)
			inode_count++;
		elif (free_np == NOT1DINO)
			{ free_np = np; free_ino_index = ino_index; }
	}

	if (free_np == NOT1DINO)
	{
		printf ("%g: NÃO achei um INODE livre em um bloco INODE_EMPTY\n");

		block_put (inode_bp);
		block_put (map_bp);

		return (NOINO);
	}

	memclr (free_np, sizeof (T1DINO)); free_np->n_magic = T1_SBMAGIC; free_np->n_mode = IFREG;

	bdwrite (inode_bp);

	if (inode_count + 1 >= T1_INOinBL4)
	{
		T1_PUT_MAP (cp, map_index, T1_MAP_INODE_FULL);

		bdwrite (map_bp);
	}
	else
	{
		block_put (map_bp);
	}

	return (T1_MAKE_INO (inode_bno, free_ino_index));

}	/* end t1_alloc_inode_in_map */
