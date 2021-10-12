/*
 ****************************************************************
 *								*
 *			lc.c					*
 *								*
 *	Lista o conteúdo de diretórios				*
 *								*
 *	Versão	1.0.0, de 12.01.87				*
 *		4.5.0, de 18.10.03				*
 *								*
 *	Módulo: lc						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
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
#include <termio.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.5.0, de 18.10.03";

#define	BYTOBL(b)	(((b) + 511) >> 9)

#define	elif	else if

entry int	exitcode = 0;	/* Código de retorno do programa */

entry int	many_files = 0;	/* Foram dados nomes de vários arquivos */

const char	**color_vector;	/* Para as cores dos modos */

const char	dev_tty_nm[] = "/dev/tty";

/*
 ******	Indicadores *********************************************
 */
entry int	rflag;		/* Arquivos regulares */
entry int	dflag;		/* Diretórios */
entry int	bflag;		/* Especial de blocos */
entry int	cflag;		/* Especial de caracteres */
entry int	lflag;		/* Elos simbólicos */
entry int	fflag;		/* Fifos */

entry int	tflag;		/* time */
entry int	aflag;		/* "." e ".." */
entry int	Rflag;		/* Subdiretórios */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ******	Disposição da linha *************************************
 */
#define FILES_per_LINE	4
#define	FILE_FMT_LEN	18

entry int	files_per_line;	/* Número de arquivos por linha */

entry int	file_fmt_len;	/* Número de colunas por arquivos */

/*
 ******	Bloco de informações de cada arquivo ********************
 */
typedef struct	info	INFO;

struct	info
{
	const char	*i_nm;	/* Nome de arquivo ou diretório */
	STAT		i_s;	/* Estado do arquivo */
	INFO		*i_next; /* Próximo elemento da lista */
};

#define	NOINFO	(INFO *)NULL

entry INFO	filelist,	/* Cabeça da lista de arquivos */
		dirlist;	/* Cabeça da lista de diretórios */

entry INFO	*dirptr = NOINFO; /* Ponteiro para o diretório sendo listado */

/*
 ******	Protótipo de funções ************************************
 */
void		help (void);
void 		proc_file (const char *);
void 		list_insert (INFO *, INFO *);
void 		show_list (INFO *, int);
void		print_nm (const char *nm, int len);
const char	*compose_name (const char *, const char *);
int 		dir_read (INFO *);
const char	*last_id (const char *);
INFO		*infodup (INFO *);

/*
 ****************************************************************
 *	Lista o conteúdos de diretórios				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd;
	int		write_dir_nm = 0;
	const char	*str, *field;
	char		*cp;
	TERMIO		termio;

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "rdbclftaRNMH")) != EOF)
	{
		switch (opt)
		{
		    case 'r':			/* Regular */
			rflag++;
			break;

		    case 'd':			/* Diretório */
			dflag++;
			break;

		    case 'b':			/* Especial de blocos */
			bflag++;
			break;

		    case 'c':			/* Especial de caracteres */
			cflag++;
			break;

		    case 'l':			/* Elos simbólicos */
			lflag++;
			break;

		    case 'f':			/* Fifo */
			fflag++;
			break;

		    case 't':			/* time */
			tflag++;
			break;

		    case 'a':			/* "." e ".." */
			aflag++;
			break;

		    case 'R':			/* Subdiretórios */
			Rflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'M':			/* Sticky */
			exit (0);

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Pequena consistência
	 */
	if (rflag + dflag + bflag + cflag + lflag + fflag == 0)
		{ rflag++; dflag++; bflag++; cflag++; lflag++; fflag++; }

	if (aflag + Rflag > 1)
		help ();

	/*
	 *	Tenta obter o número de colunas
	 */
	termio.t_ncol = 0;

	if ((fd = open (dev_tty_nm, O_RDONLY)) >= 0)
		{ ioctl (fd, TCGETS, &termio); close (fd); }

	/*
	 *	A disposição da linha também pode ser dada no ambiente
	 */
	cp = "";

	if ((str = getenv ("LC")) != NOSTR)
	{
		if ((cp = strdup (str)) == NOSTR)
			cp = "";
	}

	for (field = strfield (cp, ','); field != NOSTR; field = strfield (NOSTR, ','))
	{
		if   (strncmp (field, "len=", 4) == 0)
		{
			file_fmt_len = strtol (field + 4, &str, 0);
		}
		elif (strncmp (field, "files=", 6) == 0)
		{
			files_per_line = strtol (field + 6, &str, 0);
		}
	}

	if (file_fmt_len <= 0)
		file_fmt_len = FILE_FMT_LEN;

	if (files_per_line <= 0)
	{
		if (termio.t_ncol >= 0)
			files_per_line = (termio.t_ncol - 2) / (file_fmt_len + 1);
		else
			files_per_line = FILES_per_LINE;
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
		error ("Os argumentos supérfluos serão ignorados");

	/*
	 *	Procura os argumentos
	 */
	if   (Nflag)				/* Lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) == area)
			proc_file (area);
	}
	elif (*argv == NOSTR)
	{
		/*
		 *	Não foram dados argumentos nem "-N": considera "."
		 */
		proc_file (".");
	}
	else
	{
		/* Foram dados argumentos */

		if (argc > 1)
			many_files++;

		for (/* vazio */; *argv; argv++)
			proc_file (*argv);
	}

#undef	DEBUG
#ifdef	DEBUG
{	INFO		*ip;

	for (ip = filelist.i_next; ip; ip = ip->i_next)
		printf ("%s\n", ip->i_nm); 	

	printf ("\n"); 	

	for (ip = dirlist.i_next; ip; ip = ip->i_next)
		printf ("%s\n", ip->i_nm); 	
}
#endif	DEBUG

	/*
	 *	Lista os arquivos
	 */
	if (argc > 1 || Nflag || Rflag)
		write_dir_nm = 1;

	/*
	 *	Lista os diretórios; repare que "dir_read" estende "dirlist"
	 */
	for (dirptr = dirlist.i_next; dirptr != NOINFO; dirptr = dirptr->i_next)
	{
		if (write_dir_nm)
		{
			if (write_dir_nm++ > 1)
				printf ("\n");

			printf ("\"%s\":\n", dirptr->i_nm);
		}

		dir_read (dirptr);

		show_list (&filelist, write_dir_nm);

	}	/* end for */

	return (exitcode);

}	/* end lc */

/*
 ****************************************************************
 *	Processa um nome					*
 ****************************************************************
 */
void
proc_file (const char *filenm)
{
	INFO		*ip;

	if ((ip = malloc (sizeof (INFO))) == NOINFO)
		error (NOSTR);

	if (stat (filenm, &ip->i_s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", filenm);
		free (ip);
		exitcode++;
		return;
	}

	ip->i_nm = filenm;

	if (S_ISDIR (ip->i_s.st_mode))
	{
		list_insert (&dirlist, ip);
	}
	else
	{
		if (many_files == 0)
			error ("\"%s\" não é um diretório", filenm);

		free (ip);
		exitcode++;
		return;
	}

}	/* end proc_file */

/*
 ****************************************************************
 *	Insere no ponto correto da lista			*
 ****************************************************************
 */
void
list_insert (INFO *list, INFO *member)
{
	INFO		*ip;
	time_t		Time;
	const char	*nm;

	if   (list == &dirlist)
	{
		/*
		 *	Se for um diretório, insere alfabeticamente,
		 *	mas depois do diretório atual
		 */
		if ((ip = dirptr) == NOINFO)
			ip = list;

		nm = member->i_nm;

		for (/* vazio */; ip->i_next != NOINFO; ip = ip->i_next)
		{
#define	FOLD
#ifdef	FOLD
			if (strttcmp (nm, ip->i_next->i_nm, cmpfoldtb, cmpafoldtb) <= 0)
#else
			if (strcmp (nm, ip->i_next->i_nm) <= 0)
#endif	FOLD
				break;
		}
	}
	elif (tflag)
	{
		/*
		 *	Insere pelo tempo de modificação
		 */
		Time = member->i_s.st_mtime;

		for (ip = list; ip->i_next != NOINFO; ip = ip->i_next)
		{
			if (Time >= ip->i_next->i_s.st_mtime)
				break;
		}
	}
	else
	{
		/*
		 *	Insere alfabeticamente
		 */
		nm = member->i_nm;
	
		for (ip = list; ip->i_next != NOINFO; ip = ip->i_next)
		{
#ifdef	FOLD
			if (strttcmp (nm, ip->i_next->i_nm, cmpfoldtb, cmpafoldtb) <= 0)
#else
			if (strcmp (nm, ip->i_next->i_nm) <= 0)
#endif	FOLD
				break;
		}
	}

	member->i_next = ip->i_next;
	ip->i_next = member;

}	/* end list_insert */

/*
 ****************************************************************
 *	Lista as informações de uma LISTA			*
 ****************************************************************
 */
void
show_list (INFO *list, int write_dir_nm)
{
	INFO		*ip, *nextip;
	int		i = 0, j = 0;
	char		*cp;

	if (write_dir_nm)
		cp = "  ";
	else
		cp = "";

#if (0)	/*******************************************************/
	files_per_line = 5;
	file_fmt_len = 14;
#endif	/*******************************************************/

	/*
	 *	Diretórios
	 */
	if (dflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISDIR (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				printf
				(	"%sDiretórios:%s",
					cp,
					color_vector[S_IFDIR >> 12]
				);
			}

			printf ("\n%s ", cp);
		}

		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	if (i > 0)
		{ printf ("\n"); j++; i = 0; }

	/*
	 *	Arquivos regulares
	 */
	if (rflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISREG (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				if (j)
					printf ("\n");

				printf
				(	"%s%sArquivos regulares:%s",
					cp,
					color_vector[MC_DEF],
					color_vector[S_IFREG >> 12]
				);
			}

			printf ("\n%s ", cp);
		}
	
		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	if (i > 0)
		{ printf ("\n"); j++; i = 0; }

	/*
	 *	Elos Simbólicos
	 */
	if (lflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISLNK (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				if (j)
					printf ("\n");

				printf
				(	"%s%sElos simbólicos:%s",
					cp,
					color_vector[MC_DEF],
					color_vector[S_IFLNK >> 12]
				);
			}

			printf ("\n%s ", cp);
		}
	
		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	if (i > 0)
		{ printf ("\n"); j++; i = 0; }

	/*
	 *	Arquivos especiais de bloco
	 */
	if (bflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISBLK (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				if (j)
					printf ("\n");

				printf
				(	"%s%sEspeciais de blocos:%s",
					cp,
					color_vector[MC_DEF],
					color_vector[S_IFBLK >> 12]
				);
			}

			printf ("\n%s ", cp);
		}
	
		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	if (i > 0)
		{ printf ("\n"); j++; i = 0; }

	/*
	 *	Arquivos especiais de caracteres
	 */
	if (cflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISCHR (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				if (j)
					printf ("\n");

				printf
				(	"%s%sEspeciais de caracteres:%s",
					cp,
					color_vector[MC_DEF],
					color_vector[S_IFCHR >> 12]
				);
			}

			printf ("\n%s ", cp);
		}
	
		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	if (i > 0)
		{ printf ("\n"); j++; i = 0; }

	/*
	 *	Fifos
	 */
	if (fflag) for (ip = list->i_next; ip != NOINFO; ip = ip->i_next)
	{
		if (!S_ISFIFO (ip->i_s.st_mode))
			continue;

		if (i % files_per_line == 0)
		{
			if (i == 0)
			{
				if (j)
					printf ("\n");

				printf
				(	"%s%sFifos:%s",
					cp,
					color_vector[MC_DEF],
					color_vector[S_IFIFO >> 12]
				);
			}

			printf ("\n%s ", cp);
		}
	
		print_nm (last_id (ip->i_nm), file_fmt_len);

		i++;
	}

	printf ("%s", color_vector[MC_DEF]);

	if (i > 0)
		printf ("\n");

	/*
	 *	Libera a memória
	 */
	for (ip = list->i_next; ip != NOINFO; ip = nextip)
	{
		nextip = ip->i_next;
		free (ip);
	}

	list->i_next = NOINFO;		/* Esvazia a LISTA */

}	/* end show_list */

/*
 ****************************************************************
 *	Imprime um identificador com tamanho dado		*
 ****************************************************************
 */
void
print_nm (const char *nm, int len)
{
	if (strlen (nm) <= len)
	{
		printf (" %-*.*s", len, len, nm);
	}
	else
	{
		printf (" %-*.*s...", len - 3, len - 3, nm);
	}

}	/* end print_nm */

/*
 ****************************************************************
 *	Le um diretório						*
 ****************************************************************
 */
int
dir_read (INFO *dirip)
{
	INFO		*ip;
	DIR		*dir_fd;
	const DIRENT	*dp;
	const char	*filenm;

	/*
	 *	Abre o diretório
	 */
	if ((dir_fd = inopendir (dirip->i_s.st_dev, dirip->i_s.st_ino)) == NODIR)
	{
		error ("*Não consegui abrir \"%s\"", dirip->i_nm);
		return (0);
	}

	/*
	 *	Le o diretório
	 */
	for (errno = NOERR; (dp = readdir (dir_fd)) != NODIRENT; /* vazio */)
	{
		if (!aflag && dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		if ((ip = malloc (sizeof (INFO))) == NOINFO)
			error (NOSTR);

		filenm = compose_name (dirip->i_nm, dp->d_name);

		if (instat (dirip->i_s.st_dev, dp->d_ino, &ip->i_s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", filenm); errno = NOERR;
			free (ip);
			exitcode++;
			continue;
		}

		ip->i_nm = filenm;

		list_insert (&filelist, ip);

		if (Rflag  &&  S_ISDIR (ip->i_s.st_mode))
			list_insert (&dirlist, infodup (ip));

	}	/* end while (fread) */

	if (errno != NOERR)
		error ("*Erro na leitura do diretório \"%s\"", dirip->i_nm);

	closedir (dir_fd);

	return (0);

}	/* end dir_read */

/*
 ****************************************************************
 *	Copia uma estrutura "INFO"				*
 ****************************************************************
 */
INFO *
infodup (INFO *ip)
{
	INFO		*ipcpy;

	if ((ipcpy = malloc (sizeof (INFO))) == NOINFO)
		error (NOSTR);

	*ipcpy = *ip;

	return (ipcpy);

}	/* end infodup */

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
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - lista o conteúdo de diretórios\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-rdbclftaRN] [<diretório> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-r: Arquivos regulares\n"
		"\t-d: Diretórios\n"
		"\t-b: Especiais de blocos\n"
		"\t-c: Especiais de caracteres\n"
		"\t-l: Elos simbólicos\n"
		"\t-f: Fifos\n"
	);
	fprintf
	(	stderr,
		"\t-t: Ordena os arquivos pela data da última modificação\n"
		"\t-a: Lista também \".\" e \"..\"\n"
		"\t-R: Lista subdiretórios recursivamente\n"
		"\t-N: Lê os <diretório>s de \"stdin\"\n"

		"\nObs.:\tSe não for dado nenhum <diretório> nem a opção \"-N\",\n"
			"\tserá considerado o diretório corrente\n"
	);

	exit (2);

}	/* end help */
