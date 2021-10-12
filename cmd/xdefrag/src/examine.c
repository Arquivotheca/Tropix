/*
 ****************************************************************
 *								*
 *			examine.c				*
 *								*
 *	Examina e desfragmenta o sistema de arquivos		*
 *								*
 *	Versão	4.3.0, de 25.10.02				*
 *		4.5.0, de 02.11.03				*
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
 ******	Variáveis globais ***************************************
 */
entry int		dev_fd = -1;		/* O descritor */

entry char		*old_bit_map,		/* Os mapas de bits */
			*new_bit_map;

entry int		fs_sz;			/* Tamanho do sistema de arquivos */
entry int		busy_bl;		/* No. de blocos ocupados */

entry INOVEC		*inovec;		/* O vetor de INODEs */
entry const INOVEC	*end_inovec;

entry INOMODE		*inomode;		/* O vetor complementar de INODEs */

entry DOUBLE		**block_map;		/* O vetor de BLOCOs */

entry signed char	*before_block_code;	/* Os códigos ANTES */

entry int		before_map_ready,	/* O mapa de alocação ANTES/DEPOIS está pronto */
			after_map_ready;

entry int		compact_inodes,		/* Número de INODEs compactos ou NÃO */
			fragmented_inodes;

entry int		percent_action_block,	/* No. de blocos/inodes a trocar */
			percent_action_inode,
			percent_action_dir,
			percent_action_done;

entry CACHE		inode_cache;		/* Cache de 1 bloco de INODE */

entry int		cache_miss, cache_hit;	/* Acertos do CACHE */

entry int		data_ready;		/* Dados prontos para desfragmentação */

/*
 ******	Protótipos de funções ***********************************
 */
DOUBLE		*create_double_vec (INOVEC *ip, const T1DINO *, const char **msg);
void		interchange_block_list (DOUBLE **dpp);

/*
 ****************************************************************
 *	Examina a alocação de blocos do sistema de arquivos	*
 ****************************************************************
 */
void
examine_function (void)
{
	int		len;
	signed char	*cp;
	DOUBLE		**dpp, **end_dpp;
	DOUBLE		*ap;
	const DOUBLE	*end_ap;
	INOVEC		*ip;
	DEVTB		*dp;
	char		path[16] = "/dev/";
	char		*cp_point = path + 5;
	char		*bp;
	daddr_t		bno;
	long		mode;
	int		bit_map_sz, index, used_ino;
	int		seg, min, hor;
	T1SB		sb;
	const T1DINO	*np;
	INOMODE		*mp = NOINOMODE;
	const char	*msg;
	int		code, size, block_map_sz;
	int		n_blocks;
	time_t		begin_time;
	char		text[64];
	T1DINO		dino[T1_INOinBL4];


	/*
	 *	Desaloca alguns recursos antigos
	 */
	time (&begin_time);

	data_ready = 0;

	clear_msg_window ();

	clear_before_window ();

	clear_after_window ();

	clear_percent_window ();

	do_XFlush ();

	before_map_ready = after_map_ready = 0;

	cache_miss = cache_hit = 0;

	/*
	 *	Desaloca os recursos antigos
	 */
	if (old_bit_map != NOSTR)
		{ free (old_bit_map); old_bit_map = NOSTR; }

	if (new_bit_map != NOSTR)
		{ free (new_bit_map); new_bit_map = NOSTR; }

	if (inovec != NOINOVEC)
	{
		for (ip = inovec; ip < end_inovec; ip++)
		{
			if (ip->i_double != NODOUBLE)
				free (ip->i_double);
		}

		free (inovec); inovec = NOINOVEC;
	}

	if (inomode != NOINOMODE)
		{ free (inomode); inomode = NOINOMODE; }

	if (block_map != (DOUBLE **)NULL)
		{ free (block_map); block_map = (DOUBLE **)NULL; }

	if (before_block_map != NULL)
		{ free (before_block_map); before_block_map = NULL; }

	if (before_block_code != NULL)
		{ free (before_block_code); before_block_code = NULL; }

	if (after_block_map != NULL)
		{ free (after_block_map); after_block_map = NULL; }

	for (index = 0; index < MSGLINES; index++)
	{
		if (msg_text[index] == NOSTR)
			continue;

		free (msg_text[index]); msg_text[index] = NOSTR;
	}

	if (inode_hash_tb != NULL)
		{ free (inode_hash_tb); inode_hash_tb = NULL; }

	inode_cache.b_dirty = 0;
	inode_cache.b_bno   = -1;

	dir_cache.b_dirty = 0;
	dir_cache.b_bno   = -1;

	/*
	 *	Obtém a entrada desejada
	 */
	if (dev_defrag_index < 0)
		{ xerror ("Não há sistema de arquivos selecionado"); return; }

	dp = &devtb[dev_defrag_index];

	strcpy (cp_point, dp->d_disktb.p_name);

	switch (dp->d_state)
	{
	    case FS_OK:
		code = O_RDWR;
		break;

	    case FS_MOUNTED:
	    case FS_RONLY:
		code = O_RDONLY;
		break;

	    default:
		{ xerror ("O Dispositivo NÃO permite a análise"); return; }
	}

	if (dev_fd < 0 && (dev_fd = open (path, code)) < 0)
		{ xerror ("*Não consegui abrir o dispositivo"); return; }

	/*
	 *	Le o superbloco
	 */
	if   (bread (&sb, T1_SBNO, sizeof (T1SB)) < 0)
		return;
	elif (sb.s_magic != T1_SBMAGIC)
		{ xerror ("*Não é um sistema de arquivos T1"); return; }

	fs_sz	= sb.s_fs_sz;
	busy_bl	= sb.s_busy_bl;

	dp->d_disktb.p_size = sb.s_fs_sz * (sb.s_bl_sz >> BLSHIFT);

	strcpy (dp->d_fname, sb.s_fname);

	draw_dev_line (dp, dev_defrag_index);

	/*
	 *	Prepara a primeira linha da mensagem
	 */
	len = snprintf
	(	text, sizeof (text),
		"%s (%s) %s MB (%s %%)%s",
		dp->d_disktb.p_name,
		dp->d_fname,
		cv_blocks_to_MB (T1_BL4toBL (fs_sz)),
		editpercent (getpercent (busy_bl, fs_sz)),
		code == O_RDONLY ? " Só ANÁLISE" : ""
	);

	if ((msg_text[0] = malloc (len + 1)) == NOSTR)
		{ xerror (NOSTR); return; }

	strcpy (msg_text[0], text);

	draw_msg_win (); do_XFlush ();

	/*
	 *	Aloca espaço para o mapa de BITs
	 */
	bit_map_sz = MAP_SZ (sb.s_fs_sz);

	if ((old_bit_map = malloc (bit_map_sz)) == NOSTR)
		{ xerror (NOSTR); return; }

	/*
	 *	Aloca espaço para o vetor de INODEs
	 */
	size = sb.s_busy_ino * sizeof (INOVEC);

	if ((inovec = ip = malloc (size)) == NOINOVEC)
		{ xerror (NOSTR); return; }

	memsetl (inovec, 0, size / sizeof (long));

	end_inovec = ip + sb.s_busy_ino;	/* Este valor será recalculado */

	if (method_in_use == METHOD_FTW)
	{
		size = sb.s_busy_ino * sizeof (INOMODE);

		if ((inomode = mp = malloc (sb.s_busy_ino * sizeof (INOMODE))) == NOINOMODE)
			{ xerror (NOSTR); return; }

		memsetl (inomode, 0, size / sizeof (long));
	}

	/*
	 *	Lê os mapas de bits do disco
	 */
	draw_action_line ("*** Fase 1: Lendo os Mapas de bits");

	bp = old_bit_map;

	for (bno = T1_BMNO; bno < sb.s_fs_sz; bno += T1_ZONE_SZ, bp += BL4SZ)
	{
		if (bread (bp, bno, BL4SZ) < 0)
			return;

		if (T1_GET_MAP (bp, T1_BMNO) != T1_MAP_MAP)
			{ xerror ("Código inválido do próprio mapa no bloco %d\n", bno); return ; }
	}

	/*
	 *	Cria os vetores de endereços dos INODEs
	 */
	draw_action_line ("*** Fase 2: Lendo os INODEs");

	bp = old_bit_map;

	for (bno = T1_ROOTNO; bno < sb.s_fs_sz; bno++)
	{
		switch (code = T1_GET_MAP (bp, bno))
		{
		    case T1_MAP_INODE_EMPTY:
		    case T1_MAP_INODE_FULL:
			break;

		    case T1_MAP_FREE:
		    case T1_MAP_MAP:
		    case T1_MAP_DATA:
		    case T1_MAP_INDIR:
		    case T1_MAP_BAD:
		    case T1_MAP_OUT:
			continue;

		    default:
			xerror ("Código inválido %d do bloco %d\n", code, bno);
			return;
		}

		if (bread (dino, bno, sizeof (dino)) < 0)
			return;

		for (np = dino, index = 0; index < T1_INOinBL4; index++, np++)
		{
			if (np->n_magic != T1_SBMAGIC)
				{ xerror ("Número mágico de INODE incorreto, bloco %d\n", bno); return; }

			if ((mode = np->n_mode) == 0)
				continue;

			switch (mode & S_IFMT)
			{
			    case S_IFBLK:
			    case S_IFCHR:
			    case S_IFIFO:
				if (method_in_use != METHOD_FTW)
					continue;
				break;
				
			    case S_IFLNK:
				if (np->n_size < sizeof (np->n_addr) && method_in_use != METHOD_FTW)
					continue;
			    case S_IFREG:
				break;

			    case S_IFDIR:
				if (np->n_size > (T1_NADDR - 3 + T1_INDIR_SZ) * BL4SZ)		/* PROVISÓRIO */
				{
					xerror
					(	"Diretório com tamanho excessivo %d\n",
						np->n_size
					);
					return;
				}

				break;

			    default:
				xerror ("Tipo de arquivo inválido, bloco %d", bno);
				return;
			}

			if (ip >= end_inovec)
				{ xerror ("Número de INODEs maior do que o esperado"); return; }

			ip->i_ino	 = T1_MAKE_INO (bno, index);
		   /***	ip->i_new_blocks = 0;	***/
			ip->i_size	 = np->n_size;

			if   (method_in_use == METHOD_FTW)
			{
				mp->i_mode	= mode;
				mp->i_uid	= np->n_uid;
				mp->i_gid	= np->n_gid;
				mp->i_nlink	= np->n_nlink;
				mp->i_atime	= np->n_atime;
				mp->i_mtime	= np->n_mtime;
				mp->i_ctime	= np->n_ctime;

			   /***	mp->i_new_ino	= 0;		***/
			   /***	mp->i_inovec	= NOINOVEC;	***/
			   /***	mp->i_next	= NOINOMODE;	***/
			}

			if   (S_ISBLK (mode) || S_ISCHR (mode))
			{
				/* Guarda o MAJOR+MINOR */

				if ((ip->i_double = malloc (sizeof (daddr_t))) == NULL)
					{ xerror (NOSTR); return; }

				*(daddr_t *)ip->i_double = np->n_addr[0];
			}
			elif (S_ISLNK (mode) && np->n_size < sizeof (np->n_addr))
			{
				/* Guarda o valor do elo simbólico */

				if ((ip->i_double = malloc (np->n_size)) == NULL)
					{ xerror (NOSTR); return; }

				memmove (ip->i_double, np->n_addr, np->n_size);
			}
			elif (ip->i_size != 0)
			{
				n_blocks = BYTOBL4 (ip->i_size);

				ip->i_virtual_blkcnt = n_blocks + compute_no_of_indir_blocks (n_blocks);

				if ((ip->i_double = create_double_vec (ip, np, &msg)) == NODOUBLE)
					{ xerror (msg); return; }
			}
			elif (method_in_use != METHOD_FTW)	/* && (size == 0) */
			{
				continue;
			}

			ip++; mp++;

		}	/* end for (um bloco de INODEs) */

	}	/* end for (percorrendo todos os blocos de INODEs) */

	/*
	 *	Coloca o tamanho correto no vetor
	 */
	if ((used_ino = ip - inovec) != sb.s_busy_ino)
	{
		if ((inovec = realloc (inovec, used_ino * sizeof (INOVEC))) == NOINOVEC)
			{ xerror (NOSTR); return; }

		end_inovec = inovec + used_ino;

		if (method_in_use == METHOD_FTW)
		{
			if ((inomode = realloc (inomode, used_ino * sizeof (INOMODE))) == NOINOMODE)
				{ xerror (NOSTR); return; }
		}
	}

	/*
	 *	Cria o mapa de blocos
	 */
	block_map_sz = sb.s_fs_sz * sizeof (DOUBLE *);

	if ((block_map = malloc (block_map_sz)) == (DOUBLE **)NULL)
		{ xerror (NOSTR); return; }

	memsetl (block_map, 0, block_map_sz / sizeof (long));

	for (ip = inovec; ip < end_inovec; ip++)
	{
		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; ap < end_ap; ap++)
		{
			bno = ap->a_old_bno;

			if (bno == NODADDR)
				continue;

			if (bno < T1_ROOTNO || bno >= fs_sz)
				{ xerror ("Bloco %d inválido", bno); return; }

			dpp = &block_map[bno];

			if (*dpp)
				{ xerror ("Bloco %d duplicado", bno); return; }

			*dpp = ap;
		}
	}

	/*
	 *	Aloca espaço para o novo mapa de BITs
	 */
	if ((new_bit_map = malloc (MAP_SZ (fs_sz))) == NOSTR)
		{ xerror (NOSTR); return; }

	/*
	 *	Aloca espaço para o mapa de blocos original
	 */
	if ((before_block_code = malloc (fs_sz)) == NULL)
		{ xerror (NOSTR); return; }

	/*
	 *	Calcula os novos endereços
	 *
	 *	A função chamada atualiza "compact_inodes" e "fragmented_inodes"
	 */
	compact_inodes = fragmented_inodes = 0;

	draw_action_line ("*** Fase 3: Obtendo os novos endereços dos blocos");

	switch (method_in_use)
	{
	    case METHOD_FILE:
	    case METHOD_FILE_AND_HOLE:
		if (method_file_compact () < 0)
			return;
		break;

	    case METHOD_FTW:
		if (method_ftw_compact () < 0)
			return;
		break;

	    default:
		{ xerror ("Método NÃO definido"); return; }
	};

	/*
	 *	Prepara o mapa de blocos original
	 */
	dpp = block_map; end_dpp = dpp + fs_sz;

	for (cp = before_block_code; dpp < end_dpp; dpp++)
	{
		if   ((ap = *dpp) == NODOUBLE)
			*cp++ = 0;	/* NÃO alocado */
		elif (ap->a_new_bno == ap->a_old_bno)
			*cp++ = 1;	/* Bem alocado */
		else
			*cp++ = -1;	/* Mal alocado */
	}

	if (compute_before_blocks () < 0)
		return;

	/*
	 *	Conta o número de trocas de blocos mais as atualizações de INODEs e diretórios
	 */
	percent_action_block = percent_action_inode = percent_action_dir = percent_action_done = 0;

	for (ip = inovec; ip < end_inovec; ip++)
	{
		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; ap < end_ap; ap++)
		{
			if (ap->a_old_bno != 0 && ap->a_old_bno != ap->a_new_bno)
				percent_action_block++;
		}
	}

	for (ip = inovec; ip < end_inovec; ip++)
	{
		if (ip->i_compact == INO_FRAGMENTED)
			percent_action_inode++;
	}

	if (method_in_use == METHOD_FTW)
		percent_action_dir = fs_dir_count;

	/*
	 *	Prepara as linhas da mensagem
	 */
	len = snprintf
	(	text, sizeof (text),
		"%d arquivos, %d fragmentados (%s %%)",
		compact_inodes + fragmented_inodes,
		fragmented_inodes,
		editpercent (getpercent (fragmented_inodes, compact_inodes + fragmented_inodes))
	);

	if ((msg_text[1] = malloc (len + 1)) == NOSTR)
		{ xerror (NOSTR); return; }

	strcpy (msg_text[1], text);

	len = snprintf
	(	text, sizeof (text),
		"Serão atualizados %d blocos e %d INODEs",
		percent_action_block, percent_action_inode
	);

	if ((msg_text[2] = malloc (len + 1)) == NOSTR)
		{ xerror (NOSTR); return; }

	strcpy (msg_text[2], text);

	before_map_ready = 1; draw_before_blocks ();

	if (fragmented_inodes != 0)
		data_ready = 1;

	/*
	 *	Escreve o tempo
	 */
	seg = time ((time_t *)NULL) - begin_time;

	min  = seg / 60;
	seg  = seg % 60;

	hor  = min / 60;
	min  = min % 60;

	len = snprintf
	(	text, sizeof (text),
		"Análise concluída, Tempo = %02d:%02d:%02d",
		hor, min, seg
	);

	draw_action_line (text);

}	/* end examine_function */

/*
 ****************************************************************
 *	Realiza a alocação de blocos do sistema de arquivos	*
 ****************************************************************
 */
void
defrag_function (void)
{
	const DEVTB	*dp;
	DOUBLE		**dpp;
	DOUBLE		**end_blockmap;
	char		*bp;
	daddr_t		bno;
	time_t		begin_time;
	int		seg, min, hor, len;
	int		fase = 1;
	char		text[64];

	/*
	 *	Verifica se este dispositivo permite a desfragmentação
	 */
	time (&begin_time);

	cache_miss = cache_hit = 0;

	draw_action_line ("");

	dp = &devtb[dev_defrag_index];

	if (dp->d_state != FS_OK)
		{ xerror ("Este dispositivo NÃO permite a desfragmentação"); return; }

	if (dev_fd < 0)
		{ xerror ("O dispositivo está fechado"); return; }

	/*
	 *	Prepara o mapa de blocos DEPOIS
	 */
	if (!data_ready)
		return;

	alloc_after_blocks (1);		/* Com cópia */
	draw_after_blocks ();
	after_map_ready = 1;

	do_XFlush ();

	/*
	 *	Atualiza os blocos
	 */
	draw_action_line ("*** Fase %d: Realocando os blocos", fase++);

	for (dpp = block_map, end_blockmap = dpp + fs_sz; dpp < end_blockmap; dpp++)
		interchange_block_list (dpp);

	/*
	 *	Atualiza os INODEs
	 */
	switch (method_in_use)
	{
	    case METHOD_FILE:
	    case METHOD_FILE_AND_HOLE:
		draw_action_line ("*** Fase %d: Atualizando os INODEs", fase++);

		update_all_inodes ();
		break;

	    case METHOD_FTW:
		draw_action_line ("*** Fase %d: Atualizando os INODEs", fase++);

		write_all_moved_inodes ();

		draw_action_line ("*** Fase %d: Atualizando os Diretórios", fase++);

		update_dir_entries ();
		update_sb ();
		break;
	};

	/*
	 *	Escreve os mapas de bits do disco
	 */
	draw_action_line ("*** Fase %d: Atualizando os Mapas de bits", fase++);

	bp = new_bit_map;

	for (bno = T1_BMNO; bno < fs_sz; bno += T1_ZONE_SZ, bp += BL4SZ)
	{
		if (bwrite (bp, bno, BL4SZ) < 0)
			return;
	}

	after_map_ready = 1; draw_after_blocks ();

#ifdef	DEBUG
	printf
	(	"CACHE: total = %d, acertos = %d, %d %%\n",
		cache_miss + cache_hit, cache_hit,
		cache_hit * 100 / (cache_hit + cache_miss)
	);
#endif	DEBUG

	draw_action_line ("*** Fase %d: Atualizando os blocos no disco", fase++);

	close (dev_fd); dev_fd = -1;

	data_ready = 0;

	/*
	 *	Escreve o tempo
	 */
	seg = time ((time_t *)NULL) - begin_time;

	min  = seg / 60;
	seg  = seg % 60;

	hor  = min / 60;
	min  = min % 60;

	len = snprintf
	(	text, sizeof (text),
		"Desframentação concluída, Tempo = %02d:%02d:%02d",
		hor, min, seg
	);

	draw_action_line (text);

}	/* end defrag_function */

/*
 ****************************************************************
 *	Gera um vetor de endereços para um INODE		*
 ****************************************************************
 */
DOUBLE *
create_double_vec (INOVEC *ip, const T1DINO *np, const char **msg)
{
	DOUBLE		*double_vec, *ap;
	const DOUBLE	*end_ap;
	const daddr_t	*p0;
	const daddr_t	*p1, *end_p1;
	const daddr_t	*p2, *end_p2;
	const char	*bp = old_bit_map;
	daddr_t		bno, addr1, addr2;
	daddr_t		addrvec2[T1_INDIR_SZ];
	daddr_t		addrvec1[T1_INDIR_SZ];

	/*
	 *	Prepara o espaço para os endereços dos blocos
	 */
	*msg = NOSTR;	/* Memória esgotada */

	if ((double_vec = ap = malloc (ip->i_virtual_blkcnt * sizeof (DOUBLE))) == NODOUBLE)
		return (NODOUBLE);

	end_ap = ap + ip->i_virtual_blkcnt;

	ip->i_phys_blkcnt = 0;

	/*
	 *	Prepara os blocos diretos
	 */
	for (p0 = &np->n_addr[0]; p0 < &np->n_addr[T1_NADDR - 3]; p0++, ap++)
	{
		if (ap >= end_ap)
			return (double_vec);

		if ((bno = *p0) != NODADDR)
		{
			if (T1_GET_MAP (bp, bno) != T1_MAP_DATA)
				goto bad_block;

			ip->i_phys_blkcnt++;
		}

		ap->a_old_bno = bno;
	}

	if (ap >= end_ap)
		return (double_vec);

	/*
	 *	Prepara os blocos indiretos de grau 1
	 */
	if ((addr1 = np->n_addr[T1_NADDR - 3]) != NODADDR)
	{
		if (T1_GET_MAP (bp, addr1) != T1_MAP_INDIR)
			goto bad_indir;

		ip->i_phys_blkcnt++;

		ap->a_old_bno = addr1; ap++;

		if (bread (addrvec1, addr1, sizeof (addrvec1)) < 0)
			return (NODOUBLE);

		for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
		{
			if (ap >= end_ap)
				return (double_vec);

			if ((bno = *p1) != NODADDR)
			{
				if (T1_GET_MAP (bp, bno) != T1_MAP_DATA)
					goto bad_block;

				ip->i_phys_blkcnt++;
			}

			ap->a_old_bno = bno;
		}
	}
	else 	/* NÃO há blocos indiretos de grau 1 */
	{
		ap->a_old_bno = NODADDR; ap++;

		for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
		{
			if (ap >= end_ap)
				return (double_vec);

			ap->a_old_bno = NODADDR;
		}
	}

	if (ap >= end_ap)
		return (double_vec);

	/*
	 *	Prepara os blocos indiretos de grau 2
	 */
	if ((addr2 = np->n_addr[T1_NADDR - 2]) != NODADDR)
	{
		if (T1_GET_MAP (bp, addr2) != T1_MAP_INDIR)
			goto bad_indir;

		ip->i_phys_blkcnt++;

		ap->a_old_bno = addr2; ap++;

		if (ap >= end_ap)
			return (double_vec);

		if (bread (addrvec2, addr2, sizeof (addrvec2)) < 0)
			return (NODOUBLE);

		for (p2 = addrvec2, end_p2 = p2 + T1_INDIR_SZ; p2 < end_p2; p2++)
		{
			if ((bno = *p2) != NODADDR)
			{
				if (T1_GET_MAP (bp, bno) != T1_MAP_INDIR)
					goto bad_indir;

				ip->i_phys_blkcnt++;

				ap->a_old_bno = bno; ap++;

				if (ap >= end_ap)
					return (double_vec);

				if (bread (addrvec1, bno, sizeof (addrvec1)) < 0)
					return (NODOUBLE);

				for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
				{
					if (ap >= end_ap)
						return (double_vec);

					if ((bno = *p1) != NODADDR)
					{
						if (T1_GET_MAP (bp, bno) != T1_MAP_DATA)
							goto bad_block;

						ip->i_phys_blkcnt++;
					}

					ap->a_old_bno = bno;
				}
			}
			else 	/* NÃO há blocos indiretos de grau 1 deste item do grau 2 */
			{
				ap->a_old_bno = NODADDR; ap++;

				for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
				{
					if (ap >= end_ap)
						return (double_vec);

					ap->a_old_bno = NODADDR;
				}
			}

			if (ap >= end_ap)
				return (double_vec);
		}
	}
	else 	/* NÃO há blocos indiretos de grau 2 */
	{
		ap->a_old_bno = NODADDR; ap++;

		for (p2 = addrvec2; p2 < addrvec2 + T1_INDIR_SZ; p2++)
		{
			if (ap >= end_ap)
				return (double_vec);

			ap->a_old_bno = NODADDR; ap++;

			for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
			{
				if (ap >= end_ap)
					return (double_vec);

				ap->a_old_bno = NODADDR;
			}
		}
	}

   /***	if (ap >= end_ap) ***/
	   /***	return (double_vec); ***/

	/*
	 *	Prepara os blocos indiretos de grau 3 (A FAZER)
	 */
	return (double_vec);

	/*
	 *	Condições de erro
	 */
   bad_block:
	*msg = "Bloco de dados NÃO encontrado no mapa de bits";
	return (NODOUBLE);

   bad_indir:
	*msg = "Bloco indireto NÃO encontrado no mapa de bits";
	return (NODOUBLE);

}	/* end create_double_vec */

/*
 ****************************************************************
 *	Coloca no lugar correto todos os blocos de uma lista	*
 ****************************************************************
 */
void
interchange_block_list (DOUBLE **dpp)
{
	DOUBLE		*ap0, *ap1;
	typedef struct { DOUBLE *a_addr; char a_buf[BL4SZ]; } BUF;
	BUF		buf0, buf1;
	BUF		*bp0 = &buf0, *bp1 = &buf1;
	BUF		*save;
	daddr_t		old_bno;

	/*
	 *	Se o bloco não tem dados, nada precisa ser feito
	 */
	if ((ap0 = *dpp) == (DOUBLE *)NULL)
		return;

	/*
	 *	Se NÃO há bloco, ou ele já está no lugar, também nada precisa ser feito
	 */
	if (ap0->a_old_bno == ap0->a_new_bno)
		return;

	/*
	 *	Lê o conteúdo do bloco e atualiza as estruturas de dados
	 */
	bp0->a_addr = ap0;

	bread (bp0->a_buf, ap0->a_old_bno, BL4SZ);

   /***	ap0->a_old_bno = NODADDR; ***/

	*dpp = (DOUBLE *)NULL;

	redraw_after_block (ap0->a_old_bno);

	/*
	 *	Percorre a lista de blocos
	 */
	for (EVER)
	{
		/*
		 *	Se for o caso, salva os dados do destino
		 */
		dpp = &block_map[ap0->a_new_bno];

		if ((bp1->a_addr = ap1 = *dpp) != (DOUBLE *)NULL)
		{
			if (ap1->a_old_bno != ap0->a_new_bno)
			{
				fprintf
				(	stderr,
					"%s: Bloco dado (%d) não coincidiu com bloco atual (%d)\n",
					pgname, ap0->a_new_bno, ap1->a_old_bno
				);
				return;
			}

			bread (bp1->a_buf, ap1->a_old_bno, BL4SZ);

		   /***	ap1->a_old_bno = NODADDR; ***/

		   /***	*dpp = (DOUBLE *)NULL; ***/
		}

		/*
		 *	Agora, copia os dados para o destino
		 */
		bwrite (bp0->a_buf, ap0->a_new_bno, BL4SZ);

		old_bno = ap0->a_old_bno;

		ap0->a_old_bno = ap0->a_new_bno;

		*dpp = ap0;

		percent_action_done++;

		redraw_after_block (ap0->a_new_bno);

		/*
		 *	Agora, troca os "buffers" e recomeça
		 */
		save = bp0;	bp0 = bp1;	bp1 = save;

		/*
		 *	Verifica se há dados a mover
		 */
		if ((ap0 = bp0->a_addr) == (DOUBLE *)NULL)
		{
			redraw_after_block (old_bno); return;
		}

	   /***	if (ap0->a_old_bno == ap0->a_new_bno) ***/
		   /***	return; ***/

	}	/* end for (EVER) */

}	/* end interchange_block_list */

/*
 ****************************************************************
 *	Atualiza um INODE					*
 ****************************************************************
 */
int
update_inode_addr (ino_t ino, INOVEC *ip)
{
	int		index;
	daddr_t		ino_bno, *p0, *p1, *p2;
	daddr_t		addr1, addr2;
	const daddr_t	*end_p1, *end_p2;
	T1DINO		*np;
	DOUBLE		*ap;
	const DOUBLE	*end_ap;
	daddr_t		addrvec1[T1_INDIR_SZ];
	daddr_t		addrvec2[T1_INDIR_SZ];

	/*
	 *	Lê o INODE
	 */
	ino_bno = T1_INOtoADDR (ino);

	bget (&inode_cache, ino_bno);

	np = (T1DINO *)inode_cache.b_area + T1_INOtoINDEX (ino);

	ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

	/*
	 *	Prepara os blocos diretos
	 */
	memsetl (np->n_addr, 0, T1_NADDR);

	for (index = 0, p0 = np->n_addr; index < T1_NADDR - 3; index++, p0++, ap++)
	{
		if (ap >= end_ap)
			goto write_ino;

		*p0 = ap->a_new_bno;
	}

	/*
	 *	Prepara os blocos indiretos de grau 1
	 */
	if (ap >= end_ap)
		goto write_ino;

	addr1 = ap->a_new_bno; ap++;

	np->n_addr[T1_NADDR - 3] = addr1;

	if (addr1 != NODADDR)
	{
		memsetl (addrvec1, 0, T1_INDIR_SZ);

		for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
		{
			if (ap >= end_ap)
				break;

			*p1 = ap->a_new_bno;
		}

		if (bwrite (addrvec1, addr1, sizeof (addrvec1)) < 0)
			return (-1);
	}
	else
	{
		ap += T1_INDIR_SZ;
	}

	/*
	 *	Prepara os blocos indiretos de grau 2
	 */
	if (ap >= end_ap)
		goto write_ino;

	addr2 = ap->a_new_bno; ap++;

	np->n_addr[T1_NADDR - 2] = addr2;

	if (addr2 != NODADDR)
	{
		memsetl (addrvec2, 0, T1_INDIR_SZ);

		for (p2 = addrvec2, end_p2 = p2 + T1_INDIR_SZ; p2 < end_p2; p2++)
		{
			if (ap >= end_ap)
				break;

			addr1 = ap->a_new_bno; ap++;

			if ((*p2 = addr1) != NODADDR)
			{
				memsetl (addrvec1, 0, T1_INDIR_SZ);

				for (p1 = addrvec1, end_p1 = p1 + T1_INDIR_SZ; p1 < end_p1; p1++, ap++)
				{
					if (ap >= end_ap)
						break;

					*p1 = ap->a_new_bno;
				}

				if (bwrite (addrvec1, addr1, sizeof (addrvec1)) < 0)
					return (-1);
			}
			else
			{
				ap += T1_INDIR_SZ;
			}
		}

		if (bwrite (addrvec2, addr2, sizeof (addrvec2)) < 0)
			return (-1);
	}

	/*
	 *	Prepara os blocos indiretos de grau 3 (A FAZER)
	 */

	/*
	 *	Escreve o INODE
	 */
    write_ino:
	inode_cache.b_dirty = 1;

	return (0);

}	/* end update_inode_addr */
