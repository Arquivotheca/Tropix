/*
 ****************************************************************
 *								*
 *			dos_ftw.c				*
 *								*
 *	Caminha em uma uma árvore DOS				*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.2, de 02.06.96				*
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
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Item de arquivo (diretório ou não) **********************
 */
#define	NO_ITEM (ITEM *)NULL

typedef struct	item	ITEM;

struct	item
{
#define	i_nm	i_z.z_lfn_nm	/* Nome (somente um nome) */
	char	*i_path;	/* Caminho (nem sempre disponível) */
	DOSSTAT	i_z;		/* Estado (Diretório) */
	ITEM	*i_next;	/* Ponteiro para o próximo */
};

/*
 ******	Protótipos de funções ***********************************
 */
int	dos_ftw_recursion  (const char *, DOSSTAT *, int (*ftn) (const char *, DOSSTAT *, int));

/*
 ****************************************************************
 *	Implementa a busca					*
 ****************************************************************
 */
int
dos_ftw (const char *root, int (*ftn) (const char *, DOSSTAT *, int))
{
	DOSSTAT		z;

	/*
	 *	Inicia o procedimento com a raiz da árvore
	 */
	if (dos_stat (root, &z) < 0)
		return ((*ftn) (root, &z, DOS_FTW_NS));

	if (!Z_ISDIR (z.z_mode))
		return ((*ftn) (root, &z, DOS_FTW_F));

	return (dos_ftw_recursion (root, &z, ftn));

}	/* end dos_ftw */

/*
 ****************************************************************
 *	Função recursiva para processar os diretórios		*
 ****************************************************************
 */
int
dos_ftw_recursion (const char *path, DOSSTAT *zp, int (*ftn) (const char *, DOSSTAT *, int))
{
	char		*nm;
	ITEM		*ip, *lp;
	int		ret, dir_clusno;
	int		path_len = strlen (path);
	int		len, max_len = 0;
	char		*cp_point;
	ITEM		file_list;
	DOSFILE		f;

	/*
	 *	Verifica se veio interrupção
	 */
	if (received_intr)
		return (-1);

	/*
	 *	Chama a função para o diretório
	 */
	if ((ret = (*ftn) (path, zp, DOS_FTW_D)) != 0)
		return (ret);

	/*
	 *	Lê o diretório
	 */
	dir_clusno = zp->z_cluster;

	if (dos_open_by_clus (&f, dir_clusno, BIG_SZ) < 0)
		return (1);

	file_list.i_next = NO_ITEM;

	ip = alloca (sizeof (ITEM));

	dos_lfn_init (&ip->i_z); ip->i_z.z_lfn_entries = 0;

	while (dos_read (&f, &ip->i_z.z_d, sizeof (DOSDIR)) > 0)
	{
		if (ip->i_z.z_name[0] == '\0')
			break;

		if (ip->i_z.z_name[0] == Z_EMPTY)
		{
			dos_lfn_reset (&ip->i_z);
			ip->i_z.z_lfn_entries = 0;
			continue;
		}

		if (ip->i_z.z_lfn_entries++ == 0)
		{
			ip->i_z.z_lfn_clusno    = f.f_clusno;
			ip->i_z.z_lfn_blkno     = f.f_blkno;
			ip->i_z.z_lfn_end_blkno = f.f_end_blkno;
			ip->i_z.z_lfn_offset    = f.f_ptr - f.f_area - sizeof (DOSDIR);
		}

		if (Z_ISLFN (ip->i_z.z_mode))
			{ dos_lfn_add (&ip->i_z); continue; }

		dos_lfn_get_nm (&ip->i_z);

		if (streq (ip->i_z.z_lfn_nm, ".") || streq (ip->i_z.z_lfn_nm, ".."))
		{
			dos_lfn_reset (&ip->i_z);
			ip->i_z.z_lfn_entries = 0;
			continue;
		}

		/*
		 *	Prepara o item.	Calcula o "offset"
		 *	(particular para DIRs)
		 */
		GET_CLUSTER (&ip->i_z);
	   	ip->i_z.z_blkno   = f.f_blkno;
	   	ip->i_z.z_offset  = f.f_ptr - f.f_area - sizeof (DOSDIR);

	   /***	strcpy (ip->i_nm, ip->i_z.z_lfn_nm); ***/
	   /***	ip->i_path = ... ***/	/* Mais tarde */
	   /***	ip->i_z    = ... ***/	/* Já foi feito */
	   /***	ip->i_next = ...	/* Abaixo  */

		/*
		 *	Insere na lista
		 */
		for (lp = &file_list; lp->i_next != NO_ITEM; lp = lp->i_next)
		{
			if (strcmp (ip->i_z.z_lfn_nm, lp->i_next->i_nm) <= 0)
				break;
		}

		len = strlen (ip->i_z.z_lfn_nm);

		if (len > max_len)
			max_len = len;

		ip->i_next = lp->i_next;
		lp->i_next = ip;

		ip = alloca (sizeof (ITEM));	/* Já aloca um seguinte */

		dos_lfn_init (&ip->i_z); ip->i_z.z_lfn_entries = 0;

	}	/* end lendo o diretório */

	/*
	 *	Prepara uma área para o caminho (completo) novo
	 */
	nm = alloca (path_len + max_len + 2);

	if (path[0] == '\0' || path[0] == '.' && path[1] == '\0')
	{
		cp_point = nm;
	}
	else
	{
		strcpy (nm, path); cp_point = nm + path_len;

		if (cp_point[-1] != '/')
			*cp_point++ = '/';
	}

	/*
	 *	Percorre a lista, processando os arquivos regulares
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
		if (received_intr)
			return (-1);

		ip->i_path = nm;	/* Armazena o caminho completo */

		if (!Z_ISDIR (ip->i_z.z_mode))
		{
			strcpy (cp_point, ip->i_nm);

			if ((*ftn) (nm, &ip->i_z, DOS_FTW_F) < 0)
				return (-1);
			else
				continue;
		}

	}	/* Processando arquivos regulares */

	/*
	 *	Percorre a lista, processando os diretórios
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
		if (Z_ISDIR (ip->i_z.z_mode))
		{
			strcpy (cp_point, ip->i_nm);

			if ((ret = dos_ftw_recursion (ip->i_path, &ip->i_z, ftn)) < 0)
				return (ret);
		}

	}	/* Processando arquivos regulares */

	return (0);

}	/* end dos_ftw_recursion */
