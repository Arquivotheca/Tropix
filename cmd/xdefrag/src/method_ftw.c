/*
 ****************************************************************
 *								*
 *			method_ftw.c				*
 *								*
 *	Examina e desfragmenta o sistema de arquivos		*
 *								*
 *	Versão	4.3.0, de 26.10.03				*
 *		4.3.0, de 02.11.03				*
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
#include <fcntl.h>
#include <stat.h>
#include <errno.h>

#include "../h/xdefrag.h"

/*
 ****** Definições **********************************************
 */
#define	INODE_RES	4			/* No. de INODEs de folga em cada bloco */

/*
 ******	Variáveis globais ***************************************
 */
entry CACHE		dir_cache;		/* Cache de 1 bloco de DIR */ 

entry daddr_t		new_busy_bl;		/* Novo número de blocos alocados */

entry int		fs_dir_count;		/* Número de diretórios */

entry int		get_new_inode_index;	/* Para a alocação de INODEs */

entry daddr_t		get_new_inode_bno;	/* Para a alocação de INODEs */

entry daddr_t		get_new_block_last_bno;	/* Para a alocação de blocos */

/*
 ******	o HASH dos INODEs ***************************************
 */
entry INOMODE		**inode_hash_tb, **link_place;

#define INOHASHSZ	(1 << 12)
#define INOHASHMASK	(INOHASHSZ - 1)

/*
 ******	Protótipos de funções ***********************************
 */
int		dir_walk (const INOMODE *mp);
int		alloc_file_blocks (INOVEC *ip);
int		alloc_dir_blocks (INOVEC *ip);
ino_t		get_new_inode (void);
daddr_t		get_new_block (int type);
int		write_moved_inode (INOMODE *mp);
INOMODE		*old_ino_hash (ino_t ino);
INOMODE		*new_ino_hash (ino_t ino);
int		old_ino_hash_tst (const char *str);
int		new_ino_hash_tst (const char *str);
void		compute_inode_blocks_percentage (void);
void		compute_old_distances (void);

/*
 ****************************************************************
 *	Calcula os endereços desejados: compacta os arquivos	*
 ****************************************************************
 */
int
method_ftw_compact (void)
{
	INOVEC		*ip;
	INOMODE		*mp;
	DOUBLE		*ap;
	const DOUBLE	*end_ap;
	int		size;
	daddr_t		bno;
	char		*bp;
	int		i;

	/*
	 *	Prólogo (importante)
	 */
	fs_dir_count = 0;

	get_new_inode_index = T1_INOinBL4;

	get_new_inode_bno = 0;

	get_new_block_last_bno = T1_BMNO;

	/*
	 *	Aloca e cria a tabela HASH de INODEs (pelos números antigos)
	 */
	size = INOHASHSZ * sizeof (INOMODE *);

	if ((inode_hash_tb = malloc (size)) == (INOMODE **)NULL)
		{ xerror (NOSTR); return (-1); }

	memsetl (inode_hash_tb, 0, size / sizeof (long));

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		mp->i_inovec = ip;

		if (old_ino_hash (ip->i_ino) != NOINOMODE)
			{ xerror ("INODE %d repetido", ip->i_ino); return (-1); }

		*link_place = mp;
	}

#define	TST
#ifdef	TST
	if (old_ino_hash_tst ("method_ftw_compact") < 0)
		return (-1);
#endif	TST

	/*
	 *	Calcula algumas estatísticas
	 */
#undef	STATISTICS
#ifdef	STATISTICS
	compute_inode_blocks_percentage ();

	compute_old_distances ();
#endif	STATISTICS

	/*
	 *	Prepara o mapa de bits novo VAZIO
	 */
	bp = new_bit_map;

	memset (bp, ((T1_MAP_FREE << 4) | T1_MAP_FREE), MAP_SZ (fs_sz));

	T1_PUT_MAP (bp, T1_BOOTNO, T1_MAP_OUT);	/* Bloco de BOOT */
	T1_PUT_MAP (bp, T1_SBNO,   T1_MAP_OUT);	/* Bloco do Super-Bloco */

	new_busy_bl = 2;	/* BOOT, SB */

	for (bno = T1_BMNO; bno < fs_sz; bno += T1_ZONE_SZ, bp += BL4SZ)
	{
		T1_PUT_MAP (bp, T1_BMNO,   T1_MAP_MAP);	/* Marca a entrada do próprio mapa */

		new_busy_bl += 1;

		if ((size = fs_sz - (bno - T1_BMNO)) < T1_ZONE_SZ)
		{
			for (i = size; i < T1_ZONE_SZ; i++)
				T1_PUT_MAP (bp, i, T1_MAP_OUT);
		}
	}

	/*
	 *	Caminha começando pela RAIZ do sistema de arquivos
	 */
	if ((mp = old_ino_hash (T1_ROOT_INO)) == NOINOMODE)
		{ xerror ("NÃO achei o INODE da RAIZ"); return (-1); }

	if ((mp->i_new_ino = get_new_inode ()) < 0)
		return (-1);

	ip = mp->i_inovec;

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"\nDiretório %s, old_ino = %d, new_ino = %d\n",
		"/", ip->i_ino, mp->i_new_ino
	);
#endif	DEBUG

	if (alloc_file_blocks (ip) < 0)
		return (-1);

	fs_dir_count += 1;

	if (dir_walk (mp) < 0)
		return (-1);

	/*
	 *	Verifica se cada arquivo está bem alocado
	 */
	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (mp->i_new_ino != ip->i_ino)
			{ ip->i_compact = INO_FRAGMENTED; fragmented_inodes++; continue; }

		bno = NODADDR; ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; /* abaixo */; ap++)
		{
			if   (ap >= end_ap)
			{
				ip->i_compact = INO_COMPACT; compact_inodes++; break;
			}
			elif (ap->a_old_bno == NODADDR)
			{
				/* Nada a fazer */
			}
			elif (ap->a_old_bno != ap->a_new_bno)
			{
				ip->i_compact = INO_FRAGMENTED; fragmented_inodes++; break;
			}

		}	/* for (Blocos de um INODE) */

	}	/* end for (INODEs) */

	return (0);

}	/* end method_ftw_compact */

/*
 ****************************************************************
 *	Percorre os blocos de um diretório			*
 ****************************************************************
 */
int
dir_walk (const INOMODE *par_mp)
{
	const INOVEC	*par_ip = par_mp->i_inovec;
	INOVEC		*ip;
	INOMODE		*mp;
	DOUBLE		*ap;
	const DOUBLE	*end_ap, *indir_ap;
	daddr_t		bno;
	const T1DIR	*dp, *end_dp;

	/*
	 *	Aloca os arquivos (NÃO diretórios)
	 *
	 *	PROVISÓRIO: Supondo o diretório sem endereçamento duplamente indireto
	 */
	ap = par_ip->i_double; end_ap = ap + par_ip->i_virtual_blkcnt; indir_ap = ap + T1_NADDR - 3;

	for (/* acima */; ap < end_ap; ap++)
	{
		if (ap == indir_ap)
			ap++;

		bno = ap->a_old_bno;

		if (bno == NODADDR)
			continue;

		dp = bget (&dir_cache, bno); end_dp = (T1DIR *)&dir_cache.b_area[BL4SZ];

		for (/* acima */; dp < end_dp; dp = T1DIR_SZ_PTR (dp))
		{
			if (dp->d_name[0] == '.')
			{
				if (dp->d_name[1] == '\0')
					continue;

				if (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
					continue;
			}

			if ((mp = old_ino_hash (dp->d_ino)) == NOINOMODE)
				{ xerror ("NÃO achei o INODE %d (%s)", dp->d_ino, dp->d_name); return (-1); }

			if (S_ISDIR (mp->i_mode))
				continue;

			/* Aqui começa a alocação do arquivo */

			if (mp->i_new_ino == 0 && (mp->i_new_ino = get_new_inode ()) < 0)
				return (-1);

			ip = mp->i_inovec;
#ifdef	DEBUG
			printf 
			("	%s, old_ino = %d, new_ino = %d\n", 
				dp->d_name, ip->i_ino, mp->i_new_ino
			);
#endif	DEBUG
			if (ip->i_size == 0)
				continue;

			if (alloc_file_blocks (ip) < 0)
				return (-1);
		}
	}

	/*
	 *	Aloca os Diretórios
	 *
	 *	PROVISÓRIO: Supondo o diretório sem endereçamento duplamente indireto
	 */
	for (ap = par_ip->i_double; ap < end_ap; ap++)
	{
		if (ap == indir_ap)
			ap++;

		bno = ap->a_old_bno;

		if (bno == NODADDR)
			continue;

		dp = bget (&dir_cache, bno); end_dp = (T1DIR *)&dir_cache.b_area[BL4SZ];

		for (/* acima */; dp < end_dp; dp = T1DIR_SZ_PTR (dp))
		{
			if (dp->d_name[0] == '.')
			{
				if (dp->d_name[1] == '\0')
					continue;

				if (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
					continue;
			}

			if ((mp = old_ino_hash (dp->d_ino)) == NOINOMODE)
				{ xerror ("NÃO achei o INODE %d (%s)", dp->d_ino, dp->d_name); return (-1); }

			if (!S_ISDIR (mp->i_mode))
				continue;

			fs_dir_count += 1;

			/* Aqui começa a alocação do diretório */

			if (mp->i_new_ino == 0 && (mp->i_new_ino = get_new_inode ()) < 0)
				return (-1);

			ip = mp->i_inovec;
#ifdef	DEBUG
			printf
			(	"\nDiretório %s, old_ino = %d, new_ino = %d\n",
				dp->d_name, ip->i_ino, mp->i_new_ino
			);
#endif	DEBUG

			if (ip->i_size == 0)
				continue;

			if (alloc_dir_blocks (ip) < 0)
				return (-1);

			dir_walk (mp);

			bget (&dir_cache, bno);
		}
	}

	return (0);

}	/* end dir_walk */

/*
 ****************************************************************
 * Aloca os blocos de um diretório (atualizando o mapa de bits)	*
 ****************************************************************
 */
int
alloc_dir_blocks (INOVEC *dir_ip)
{
	INOMODE		*mp;
	DOUBLE		*ap;
	const DOUBLE	*end_ap, *indir_ap;
	daddr_t		bno;
	const T1DIR	*dp, *end_dp;

	/*
	 *	Se já for feito, ...
	 */
	if (dir_ip->i_new_blocks != 0)
		return (0);

	dir_ip->i_new_blocks = 1;

	/*
	 *	Percorre o diretório
	 *
	 *	Supõe diretórios sem endereçamento duplamente indireto!
	 */
	ap = dir_ip->i_double; end_ap = ap + dir_ip->i_virtual_blkcnt; indir_ap = ap + T1_NADDR - 3;

	for (/* acima */; ap < end_ap; ap++)
	{
		if (ap == indir_ap)
		{
			if ((ap->a_new_bno = get_new_block (T1_MAP_INDIR)) == NODADDR)
				return (-1);

			continue;
		}

		if ((bno = ap->a_old_bno) == NODADDR)
			{ ap->a_new_bno = NODADDR; continue; }

		if ((ap->a_new_bno = get_new_block (T1_MAP_DATA)) == NODADDR)
			return (-1);

		get_new_inode_index = T1_INOinBL4;	/* Novo bloco de INODEs */

		dp = bget (&dir_cache, bno); end_dp = (T1DIR *)&dir_cache.b_area[BL4SZ];

		for (/* vazio */; dp < end_dp; dp = T1DIR_SZ_PTR (dp))
		{
			if (dp->d_name[0] == '.')
			{
				if (dp->d_name[1] == '\0')
					continue;

				if (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
					continue;
			}

			if ((mp = old_ino_hash (dp->d_ino)) == NOINOMODE)
				{ xerror ("NÃO achei o INODE %d (%s)", dp->d_ino, dp->d_name); return (-1); }

			if (mp->i_new_ino == 0 && (mp->i_new_ino = get_new_inode ()) < 0)
				return (-1);
		}
	}

	return (0);

}	/* end alloc_dir_blocks */

/*
 ****************************************************************
 *  Aloca os blocos de um arquivo (atualizando o mapa de bits)	*
 ****************************************************************
 */
int
alloc_file_blocks (INOVEC *ip)
{
	DOUBLE		*ap = ip->i_double;
	const DOUBLE	*end_ap = ap + ip->i_virtual_blkcnt;
	int		index0, index1, index2;

	/*
	 *	Se já for feito, ...
	 */
	if (ip->i_new_blocks != 0)
		return (0);

	ip->i_new_blocks = 1;

	/*
	 *	Prepara os blocos diretos
	 */
	for (index0 = 0; index0 < T1_NADDR - 3; index0++, ap++)
	{
		if (ap >= end_ap)
			return (0);

		if (ap->a_old_bno == NODADDR)
			{ ap->a_new_bno = NODADDR; continue; }

		if ((ap->a_new_bno = get_new_block (T1_MAP_DATA)) == NODADDR)
			return (-1);
	}

	if (ap >= end_ap)
		return (0);

	/*
	 *	Prepara os blocos indiretos de grau 1
	 */
	if (ap->a_old_bno != NODADDR)
	{
		if ((ap->a_new_bno = get_new_block (T1_MAP_INDIR)) == NODADDR)
			return (-1);

		ap++;

		for (index1 = 0; index1 < T1_INDIR_SZ; index1++, ap++)
		{
			if (ap >= end_ap)
				return (0);

			if (ap->a_old_bno == NODADDR)
				{ ap->a_new_bno = NODADDR; continue; }

			if ((ap->a_new_bno = get_new_block (T1_MAP_DATA)) == NODADDR)
				return (-1);
		}
	}
	else
	{
		ap += 1 + T1_INDIR_SZ;
	}

	if (ap >= end_ap)
		return (0);

	/*
	 *	Prepara os blocos indiretos de grau 2
	 */
	if (ap->a_old_bno != NODADDR)
	{
		if ((ap->a_new_bno = get_new_block (T1_MAP_INDIR)) == NODADDR)
			return (-1);

		ap++;

		if (ap >= end_ap)
			return (0);

		for (index2 = 0; index2 < T1_INDIR_SZ; index2++)
		{
			if (ap->a_old_bno != NODADDR)
			{
				if ((ap->a_new_bno = get_new_block (T1_MAP_INDIR)) == NODADDR)
					return (-1);

				ap++;

				for (index1 = 0; index1 < T1_INDIR_SZ; index1++, ap++)
				{
					if (ap >= end_ap)
						return (0);

					if (ap->a_old_bno == NODADDR)
						{ ap->a_new_bno = NODADDR; continue; }

					if ((ap->a_new_bno = get_new_block (T1_MAP_DATA)) == NODADDR)
						return (-1);
				}
			}
			else
			{
				ap += 1 + T1_INDIR_SZ;
			}
		}

		if (ap >= end_ap)
			return (0);
	}
	else
	{
		ap += 1 + T1_INDIR_SZ * (1 + T1_INDIR_SZ);
	}

   /***	if (ap >= end_ap) ***/
	   /***	return (0); ***/

	/*
	 *	Prepara os blocos indiretos de grau 3 (A FAZER)
	 */
	return (0);

}	/* end alloc_file_blocks */

/*
 ****************************************************************
 *	Obtém um novo INODE					*
 ****************************************************************
 */
ino_t
get_new_inode (void)
{
	if (get_new_inode_index >= T1_INOinBL4 - INODE_RES)
	{
		if ((get_new_inode_bno = get_new_block (T1_MAP_INODE_EMPTY)) == NODADDR)
			return (-1);

		get_new_inode_index = 0;
	}

	return (T1_MAKE_INO (get_new_inode_bno, get_new_inode_index++));

}	/* end get_new_inode */

/*
 ****************************************************************
 *	Obtém um novo bloco no mapa de bits			*
 ****************************************************************
 */
daddr_t
get_new_block (int type)
{
	daddr_t		bno;
	char		*bp = new_bit_map;

	for (bno = get_new_block_last_bno + 1; /* abaixo */; bno++)
	{
		if (bno >= fs_sz)
			{ xerror ("NÃO consegui mais blocos"); return (NODADDR); }

		if (T1_GET_MAP (bp, bno) == T1_MAP_FREE)
			break;
	}

	T1_PUT_MAP (bp, bno, type);

	new_busy_bl++;

	return (get_new_block_last_bno = bno);

}	/* end get_new_block */

/*
 ****************************************************************
 *	Atualiza os INODEs no disco				*
 ****************************************************************
 */
void
write_all_moved_inodes (void)
{
	daddr_t		bno;
	INOVEC		*ip;
	INOMODE		*mp;
	int		index, ino, size;
	T1DINO		*np;
	const T1DINO	*end_np;

	/*
	 *	Cria a tabela HASH de INODEs (pelo números novos)
	 */
	size = INOHASHSZ * sizeof (INOMODE *);

	memsetl (inode_hash_tb, 0, size / sizeof (long));

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
		mp->i_next = NOINOMODE;

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (new_ino_hash (mp->i_new_ino) != NOINOMODE)
			{ xerror ("INODE %d repetido", mp->i_new_ino); return; }

		*link_place = mp;
	}

#ifdef	TST
	if (new_ino_hash_tst ("write_all_moved_inodes (1)") < 0)
		return;
#endif	TST

	/*
	 *	Procura os blocos de INODEs
	 */
	for (bno = T1_ROOTNO; bno < fs_sz; bno++)
	{
		if (T1_GET_MAP (new_bit_map, bno) != T1_MAP_INODE_EMPTY)
			continue;

		/* Obtém e zera o bloco de INODEs */

		np = bclear (&inode_cache, bno); end_np = np + T1_INOinBL4;

		for (/* acima */; np < end_np; np++)
			np->n_magic = T1_SBMAGIC;

		/* Preenche-o com os INODEs */

		np = (T1DINO *)inode_cache.b_area;

		for (index = 0; index < T1_INOinBL4 - INODE_RES; index++, np++)
		{
			ino = T1_MAKE_INO (bno, index);

			if ((mp = new_ino_hash (ino)) == NOINOMODE)
				continue;

			ip = mp->i_inovec;

		   /***	np->n_magic	= T1_SBMAGIC;		***/
			np->n_mode	= mp->i_mode;
			np->n_uid	= mp->i_uid;
			np->n_gid	= mp->i_gid;
			np->n_nlink	= mp->i_nlink;
			np->n_size	= ip->i_size;
		   /***	np->n_high_size	= mp->i_high_size;	***/
			np->n_atime	= mp->i_atime;
			np->n_mtime	= mp->i_mtime;
			np->n_ctime	= mp->i_ctime;
		   /***	np->n_addr	= ...;			***/
		   /***	np->n_reser1	= ...;			***/

			if   (S_ISBLK (np->n_mode) || S_ISCHR (np->n_mode))
			{
				np->n_addr[0] = *(daddr_t *)ip->i_double;
			}
			elif (S_ISLNK (np->n_mode) && np->n_size < sizeof (np->n_addr))
			{
				memmove (np->n_addr, ip->i_double, np->n_size);
			}
			elif (np->n_size != 0)
			{
				if (update_inode_addr (ino, ip) < 0)
					return;
			}

			percent_action_done++;

		}	/* end for (INODEs de um bloco) */

		inode_cache.b_dirty = 1;

		draw_percent_percent
		(	percent_action_done,
			percent_action_block + percent_action_inode + percent_action_dir
		);

		do_XFlush ();

	}	/* end for (todos os blocos) */

	bflush (&inode_cache);

}	/* end write_all_moved_inodes */

/*
 ****************************************************************
 *	Atualiza os números dos INODEs nos diretórios		*
 ****************************************************************
 */
void
update_dir_entries (void)
{
	INOVEC		*ip;
	INOMODE		*mp, *ino_mp;
	const DOUBLE	*ap, *end_ap, *indir_ap;
	T1DIR		*dp;
	const T1DIR	*end_dp;
	daddr_t		bno;
	int		size;

	/*
	 *	Restaura a tabela HASH de INODEs (pelos números antigos)
	 */
	size = INOHASHSZ * sizeof (INOMODE *);

	memsetl (inode_hash_tb, 0, size / sizeof (long));

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
		mp->i_next = NOINOMODE;

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
	   /***	mp->i_inovec = ip; ***/

		if (old_ino_hash (ip->i_ino) != NOINOMODE)
			{ xerror ("INODE %d repetido", ip->i_ino); return; }

		*link_place = mp;
	}

#ifdef	TST
	if (old_ino_hash_tst ("update_dir_entries (1)") < 0)
		return;
#endif	TST

	/*
	 *	Percorre os diretórios
	 *
	 *	PROVISÓRIO: Supondo os diretórios sem endereçamento duplamente indireto
	 */
	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (!S_ISDIR (mp->i_mode))
			continue;

		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt; indir_ap = ap + T1_NADDR - 3;

		for (/* acima */; ap < end_ap; ap++)
		{
			if (ap == indir_ap)
				ap++;

			bno = ap->a_new_bno;

			if (bno == NODADDR)
				continue;

			dp = bget (&dir_cache, bno); end_dp = (T1DIR *)&dir_cache.b_area[BL4SZ];

			for (/* acima */; dp < end_dp; dp = T1DIR_SZ_PTR (dp))
			{
				if ((ino_mp = old_ino_hash (dp->d_ino)) == NOINOMODE)
				{
					xerror ("NÃO achei o INODE %d (%s)", dp->d_ino, dp->d_name);
					continue;
				}

				if (dp->d_ino != ino_mp->i_new_ino)
				{
					dp->d_ino = ino_mp->i_new_ino;
					dir_cache.b_dirty = 1;
				}
			}
		}

		percent_action_done++;

		draw_percent_percent
		(	percent_action_done,
			percent_action_block + percent_action_inode + percent_action_dir
		);

		do_XFlush ();
	}

	bflush (&dir_cache);

}	/* end update_dir_entries */

/*
 ****************************************************************
 *	Atualiza o SB						*
 ****************************************************************
 */
void
update_sb (void)
{
	T1SB		sb;

	bread (&sb, T1_SBNO, BL4SZ);

	if (sb.s_busy_bl != new_busy_bl)
	{
		sb.s_busy_bl = new_busy_bl;

		bwrite (&sb, T1_SBNO, BL4SZ);
	}

}	/* end update_sb */

/*
 ****************************************************************
 *	Função de HASH para INODEs (valor antigo)		*
 ****************************************************************
 */
INOMODE *
old_ino_hash (ino_t ino)
{
	INOMODE		*mp;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	link_place = &inode_hash_tb[ino & INOHASHMASK];

	if ((mp = *link_place) == NOINOMODE)
		return (NOINOMODE);

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (mp->i_inovec->i_ino == ino)
			return (mp);

		if (mp->i_next == NOINOMODE)
			{ link_place = &mp->i_next; return (NOINOMODE); }
		else
			mp = mp->i_next;
	}

}	/* end old_ino_hash */

/*
 ****************************************************************
 *	Função de HASH para INODEs (valor novo)			*
 ****************************************************************
 */
INOMODE *
new_ino_hash (ino_t ino)
{
	INOMODE		*mp;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	link_place = &inode_hash_tb[ino & INOHASHMASK];

	if ((mp = *link_place) == NOINOMODE)
		return (NOINOMODE);

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (mp->i_new_ino == ino)
			return (mp);

		if (mp->i_next == NOINOMODE)
			{ link_place = &mp->i_next; return (NOINOMODE); }
		else
			mp = mp->i_next;
	}

}	/* end new_ino_hash */

#ifdef	TST
/*
 ****************************************************************
 *	Testa a tabela HASH antiga				*
 ****************************************************************
 */
int
old_ino_hash_tst (const char *str)
{
	int		code = 0;
	const INOVEC	*ip;
	const INOMODE	*mp;

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (old_ino_hash (ip->i_ino) != mp)
			{ printf ("%s: Erro no INODE %d\n", str, ip->i_ino);  code = -1; }
	}

	return (code);

}	/* end old_ino_hash_tst */

/*
 ****************************************************************
 *	Testa a tabela HASH nova				*
 ****************************************************************
 */
int
new_ino_hash_tst (const char *str)
{
	int		code = 0;
	const INOVEC	*ip;
	const INOMODE	*mp;

	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (new_ino_hash (mp->i_new_ino) != mp)
			{ printf ("%s: Erro no INODE %d\n", str, mp->i_new_ino);  code = -1; }
	}

	return (code);

}	/* end new_ino_hash_tst */
#endif	TST

#ifdef	STATISTICS
/*
 ****************************************************************
 *	Obtém a ocupação do blocos de INODEs			*
 ****************************************************************
 */
void
compute_inode_blocks_percentage (void)
{
	daddr_t		bno;
	T1DINO		*np;
	const T1DINO	*end_np;
	int		index;
	int		inode_count = 0, full_inode_count = 0;

	/*
	 *	Procura os blocos de INODEs
	 */
	for (bno = T1_ROOTNO; bno < fs_sz; bno++)
	{
		switch (T1_GET_MAP (old_bit_map, bno))
		{
		    case T1_MAP_INODE_EMPTY:
		    case T1_MAP_INODE_FULL:
			break;

		    default:
			continue;
		}

		np = bget (&inode_cache, bno); end_np = np + T1_INOinBL4;

		for (index = 0; index < T1_INOinBL4; index++, np++)
		{
			inode_count += 1;

			if (np->n_mode != 0)
				full_inode_count += 1;
		}
	}

	printf
	(	"\n%s: busy_bl = %d, INODEs = %d, ocupados = %d, %d %%\n",
		devtb[dev_defrag_index].d_disktb.p_name,
		busy_bl, inode_count, full_inode_count, 100 * full_inode_count / inode_count
	);

}	/* end compute_inode_blocks_percentage */

/*
 ****************************************************************
 *	Calcula as distâncias médias antigas			*
 ****************************************************************
 */
void
compute_old_distances (void)
{
	const INOVEC	*ip, *ino_ip;
	const INOMODE	*mp, *ino_mp;
	const DOUBLE	*ap, *end_ap, *indir_ap;
	const T1DIR	*dp;
	const T1DIR	*end_dp;
	daddr_t		bno, ino_bno, file_bno;
	int		dir_ino_dist, ino_blk_dist, files = 0;
	int		total_dir_ino_dist = 0, total_ino_blk_dist = 0;
	int		dir_count = 0, dir_entry_count = 0;
	int		big_dir_count = 0;
	daddr_t		last_bno = -1, last_ino_bno = -1;
	int		dir_ino_count = 0;
	int		bad_ino = 0, bad_file = 0;

	/*
	 *	Percorre os diretórios
	 *
	 *	PROVISÓRIO: Supondo os diretórios sem endereçamento duplamente indireto
	 */
	for (mp = inomode, ip = inovec; ip < end_inovec; ip++, mp++)
	{
		if (!S_ISDIR (mp->i_mode))
			continue;

		dir_count += 1;

		if (ip->i_size > BL4SZ)
			big_dir_count += 1;

		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt; indir_ap = ap + T1_NADDR - 3;

		for (/* acima */; ap < end_ap; ap++)
		{
			if (ap == indir_ap)
				ap++;

			bno = ap->a_old_bno;

			if (bno == NODADDR)
				continue;

			dp = bget (&dir_cache, bno); end_dp = (T1DIR *)&dir_cache.b_area[BL4SZ];

			for (/* acima */; dp < end_dp; dp = T1DIR_SZ_PTR (dp))
			{
				if (dp->d_name[0] == '.')
				{
					if (dp->d_name[1] == '\0')
						continue;

					if (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
						continue;
				}

				dir_entry_count += 1;

				ino_bno = T1_INOtoADDR (dp->d_ino);

				if ((ino_mp = old_ino_hash (dp->d_ino)) == NOINOMODE)
				{
					xerror ("NÃO achei o INODE %d (%s)", dp->d_ino, dp->d_name);
					continue;
				}

				if (!S_ISREG (ino_mp->i_mode))
					continue;

				ino_ip = ino_mp->i_inovec;

				if (ino_ip->i_double == NODOUBLE)
					continue;

				if ((file_bno = ino_ip->i_double[0].a_old_bno) == NODADDR)
					continue;

				if (ino_bno < bno)
					bad_ino++;

				if (file_bno < ino_bno)
					bad_file++;

				dir_ino_dist = abs (ino_bno - bno);
				ino_blk_dist = abs (file_bno - ino_bno);

				files += 1;

				if (bno != last_bno || ino_bno != last_ino_bno)
				{
					total_dir_ino_dist += dir_ino_dist;
					dir_ino_count += 1;
				}

				total_ino_blk_dist += ino_blk_dist;

				last_bno = bno; last_ino_bno = ino_bno;
			}
		}
	}

	if (files > 0) printf
	(	"%d arquivos, %d/%d diretórios, entradas/dir = %d, ino - dir = %g, arq - ino = %g\n",
		files, dir_count, big_dir_count, dir_entry_count / dir_count,
		(double)total_dir_ino_dist / dir_ino_count, (double)total_ino_blk_dist / files
	);

	printf ("Inodes antes do diretório = %d, arquivo antes do INODE = %d\n", bad_ino, bad_file);

	printf
	(	"CACHE: total = %d, acertos = %d, %d %%\n",
		cache_miss + cache_hit, cache_hit,
		cache_hit * 100 / (cache_hit + cache_miss)
	);

}	/* end compute_old_distances */
#endif	STATISTICS
