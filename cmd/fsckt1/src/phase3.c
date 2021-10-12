/*
 ****************************************************************
 *								*
 *			phase3.c				*
 *								*
 *	Fase 3: Analisa as entradas dos diretórios		*
 *								*
 *	Versão	4.3.0, de 16.07.02				*
 *		4.3.0, de 16.08.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
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
 *	Analisa as entradas dos diretórios			*
 ****************************************************************
 */
int
phase3 (void)
{
	INO_HASH	*ip;
	T1DINO		*np;
	DIRINFO		root_info;

	printf ("*** Fase 3: Analisa as entradas dos diretórios\n");

	if ((ip = ino_hash (T1_ROOT_INO)) == NO_INO_HASH)
		goto case_NO_STATE;

	np = inode_get (T1_ROOT_INO);

	/*
	 *	Examina a RAIZ
	 */
	switch (ip->i_state)
	{
	    case NO_STATE:
	    case_NO_STATE:
		printf ("\nA RAIZ do sistema de arquivos não está alocada!\n");

		/* Talvez consiga colocar tudo em "lost+found" ... */

		return (CONTINUE);

	    case FILE_STATE:
		printf ("\nA RAIZ do sistema de arquivos não é um diretório. ");

		if (faskyesno ("Ajusta") <= 0)
			return (STOP);

		np->n_mode &= ~S_IFMT; np->n_mode |= S_IFDIR;

		inoblk.b_dirty++;

		ip->i_state = DIR_STATE;

	   /***	goto case_DIR_STATE; ***/

	    case DIR_STATE:
	    case_DIR_STATE:
		root_info.f_hash_ip	= ip;
		root_info.f_mode	= np->n_mode;
		memmove (root_info.f_addr, np->n_addr, sizeof (np->n_addr));
		root_info.f_offset	= 0;
		root_info.f_dir_ino	= T1_ROOT_INO;
		root_info.f_ent_ino	= T1_ROOT_INO;
	   /***	root_info.f_data	= 0;	***/
		root_info.f_path_len	= 1;
		root_info.f_path	= "/";

		dir_walk (&root_info, dir_check_entry);	/* Caminha a árvore recursivamente */

		return (CONTINUE);

	    case CLR_STATE:
		printf
		(	"\nA RAIZ do sistema de arquivos contém "
			"blocos inválidos e/ou duplicados\n"
			"O sistema de arquivos NÃO poderá ser consertado "
			"inteiramente\n"
		);

		ip->i_state = DIR_STATE;	/* Desespêro !! */

		goto case_DIR_STATE;

	    default:
		printf ("phase3: Estado inválido da RAIZ\n");

		return (STOP);

	}	/* end switch */

}	/* end phase3 */

/*
 ****************************************************************
 *	Caminha pelas entradas de um DIRETÓRIO			*
 ****************************************************************
 */
int
dir_walk (DIRINFO *fp, int (*func) (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *))
{
	daddr_t		*bp, bno;
	int		level, ret;

	/*
	 *	Pequena consistência
	 */
	if ((fp->f_mode & S_IFMT) != S_IFDIR)
		{ error ("dir_walk: chamada para NÃO diretório"); return (CONTINUE); }

	/*
	 *	O Diretório agora já foi visto
	 */
	fp->f_hash_ip->i_state = SEEN_DIR_STATE;

	/*
	 *	Chama a função para os blocos diretos
	 */
	for (bp = &fp->f_addr[0]; bp < &fp->f_addr[T1_NADDR - 3]; bp++)
	{
		bno = *bp;

		if (bno <= T1_BMNO || bno >= ((T1SB *)sb.b_area)->s_fs_sz)
			continue;

		if ((ret = dir_block_walk (fp, bno, func)) & STOP)
			return (ret);
	}

	/*
	 *	Chama a função para os blocos indiretos
	 */
	for (level = 1; level <= 3; level++, bp++)
	{
		bno = *bp;

		if (bno <= T1_BMNO || bno >= ((T1SB *)sb.b_area)->s_fs_sz)
			continue;

		if ((ret = dir_ind_walk (fp, bno, level, func)) & STOP)
			return (ret);
	}

	return (CONTINUE);

}	/* end dir_walk */

/*
 ****************************************************************
 * Caminha pelas entradas dos blocos indiretos de um DIRETÓRIO	*
 ****************************************************************
 */
int
dir_ind_walk (DIRINFO *fp, daddr_t bno, int level, int (*func) (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *))
{
	daddr_t		*bp;
	int		ret;
	daddr_t		iblk[T1_INDIR_SZ];

	/*
	 *	Obtém o bloco de endereços indiretos e guarda-o
	 */
	bread (iblk, bno, BL4SZ);

	/*
	 *	Processa os diversos níveis indiretos
	 */
	level--;

	for (bp = &iblk[0]; bp < &iblk[T1_INDIR_SZ]; bp++)
	{
		bno = *bp;

		if (bno <= T1_BMNO || bno >= ((T1SB *)sb.b_area)->s_fs_sz)
			continue;

		if (level > 0)
			ret = dir_ind_walk (fp, bno, level, func);
		else
			ret = dir_block_walk (fp, bno, func);

		if (ret & STOP)
			return (ret);
	}

	return (CONTINUE);

}	/* end dir_ind_walk */

/*
 ****************************************************************
 *	Percorre as entradas do diretório			*
 ****************************************************************
 */
int
dir_block_walk (DIRINFO *dir_fp, daddr_t bno, int (*func) (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *))
{
	T1DIR		*adp, *dp, *ndp;
	int		ret;
	char		*cat_point;
	DIRINFO		ent_info;

	/*
	 *	Obtém o bloco do diretório
	 */
	bget (&fileblk, bno);

	/*
	 *	Prepara a parte comum de ent_info
	 */
	ent_info.f_dir_ino  = dir_fp->f_ent_ino;
	ent_info.f_path     = alloca (dir_fp->f_path_len + MAXNAMLEN + 2);

	strcpy (ent_info.f_path, dir_fp->f_path);

	if (dir_fp->f_path_len > 1)
		strcat (ent_info.f_path, "/");

	cat_point = strend (ent_info.f_path);

	/*
	 *	Percorre as entradas de diretórios deste bloco
	 */
	for
	(	adp = NOT1DIR, dp = (T1DIR *)fileblk.b_area;
		dp < (T1DIR *)&fileblk.b_area[BL4SZ];
		adp = dp, dp = ndp
	)
	{
		/*
		 *	Completa o DIRINFO
		 */
		ent_info.f_hash_ip  = ino_hash (dp->d_ino);
	   /***	ent_info.f_mode     = ...; na função recursiva ***/
	   /***	ent_info.f_addr     = ...; na função recursiva ***/
	   /***	ent_info.f_offset   = ...; na função recursiva ***/
	   /***	ent_info.f_dir_ino  = ...; acima ***/
		ent_info.f_ent_ino  = dp->d_ino;
	   /***	end_info.f_data	    = 0;	***/
		ent_info.f_path_len = dir_fp->f_path_len + dp->d_nm_len + 1;
	   /***	ent_info.f_path     = ...; completa abaixo ***/

		/*
		 *	Compõe o caminho completo
		 */
		strcpy (cat_point, dp->d_name);

		/*
		 *	Chama a função final
		 */
		ndp = T1DIR_SZ_PTR (dp);

		if ((ret = (*func) (dir_fp, &ent_info, adp, dp)) & STOP)
			return (ret);

		/*
		 *	Obtém o bloco do diretório de volta (se for o caso)
		 */
		bget (&fileblk, bno);

		if (dp->d_ent_sz != 0)
			ndp = T1DIR_SZ_PTR (dp);

#if (0)	/*******************************************************/
		if (dp->d_ent_sz != 0)
		{
			ndp = T1DIR_SZ_PTR (dp);

			if (ndp->d_ino == 0)
			{
				ndp = (T1DIR *)&fileblk.b_area[BL4SZ];

				dp->d_ent_sz = (int)ndp - (int)dp;
			}
		}
#endif	/*******************************************************/

		dir_fp->f_offset += (int)ndp - (int)dp;

	}	/* end percorrendo os diretórios deste bloco */

	/*
	 *	As somas das entradas deve ter exatamente BL4SZ
	 */
	if (dp != (T1DIR *)&fileblk.b_area[BL4SZ])
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Bloco %d com soma de tamanhos incorreta (%d :: %d). ",
			ent_info.f_path, bno,
			(char *)dp - &fileblk.b_area[0], BL4SZ
		);

		if (faskyesno ("Ajusta") > 0)
		{
			if (adp == NOT1DIR)
				dp->d_ent_sz = BL4SZ;
			else
				adp->d_ent_sz = (int)&fileblk.b_area[BL4SZ] - (int)adp;

			fileblk.b_dirty++;
		}
	}

	return (CONTINUE);

}	/* end dir_block_walk */

/*
 ****************************************************************
 *	Confere uma entrada do diretório			*
 ****************************************************************
 */
int
dir_check_entry (DIRINFO *dir_fp, DIRINFO *ent_fp, T1DIR *adp, T1DIR *dp)
{
	T1DIR		*ndp;
	T1DINO		*np;
	INO_HASH	*ip;
	int		ino_inval = 0, len, min_ent_sz;

	/*
	 *	Verifica se o tamanho é excessivo
	 */
	if (T1DIR_SZ_PTR (dp) > (T1DIR *)&fileblk.b_area[BL4SZ])
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada \"%s\" com tamanho %d excessivo. ",
			dir_fp->f_path, dp->d_name, dp->d_ent_sz
		);

		dp->d_ent_sz = (int)&fileblk.b_area[BL4SZ] - (int)dp;

		if (faskyesno ("Ajusta") > 0)
			fileblk.b_dirty++;
	}

	/*
	 *	Lembrar que uma entrada vazia deve estar no início do bloco
	 */
	if (ent_fp->f_ent_ino == 0)
	{
		if (dir_fp->f_offset & BL4MASK)
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"Entrada vazia no meio do bloco. ",
				dir_fp->f_path
			);

			ndp = T1DIR_SZ_PTR (dp);

			if (ndp->d_ino == 0)
				dp->d_ent_sz = (int)&fileblk.b_area[BL4SZ] - (int)dp;

			if (faskyesno ("Remove") > 0)
				dir_remove_entry (adp, dp);
		}

		return (CONTINUE);
	}

	/*
	 *	Verifica se o nome do INODE na entrada está vazio
	 */
	if (dp->d_name[0] == '\0')
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada de INODE %d com nome vazio. ",
			dir_fp->f_path, ent_fp->f_ent_ino
		);

		ndp = T1DIR_SZ_PTR (dp);

		if (ndp->d_ino == 0)
			dp->d_ent_sz = (int)&fileblk.b_area[BL4SZ] - (int)dp;

		if (faskyesno ("Remove") > 0)
			dir_remove_entry (adp, dp);

		return (CONTINUE);
	}

	/*
	 *	Verifica se o tamanho da entrada é múltiplo de 4
	 */
	if ((dp->d_ent_sz & 3) || dp->d_ent_sz < T1DIR_SIZEOF (0))
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada \"%s\" com tamanho %d inválido. ",
			dir_fp->f_path, dp->d_name, dp->d_ent_sz
		);

		dp->d_ent_sz = T1DIR_SIZEOF (dp->d_nm_len);

		if (faskyesno ("Ajusta") > 0)
			fileblk.b_dirty++;
	}

	/*
	 *	Verifica se a entrada tem um número válido de INODE
	 */
	if (ent_fp->f_hash_ip == NO_INO_HASH)
		ino_inval++;

	/*
	 *	Verifica se o início do diretório está com "."
	 */
	if (dir_fp->f_offset == 0)
	{
		if (dp->d_name[0] != '.' || dp->d_name[1] != '\0')
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"A primeira entrada NÃO é \".\". ",
				dir_fp->f_path
			);

			if (faskyesno ("Conserta") > 0)
			{
				dp->d_ino    = dir_fp->f_ent_ino;
				dp->d_ent_sz = T1DIR_SIZEOF (1);
				dp->d_nm_len = 1;
				strcpy (dp->d_name, ".");

				if (!ino_inval)
					ent_fp->f_hash_ip->i_mem_link++;

				fileblk.b_dirty++;

				return (CONTINUE);
			}
		}
	}

	/*
	 *	Se for ".", ...
	 */
	if (dp->d_name[0] == '.' && dp->d_name[1] == '\0')
	{
		if   (dir_fp->f_offset != 0)
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"Entrada \".\" em local inválido. ",
				dir_fp->f_path
			);

			if (faskyesno ("Remove") > 0)
			{
				dir_remove_entry (adp, dp);
				return (CONTINUE);
			}
		}
		elif (ent_fp->f_ent_ino != dir_fp->f_ent_ino)
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"Entrada \".\" com número de INODE inválido "
				"(%d :: %d). ",
				dir_fp->f_path,
				ent_fp->f_ent_ino, dir_fp->f_ent_ino
			);

			if (faskyesno ("Ajusta") > 0)
			{
				dp->d_ino = ent_fp->f_ent_ino = dir_fp->f_ent_ino;

				ent_fp->f_hash_ip = ino_hash (dp->d_ino);
				ino_inval = 0;

				fileblk.b_dirty++;
			}
		}

		if (!ino_inval)
			ent_fp->f_hash_ip->i_mem_link++;

		return (CONTINUE);
	}

	/*
	 *	Verifica se a segunda entrada está com ".."
	 */
	if (dir_fp->f_offset == T1DIR_SIZEOF (1))
	{
		if (dp->d_name[0] != '.' || dp->d_name[1] != '.' || dp->d_name[2] != '\0')
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"A segunda entrada NÃO é \"..\". ",
				dir_fp->f_path
			);

			if (faskyesno ("Conserta") > 0)
			{
				dp->d_ino	= dir_fp->f_dir_ino;
				dp->d_ent_sz	= T1DIR_SIZEOF (2);
				dp->d_nm_len	= 2;
				strcpy (dp->d_name, "..");

			   	dir_fp->f_hash_ip->i_mem_link++;
			   	dir_fp->f_hash_ip->i_disk_link++;
				fileblk.b_dirty++;

				return (CONTINUE);
			}
		}
	}

	/*
	 *	Se for "..", ...
	 */
	if (dp->d_name[0] == '.' && dp->d_name[1] == '.' && dp->d_name[2] == '\0')
	{
		if (dir_fp->f_offset != T1DIR_SIZEOF (1))
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"Entrada \"..\" em local inválido. ",
				dir_fp->f_path
			);

			if (faskyesno ("Remove") > 0)
			{
				dir_remove_entry (adp, dp);
				return (CONTINUE);
			}
		}
		elif (ent_fp->f_ent_ino != dir_fp->f_dir_ino)
		{
			printf
			(	"\nDIRETÓRIO: \"%s\"\n"
				"Entrada \"..\" com número de INODE inválido "
				"(%d :: %d). ",
				dir_fp->f_path,
				ent_fp->f_ent_ino, dir_fp->f_dir_ino
			);

			if (faskyesno ("Ajusta") > 0)
			{
				dp->d_ino = ent_fp->f_ent_ino = dir_fp->f_dir_ino;

				ent_fp->f_hash_ip = ino_hash (dp->d_ino);
				ino_inval = 0;

				fileblk.b_dirty++;
			}
		}

		if (!ino_inval)
			ent_fp->f_hash_ip->i_mem_link++;

		return (CONTINUE);
	}

	/*
	 *	Verifica se o tamanho do nome na entrada está correto
	 */
	if ((len = strlen (dp->d_name)) != dp->d_nm_len)
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada \"%s\" com tamanho do nome inválido (%d :: %d). ",
			dir_fp->f_path, dp->d_name, dp->d_nm_len, len 
		);

		if (faskyesno ("Ajusta") > 0)
		{
			dp->d_nm_len = len;

			fileblk.b_dirty++;

		}
	}

	/*
	 *	Verifica se o tamanho da entrada é compatível com o do nome
	 */
	min_ent_sz = T1DIR_SIZEOF (len);

	if (dp->d_ent_sz < min_ent_sz)
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada \"%s\" com tamanho da entrada insuficiente (%d :: %d). ",
			dir_fp->f_path, dp->d_name, dp->d_ent_sz, min_ent_sz
		);

		if (faskyesno ("Ajusta") > 0)
		{
			char		*cp = (char *)T1DIR_SZ_PTR (dp);

			cp[-1] = '\0';
			dp->d_nm_len = strlen (dp->d_name);

			fileblk.b_dirty++;
		}
	}

	/*
	 *	Verifica se o no. do INODE na entrada é válido
	 */
	if (ino_inval)
	{
		printf
		(	"\nDIRETÓRIO: \"%s\"\n"
			"Entrada \"%s\" com número de INODE %d inválido. ",
			ent_fp->f_path, dp->d_name, dp->d_ino
		);

		if (faskyesno ("Remove a entrada") > 0)
			dir_remove_entry (adp, dp);

		return (CONTINUE);
	}

	/*
	 ******	Caminha na subárvore ************************************
	 */
	ip = ent_fp->f_hash_ip; ip->i_mem_link++;

	np = inode_get (ip->i_ino);

    again:
	switch (ip->i_state)
	{
	    case DIR_STATE:
		ent_fp->f_hash_ip	= ip;
		ent_fp->f_mode		= np->n_mode;
		memmove (ent_fp->f_addr, np->n_addr, sizeof (np->n_addr));
		ent_fp->f_offset	= 0;
	   /***	ent_fp->f_dir_ino	= ...; herdado de dir_walk ***/
	   /***	ent_fp->f_ent_ino	= ...; herdado de dir_walk ***/
	   /***	ent_fp->f_data		= 0;	***/
	   /***	ent_fp->f_path_len	= ...; herdado de dir_walk ***/
	   /***	ent_fp->f_path		= ...; herdado de dir_walk ***/

		dir_walk (ent_fp, dir_check_entry);	/* Caminha a árvore recursivamente */

		return (CONTINUE);

	    case SEEN_DIR_STATE:
		printf
		(	"\nENTRADA: \"%s\"\n"
			"Referenciando o INODE %d que é um diretório já visto anteriormente.\n"
			"Esta entrada está causando um ciclo no sistema de arquivos.\n",
			ent_fp->f_path, ent_fp->f_ent_ino
		);

		if (faskyesno ("Remove a entrada") > 0)
		{
			dir_remove_entry (adp, dp);

			ip->i_mem_link--;
		}

		return (CONTINUE);

	    case FILE_STATE:
		return (CONTINUE);	/* Esta fase só examina diretórios */

	    case CLR_STATE:
		printf
		(	"\nENTRADA: \"%s\"\n"
			"Referenciando o INODE %d que tem blocos inválidos "
			"e/ou duplicados.\n",
			ent_fp->f_path, ent_fp->f_ent_ino
		);

		if (faskyesno ("Remove a entrada") > 0)
		{
			dir_remove_entry (adp, dp);

			ip->i_mem_link--;

			return (CONTINUE);
		}

		ip->i_state = ((np->n_mode & S_IFMT) == S_IFDIR) ? DIR_STATE : FILE_STATE;

		goto again;

	    default:
		printf ("dir_check_entry: Encontrei estado %d inválido\n", ip->i_state);
		return (CONTINUE);

	}	/* end switch */

}	/* end dir_check_entry */

/*
 ****************************************************************
 *	Remove a entrada de um diretório			*
 ****************************************************************
 */
void
dir_remove_entry (T1DIR *adp, T1DIR *dp)
{
	short		ent_sz;

	ent_sz = dp->d_ent_sz;

	memset (dp, 0, ent_sz);

	if (adp == NOT1DIR)
		dp->d_ent_sz = ent_sz;
	else
		adp->d_ent_sz += ent_sz;

	fileblk.b_dirty++;

}	/* end dir_remove_entry */
