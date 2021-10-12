/*
 ****************************************************************
 *								*
 *			phase4.c				*
 *								*
 *	Quarta Fase: Procura subárvores desconectadas		*
 *								*
 *	Versão	4.3.0, de 22.07.02				*
 *		4.3.0, de 16.08.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/stat.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "../h/fsckt1.h"

/*
 ****************************************************************
 *	Quarta fase: verifica a conectividade			*
 ****************************************************************
 */
int
phase4 (void)
{
	INO_HASH	*ip, *orphan_ip, *par_ip, **hp;
	INO_HASH	*lf_ip = NO_INO_HASH;
	ino_t		orphan_ino;
	DIRINFO		info;
	T1DINO		*np;
	ino_t		lf_ino, par_ino;

	printf ("*** Fase 4: Procura subárvores desconectadas\n");

	/*
	 *	Em primeiro lugar, procura o diretório "lost+found"
	 */
	if ((ip = ino_hash (T1_ROOT_INO)) == NO_INO_HASH)
		goto no_lf;

	np = inode_get (T1_ROOT_INO);

	info.f_hash_ip	= ip;
	info.f_mode	= np->n_mode;
	memmove (info.f_addr, np->n_addr, sizeof (np->n_addr));
	info.f_offset	= 0;
	info.f_dir_ino	= 0;	/* Aqui vem o resultado */
	info.f_ent_ino	= T1_ROOT_INO;
   /***	info.f_data	= 0;	***/
	info.f_path_len	= 1;
	info.f_path	= "/";

	dir_walk (&info, find_lost_found);

	lf_ino = info.f_dir_ino;

	if ((lf_ip = ino_hash (lf_ino)) == NO_INO_HASH)
	{
	    no_lf:
		printf ("\nO sistema de arquivos NÃO possui o diretório \"/lost+found\"\n");
		lf_ino = 0;
	}

	if (lf_ino != 0 && lf_ip->i_state != SEEN_DIR_STATE)
	{
		printf ("\nO arquivo \"/lost+found\" NÃO é um diretório\n");
		lf_ino = 0;
	}

	lost_found_ip = lf_ip;

	/*
	 *	Percorre todos os INODEs
	 */
	for (hp = &inode_hash_tb[0]; hp < &inode_hash_tb[INO_HASH_SZ]; hp++)
	{
		for (ip = *hp; ip != NO_INO_HASH; ip = ip->i_next)
		{
			if (ip->i_state != DIR_STATE)
				continue;

			/*
			 *	Neste ponto, só chegam diretórios NÃO
			 *	caminhados na fase 3, ou seja, desconectados
			 *
			 *	Sobe até achar a raiz desta subárvore desconectada
			 */
		   	par_ino = ip->i_ino; par_ip = ip;

			do
			{
				orphan_ino = par_ino; orphan_ip = par_ip;

				np = inode_get (orphan_ino);

				info.f_hash_ip	= orphan_ip;
				info.f_mode	= np->n_mode;
				memmove (info.f_addr, np->n_addr, sizeof (np->n_addr));
				info.f_offset	= 0;
				info.f_dir_ino  = 0;	/* A incógnita */
				info.f_ent_ino	= orphan_ino;
			   /***	info.f_data	= 0;	***/
				info.f_path_len	= 1;
				info.f_path	= "/";

				dir_walk (&info, find_parent);

				if ((par_ino = info.f_dir_ino) == NOINO)
					break;

				if ((par_ip = ino_hash (par_ino)) == NO_INO_HASH)
					break;

			}	while (par_ip->i_state == DIR_STATE);

			/*
			 *	Verifica se o pai está correto
			 */
			if (par_ip != NO_INO_HASH && par_ip->i_state != SEEN_DIR_STATE)
				par_ip = NO_INO_HASH;

			/*
			 *	Tenta reconectar ao pai (de verdade ou "lost+found")
			 */
			if (par_ip != NO_INO_HASH && link_up (orphan_ip, par_ip) != CONTINUE)
				par_ip = NO_INO_HASH;

			if (par_ip == NO_INO_HASH && link_up (orphan_ip, lf_ip) == CONTINUE)
				par_ip = lf_ip;

			if (par_ip == NO_INO_HASH)
				continue;

			/*
			 *	Conseguiu reconectar: caminha nesta subárvore
			 */
			np = inode_get (orphan_ino);

			info.f_hash_ip	= orphan_ip;
			info.f_mode	= np->n_mode;
			memmove (info.f_addr, np->n_addr, sizeof (np->n_addr));
			info.f_offset	= 0;
		   	info.f_dir_ino  = par_ino;
			info.f_ent_ino  = orphan_ino;
		   /***	info.f_data	= 0;	***/
			info.f_path_len	= 1;
			info.f_path	= "/";

			dir_walk (&info, dir_check_entry);
		}

	}	/* end varrendo todos os INODEs */ 

	return (CONTINUE);

}	/* end phase4 */

/*
 ****************************************************************
 *	Procura a entrada "lost+found" no diretório		*
 ****************************************************************
 */
int
find_lost_found (DIRINFO *dir_fp, DIRINFO *ent_fp, T1DIR *adp, T1DIR *dp)
{
	/*
	 *	Se for uma entrada vazia ou NÃO for "lost+found", ...
	 */
	if (dp->d_ino == 0 || !streq (dp->d_name, "lost+found"))
		return (CONTINUE);

	/*
	 *	É "lost+found"
	 */
	dir_fp->f_dir_ino = ent_fp->f_ent_ino;

	if (vflag)
	{
		T1DINO		*np;

		np = inode_get (ent_fp->f_ent_ino);

		printf ("O diretório \"/lost+found\" tem %d bytes\n", np->n_size);
	}

	return (STOP);

}	/* end find_lost_found */

/*
 ****************************************************************
 *	Procura a entrada ".." de um diretório			*
 ****************************************************************
 */
int
find_parent (DIRINFO *dir_fp, DIRINFO *ent_fp, T1DIR *adp, T1DIR *dp)
{
	/*
	 *	Se for uma entrada vazia ou NÃO for "..", ...
	 */
	if (dp->d_ino == 0 || !streq (dp->d_name, ".."))
		return (CONTINUE);

	/*
	 *	É ".."
	 */
	dir_fp->f_dir_ino = ent_fp->f_ent_ino;

	return (STOP);

}	/* end find_parent */

/*
 ****************************************************************
 *	Reconecta ao pai ou ao "lost+found"			*
 ****************************************************************
 */
int
link_up (INO_HASH *orphan_ip, INO_HASH *par_ip)
{
	T1DINO		*orphan_np, *par_np;
	int		isdir;
	DIRINFO		info;
	char		msg[64];

	/*
	 *	A variável "par_ip" pode se referir ao pai ou ao "lost+found"
	 *
	 *	Se "par_ip" == NO_INO_HASH, significa que "lost+found" NÃO existe
	 */
	orphan_np = inode_get (orphan_ip->i_ino);

	isdir = (orphan_np->n_mode & S_IFMT) == S_IFDIR;

	print_inode (orphan_ip->i_ino, orphan_np);
	printf ("%s NÃO referenciado. ", isdir ? "Diretório" : "Arquivo");

	if   (par_ip == NO_INO_HASH)
	{
		printf
		(	"\nO diretório \"/lost+found\" não está disponível. "
			"NÃO é possível reconectar\n"
		);
		return (SKIP);
	}
	elif (par_ip == lost_found_ip)
	{
		sprintf (msg, "Reconecta ao diretório \"/lost+found\"");
	}
	else	/* Pai legítimo */
	{
		sprintf (msg, "Reconecta ao diretório pai INODE %d", par_ip->i_ino);
	}

	if (faskyesno (msg) <= 0)
		return (SKIP);

	/*
	 *	Tenta criar a entrada
	 */
	par_np = inode_get (par_ip->i_ino);

	info.f_hash_ip	= par_ip;
	info.f_mode	= par_np->n_mode;
	memmove (info.f_addr, par_np->n_addr, sizeof (par_np->n_addr));
	info.f_offset	= 0;
	info.f_dir_ino  = 0;
	info.f_ent_ino  = par_ip->i_ino;
	info.f_data	= orphan_ip->i_ino;
	info.f_path_len	= 1;
	info.f_path	= "/";

	if ((dir_walk (&info, make_numerical_entry) & ALTERED) == 0)
	{
		printf ("O diretório está cheio. NÃO foi possível reconectar\n");
		return (SKIP);
	}

	orphan_ip->i_mem_link++;

	/*
	 *	Se não for diretório, acabou!
	 */
	if (!isdir)
		return (CONTINUE);

	/*
	 *	Ajusta a entrada ".." do diretório reconectado
	 */
	orphan_np = inode_get (orphan_ip->i_ino);

	info.f_hash_ip	= orphan_ip;
	info.f_mode	= orphan_np->n_mode;
	memmove (info.f_addr, orphan_np->n_addr, sizeof (orphan_np->n_addr));
	info.f_offset	= 0;
	info.f_dir_ino  = par_ip->i_ino;
	info.f_ent_ino  = orphan_ip->i_ino;
   /***	info.f_data	= 0;	***/
	info.f_path_len	= 1;
	info.f_path	= "/";

	if ((dir_walk (&info, dir_change_parent) & ALTERED) == 0)
		printf ("NÃO consegui ajustar a entrada \"..\" do diretório a ser reconectado!!\n");

	/*
	 *	Incrementa o número de LINKs do pai apenas no disco.
	 */
	par_np = inode_get (par_ip->i_ino);

	par_np->n_nlink++;

	par_ip->i_disk_link = par_np->n_nlink;

	inoblk.b_dirty++;

	return (CONTINUE);

}	/* end link_up */

/*
 ****************************************************************
 *	Cria a entrada para o arquivo no pai ou "lost+found"	*
 ****************************************************************
 */
int
make_numerical_entry (DIRINFO *dir_fp, DIRINFO *ent_fp, T1DIR *adp, T1DIR *dp)
{
	int		len, avail_sz, ent_sz, old_ent_sz;
	char		area[16];

	/*
	 *	Verifica se há algum espaço nesta entrada
	 */
	if (dp->d_ino == 0)
		avail_sz = dp->d_ent_sz;
	else
		avail_sz = dp->d_ent_sz - T1DIR_SIZEOF (dp->d_nm_len);

	if (avail_sz <= 0)
		return (CONTINUE);

	/*
	 *	Acha o tamanho necessário
	 */
	len = sprintf (area, "%d", dir_fp->f_data);

	ent_sz = T1DIR_SIZEOF (len);

	/*
	 *	Verifica se esta entrada tem espaço suficiente
	 */
	if (ent_sz > avail_sz)
		return (CONTINUE);

	/*
	 *	Prepara a entrada
	 */
	if (dp->d_ino == 0)
	{
	   /***	dp->d_ent_sz = ...;	***/
	}
	else
	{
		adp = dp; old_ent_sz = adp->d_ent_sz;

		adp->d_ent_sz = T1DIR_SIZEOF (adp->d_nm_len);

		dp = T1DIR_LEN_PTR (adp);

		dp->d_ent_sz = old_ent_sz - adp->d_ent_sz;
	}

	dp->d_ino    = dir_fp->f_data;
   /***	dp->d_ent_sz = ...;	***/
	dp->d_nm_len = len;
	strcpy (dp->d_name, area);

	fileblk.b_dirty++;

	return (ALTERED|STOP);

}	/* end make_numerical_entry */

/*
 ****************************************************************
 *	Altera o pai de um diretório				*
 ****************************************************************
 */
int
dir_change_parent (DIRINFO *dir_fp, DIRINFO *ent_fp, T1DIR *adp, T1DIR *dp)
{
	/*
	 *	Se for uma entrada vazia ou NÃO for "..", ...
	 */
	if (dp->d_ino == 0 || !streq (dp->d_name, ".."))
		return (CONTINUE);

	/*
	 *	É ".."
	 */
	dp->d_ino = dir_fp->f_dir_ino;

	fileblk.b_dirty++;

	return (ALTERED|STOP);

}	/* end dir_change_parent */
