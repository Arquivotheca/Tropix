/*
 ****************************************************************
 *								*
 *			cmptree.c				*
 *								*
 *	Compara árvores						*
 *								*
 *	Versão	1.0.2, de 09.07.87				*
 *		4.5.0, de 28.10.03				*
 *								*
 *	Módulo: cmptree						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/a.out.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ftw.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.5.0, de 28.10.03";

#define	IN_CALLs		/* Usa as novas chamadas */

#define	EVER	;;
#define	NOSTAT	(STAT *)NULL
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	NPAT	20		/* No. maximo de Padrões */

/*
 ******	Indicadores *********************************************
 */
entry int	uflag;		/* Atualiza */
entry int	fflag;		/* NÃO interativo */
entry int	gflag;		/* Garfield */
entry int	lflag;		/* Lista as linhas diferentes */
entry int	rflag;		/* Rigoroso */
entry int	qflag;		/* Comparação rápida */
entry int	mflag;		/* Mantém o estado do arquivo */
entry int	Lflag;		/* Não entra em subdiretórios */
entry int	dotflag;	/* Não ignora ".old" ... */
entry int	dflag;		/* Não entra em novos dispositivos */
entry int	Dflag;		/* Debug */

/*
 ******	Variáveis globais ***************************************
 */
entry dev_t	old_dev;	/* Dispositivo do diretório fonte */

entry dev_t	new_dev;	/* Dispositivo do diretório destino */
entry ino_t	new_ino;	/* Nó-índice do diretório destino */

entry int	old_dir_sz;	/* Tamanho do nome da árvore fonte */
entry int	new_dir_sz;	/* Tamanho do nome da árvore destino */
entry char	*old_dir_nm;	/* Nome da árvore fonte */
entry char	*new_dir_nm;	/* Nome da árvore destino */

entry int	old_path_case;	/* Tipo do caminho fonte */
entry int	new_path_case;	/* Tipo do caminho fonte */

entry int	ftw_file_cnt = 0;	/* Para a opção "-L" */

entry int	nerrors;	/* No. de erros ocorridos */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Os padrões **********************************************
 */
entry const char *inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int	inc_pati;

entry const char *exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int	exc_pati;

/*
 ****** Garfield ************************************************
 */
const char	garfield[] = "garfield";
entry FILE	*gfp;

/*
 ******	Estruturas **********************************************
 */
typedef struct litem	LITEM;

struct litem
{
	dev_t	l_olddev;	/* Dispositivo fonte */
	ino_t	l_oldino;	/* No. do Inode fonte */
	dev_t	l_newdev;	/* Dispositivo destino */
	ino_t	l_newino;	/* No. do Inode destino */
	char	*l_newname;	/* Nome destino */
	LITEM	*l_next;	/* o Proximo */
};

entry LITEM	*link_list;	/* Começos das listas */

#define	NOLITEM	(LITEM *)NULL

/*
 ******	Estrutura de modos **************************************
 */
typedef struct mode	MODE;

struct mode
{
	const char	*m_name;	/* Nome */
	STAT		m_s;		/* O estado */
	MODE		*m_next;	/* o Proximo */
};

MODE	*modelist;		/* Comeco da Lista */

#define	NOMODE	(MODE *)NULL

/*
 ****** Tabela de comparação de módulos objeto ******************
 */
typedef struct
{
	int		c_offset;
	int		c_size;
	const char	*c_text;

}	COMPAR;

#define	offset_and_size(type, member)	offsetof (type, member), sizeof (((type *)0)->member)

const COMPAR	compar_vec[] =
{
	offset_and_size (HEADER, h_machine),	"\t*** Os computadores diferem: (%04X :: %04X)\n",
	offset_and_size (HEADER, h_magic),	"\t*** Os números mágicos diferem: (%04X :: %04X)\n",
	offset_and_size (HEADER, h_version),	"\t*** As versões diferem: (%d :: %d)\n",
	offset_and_size (HEADER, h_flags),	"\t*** Os indicadores diferem: (%04X :: %04X)\n",
   /***	offset_and_size (HEADER, h_time),	"\t*** Os tempos diferem: (%08X :: %08X)\n", ***/
	offset_and_size (HEADER, h_serial),	"\t*** Os números seriais diferem: (%d :: %d)\n",
	offset_and_size (HEADER, h_tstart),	"\t*** Os inícios do TEXT diferem: (%P :: %P)\n",
	offset_and_size (HEADER, h_dstart),	"\t*** Os inícios do DATA diferem: (%P :: %P)\n",
	offset_and_size (HEADER, h_entry),	"\t*** Os pontos de entrada diferem: (%P :: %P)\n",
	0, 0
};

/*
 ******	Protótipos de funções ***********************************
 */
int		cmp_analysis (const char *, STAT *, int);
void		put_s_option (void);
int		put_p_option (const char *, const char *[], int *);
int		file_prolog (const char **, const STAT *, FILE **);
int		file_compare (const char *, const char *, const STAT *, const STAT *);
void		file_copy (const char *, const char *, const STAT *);
void		file_time_cmp (const STAT *, const STAT *);
void		obj_time_cmp (const HEADER *, const HEADER *);
int		tst_obj_mod (const char *, const STAT *, const HEADER *);
int		obj_compare (const char *, const char *, FILE *, FILE *, const HEADER *, const HEADER *);
int		line_compare (const char *, const char *, FILE *, FILE *, int);
int		section_cmp (const char *, long, FILE *, FILE *, off_t, off_t);
LITEM		*proc_link (const STAT *, const STAT *, const char *);
char		*path_can (const char *);
int		pattern_accept (const char *);
int		verdir (const char *);
void		add_mode (const STAT *, const char *);
void		proc_mode_list (void);
int		status_compare (const STAT *, STAT *, const char *);
void		status_copy (const STAT *, STAT *, const char *);
void		header_endian_conversion (HEADER *);
void		help (void);

/*
 ****************************************************************
 *	Copia uma árvore					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		ret, opt;
	STAT		old_s, new_s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ufglrqmLdb.sp:P:DvH")) != EOF)
	{
		switch (opt)
		{
		    case 'u':			/* Atualiza */
			uflag++;
			break;

		    case 'f':			/* NÃO interativo */
			fflag++;
			break;

		    case 'g':			/* Garfield */
			gflag++;
			break;

		    case 'l':			/* Lista as linhas diferentes */
			lflag++;
			break;

		    case 'r':			/* Rigoroso */
			rflag++;
			mflag++;
			break;

		    case 'q':			/* Comparação rápida */
			qflag++;
			break;

		    case 'm':			/* Dono/grupo e datas */
			mflag++;
			break;

		    case 'L':			/* Não entra em subdiretórios */
			Lflag++;
			break;

		    case 'd':			/* Não entra em novos dispositivos */
			dflag++;
			break;

		    case '.':			/* Não ignora ".old" ... */
			dotflag++;
			break;

		    case 's':			/* Padrões =	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':		/* Padrões de inclusão */
			put_p_option (optarg, inc_pat, &inc_pati);
			break;

		    case 'P':		/* Padrões de exclusão */
			put_p_option (optarg, exc_pat, &exc_pati);
			break;

		    case 'D':			/* Debug */
			Dflag++;
			break;

		    case 'v':			/* Verboso */
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

	if (argc != 2)
		help ();

	/*
	 *	Prepara o terminal
	 */
	if (!fflag)
	{
		if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
			error ("$*Não consegui abrir \"/dev/tty\"");

		askyesno_stderr = stdout;
		askyesno_set (1);
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Normaliza os caminhos
	 */
	argv[0] = path_can (argv[0]);
	argv[1] = path_can (argv[1]);

	if (patmatch (argv[0], ".!.*") || patmatch (argv[0], "*/.!.*"))
		dotflag++;

	if (patmatch (argv[1], ".!.*") || patmatch (argv[1], "*/.!.*"))
		dotflag++;

	/*
	 *	Prepara o "garfield"
	 */
	if (gflag && (gfp = fopen (garfield, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", garfield);

	/*
	 *	Obtém o estado do diretório fonte
	 */
	if (stat (argv[0], &old_s) < 0)
		error ("$*Não consegui obter o estado de \"%s\"", argv[0]);

	if (!S_ISDIR (old_s.st_mode))
		error ("$\"%s\" não é um diretório", argv[0]);

	old_dev = old_s.st_dev;

	/*
	 *	Obtém o estado do diretório destino
	 */
	if (stat (argv[1], &new_s) >= 0)
	{
		if (!S_ISDIR (new_s.st_mode))
			error ("$\"%s\" não é um diretório", argv[1]);

		if (old_s.st_ino == new_s.st_ino && old_s.st_dev == new_s.st_dev)
			error ("$\"%s\" e \"%s\" são a mesma árvore", argv[0], argv[1]);
	}
	else		/* O diretório destino ainda não existe */
	{
		printf ("O diretório destino \"%s\" não existe", argv[1]);

		if (gflag)
			fnputs (argv[1], gfp);

		if (!uflag)
			{ printf ("\n"); exit (1); }

		if (!fflag)
		{
			fprintf (stdout, ". Cria? (n): ", argv[1]);

			if (askyesno () <= 0)
				exit (1);
		}

		printf ("\n");

		if (verdir ((char *)argv[1]) < 0)
			error ("$*Erro na criação dos diretórios intermediários de \"%s\"", argv[1]);

		if (mkdir (argv[1], old_s.st_mode) < 0)
			error ("$*Erro na criação do diretório \"%s\"", argv[1]);

		if (stat (argv[1], &new_s) < 0)
			error ("$*Não consegui obter o estado de \"%s\"", argv[1]);

		if (mflag)
			add_mode (&old_s, argv[1]);
	}

	/*
	 *	Guarda o "ino" e "dev" do <destino> para evitar recursão
	 */
	new_ino = new_s.st_ino;
	new_dev = new_s.st_dev;

	/*
	 *	Prepara os nomes
	 */
	old_dir_nm = (char *)argv[0];	old_dir_sz = strlen (argv[0]);
	new_dir_nm = (char *)argv[1];	new_dir_sz = strlen (argv[1]);

	if   (streq (old_dir_nm, "/"))
		old_path_case = 0;
	elif (streq (old_dir_nm, "."))
		old_path_case = 1;
	else	/* Nem "/" nem "." */
		old_path_case = 2;

	if   (streq (new_dir_nm, "/"))
		new_path_case = 0;
	elif (streq (new_dir_nm, "."))
		new_path_case = 1;
	else	/* Nem "/" nem "." */
		new_path_case = 2;

	if (Dflag)
	{
		printf
		(	"\nold_dir_nm = \"%s\", old_dir_sz = %d\n"
			"new_dir_nm = \"%s\", new_dir_sz = %d\n"
			"old_path_case = %d, new_path_case = %d\n\n",
			argv[0], old_dir_sz, new_dir_nm, new_dir_sz,
			old_path_case, new_path_case
		);
	}

	/*
	 *	Percorre árvore fonte
	 */
   /***	ftw_file_cnt = 0; ***/

	ret = ftw (argv[0], cmp_analysis);

	/*
	 *	Se for o caso, atualiza o modo dos diretórios
	 */
	if (mflag)
		proc_mode_list ();

	return (ret + nerrors);

}	/* end cmptree */

/*
 ****************************************************************
 *	Processa um nome					*
 ****************************************************************
 */
int
cmp_analysis (const char *old_nm, STAT *old_sp, int code)
{
	const char	*nm;
	char		*new_nm;
	LITEM		*lp;
	STAT		new_s, dst_s;
	static int	not_first = 0;
	const char	*last_name;
	int		content_diff, status_diff;

	/*
	 ****** Verifica se deve ficar no dispositivo fonte *****
	 */
	if (dflag && old_sp->st_dev != old_dev)
		return (1);

	ftw_file_cnt++;

	/*
	 ******	Obtém o nome do <destino> ***********************
	 */
	nm = old_nm;

	switch (old_path_case)
	{
	    case 0:		/* "/" */
		nm += 1;
		break;

	    case 1:		/* "." */
		/* vazio */;
		break;

	    default:		/* Demais */
		nm += old_dir_sz;

		if (nm[0] == '/')
			nm += 1;

		break;

	}

	new_nm = alloca (new_dir_sz + 1 + strlen (nm) + 1);

	strcpy (new_nm, new_dir_nm);

	if (new_path_case != 0)		/* Exceto "/" */
		strcat (new_nm, "/");

	strcat (new_nm, nm);

	new_nm = path_can (new_nm);

	if (Dflag)
	{
		printf ("%s	=>	%s\n", old_nm, new_nm);

		if (Dflag > 1)
			return (0);
	}

	/*
	 *	Obtém o último nome do caminho fonte
	 */
	if ((last_name = strrchr (old_nm, '/')) == NOSTR)
		last_name = old_nm;
	else
		last_name++;

	/*
	 *	Obtém o estado do <destino>
	 */
	if ((lstat (new_nm, &new_s)) < 0)
		new_s.st_mode = 0;

	/*
	 *	Identifica o <fonte> (a partir de ftw)
	 */
	switch (code)
	{
		/*
		 ******	Diretório *******************************
		 */
	    case FTW_D:		/* Diretórios ".old": pula toda subárvore */
		if (!dotflag)
		{
			if (patmatch (old_nm, ".!.*") || patmatch (old_nm, "*/.!.*"))
				return (1);
		}

		/*
		 *	Processa a opção "-L"
		 */
		if (Lflag && ftw_file_cnt > 1)
			return (1);

		if (not_first++)
			putc ('\n', stdout);

		printf
		(	"Diretório \"%s%s%s\":\n",
			color_vector[(old_sp->st_mode & S_IFMT) >> 12],
			old_nm,
			color_vector[MC_DEF]
		);

		/*
		 *	Se for a raiz da <árvore destino> ...
		 */
		if (old_sp->st_dev == new_dev && old_sp->st_ino == new_ino)
		{
			printf ("\t*** É a raiz da árvore destino\n");
			return (1);
		}

		/*
		 *	Verifica se o arquivo já existe, e é um diretório
		 */
		if (S_ISDIR (new_s.st_mode))
		{
			if (!rflag || status_compare (old_sp, &new_s, new_nm) == 0)
				return (0);

			if (gflag)
				fnputs (old_nm, gfp);

			if (!uflag)
				{ nerrors++; return (0); }

			if (!fflag)
			{
				fprintf
				(	stdout,
					"\t\tAtualiza a árvore destino? (n): "
				);

				if (askyesno () <= 0)
					{ nerrors++; return (0); }
			}

			add_mode (old_sp, new_nm);
			return (0);
		}

		/*
		 *	Não é um diretório; verifica se existe
		 */
		if (new_s.st_mode == 0)
		{
			/*
			 *	O arquivo não existe
			 */
			printf ("\t*** Não foi encontrado na árvore destino\n");

			if (gflag)
				fnputs (old_nm, gfp);

			if (!uflag)
				{ nerrors++; return (1); }

			if (!fflag)
			{
				fprintf
				(	stdout,
					"\t\tAtualiza a árvore destino? (n): "
				);

				if (askyesno () <= 0)
					{ nerrors++; return (1); }
			}
		}
		else
		{
			/*
			 *	O arquivo existe, mas não é um diretório
			 */
			printf ("\t*** Não é um diretório na árvore destino\n");

			if (gflag)
				fnputs (old_nm, gfp);

			if (!uflag)
				{ nerrors++; return (1); }

			if (!fflag)
			{
				fprintf
				(	stdout,
					"\t\tAtualiza a árvore destino? (n): "
				);

				if (askyesno () <= 0)
					{ nerrors++; return (1); }
			}

			if (unlink (new_nm) < 0)
			{
				error ("*Não consegui remover \"%s\"", new_nm);
				nerrors++;
				return (1);
			}
		}

		/*
		 *	Cria o diretório
		 */
		if (mkdir (new_nm, old_sp->st_mode) < 0)
		{
			error ("*Não consegui criar o diretório \"%s\"", new_nm);
			nerrors++;
			return (1);
		}

		/*
		 *	Atualiza o modo, ... - repare que (new_s.st_mode == 0)
		 */
		if (mflag)
			add_mode (old_sp, new_nm);

		return (0);

		/*
		 ****** Arquivo normal **************************
		 */
	    case FTW_F:
		if (!pattern_accept (old_nm))
			return (0);

	    case_FTW_F:
		printf
		(	"    %s%s%s",
			color_vector[(old_sp->st_mode & S_IFMT) >> 12],
			last_name,
			color_vector[MC_DEF]
		);

		if (S_ISLNK (old_sp->st_mode))
		{
			char		*symlink_nm;

			symlink_nm = alloca (old_sp->st_size + 1);

			if (readlink (old_nm, symlink_nm, old_sp->st_size + 1) < 0)
				symlink_nm = "???";

			printf (" => %s", symlink_nm);

			if (dst_s.st_mode == 0)
				printf (" (!)");
		}

		printf (":\n");

		/*
		 *	Examina o nome novo
		 */
		if (S_ISDIR (new_s.st_mode))
		{
			printf ("\t*** É um diretório na árvore destino\n");
			nerrors++;
			return (0);
		}

		if (new_s.st_mode == 0)
		{
			printf ("\t*** Não foi encontrado na árvore destino\n");

			if (gflag)
				fnputs (old_nm, gfp);

			if (!uflag)
				{ nerrors++; return (0); }

			if (!fflag)
			{
				fprintf
				(	stdout,
					"\t\tAtualiza a árvore destino? (n): "
				);

				if (askyesno () <= 0)
				{
					fprintf
					(	stdout,
						"\t\tRemove o arquivo da árvore fonte? (n): "
					);

					if (askyesno () > 0 && unlink (old_nm) < 0)
						error ("*Não consegui remover \"%s\"", old_nm);

					nerrors++; return (0);
				}
			}

			if
			(	old_sp->st_nlink > 1 &&
				(lp = proc_link (old_sp, &new_s, new_nm))
			)
				goto link_pt;

			file_copy (old_nm, new_nm, old_sp);

			/*
			 *	Atualiza o modo, ... - repare
			 *	que (new_s.st_mode == 0)
			 */
			if (mflag)
				status_copy (old_sp, &new_s, new_nm);

			return (0);
		}

		if
		(	old_sp->st_dev == new_s.st_dev  &&
			old_sp->st_ino == new_s.st_ino
		)
		{
			printf ("\t*** É o próprio arquivo da árvore fonte\n");
			return (0);
		}

		/*
		 *	Confere a relação de elos físicos
		 */
		if (old_sp->st_nlink > 1 && (lp = proc_link (old_sp, &new_s, new_nm)))
		{
			if
			(	lp->l_newdev == new_s.st_dev  &&
				lp->l_newino == new_s.st_ino
			)
				return (0);

			printf
			(	"\t*** O elo físico para \"%s\" não confere\n",
				lp->l_newname
			);

			if (gflag)
				fnputs (old_nm, gfp);

			if (!uflag)
				{ nerrors++; return (0); }

			if (!fflag)
			{
				fprintf
				(	stdout,
					"\t\tAtualiza a árvore destino? (n): "
				);

				if (askyesno () <= 0)
					{ nerrors++; return (0); }
			}

			if (unlink (new_nm) < 0)
			{
				error ("*Não consegui remover \"%s\"", new_nm);
				nerrors++;
				return (0);
			}

		    link_pt:
			if (link (lp->l_newname, new_nm) < 0)
			{
				error
				(	"*Não consegui criar o elo físico de \"%s\" para \"%s\"",
					lp->l_newname, new_nm
				);
				nerrors++;
			}
			return (0);
		}

		/*
		 *	Compara o nó-índice + conteúdo
		 */
		content_diff = file_compare (old_nm, new_nm, old_sp, &new_s);

		if (rflag)
			status_diff = status_compare (old_sp, &new_s, new_nm);
		else
			status_diff = 0;

		if (content_diff == 0 && status_diff == 0)
			return (0);

		file_time_cmp (old_sp, &new_s);

		if (gflag)
			fnputs (old_nm, gfp);

		if (!uflag)
			{ nerrors++; return (0); }

		if (!fflag)
		{
			fprintf
			(	stdout,
				"\t\tAtualiza a árvore destino? (n): "
			);

			if (askyesno () <= 0)
				{ nerrors++; return (0); }
		}

		if
		(	content_diff &&
			((old_sp->st_mode & S_IFMT) != (new_s.st_mode & S_IFMT) ||
			!S_ISREG (old_sp->st_mode))
		)
		{
			if (unlink (new_nm) < 0)
			{
				error ("*Não consegui remover \"%s\"", new_nm);
				nerrors++;
				return (0);
			}
		}

		if (content_diff)
		{
			file_copy (old_nm, new_nm, old_sp);
			new_s.st_mode = 0;
			status_diff++;
		}

		if (mflag && status_diff)
			status_copy (old_sp, &new_s, new_nm);

		return (0);

		/*
		 ****** Elo simbólico ***************************
		 */
	    case FTW_SL:
	    {
		/*
		 *	Examina o arquivo alvo
		 */
		if (stat (old_nm, &dst_s) >= 0)
		{
			if (S_ISDIR (dst_s.st_mode))	/* Diretório */
			{
				if (!dotflag)
				{
					if (patmatch (old_nm, ".!.*") || patmatch (old_nm, "*/.!.*"))
						return (0);
				}
			}
			else				/* Outros */
			{
				if (!pattern_accept (old_nm))
					return (0);
			}
		}
		else
		{
			dst_s.st_mode = 0;
		}

		goto case_FTW_F;
	    }

		/*
		 ******	Arquivo ilegível ************************
		 */
	    case FTW_NS:
		printf ("    %s:\n", last_name);
		printf ("\t*** Não consegui obter o estado\n");

		nerrors++;
		return (0);

		/*
		 ******	Diretório ilegível **********************
		 */
	    case FTW_DNR:
		if (!dotflag)
		{
			if (patmatch (old_nm, ".!.*") || patmatch (old_nm, "*/.!.*"))
				return (1);
		}

		if (not_first++)
			putc ('\n', stdout);

		printf
		(	"Diretório \"%s\":\n"
			"\t*** Não consegui abrir para leituras (%s)\n",
			old_nm, strerror (errno)
		);

		nerrors++;
		return (1);

		/*
		 ******	Código inválido *************************
		 */
	    default:
		error ("Código inválido %d da \"ftw\" para \"%s\"", code, old_nm);

		nerrors++;
		return (0);

	}	/* end switch */

}	/* end cmp_analysis */

/*
 ****************************************************************
 *	Compara um par de arquivos quaisquer			*
 ****************************************************************
 */
int
file_compare (const char *old_nm, const char *new_nm, const STAT *old_sp, const STAT *new_sp)
{
	int		n;
	FILE		*oldfp, *newfp;
	HEADER		oldh, newh;
	int		magic, old_obj = 0, new_obj = 0;

	if (Dflag)
		fprintf (stdout, "=> file_compare (%s, %s)\n", old_nm, new_nm);

	/*
	 *	Analisa os dois arquivos
	 */
	if (n = file_prolog (&old_nm, old_sp, &oldfp))
		return (n);

	if (n = file_prolog (&new_nm, new_sp, &newfp))
		{ fclose (oldfp); return (n); }

	/*
	 *	Compara os conteúdo dos arquivos
	 */
	if (S_ISREG (old_sp->st_mode) && S_ISREG (new_sp->st_mode))
	{
		n = fread (&oldh, sizeof (oldh), 1, oldfp);

		rewind (oldfp);

		magic = oldh.h_magic;

		if (n == 1 && (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC))
		{
			if (tst_obj_mod (old_nm, old_sp, &oldh) == 0)
				old_obj++;
		}

		n = fread (&newh, sizeof (newh), 1, newfp);

		rewind (newfp);

		magic = newh.h_magic;

		if (n == 1 && (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC))
		{
			if (tst_obj_mod (new_nm, new_sp, &newh) == 0)
				new_obj++;
		}

		/*
		 *	Comparação de módulos objeto
		 */
		if (old_obj && new_obj)
		{
			if (qflag)
			{
				if
				(	old_sp->st_size == new_sp->st_size &&
					oldh.h_time   == newh.h_time
				)
				{
					fclose (oldfp); fclose (newfp);
					return (0);
				}

			}

			return
			(	obj_compare
				(	old_nm, new_nm,
					oldfp, newfp,
					&oldh, &newh
				)
			);
		}

		/*
		 *	Verifica se ambos são vazios
		 */
		if (old_sp->st_size == 0 && new_sp->st_size == 0)
		{
#ifdef	VAZIO
			printf
			(	"\t*** Os arquivos fonte e "
				"destino estão vazios\n"
			);
#endif	VAZIO
			fclose (oldfp); fclose (newfp);
			return (0);
		}

		n = line_compare (old_nm, new_nm, oldfp, newfp, old_obj||new_obj);

		fclose (oldfp); fclose (newfp);
		return (n);
	}

	fclose (oldfp); fclose (newfp);

	/*
	 *	Compara os elos simbólicos
	 */
	if (S_ISLNK (old_sp->st_mode) && S_ISLNK (new_sp->st_mode))
	{
		char		*old_symlink_nm, *new_symlink_nm;

		if (old_sp->st_size != new_sp->st_size)
		{
			printf ("\t*** Os elos simbólicos têm conteúdos diferentes\n");
			return (1);
		}

		old_symlink_nm = alloca (old_sp->st_size + 1);

		if (readlink (old_nm, old_symlink_nm, old_sp->st_size + 1) < 0)
		{
			error ("*Não consegui ler o conteúdo de \"%s\"", old_nm);
			nerrors++;
			return (1);
		}

		new_symlink_nm = alloca (new_sp->st_size + 1);

		if (readlink (new_nm, new_symlink_nm, new_sp->st_size + 1) < 0)
		{
			error ("*Não consegui ler o conteúdo de \"%s\"", new_nm);
			nerrors++;
			return (1);
		}

		return (strcmp (old_symlink_nm, new_symlink_nm));
	}

	/*
	 *	Compara os nós-índice (old ou new especiais)
	 */
	if   ((old_sp->st_mode & S_IFMT) != (new_sp->st_mode & S_IFMT))
	{
		printf ("\t*** É um tipo diferente de arquivo na árvore destino\n");
		return (1);
	}
	elif (old_sp->st_rdev != new_sp->st_rdev)
	{
		printf ("\t*** O arquivo têm (MAJOR+MINOR) diferentes\n");
		return (1);
	}

	return (0);

}	/* end file_compare */

/*
 ****************************************************************
 *	Identifica, obtém o estado e abre arquivos		*
 ****************************************************************
 */
int
file_prolog (const char **filenm, const STAT *s, FILE **fp)
{
	int		fd;

	/*
	 *	Trata os arquivos especiais
	 */
	if (!S_ISREG (s->st_mode))
		{ *fp = NOFILE; return (0); }

	/*
	 *	Abre os arquivos restantes
	 */
	if ((fd = inopen (s->st_dev, s->st_ino)) < 0 || (*fp = fdopen (fd, "r")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\"", *filenm);
		return (2);
	}

	return (0);

}	/* end file_prolog */

/*
 ****************************************************************
 *	Compara um par de arquivos regulares			*
 ****************************************************************
 */
int
line_compare (const char *old_nm, const char *new_nm,
			FILE *oldfp, FILE *newfp, int objeto)
{
	int		oldc, newc;
	char		*oldcp, *newcp;
	long		nc = 1;
	long		nl = 1;
	char		oldline[BLSZ+1], newline[BLSZ+1];

	/*
	 *	Percorre os dois arquivos
	 */
	for
	(	oldcp = oldline, newcp = newline, newc = '\0';
		(oldc = getc (oldfp)) != EOF && (newc = getc (newfp)) != EOF;
		nc++
	)
	{
		/*
		 *	Guarda caracteres
		 */
		if (lflag  &&  !objeto  &&  oldcp < (oldline + BLSZ))
		{
			if (oldc != '\n')
				*oldcp++ = oldc;
			if (newc != '\n')
				*newcp++ = newc;
		}

		/*
		 *	Testa se os caracteres são diferentes
		 */
		if (oldc != newc)
		{
			/*
			 *	Indica o local da diferença
			 */
			fprintf
			(	stdout,
				"\t*** Difere na linha %d, caracter %d\n",
				nl, nc
			);

			/*
			 *	Lista as linhas diferentes
			 */
			if (lflag  &&  !objeto)
			{
				if (oldc != '\n')
				{
					while ((oldc = getc (oldfp)) != '\n')
					{
						if (oldcp >= (oldline + BLSZ))
							break;
						*oldcp++ = oldc;
					}
				}
				*oldcp = '\0';

				fprintf (stdout, "fonte:  \t%s\n", oldline);

				if (newc != '\n')
				{
					while ((newc = getc (newfp)) != '\n')
					{
						if (newcp >= (newline + BLSZ))
							break;
						*newcp++ = newc;
					}
				}
				*newcp = '\0';

				fprintf (stdout, "destino:\t%s\n", newline);
			}

			return (1);

		}	/* end	if (oldc != newc) */

		if (oldc == '\n')
		{
			nl++;
			oldcp = oldline;
			newcp = newline;
		}

	}	/* end lendo as linhas dos arquivos */

	/*
	 *	Verifica se houve erros
	 */
	if (ferror (oldfp))
	{
		error ("*Erro de leitura em \"%s\"", old_nm);
		return (1);
	}

	if (ferror (newfp))
	{
		error ("*Erro de leitura em \"%s\"", new_nm);
		return (1);
	}

	/*
	 *	Verifica se um dos arquivos terminou antes do outro
	 */
	if (newc == EOF)
	{
		if (nc == 1)
		{
			printf ("\t*** O arquivo destino está vazio\n");
		}
		else
		{
			printf ("\t*** O arquivo destino é um prefixo do fonte\n");
		}
		return (1);
	}

	if (oldc == EOF  &&  getc (newfp) != EOF)
	{
		if (nc == 1)
		{
			printf ("\t*** O arquivo fonte está vazio\n");
		}
		else
		{
			printf ("\t*** O arquivo fonte é um prefixo do destino\n");
		}
		return (1);
	}

	return (0);

}	/* end line_compare */

/*
 ****************************************************************
 *	Verifica se é um módulo-objeto válido			*
 ****************************************************************
 */
int
tst_obj_mod (const char *mod_nm, const STAT *sp, const HEADER *hp)
{
	off_t		total = 0;

	if (hp->h_tsize < 0 || hp->h_dsize < 0  || hp->h_bsize < 0)
		total = -1;

	if (hp->h_ssize < 0 || hp->h_rtsize < 0 || hp->h_rdsize < 0)
		total = -1;

	if (total == 0)
		total = hp->h_tsize + hp->h_dsize + hp->h_ssize + hp->h_rtsize + hp->h_rdsize;

	if (total >= 0 && total < sp->st_size)
		return (0);

	printf
	(	"\t*** O módulo objeto \"%s\" tem cabeçalho inválido\n",
		mod_nm
	);

	return (-1);

}	/* end tst_obj_mod */

/*
 ****************************************************************
 *	Compara um par de módulos objeto			*
 ****************************************************************
 */
int
obj_compare (const char *old_nm, const char *new_nm, FILE *oldfp,
		FILE *newfp, const HEADER *oldh, const HEADER *newh)
{
	int		ret = 0;
	off_t		oldoff, newoff;
	const COMPAR	*cp;

	if (Dflag)
		fprintf (stdout, "=> obj_compare (%s, %s)\n", old_nm, new_nm);

	/*
	 *	Compara diversos campos
	 */
	for (cp = compar_vec; cp->c_size != 0; cp++)
	{
		ulong		old, new;

		if (cp->c_size == sizeof (ushort))
		{
			old = *(ushort *)((char *)oldh + cp->c_offset);
			new = *(ushort *)((char *)newh + cp->c_offset);
		}
		else
		{
			old = *(ulong *)((char *)oldh + cp->c_offset);
			new = *(ulong *)((char *)newh + cp->c_offset);
		}

		if (old != new)
			{ printf (cp->c_text, old, new); ret++; }
	}

	/*
	 *	Compara os textos
	 */
	oldoff = sizeof (HEADER);
	newoff = sizeof (HEADER);

	if (oldh->h_tsize != newh->h_tsize)
	{
		printf
		(	"\t*** Os tamanhos dos \"text\"s diferem:  (%d :: %d)\n",
			oldh->h_tsize, newh->h_tsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"text", oldh->h_tsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_tsize;
	newoff += newh->h_tsize;

	/*
	 *	Compara os "datas"
	 */
	if (oldh->h_dsize != newh->h_dsize)
	{
		printf
		(	"\t*** Os tamanhos dos \"data\"s diferem:  (%d :: %d)\n",
			oldh->h_dsize, newh->h_dsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"data", oldh->h_dsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_dsize;
	newoff += newh->h_dsize;

	/*
	 *	Compara os bsss
	 */
	if (oldh->h_bsize != newh->h_bsize)
	{
		printf
		(	"\t*** Os tamanhos dos \"bss\"s  diferem:  (%d :: %d)\n",
			oldh->h_bsize, newh->h_bsize
		);
		ret++;
	}

	/*
	 *	Compara as symtb
	 */
	if (oldh->h_ssize != newh->h_ssize)
	{
		printf
		(	"\t*** Os tamanhos das tabelas de "
			"simbolos diferem:  (%d :: %d)\n",
			oldh->h_ssize, newh->h_ssize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"symtb", oldh->h_ssize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_ssize;
	newoff += newh->h_ssize;

	/*
	 *	Compara relocação do texto
	 */
	if (oldh->h_rtsize != newh->h_rtsize)
	{
		printf
		(	"\t*** Os tamanhos das relocações "
			"de \"text\" diferem:  (%d :: %d)\n",
			oldh->h_rtsize, newh->h_rtsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"relocação do texto", oldh->h_rtsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_rtsize;
	newoff += newh->h_rtsize;

	/*
	 *	Compara relocação do dado
	 */
	if (oldh->h_rdsize != newh->h_rdsize)
	{
		printf
		(	"\t*** Os tamanhos das relocações "
			"de \"data\" diferem:  (%d :: %d)\n",
			oldh->h_rdsize, newh->h_rdsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"relocação do dado", oldh->h_rdsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_rdsize;
	newoff += newh->h_rdsize;

	/*
	 *	Compara as tabelas de linhas
	 */
	if (oldh->h_lnosize != newh->h_lnosize)
	{
		printf
		(	"\t*** Os tamanhos das tabelas de linhas "
			"diferem:  (%d :: %d)\n",
			oldh->h_lnosize, newh->h_lnosize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de linhas", oldh->h_lnosize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_lnosize;
	newoff += newh->h_lnosize;

	/*
	 *	Compara as tabelas de depuração
	 */
	if (oldh->h_dbsize != newh->h_dbsize)
	{
		printf
		(	"\t*** Os tamanhos das tabelas de depuração "
			"diferem:  (%d :: %d)\n",
			oldh->h_dbsize, newh->h_dbsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de depuração", oldh->h_dbsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_dbsize;
	newoff += newh->h_dbsize;

	/*
	 *	Compara as tabela de módulos
	 */
	if (oldh->h_modsize != newh->h_modsize)
	{
		printf
		(	"\t*** Os tamanhos das tabelas de módulos "
			"diferem:  (%d :: %d)\n",
			oldh->h_modsize, newh->h_modsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de módulos", oldh->h_modsize,
			oldfp, newfp, oldoff, newoff
		);
	}

	oldoff += oldh->h_modsize;
	newoff += newh->h_modsize;

	/*
	 *	Compara as tabelas de referências externas
	 */
	if (oldh->h_refsize != newh->h_refsize)
	{
		printf
		(	"\t*** Os tamanhos das tabelas de referências "
			"externas diferem:  (%d :: %d)\n",
			oldh->h_refsize, newh->h_refsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de referências externas", oldh->h_refsize,
			oldfp, newfp, oldoff, newoff
		);
	}

   /***	oldoff += oldh->h_refsize; ***/
   /***	newoff += newh->h_refsize; ***/

	fclose (oldfp);
	fclose (newfp);

	if (ret)
	{
		printf
		(	"\t*** Tamanhos totais: (%d :: %d)\n",
			oldh->h_tsize + oldh->h_dsize + oldh->h_bsize,
			newh->h_tsize + newh->h_dsize + newh->h_bsize
		);
		obj_time_cmp (oldh, newh);
		return (1);
	}

	return (0);

}	/* end obj_compare */

/*
 ****************************************************************
 *	Compara uma sessão					*
 ****************************************************************
 */
int
section_cmp (const char *sectionm, long n, FILE *oldfp,
			FILE *newfp, off_t oldoff, off_t newoff)
{
	int		i, c;

	fseek (oldfp, oldoff, SEEK_SET);
	fseek (newfp, newoff, SEEK_SET);

	for (i = 0; i < n; i++)
	{
		if ((c = getc (oldfp)) < 0)	/* EOF */
		{
			printf
			(	"\t*** EOF prematuro na sessão \"%s\" no byte %d\n",
				sectionm,
				i
			);
			return (1);
		}

		if (getc (newfp) != c)
		{
			printf
			(	"\t*** Diferença na sessão \"%s\" no byte %d\n",
				sectionm,
				i
			);
			return (1);
		}
	}

	return (0);

}	/* end section_cmp */

/*
 ****************************************************************
 *	Compara a data de módulos objeto			*
 ****************************************************************
 */
void
obj_time_cmp (const HEADER *oldh, const HEADER *newh)
{
	if   (oldh->h_time > newh->h_time)
		printf ("\t*** A versão mais recente é o módulo objeto fonte\n");
	elif (oldh->h_time < newh->h_time)
		printf ("\t*** A versão mais recente é o módulo objeto destino\n");
	else
		printf ("\t*** Os módulos objeto têm a mesma data\n");

}	/* end obj_time_cmp */

/*
 ****************************************************************
 *	Compara a data de arquivos em geral			*
 ****************************************************************
 */
void
file_time_cmp (const STAT *old_sp, const STAT *new_sp)
{
	if (S_ISLNK (old_sp->st_mode))
		return;

	if   (old_sp->st_mtime > new_sp->st_mtime)
		printf ("\t*** O mais recente é o arquivo fonte\n");
	elif (old_sp->st_mtime < new_sp->st_mtime)
		printf ("\t*** O mais recente é o arquivo destino\n");
	else
		printf ("\t*** Os arquivos têm a mesma data\n");

}	/* end file_time_cmp */

/*
 ****************************************************************
 *	Copia os blocos (ou o nó-índice) do arquivo		*
 ****************************************************************
 */
void
file_copy (const char *old_nm, const char *new_nm, const STAT *old_sp)
{
	int		fdin, fdout;
	int		count;
	char		area[BL4SZ];

	if (Dflag)
		printf ("=> file_copy (%s, %s)\n", old_nm, new_nm);

	/*
	 *	Analisa o tipo do arquivo
	 */
	switch (old_sp->st_mode & S_IFMT)
	{
		/*
		 ****** Arquivo regular: Copia bloco a bloco ****
		 */
	    case S_IFREG:
		if ((fdout = creat (new_nm, old_sp->st_mode)) < 0)
		{
			error ("*Não consegui criar o arquivo \"%s\"", new_nm);
			nerrors++;
			return;
		}

		if ((fdin = inopen (old_sp->st_dev, old_sp->st_ino)) < 0)
		{
			error ("*Não consegui abrir o arquivo \"%s\"", old_nm);
			close (fdout);
			nerrors++;
			return;
		}

		while ((count = read (fdin, area, sizeof (area))) > 0)
		{
			if (write (fdout, area, count) != count)
			{
				error ("*Erro de escrita em \"%s\"", new_nm);
				close (fdin);
				close (fdout);
				nerrors++;
				return;
			}
		}

		close (fdin);
		close (fdout);

		if (count < 0)
		{
			error ("*Erro de leitura em \"%s\"", old_nm);
			nerrors++;
			return;
		}

		break;

		/*
		 ****** Arquivo especial: Recria ****************
		 */
	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFIFO:
		if (mknod (new_nm, old_sp->st_mode, old_sp->st_rdev) < 0)
		{
			error ("*Não consegui criar \"%s\"", new_nm);
			nerrors++;
			return;
		}

		break;

		/*
		 ****** Elo simbólico ***************************
		 */
	    case S_IFLNK:
	    {
		char		*symlink_nm;

		symlink_nm = alloca (old_sp->st_size + 1);

		if (readlink (old_nm, symlink_nm, old_sp->st_size + 1) < 0)
		{
			error ("*Não consegui obter o conteúdo do elo simbólico de \"%s\"", old_nm);
			nerrors++;
			return;
		}

		if (symlink (symlink_nm, new_nm) < 0)
		{
			error ("*Não consegui criar o elo simbólico \"%s\"", new_nm);
			nerrors++;
			return;
		}

		return;
	    }

	    default:
		error ("Tipo de arquivo inválido para \"%s\"", old_nm);
		nerrors++;
		return;

	}	/* end switch */

}	/* end file_copy */

/*
 ****************************************************************
 *	Compara o grupo/dono, ...				*
 ****************************************************************
 */
int
status_compare (STAT const *old_sp, STAT *new_sp, const char *new_nm)
{
	int		diff = 0;

	if (S_ISLNK (old_sp->st_mode))
		return (0);

	if (new_sp->st_mode == 0)
	{
		if (stat (new_nm, new_sp) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", new_nm);
			nerrors++;
			return (0);
		}
	}

	if ((old_sp->st_mode & ~S_IFMT) != (new_sp->st_mode & ~S_IFMT))
	{
		printf ("\t*** O modo/proteção do arquivo não confere\n");
		diff++;
	}

	if (old_sp->st_ctime != new_sp->st_ctime)
	{
		printf ("\t*** A data de criação do arquivo não confere\n");
		diff++;
	}

	if (old_sp->st_mtime != new_sp->st_mtime)
	{
		printf ("\t*** A data de modificação do arquivo não confere\n");
		diff++;
	}

	if (old_sp->st_uid != new_sp->st_uid)
	{
		printf ("\t*** O dono do arquivo (UID) não confere\n");
		diff++;
	}

	if (old_sp->st_gid != new_sp->st_gid)
	{
		printf ("\t*** O grupo do arquivo (GID) não confere\n");
		diff++;
	}

	return (diff);

}	/* end status_compare */

/*
 ****************************************************************
 *	Copia o grupo/dono, ...					*
 ****************************************************************
 */
void
status_copy (const STAT *old_sp, STAT *new_sp, const char *new_nm)
{
	if (S_ISLNK (old_sp->st_mode))
		return;

	if (new_sp->st_mode == 0)
	{
		if (stat (new_nm, new_sp) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", new_nm);
			nerrors++;
			return;
		}
	}

	if ((old_sp->st_mode & ~S_IFMT) != (new_sp->st_mode & ~S_IFMT))
	{
		if (chmod (new_nm, old_sp->st_mode & ~S_IFMT) < 0)
		{
			error ("*Não consegui restaurar a proteção de \"%s\"", new_nm);
			nerrors++;
		}
	}

	if (utime (new_nm, &old_sp->st_atime) < 0)
	{
		error ("*Não consegui restaurar os tempos de \"%s\"", new_nm);
		nerrors++;
	}

	if (old_sp->st_uid != new_sp->st_uid || old_sp->st_gid != new_sp->st_gid)
	{
		if (chown (new_nm, old_sp->st_uid, old_sp->st_gid) < 0)
		{
			error ("*Não consegui restaurar o dono/grupo de \"%s\"", new_nm);
			nerrors++;
		}
	}

}	/* end status_copy */

/*
 ****************************************************************
 *	Processa LINKs						*
 ****************************************************************
 */
LITEM *
proc_link (const STAT *old_sp, const STAT *new_sp, const char *new_nm)
{
	LITEM		*lp;
	char		*cp;

	for (lp = link_list; lp != NOLITEM; lp = lp->l_next)
	{
		if
		(	lp->l_olddev == old_sp->st_dev &&
			lp->l_oldino == old_sp->st_ino
		)
			return (lp);
	}

	if
	(	(lp = malloc (sizeof (LITEM))) == NOLITEM ||
		(cp = strdup (new_nm)) == NOSTR
	)
	{
		error ("Não obtive memória para item de elo físico");
		nerrors++;
	}
	else
	{
		lp->l_olddev  = old_sp->st_dev;
		lp->l_oldino  = old_sp->st_ino;
		lp->l_newdev  = new_sp->st_dev;
		lp->l_newino  = new_sp->st_ino;
		lp->l_newname = cp;

		lp->l_next = link_list;
		link_list  = lp;
	}

	return (NOLITEM);

}	/* end proc_link */

/*
 ****************************************************************
 *	Adiciona um MODO na lista				*
 ****************************************************************
 */
void
add_mode (const STAT *sp, const char *nm)
{
	MODE		*mp;
	char		*cp;

	if
	(	(mp = malloc (sizeof (MODE))) == NOMODE ||
		(cp = strdup (nm)) == NOSTR
	)
	{
		error ("Não obtive memória para item de modo");
		nerrors++;
	}
	else
	{
		mp->m_name = cp;
		memmove (&mp->m_s, sp, sizeof (STAT));

		mp->m_next = modelist;
		modelist = mp;
	}

}	/* end add_mode */

/*
 ****************************************************************
 *	Altera os modos dos diretórios da lista de modos	*
 ****************************************************************
 */
void
proc_mode_list (void)
{
	MODE		*mp;
	STAT		new_s;

	new_s.st_mode = 0;

	for (mp = modelist; mp != NOMODE; mp = mp->m_next)
		status_copy (&mp->m_s, &new_s, mp->m_name);

}	/* end proc_mode_list */

/*
 ****************************************************************
 *	Normaliza um caminho					*
 ****************************************************************
 */
char *
path_can (const char *path)
{
	char		*cp;

	/*
	 *	Modificação cuidadosa
	 */
	for (cp = strend (path) - 1; /* abaixo */; /* abaixo */)
	{
		if (cp <= path)
			break;

		if   (cp[0] == '/')
			{ *cp = '\0'; cp--; }
		elif (cp[0] == '.' && cp[-1] == '/')
			{ *cp = '\0'; cp--; }
		else
			break;
	}

	while (path[0] == '.' && path[1] == '/')
		path += 2;

	return ((char *)path);

}	/* end path_can */

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
			error ("No momento somente aceitamos " XSTR (NPAT) " padrões\n");
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
	 *	Testa em primeiro lugar os pontos
	 */
	if (!dotflag)
	{
		if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
			return (0);
	}

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
 *	Cria os Diretórios intemediários faltando		*
 ****************************************************************
 */
int
verdir (const char *filenm)
{
	char		*cp;

	/*
	 *	Percorre o nome dado analisando todos os diretórios
	 */
	for (cp = (char *)(filenm + 1); *cp != '\0'; cp++)
	{
		if (*cp == '/')
		{
			*cp = '\0';

			if (access (filenm, F_OK) < 0)
			{
				if (mkdir (filenm, 0777) < 0)
					{ *cp = '/'; return (-1); }
			}

			*cp = '/';

		}	/* end if */

	}	/* end for */

	return (0);

}	/* end verdir */

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
		"%s - compara árvores\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ufglrqmLd.] <PAD> <árvore fonte> <árvore destino>\n\n"
		"\tValores de <PAD>: [-s] [-p \"<p>\"] [-P \"<P>\"]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-u: Atualiza a árvore destino\n"
		"\t-f: A atualização é realizada sem consulta ao usuário\n"
		"\t-g: Gera o arquivo \"garfield\" com os nomes dos arquivos diferentes\n"
		"\t-l: Lista a primeira linha diferente entre os dois arquivos\n"
		"\t-r: Compara também o dono, grupo, proteção e datas do arquivo\n"
		"\t-q: Comparação rápida para módulos objeto\n"
	);

	fprintf
	(	stderr,
		"\t-m: Copia o dono, grupo, proteção e datas do arquivo\n"
		"\t-L: Não entra em subdiretórios\n"
		"\t-d: Não entra em subdiretórios de dispositivos montados\n"
		"\t-.: Compara também arquivos ocultos (\".old\" ...)\n"
		"\t-s: Adiciona os padrões dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	exit (2);

}	/* end help */
