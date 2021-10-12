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
	int		i_level;	/* Nível de profundidade */
	const char	*i_path_nm;	/* Caminho completo */
	int		i_path_len;	/* Comprimento do Caminho */
	const char	*i_nm;		/* Nome (primeiro nome do caminho) */
	int		i_nm_len;	/* Comprimento do nome */
	STAT		i_s;		/* Estado */
	ITEM		*i_next;	/* Ponteiro para o próximo */
};

ITEM	*item_list,			/* A lista */
	*item_list_last;

#define	NOITEM (ITEM *)NULL

#define	EVER	;;
#define	elif	else if

/*
 ******	Protótipos de funções ***********************************
 */
static int	ftw_expand_list (int, const ITEM *pp, int (*ftn) (const char *, const STAT *, int));

/*
 ****************************************************************
 *	Implementa a busca					*
 ****************************************************************
 */
int
ftw (const char *root, int (*ftn) (const char *, const STAT *, int))
{
	ITEM		*lp;
	int		n, level = 0;

	/*
	 *	Esta função só percorre por largura em pré-ordem
	 */

	/*
	 *	Cria o primeiro elemento da lista
	 */
	if ((lp = malloc (sizeof (ITEM))) == NOITEM)
		error (NOSTR);

	item_list_last = item_list = lp;

	lp->i_level	= level;
	lp->i_nm	= root;
	lp->i_nm_len	= strlen (root);
	lp->i_path_nm	= root;
	lp->i_path_len	= lp->i_nm_len;
   /***	lp->i_s		= ...;	/* abaixo */
	lp->i_next	= NOITEM;

	/*
	 *	Inicia o procedimento com a raiz da árvore
	 */
	if (stat (root, &lp->i_s) < 0)
		return ((*ftn) (root, &lp->i_s, FTW_NS));

	if (!S_ISDIR (lp->i_s.st_mode))
		return ((*ftn) (root, &lp->i_s, FTW_F));

	/*
	 *	x
	 */
	for (EVER)
	{
		for (n = 0, lp = item_list; lp != NOITEM; lp = lp->i_next)
		{
			if (lp->i_level != level)
				continue;

			if (!S_ISDIR (lp->i_s.st_mode))
				continue;

			ftw_expand_list (level + 1, lp, ftn); n += 1;
		}

		if (n <= 0)
			break;

		level += 1;
	}

	return (0);

}	/* end ftw */

/*
 ****************************************************************
 *	Acrescenta os arquivos de um diretório à lista		*
 ****************************************************************
 */
int
ftw_expand_list (int level, const ITEM *pp, int (*ftn) (const char *, const STAT *, int))
{
	ITEM		*lp, *ip, *last_ip, *next_ip;
	DIR		*dir_fd;
	const DIRENT	*dp;
	int		path_len, pp_path_len;
	ITEM		*file_list;
	char		*path_nm;
	const char	*pp_path_nm;

	/*
	 *	Chama a função para o diretório dado
	 */
	(*ftn) (pp->i_path_nm, &pp->i_s, FTW_D);

	/*
	 *	Abre o diretório
	 */
	if ((dir_fd = inopendir (pp->i_s.st_dev, pp->i_s.st_ino)) == NODIR)
	{
		(*ftn) (pp->i_path_nm, &pp->i_s, FTW_DNR);

		return (0);
	}

	/*
	 *	Prepara uma área para o caminho (completo) novo
	 */
	if (pp->i_path_nm[0] == '\0' || pp->i_path_nm[0] == '.' && pp->i_path_nm[1] == '\0')
	{
		pp_path_nm = ""; pp_path_len = 0;
	}
	else
	{
		pp_path_nm = pp->i_path_nm; pp_path_len = pp->i_path_len;

		if (pp_path_nm[pp->i_path_len - 1] == '/')
			pp_path_len--;
	}

	/*
	 *	Le o diretório
	 */
	file_list = item_list_last;

	while ((dp = readdir (dir_fd)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		path_len = pp_path_len + 1 + dp->d_namlen;

		if ((ip = malloc (sizeof (ITEM) + path_len + 1)) == NOITEM)
			error (NOSTR);

		ip->i_path_nm	= path_nm = (char *)ip + sizeof (ITEM);
		ip->i_path_len	= path_len;
		ip->i_nm	= ip->i_path_nm + pp_path_len + 1;
		ip->i_nm_len	= dp->d_namlen;

		strcpy (path_nm, pp_path_nm);
		path_nm[pp_path_len] = '/';
		strcpy (&path_nm[pp_path_len + 1], dp->d_name);

		/*
		 *	Completa os campos de ITEM
		 */
		if (instat (pp->i_s.st_dev, dp->d_ino, &ip->i_s) < 0)
			ip->i_s.st_mode = 0;

		ip->i_level	= level;
	   /***	ip->i_path_nm	= ... ***/	/* Acima */
	   /***	ip->i_path_len	= ... ***/	/* Acima */
	   /***	ip->i_nm 	= ... ***/	/* Acima */
	   /***	ip->i_nm_len	= ... ***/	/* Acima */
	   /***	ip->i_s		= ... ***/	/* Acima */
	   /***	ip->i_next	= ... ***/	/* Abaixo */

		/*
		 *	Insere na lista
		 */
		for (lp = file_list; lp->i_next != NOITEM; lp = lp->i_next)
		{
			if (strttcmp (ip->i_nm, lp->i_next->i_nm, cmpfoldtb, cmpafoldtb) <= 0)
				break;
		}

		if (lp->i_next == NOITEM)
			item_list_last = ip;

		ip->i_next = lp->i_next; lp->i_next = ip;

	}	/* end lendo o diretório */

	closedir (dir_fd);

	/*
	 *	Percorre a lista, processando os arquivos regulares
	 */
	for (last_ip = file_list, ip = last_ip->i_next; ip != NOITEM; /* abaixo */)
	{
		int		code;

		switch (ip->i_s.st_mode & S_IFMT)
		{
		    case 0:
			code = FTW_NS;
			break;

		    case S_IFDIR:
			last_ip = ip; ip = ip->i_next;
			continue;

		    case S_IFLNK:
			code = FTW_SL;
			break;

		    default:
			code = FTW_F;
			break;
		}

		(*ftn) (ip->i_path_nm, &ip->i_s, code);

		/* retira-o logo da lista */

		last_ip->i_next = ip->i_next;

		free (ip);

		ip = last_ip->i_next;

	}	/* Processando arquivos regulares */

	item_list_last = last_ip;

	return (0);

}	/* end ftw_expand_list */
