/*
 ****************************************************************
 *								*
 *			subst.c					*
 *								*
 * 	Busca/substitui cadeias em vários arquivos		*
 *								*
 *	Versão	1.0.0, de 24.03.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: subst						*
 *		Utilitários Básicos				*
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
#include <fcntl.h>
#include <errno.h>

/*
 ******	Variáveis e Definições globais **************************
 */
#define	DEBUG

const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */
entry int	fflag;		/* Não consulta para atualizar */
entry int	sflag;		/* Silencioso (apenas nomes dos arquivos) */
entry int	gflag;		/* Depuração */

/*
 ******	Definições **********************************************
 */
#define NPAT	10		/* No. max. de padrões */

#define	LINESZ	256		/* Tamanho máximo da linha */

/*
 ******	Outras variáveis ****************************************
 */
int		delim = ':';	/* Delimitador entre padrões */

const char	*old_pat[NPAT+1]; /* Apontadores para os padrões Originais */
const char	*new_pat[NPAT+1]; /* Apontadores para os padrões Substituidos */
int		pati;		/* Contador de padrões */

/*
 ****** Protótipos de funções ***********************************
 */
void		proc_file (const char *);
char		*line_edit (char *, char *, const char *, int);
void		help (void);

/*
 ****************************************************************
 * 	Busca/substitui cadeias em vários arquivos		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		i, opt;
	char		*cp;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fsd:p:gvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Não consulta para atualizar */
			fflag++;
			break;

		    case 's':			/* Silencioso */
			sflag++;
			break;

		    case 'd':			/* Delimitador */
			delim = *optarg;
			break;

		    case 'p':			/* Pares de Substituição */
			if (pati >= NPAT)
			{
				error
				(	"$No momento aceitamos apenas "
					XSTR (NPAT)
					" cadeias de busca/substituição"
				);
			}

			cp = strchr (optarg, delim);

			if (cp != NOSTR && (strrchr (optarg, delim)) != cp)
			{
				error
				(	"A cadeia \"%s\" contém o "
					"delimitador \"%c\" mais de uma vez",
					optarg,	delim
				);
			}

			old_pat[pati] = optarg;

			if (cp != NOSTR)
				{ new_pat[pati] = cp + 1; *cp = '\0'; }

			pati++;
			break;
#ifdef	DEBUG
		    case 'g':			/* Debug */
			gflag++;
			break;
#endif	DEBUG
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

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
	 *	Imprime os argumentos
	 */
#ifdef	DEBUG
	if (gflag)
	{
		printf ("Delim = \"%c\"\n", delim);

		for (i = 0; i < pati; i++)
			printf ("\"%s\" => \"%s\"\n", old_pat[i], new_pat[i]);
	}
#endif	DEBUG

	/*
	 *	Prepara o terminal para consultas
	 */
	if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
		error ("$*Não consegui abrir \"/dev/tty\"");

	/*
	 *	Verifica se foram dados cadeias
	 */
	if (pati == 0)
	{
		error ("Nenhuma cadeia de busca/substituição?\n");
		help ();
	}

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		error ("Os argumentos supérfluos serão ignorados");
		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			proc_file (area);
		}
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			proc_file (*argv);
		}
	}

	return (exit_code);

}	/* end subst */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
void
proc_file (const char *file_nm)
{
	int		fd, i, c;
	char		*cp;
	FILE		*fp;
	char		**line_vec_ptr;
	int		modif = 0, file_sz;
	char		*file_ptr, *file_end, *line_area, *ret_ptr;
	char		**line_vec;
	STAT		s;

	/*
	 *	Verifica se imprime o nome do arquivo
	 */
	if (vflag || sflag)
		printf ("%s:\n", file_nm);

	/*
	 *	Examina e abre o arquivo
	 */
	if ((fd = open (file_nm, O_RDONLY)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", file_nm);
		return;
	}

	if (fstat (fd, &s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		close (fd); return;
	}

	if (!S_ISREG (s.st_mode))
	{
		error ("\"%s\" não é regular", file_nm);
		close (fd); return;
	}

	/*
	 *	Examina e lê o arquivo
	 */
	file_sz  = s.st_size;
	file_ptr = alloca (file_sz + 1);
	file_end = file_ptr + file_sz;

	if (read (fd, file_ptr, file_sz) != file_sz)
	{
		error ("*Não consegui ler o arquivo \"%s\"", file_nm);
		close (fd); return;
	}

	close (fd);

	if (file_end[-1] != '\n')
		{ *file_end++ = '\n'; file_sz++; }

	/*
	 *	Aloca espaço para o vetor de ponteiros para as linhas
	 */
	for (cp = file_ptr, i = 0; cp < file_end; cp++)
	{
		if (*cp == '\n')
			i++;
	}

	line_vec = line_vec_ptr = alloca ((i + 1) * sizeof (char *));

	/*
	 *	Percorre o arquivo, substituindo as linhas, se for o caso
	 */
	*line_vec_ptr = file_ptr; line_area = alloca (LINESZ); i = 1;

	for (cp = file_ptr; cp < file_end; cp++)
	{
		if (*cp != '\n')
			continue;

		*cp = '\0';

		if ((ret_ptr = line_edit (*line_vec_ptr, line_area, file_nm, i)) != NOSTR)
		{
			*line_vec_ptr = ret_ptr;
			line_area = alloca (LINESZ);
			modif++;
		}

		*cp = '\n';

		*++line_vec_ptr = cp + 1; i++;

	}	/* end percorrendo o arquivo */

	*line_vec_ptr = NOSTR;

	/*
	 *	Escreve o arquivo
	 */
	if (modif == 0)
		return;

	if (!fflag)
	{
		fprintf
		(	stderr,
			"%s: Atualiza o arquivo \"%s\"? (n): ",
			pgname, file_nm
		);

		if (askyesno () <= 0)
			return;
	}

	if ((fp = fopen (file_nm, "w")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\" para escrita", file_nm);
		return;
	}

	for (line_vec_ptr = line_vec; (cp = *line_vec_ptr) != NOSTR; line_vec_ptr++)
	{
		for (/* acima */; (c = *cp++) != '\n'; /* no teste */)
			putc (c, fp);

		putc ('\n', fp);
	}

	fclose (fp);

}	/* end proc_file */

/*
 ****************************************************************
 *	Busca/altera os padrões					*
 ****************************************************************
 */
char *
line_edit (char *line_begin, char *line_area, const char *file_nm, int line_no)
{
	char		*cp;
	const char	**old_p = old_pat, **new_p = new_pat;
	int		c, old_len, new_len;
	int		len, modif = 0;
	char		*ret_ptr = NOSTR;

	for (/* acima */; *old_p != NOSTR; old_p++, new_p++)
	{
		c = **old_p; old_len = strlen (*old_p);

		for (cp = line_begin; (cp = strchr (cp, c)) != NOSTR; cp++)
		{
			if (strncmp (cp, *old_p, old_len) != 0)
				continue;

			if (*new_p != NOSTR)
			{
				if (modif++ == 0)
				{
					if ((len = strlen (line_begin)) > LINESZ - 2)
					{
						printf
						(	"%s (%d):\tLINHA MUITO COMPRIDA: %s\n",
							file_nm, line_no, line_begin
						);
						return (NOSTR);
					}

					memmove (line_area, line_begin, len);
					line_area[len]   = '\n';
					line_area[len+1] = '\0';
					cp += (line_area - line_begin);
					ret_ptr = line_begin = line_area;
				}

				new_len = strlen (*new_p);

				if (new_len != old_len)
					strmove (cp+new_len, cp+old_len);

				if (new_len != 0)
					memmove (cp, *new_p, new_len);

				cp += (new_len - 1);
			}

			if (sflag == 0)
			{
				printf
				(	"%s (%d):\t%s%s",
					file_nm, line_no, line_begin,
					modif ? "" : "\n"
				);
			}

		}	/* Examinando a linha com um padrão */

	}	/* Examinando todos os padrões */

	return (ret_ptr);

}	/* line_edit */

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
	 	"%s - busca/substitui cadeias em vários arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-fsvN] [-d <c>] -p <ab>[:<cd>] ... [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Não consulta o usuário para as atualizações\n"
		"\t-s: Modo silencioso (apenas nomes dos arquivos)\n"
		"\t-v: Verboso\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
		"\t-d: Utiliza <c> como caracter delimitador ao invés de \":\"\n"
		"\t-p: Procura a cadeia <ab> (se <cd> não for dado)\n"
		"\t    Substitui a cadeia <ab> por <cd> (se <cd> for dado)\n"
	);

	exit (2);

}	/* end help */
