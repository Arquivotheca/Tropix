/*
 ****************************************************************
 *								*
 *			ftw.c					*
 *								*
 *	Caminha em uma uma árvore				*
 *								*
 *	Versão	1.0.0, de 21.01.87				*
 *		4.2.0, de 25.04.02				*
 *								*
 *	Módulo: ftw						*
 *		libc/etc 					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "ftw.h"

/*
 ******	Item de arquivo (diretório ou não) **********************
 */
typedef struct	item	ITEM;

struct	item
{
	char	*i_nm;			/* Nome (somente um nome) */
	int	i_len;			/* Comprimento do nome */
	STAT	i_s;			/* Estado */
	ITEM	*i_next;		/* Ponteiro para o próximo */
};

#define	NOITEM (ITEM *)NULL

#define	elif	else if

/*
 ******	Protótipos de funções ***********************************
 */
static int	ftw_recursion  (const char *, int, STAT *, int (*ftn) (const char *, STAT *, int));

/*
 ****************************************************************
 *	Implementa a busca					*
 ****************************************************************
 */
int
ftw (const char *root, int (*ftn) (const char *, STAT *, int))
{
	STAT		s;

	/*
	 *	Esta função só percorre por profundidade em pré-ordem
	 */

	/*
	 *	Inicia o procedimento com a raiz da árvore
	 */
	if (stat (root, &s) < 0)
		return ((*ftn) (root, &s, FTW_NS));

	if (!S_ISDIR (s.st_mode))
		return ((*ftn) (root, &s, FTW_F));

	return (ftw_recursion (root, strlen (root), &s, ftn));

}	/* end ftw */

/*
 ****************************************************************
 *	Função recursiva para processar os diretórios		*
 ****************************************************************
 */
static int
ftw_recursion (const char *path, int path_len, STAT *sp, int (*ftn) (const char *, STAT *, int))
{
	ITEM		*lp, *ip;
	DIR		*dir_fd;
	const DIRENT	*dp;
	int		ret, base_len;
	ITEM		file_list;
	char		*nm;
	char		*cp_point;

	/*
	 *	Abre o diretório
	 */
	if ((dir_fd = inopendir (sp->st_dev, sp->st_ino)) == NODIR)
	{
		if ((ret = (*ftn) (path, sp, FTW_DNR)) < 0)
			return (ret);
		else
			return (1);
	}

	/*
	 *	Prepara uma área para o caminho (completo) novo
	 */
	nm = alloca (path_len + MAXNAMLEN + 2);

	if (path[0] == '\0' || path[0] == '.' && path[1] == '\0')
	{
		cp_point = nm;

		base_len = 0;
	}
	else
	{
		strcpy (nm, path); cp_point = nm + path_len;

		if (cp_point[-1] != '/')
			*cp_point++ = '/';

		base_len = cp_point - nm;
	}

	/*
	 *	Le o diretório
	 */
	file_list.i_next = NOITEM;

	ip = alloca (sizeof (ITEM));

	while ((dp = readdir (dir_fd)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		ip->i_nm = alloca (dp->d_namlen + 1);

		strcpy (ip->i_nm, dp->d_name);
		ip->i_len = dp->d_namlen;

		/*
		 *	Completa os campos de ITEM
		 */
		if (instat (sp->st_dev, dp->d_ino, &ip->i_s) < 0)
			ip->i_s.st_mode = 0;

	   /***	ip->i_nm   = ... ***/	/* Acima */
	   /***	ip->i_s    = ... ***/	/* Acima */
	   /***	ip->i_next = ... ***/	/* Abaixo  */

		/*
		 *	Insere na lista
		 */
		for (lp = &file_list; lp->i_next != NOITEM; lp = lp->i_next)
		{
#define	FOLD
#ifdef	FOLD
			if (strttcmp (ip->i_nm, lp->i_next->i_nm, cmpfoldtb, cmpafoldtb) <= 0)
#else
			if (strcmp (ip->i_nm, lp->i_next->i_nm) <= 0)
#endif	FOLD
				break;
		}

		ip->i_next = lp->i_next;
		lp->i_next = ip;

		ip = alloca (sizeof (ITEM));	/* Já aloca um seguinte */

	}	/* end lendo o diretório */

	closedir (dir_fd);

	/*
	 *	Chama a função para o diretório
	 */
	if ((ret = (*ftn) (path, sp, FTW_D)) != 0)
		return (ret);

	/*
	 *	Percorre a lista, processando os arquivos regulares
	 */
	for (ip = file_list.i_next; ip != NOITEM; ip = ip->i_next)
	{
		int		code;

		switch (ip->i_s.st_mode & S_IFMT)
		{
		    case 0:
			code = FTW_NS;
			break;

		    case S_IFDIR:
			continue;

		    case S_IFLNK:
			code = FTW_SL;
			break;

		    default:
			code = FTW_F;
			break;
		}

		strcpy (cp_point, ip->i_nm);

		if ((ret = (*ftn) (nm, &ip->i_s, code)) < 0)
			return (ret);

	}	/* Processando arquivos regulares */

	/*
	 *	Percorre a lista, processando os diretórios
	 */
	for (ip = file_list.i_next; ip != NOITEM; ip = ip->i_next)
	{
		if (S_ISDIR (ip->i_s.st_mode))
		{
			strcpy (cp_point, ip->i_nm);

			if ((ret = ftw_recursion (nm, base_len + ip->i_len, &ip->i_s, ftn)) < 0)
				return (ret);
		}

	}	/* Processando diretórios */

	return (0);

}	/* end ftw_recursion */
