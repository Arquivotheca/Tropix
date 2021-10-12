/*
 ****************************************************************
 *								*
 *			dir.c					*
 *								*
 *	Gera diretórios						*
 *								*
 *	Versão	4.3.0, de 04.07.02				*
 *		4.3.0, de 04.07.02				*
 *								*
 *	Módulo: mkfst1						*
 *		Utilitários especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/mkfst1.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry char	root_dir_image[BL4SZ];

entry char	lost_dir_image[BL4SZ];

entry daddr_t	root_dir_bno;

entry daddr_t	lost_dir_bno;

/*
 ****************************************************************
 *	Cria o diretório RAIZ					*
 ****************************************************************
 */
void
make_root_and_lost_dir (void)
{
	T1DINO		*ip;
	T1DIR		*dp;

	/*
	 ******	ROOT ****************************************************
	 */
	ip = ialloc ();

   /***	ip->n_magic	= ...; ***/
	ip->n_mode	= (S_IFDIR|PMODE);
	ip->n_uid	= 0;
	ip->n_gid	= 0;
	ip->n_nlink	= 2 + 1;
	ip->n_size	= BL4SZ;
	ip->n_high_size	= 0;
	ip->n_atime	=
	ip->n_mtime	=
	ip->n_ctime	= now_time;
	ip->n_addr[0]	= root_dir_bno = get_empty_block (T1_MAP_DATA);

	/*
	 *	Prepara o "."
	 */
	dp = (T1DIR *)root_dir_image;

	dp->d_ino	= T1_ROOT_INO;
	dp->d_ent_sz	= T1DIR_SIZEOF (1);
	dp->d_nm_len	= 1;
	strcpy (dp->d_name, ".");

	/*
	 *	Prepara o ".."
	 */
	dp = T1DIR_LEN_PTR (dp);

	dp->d_ino	= T1_ROOT_INO;
	dp->d_ent_sz	= T1DIR_SIZEOF (2);
	dp->d_nm_len	= 2;
	strcpy (dp->d_name, "..");

	/*
	 *	Prepara o "lost+found"
	 */
	dp = T1DIR_LEN_PTR (dp);

	dp->d_ino	= T1_ROOT_INO + 1;
	dp->d_ent_sz	= BL4SZ - ((int)dp - (int)root_dir_image);
	dp->d_nm_len	= sizeof ("lost+found") - 1;
	strcpy (dp->d_name, "lost+found");

	/*
	 ******	LOST+FOUND **********************************************
	 */
	ip = ialloc ();

   /***	ip->n_magic	= ...; ***/
	ip->n_mode	= (S_IFDIR|PMODE);
	ip->n_uid	= 0;
	ip->n_gid	= 0;
	ip->n_nlink	= 2;
	ip->n_size	= BL4SZ;
	ip->n_high_size	= 0;
	ip->n_atime	=
	ip->n_mtime	=
	ip->n_ctime	= now_time;
	ip->n_addr[0]	= lost_dir_bno = get_empty_block (T1_MAP_DATA);

	/*
	 *	Prepara o "."
	 */
	dp = (T1DIR *)lost_dir_image;

	dp->d_ino	= T1_ROOT_INO + 1;
	dp->d_ent_sz	= T1DIR_SIZEOF (1);
	dp->d_nm_len	= 1;
	strcpy (dp->d_name, ".");

	/*
	 *	Prepara o ".."
	 */
	dp = T1DIR_LEN_PTR (dp);

	dp->d_ino	= T1_ROOT_INO;
	dp->d_ent_sz	= BL4SZ - ((int)dp - (int)lost_dir_image);
	dp->d_nm_len	= 2;
	strcpy (dp->d_name, "..");

}	/* end make_root_and_lost_dir */
