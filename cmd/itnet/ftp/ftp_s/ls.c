/*
 ****************************************************************
 *								*
 *			ls.c					*
 *								*
 *	File transfer protocol (servidor)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		3.0.4, de 23.08.97				*
 *								*
 *	Módulo: FTP_S						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
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
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <xti.h>

#include "ftp_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	IN_CALLs		/* Usa as chamadas especiais */

#define	BYTOBL(b)	(((b) + 511) >> 9)

#define	elif	else if

entry char	ANO[4];		/* Ano corrente */

FILE		*data_fp;

/*
 ******	Item de arquivo (diretório ou não) **********************
 */
typedef struct	item	ITEM;

struct	item
{
	const char	*i_nm;		/* Nome (somente um nome) */
	STAT		i_s;		/* Estado */
	ITEM		*i_next;	/* Ponteiro para o próximo */
};

#define	NO_ITEM (ITEM *)NULL

/*
 ******	Protótipo de funções ************************************
 */
int		dir_proc (const char *, STAT *);
void 		show_file (const char *, const STAT *);
void 		put_time (time_t);
void 		put_uid (int);
void 		put_gid (int);
const char	*compose_name (const char *, const char *);
const char	*last_id (const char *);
void		help (void);

/*
 ****************************************************************
 * Lista características de arquivos e conteúdos de diretórios	*
 ****************************************************************
 */
int
proc_ls (const char *file_nm, FILE *fp)
{
	time_t		difuso;
	char		*tp;
	time_t		HOJE;
	STAT		s;

	data_fp = fp;

	/*
	 *	Prepara as datas para o cálculo do ano corrente
	 */
	time (&HOJE);				/* Agora GMT */

	difuso = gettzmin () * 60;

	HOJE += difuso;				/* Agora local */

	tp = ctime (&HOJE);

	strncpy (ANO, tp+20, 4);

	/*
	 *	Obtém o estado do arquivo
	 */
	if (file_nm == NOSTR)
		file_nm = ".";

	if (stat (file_nm, &s) < 0)
	{
		fprintf
		(	fp,
			"%s: Não consegui obter o estado de \"%s\" (%s)\r\n",
			pgname, file_nm, strerror (errno)
		);
		return (-1);
	}

	if (!S_ISDIR (s.st_mode))
		{ show_file (file_nm, &s); return (0); }

	return (dir_proc (file_nm, &s));

}	/* end ls */

/*
 ****************************************************************
 *	Função recursiva para processar os diretórios		*
 ****************************************************************
 */
int
dir_proc (const char *path, STAT *sp)
{
	FILE		*fp = data_fp;
	ITEM		*ip, *lp;
	DIR		*dir_fp;
	DIRENT		*dp;
	ITEM		file_list;

	/*
	 *	Abre o diretório
	 */
	if ((dir_fp = inopendir (sp->st_dev, sp->st_ino)) == NODIR)
	{
		fprintf
		(	fp,
			"%s: Não consegui ler o diretório \"%s\" (%s)\r\n",
			pgname, path, strerror (errno)
		);
		return (-1);
	}

	/*
	 *	Le o diretório
	 */
	file_list.i_next = NO_ITEM;

	ip = alloca (sizeof (ITEM));

	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		ip->i_nm = compose_name (path, dp->d_name);

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
		for (lp = &file_list; lp->i_next != NO_ITEM; lp = lp->i_next)
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

	closedir (dir_fp);

	/*
	 *	Percorre a lista
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
		show_file (ip->i_nm, &ip->i_s);

	}	/* Processando os arquivos */

	return (0);

}	/* end ftw_recursion */

/*
 ****************************************************************
 *	Compõe o nome de um arquivo de um diretório		*
 ****************************************************************
 */
const char *
compose_name (const char *dirnm, const char *filenm)
{
	char		*cp;
	int		sz;

	sz = strlen (dirnm) + strlen (filenm) + 2;

	if ((cp = malloc (sz)) == NOSTR)
		error (NOSTR);

	strcpy (cp, dirnm);

	if (*(strend (cp) - 1) != '/')
		strcat (cp, "/");

	strcat (cp, filenm);

	if (strncmp (cp, "./", 2))
		return (cp);
	else
		return (cp + 2);

}	/* end compose_name */

/*
 ****************************************************************
 *	Lista as informações de um ARQUIVO			*
 ****************************************************************
 */
void
show_file (const char *file_nm, const STAT *sp)
{
	FILE		*fp = data_fp;
	char		*modestr;

	modestr = modetostr (sp->st_mode);

	if (S_ISDIR (sp->st_mode))
		fprintf (fp, "d");
	else
		fprintf (fp, "-");

	fprintf (fp, "%s", modestr + 14);

	fprintf (fp, "%3d ", sp->st_nlink);

	put_uid (sp->st_uid);
	put_gid (sp->st_gid);

	if (S_ISBLK (sp->st_mode) || S_ISCHR (sp->st_mode))
	{
		fprintf
		(fp, 	"%3d/%3d  ",
			MAJOR (sp->st_rdev),
			MINOR (sp->st_rdev)
		);
	}
	else
	{
		fprintf (fp, "%8d ", sp->st_size);
	}

	put_time (sp->st_mtime);

	fprintf (fp, "%s\r\n", file_nm);

}	/* end show_file */

/*
 ****************************************************************
 *	Imprime uma DATA					*
 ****************************************************************
 */
void
put_time (time_t timeval)
{
	FILE		*fp = data_fp;
	char		*tp;

	tp = ctime (&timeval);

	fprintf (fp, "%-6.6s ", tp+4);		/* Mes e dia */

	if (strncmp (tp+20, ANO, 4))
		fprintf (fp, " %4.4s ",  tp+20);	/* Ano */
	else
		fprintf (fp, "%5.5s ",  tp+11);	/* Hora */

}	/* end put_time */

/*
 ****************************************************************
 *	Imprime um UID						*
 ****************************************************************
 */
void
put_uid (int uid)
{
	FILE		*fp = data_fp;
	const char	*id;

	if ((id = pwcache (uid)) == NOSTR)
		fprintf (fp, "%-8d ", uid);
	else
		fprintf (fp, "%-8.8s ", id);

}	/* end put_uid */

/*
 ****************************************************************
 *	Imprime um GID						*
 ****************************************************************
 */
void
put_gid (int gid)
{
	FILE		*fp = data_fp;
	const char	*id;

	if ((id = grcache (gid)) == NOSTR)
		fprintf (fp, "%-8d ", gid);
	else
		fprintf (fp, "%-8.8s ", id);

}	/* end put_gid */
