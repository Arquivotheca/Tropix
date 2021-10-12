/*
 ****************************************************************
 *								*
 *			rmtree.c				*
 *								*
 *	Remove árvores						*
 *								*
 *	Versão	1.0.0, de 02.09.86				*
 *		4.2.0, de 26.04.02				*
 *								*
 *	Módulo: rmtree						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 * 								*
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
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 26.04.02";

#define		NPAT	20	/* numero maximo de padroes */

#define		EVER	;;
#define		elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exitcode = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	fflag;		/* Force */
entry int	dflag;		/* Debug */

entry STAT	c_dir_s;	/* Estado do diretório corrente */

entry int	first = 1;	/* Para controlar o branco inicial */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Os padrões **********************************************
 */
entry const char *inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int	inc_pati;

entry const char *exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int	exc_pati;

/*
 ******	Bloco de informações de cada arquivo ********************
 */
typedef struct	info	INFO;

struct	info
{
	char	*i_path;	/* Nome do arquivo ou diretório */
	int	i_len;		/* Comprimento do nome */
	STAT	i_s;		/* Estado do arquivo */
	INFO	*i_next;	/* Próximo elemento da lista */
};

#define	NOINFO	(INFO *)NULL

/*
 ****** Protótipos de funções ***********************************
 */
int		cmp_analysis (const char *, STAT *, int);
void		put_s_option (void);
int		put_p_option (const char *, const char *[], int *);
int		dir_process (const char *, int, const STAT *);
int 		dir_read (const char *, INFO *);
void		list_insert (INFO *, INFO *);
char		*compose_name (const char *, const char *);
const char	*last_id (const char *);
int		file_rm (const char *, STAT *);
int		dir_rm (const char *, const STAT *);
char 		*parentnm (const char *);
int		pattern_accept (const char *);
void		help (void);

/*
 ****************************************************************
 *	Remove árvores						*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*file_nm;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vfsp:P:dH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'f':			/* Force */
			fflag++;
			break;

		    case 's':			/* Padrões =	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':			/* Padrões de inclusão */
			put_p_option (optarg, inc_pat, &inc_pati);
			break;

		    case 'P':			/* Padrões de exclusão */
			put_p_option (optarg, exc_pat, &exc_pati);
			break;

		    case 'd':			/* Debug */
			dflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argc -= optind;
	argv += optind;

	if (argc < 1)
		help ();

	/*
	 *	Prepara o terminal
	 */
	if (!fflag)
	{
		if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
			error ("$*Não consegui abrir \"/dev/tty\"");

		askyesno_set (1);
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Obtém o estado do diretório corrente
	 */
	if (stat (".", &c_dir_s) < 0)
		error ("$*Não consegui obter o estado de \".\"");

	/*
	 *	Processa os argumentos
	 */
	while ((file_nm = *argv++) != NOSTR)
	{
		if ((!fflag  ||  vflag)  &&  !first)
			fputc ('\n', stdout);

		/*
		 *	Obtém o estado do arquivo
		 */
		if (stat (file_nm, &s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", file_nm);
			first = 0;
			exitcode++;
			continue;
		}

		/*
		 *	Verifica se é um diretório
		 */
		if (!S_ISDIR (s.st_mode))
		{
			error ("\"%s\" não é um diretório", file_nm);
			first = 0;
			exitcode++;
			continue;
		}

		/*
		 *	Percorre a arvore
		 */
		dir_process (file_nm, strlen (file_nm), &s);
	}

	return (exitcode);

}	/* end rmtree */

/*
 ****************************************************************
 *	Percorre a arvore					*
 ****************************************************************
 */
int
dir_process (const char *dir_path, int dir_len, const STAT *sp)
{
	int		files_not_removed = 0;
	DIR		*dir_fd;
	const DIRENT	*dp;
	INFO		dir_list, *lp, *ip;

	/*
	 *	Pula uma linha, se for o caso
	 */
	if (vflag || !fflag)
	{
		if (!first)
			fputc ('\n', stdout);
		else
			first = 0;
	}

	/*
	 *	Verifica o modo de funcionamento
	 */
	if   (!fflag)
	{
		fprintf
		(	stderr,
			"Diretório \"%s%s%s\", entra? (n): ",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);

		if (askyesno () <= 0)
			return (1);
	}
	elif (vflag)
	{
		fprintf
		(	stdout,
			"Diretório \"%s%s%s\":\n",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);
	}

	/*
	 *	Abre o diretório
	 */
	if ((dir_fd = inopendir (sp->st_dev, sp->st_ino)) == NODIR)
	{
		error ("*Não consegui abrir o diretório \"%s\"", dir_path);
		exitcode++;
		return (1);
	}

	/*
	 *	Lê o conteúdo do diretório
	 */
	dir_list.i_next = NOINFO;

	ip = alloca (sizeof (INFO));

	while ((dp = readdir (dir_fd)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		/*
		 *	Obtém o nome completo
		 */
		ip->i_len = dir_len + dp->d_namlen + 1;

		ip->i_path = alloca (ip->i_len + 1);

		if (streq (dir_path, "."))
		{
			strcpy (ip->i_path, dp->d_name);
			ip->i_len = dp->d_namlen;
		}
		else
		{
			strcpy (ip->i_path, dir_path);
			strcat (ip->i_path, "/");
			strcat (ip->i_path, dp->d_name);
		}

		/*
		 *	Obtém o estado do arquivo
		 */
		if (instat (sp->st_dev, dp->d_ino, &ip->i_s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", dp->d_name);
			files_not_removed++;
			exitcode++;
			continue;
		}

		/*
		 *	Verifica se é um diretório ou não
		 */
		if (!S_ISDIR (ip->i_s.st_mode))
		{
			if   (pattern_accept (ip->i_path) == 0)
				files_not_removed++;
			elif (file_rm (ip->i_path, &ip->i_s) < 0)
				files_not_removed++;

			continue;
		}

		/*
		 *	É um diretório: Insere na lista
		 */
		for (lp = &dir_list; lp->i_next != NOINFO; lp = lp->i_next)
		{
#define	FOLD
#ifdef	FOLD
			if (strttcmp (ip->i_path, lp->i_next->i_path, cmpfoldtb, cmpafoldtb) <= 0)
#else
			if (strcmp (ip->i_path, lp->i_next->i_path) <= 0)
#endif	FOLD
				break;
		}

		ip->i_next = lp->i_next;
		lp->i_next = ip;

		ip = alloca (sizeof (INFO));	/* Já aloca um seguinte */

	}	/* end lendo o diretório */

	closedir (dir_fd);

	/*
	 *	Agora, processa a lista removendo os diretórios
	 */
	for (ip = dir_list.i_next; ip != NOINFO; ip = ip->i_next)
	{
		files_not_removed += dir_process (ip->i_path, ip->i_len, &ip->i_s);
	}

	/*
	 *	Remove o diretório (se ficou vazio)
	 */
	if (files_not_removed == 0)
	{
		if (dir_rm (dir_path, sp) < 0)
			files_not_removed++;
	}

	return (files_not_removed);

}	/* end dir_process */

/*
 ****************************************************************
 *	Obtém o último componente de um nome			*
 ****************************************************************
 */
const char *
last_id (const char *pathname)
{
	char		*cp;

	if (cp = strrchr (pathname, '/'))
		return (cp + 1);
	else
		return (pathname);

} 	/* end last_id */

/*
 ****************************************************************
 *	Remove arquivos						*
 ****************************************************************
 */
int
file_rm (const char *file_nm, STAT *sp)
{
	/*
	 *	Verifica o modo de funcionamento
	 */
	if   (!fflag)
	{
		fprintf
		(	stderr,
			"    %s%s%s? (n): ",
			color_vector[(sp->st_mode & S_IFMT) >> 12],
			last_id (file_nm),
			color_vector[MC_DEF]
		);

		if (askyesno () <= 0)
			return (-1);
	}
	elif (vflag)
	{	
		fprintf
		(	stdout,
			"    %s%s%s:\n",
			color_vector[(sp->st_mode & S_IFMT) >> 12],
			last_id (file_nm),
			color_vector[MC_DEF]
		);
	}

	/*
	 *	Verifica se pode escrever no arquivo
	 */
	if (!fflag && !S_ISLNK (sp->st_mode) && access (file_nm, W_OK) < 0)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" não tem permissão de escrita. Apaga? (n): ",
			pgname, file_nm
		);

		if (askyesno () <= 0)
			return (-1);
	}

	/*
	 *	Remove o arquivo
	 */
	if (dflag)
	{
		printf ("	Ia remover o arquivo \"%s\"\n", file_nm);
		return (0);
	}

	if (unlink (file_nm) < 0)
	{
		error ("*Não consegui remover \"%s\"", file_nm);
		exitcode++;
		return (-1);
	}

	/*
	 *	Verifica se ainda há links para o arquivo
	 */
	if (sp->st_nlink-- > 1  &&  (vflag || !fflag || inc_pati > 0 || exc_pati > 0))
		error ("\t***** Ainda há %d elo(s) físico(s) para \"%s\"", sp->st_nlink, last_id (file_nm));

	return (0);

}	/* end file_rm */

/*
 ****************************************************************
 *	Remove um diretório					*
 ****************************************************************
 */
int
dir_rm (const char *dir_path, const STAT *s)
{
	/*
	 *	Não permite remoção do diretório corrente
	 */
	if (c_dir_s.st_dev == s->st_dev  &&  c_dir_s.st_ino == s->st_ino)
	{
		error ("Não posso remover o diretório corrente \"%s\"", dir_path);
		exitcode++;
		return (-1);
	}

	/*
	 *	Verifica o modo de funcionamento
	 */
	if   (!fflag)
	{
		fprintf
		(	stderr,
			"\nDiretório \"%s%s%s\", apaga? (n): ",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);

		if (askyesno () <= 0)
			return (-1);
	}
	elif (vflag)
	{	
		fprintf
		(	stdout,
			"\nRemovendo o diretório \"%s%s%s\"\n",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);
	}

	/*
	 *	remove o diretório
	 */
	if (rmdir (dir_path) < 0)
	{
		error ("*Não consegui remover o diretório \"%s\"", dir_path);
		exitcode++;
		return (-1);
	}

	return (0);

}	/* end dir_rm */

/*
 ****************************************************************
 *	Executa a opção "-s" (padrões de "C")			*
 ****************************************************************
 */
void
put_s_option (void)
{
	put_p_option ("*.[cshryv]", inc_pat, &inc_pati);
	put_p_option ("*[Mm]akefile", inc_pat, &inc_pati);

}	/* end put_s_option */

/*
 ****************************************************************
 *	Executa a opção "-p" (outros padrões)			*
 ****************************************************************
 */
int
put_p_option (const char *cp, const char *pat_vec[], int *pati_ptr)
{
	char		c;

	for (EVER)
	{
		while ((c = *cp) == ' ' || c == '\t' )
			cp++;

		if (c == '\0')
			return (0);

		if (*pati_ptr >= NPAT)
		{
			printf
			(	"%s: No momento somente aceitamos "
				XSTR (NPAT) " padrões\n\n",
				pgname
			);
			help ();
		}

		pat_vec[(*pati_ptr)++] = cp++;

		while ((c = *cp) != '\0' && c != ' ' && c != '\t')
			cp++;

		if (c == '\0')
			return (0);

		*(char *)cp++ = '\0';

	}	/* end for (EVER) */

}	/* end put_p_option */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	/*
	 *	Finalmente testa os padrões de exclusão
	 */
	if ((pp = exc_pat)[0] != NOSTR)
	{
		for (/* vazio */; *pp != NOSTR; pp++)
		{
			if (patmatch (file_nm, *pp))
				return (0);
		}
	}

	return (1);

}	/* end pattern_accept */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - remove árvores\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vfs] [-p \"<p>\"] [-P \"<P>\"] <árvore> ...\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Lista os arquivos/diretórios sendo removidos\n"
		"\t-f: Remove os arquivos/diretórios sem pedir confirmação "
			"do usuário\n"
		"\t-s: Adiciona os <padrões> dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	exit (2);

}	/* end help */
