/*
 ****************************************************************
 *								*
 *			do_gettree.c				*
 *								*
 *	Copia árvores DOS para o sistema TROPIX			*
 *								*
 *	Versão	3.0.0, de 23.11.93				*
 *		3.1.0, de 15.01.99				*
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
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Variáveis deste comando *********************************
 */
entry int	not_first;	/* Já escreveu o nome de um diretório */

/*
 ******	Protótipos de funções ***********************************
 */
int		gettree_ftw_function (const char *, DOSSTAT *, int);
int		sofix_verdir (const char *);
void		do_gettree_help (void);

extern int	cmp_one_file (const char *, int, STAT *, DOSSTAT *);

/*
 ****************************************************************
 *	Copia árvores DOS para o sistema TROPIX			*
 ****************************************************************
 */
void
do_gettree (int argc, const char *argv[])
{
	const char	*tree;
	const char	*str;
	int		opt;
	DOSSTAT		old_z;

	/*
	 *	Inicializações
	 */
	not_first = 0;

	/*
	 *	Analisa as opções
	 */
	if (streq (cmd_nm, "cmpgettree"))
		{ str = "ui.sp:P:H"; cmd_cflag++; }
	else
		  str = "ifv.sp:P:H";

	while ((opt = cmd_getopt (argv, str)) >= 0)
	{
		switch (opt)
		{
		    case 'i':			/* Interativo */
			cmd_iflag++;
			break;

		    case 'u':			/* Atualização */
			cmd_uflag++;
			break;

		    case 'f':			/* Força */
			cmd_fflag++;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case '.':			/* ".old" */
			cmd_dotflag++;
			break;

		    case 's':			/* Padrões = 	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':		/* Padrões de inclusão */
			if (put_p_option (cmd_optarg, inc_pat, &inc_pati) < 0)
				{ do_gettree_help (); return; }
			break;

		    case 'P':		/* Padrões de exclusão */
			if (put_p_option (cmd_optarg, exc_pat, &exc_pati) < 0)
				{ do_gettree_help (); return; }
			break;

		    case 'H':			/* Help */
			do_gettree_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_gettree_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_gettree_help (); return; }

	/*
	 *	Obtém o estado do diretório fonte DOS
	 */
	tree = path_can ((char *)argv[0]);

	if (dos_stat (tree, &old_z) < 0)
	{
		printf
		(	"%s: Não achei o arquivo %s \"%s\" (%s)\n",
			cmd_nm, "DOS", tree, strerror (errno)
		);
		return;
	}

	if (!Z_ISDIR (old_z.z_mode))
	{
		printf
		(	"%s: O arquivo DOS \"%s\" não é um diretório\n",
			cmd_nm, tree
		);
		return;
	}

	/*
	 *	Percorre árvore DOS
	 */
	dos_ftw (tree, gettree_ftw_function);

}	/* end do_gettree */

/*
 ****************************************************************
 *	Obtém um arquivo					*
 ****************************************************************
 */
int
gettree_ftw_function (const char *path, DOSSTAT *zp, int code)
{
	STAT		s;
	int		n, fd;
	char		area[BLSZ];
	const char	*nm;
	DOSFILE		f;

	/*
	 *	Obtém o estado do <destino> TROPIX
	 */
	if ((stat (path, &s)) < 0)
		s.st_mode = 0;

	nm = last_nm (path);

	/*
	 *	Identifica o <fonte> (a partir de dos_ftw)
	 */
	switch (code)
	{
		/*
		 ******	Diretório *******************************
		 */
	    case DOS_FTW_D:	/* Diretórios ".old": pula toda subárvore */
		if (!cmd_dotflag)
		{
			if (zp->z_mode & Z_HIDDEN)
				return (1);

			if (patmatch (path, ".!.*") || patmatch (path, "*/.!.*"))
				return (1);
		}

		/*
		 *	Verifica se é interativo
		 */
		if   (cmd_cflag)
		{
			if (not_first++)
				printf ("\n");

			printf ("Diretório \"%s\":\n", path);
		}
		elif (cmd_iflag)
		{
			if (not_first++)
				printf ("\n");

			fprintf (stderr, "Diretório \"%s\"? (n): ", path);

			if (askyesno () <= 0)
				return (1);
		}
		elif (cmd_vflag)
		{
			if (not_first++)
				printf ("\n");

			printf ("Diretório \"%s\":\n", path);
		}

		/*
		 *	Verifica se o arquivo já existe, e é um diretório
		 */
		if (S_ISDIR (s.st_mode))
			return (0);

		/*
		 *	Se não for um diretório ...
		 */
		if (s.st_mode == 0)
		{
			if (cmd_cflag)
			{
				printf
				(	"\t*** Não achei o diretório \"%s\" (%s)\n",
					nm, strerror (errno)
				);

				if (!cmd_uflag)
					return (1);

				if (cmd_iflag)
				{
					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						sys_nm
					);

					if (askyesno () <= 0)
						return (1);
				}
			}
		}
		else	/* Existe, mas não é diretório */
		{
			if   (cmd_cflag)
			{
				printf
				(	"\t*** O arquivo \"%s\" deveria ser um "
					"diretório\n",
					path
				);

				if (!cmd_uflag)
					return (1);

				if (cmd_iflag)
				{
					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						sys_nm
					);

					if (askyesno () <= 0)
						return (1);
				}
			}
			elif (!cmd_fflag)
			{
				fprintf
				(	stderr,
					"\t*** O arquivo \"%s\" deveria ser um "
					"diretório\n",
					path
				);

				fprintf
				(	stderr,
					"\t*** Atualiza no %s? (n): ",
					sys_nm
				);

				if (askyesno () <= 0)
					return (1);
			}

			if (unlink (path) < 0)
			{
				fprintf
				(	stderr,
					"%s: Não consegui remover \"%s\" (%s)\n",
					cmd_nm, path, strerror (errno)
				);
				return (1);
			}
		}

		/*
		 *	Cria o diretório
		 */
		if (sofix_verdir (path) < 0)
		{
			printf
			(	"%s: Erro na criação dos diretórios "
				"intermediários de \"%s\" (%s)\n",
				cmd_nm, path, strerror (errno)
			);
			return (1);
		}

		if (mkdir (path, 0777) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui criar o diretório \"%s\" (%s)\n",
				cmd_nm, path, strerror (errno)
			);
			return (1);
		}

		return (0);

		/*
		 ****** Arquivo normal **************************
		 */
	    case DOS_FTW_F:
		if (!cmd_dotflag && zp->z_mode & Z_HIDDEN)
			return (0);

		if (!pattern_accept (path))
			return (0);

		/*
		 *	Verifica se é interativo
		 */
		if   (cmd_cflag)
		{
			printf ("    %s:\n", nm);
		}
		elif (cmd_iflag)
		{
			fprintf (stderr, "    %s? (n): ", nm);

			if (askyesno () <= 0)
				return (0);
		}
		elif (cmd_vflag)
		{
			printf ("    %s:\n", nm);
		}

		/*
		 *	Deve ser regular
		 */
		if (!Z_ISREG (zp->z_mode))
		{
			printf
			(	"\t*** O arquivo DOS \"%s\" não é regular\n",
				path
			);
			return (0);
		}

		/*
		 *	Examina o nome novo
		 */
		switch (s.st_mode & S_IFMT)
		{
		    case 0:	/* Arquivo ainda não existe no TROPIX */
			if (cmd_cflag)
			{
				printf
				(	"\t*** Não achei o arquivo (%s)\n",
					strerror (errno)
				);

				if (!cmd_uflag)
					return (0);

				if (cmd_iflag)
				{
					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						sys_nm
					);

					if (askyesno () <= 0)
						return (0);
				}
			}
			break;

		    case S_IFDIR:
			printf
			(	"\t*** \"%s\" já existe e é um diretório\n",
				path
			);
			return (0);
	
		    case S_IFREG:
			if   (cmd_cflag)
			{
				if ((fd = inopen (s.st_dev, s.st_ino)) < 0)
				{
					printf
					( 	"%s: Não consigo abrir \"%s\" (%s)\n",
						cmd_nm, path, strerror (errno)
					);
					return (0);
				}

				if (cmp_one_file (path, fd, &s, zp) == 0)
					{ close (fd); return (0); }

				close (fd);

				if (!cmd_uflag)
					return (0);

				if (cmd_iflag)
				{
					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						sys_nm
					);

					if (askyesno () <= 0)
						return (0);
				}
			}
			elif (!cmd_fflag)
			{
				fprintf
				(	stderr,
					"\t*** \"%s\" já existe. "
					"Apaga/reescreve? (n): ",
					path
				);

				if (askyesno () <= 0)
					return (0);
			}
			break;
	
		    default:
			if (cmd_cflag)
			{
				printf
				(	"\t*** O arquivo %s \"%s\" já existe "
					"e não é regular\n",
					sys_nm, path
				);

				if (!cmd_uflag)
					return (0);

				if (cmd_iflag)
				{
					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						sys_nm
					);

					if (askyesno () <= 0)
						return (0);
				}
			}

			if (unlink (path) < 0)
			{
				printf
				(	"%s: Não consigo remover \"%s\" (%s)\n",
					cmd_nm, path, strerror (errno)
				);
				return (0);
			}
	
		}	/* end if (já existe o arquivo TROPIX (dst)) */ 
	
		/*
		 *	Copia o contéudo
		 */
		if (dos_open (&f, zp) < 0)
			return (0);

		if ((fd = creat (path, 0666)) < 0)
		{
			printf
			( 	"%s: Não consigo criar \"%s\" (%s)\n",
				cmd_nm, path, strerror (errno)
			);
			return (0);
		}
	
		while ((n = dos_read (&f, area, BLSZ)) > 0)
		{
			if (write (fd, area, n) != n)
			{
				printf
				(	"%s: Erro na escrita de \"%s\" (%s)\n",
					cmd_nm, path, strerror (errno)
				);
				close (fd);
				return (0);
			}
		}
	
		close (fd);

		return (0);

		/*
		 ******	Arquivo ilegível ************************
		 */
	    case DOS_FTW_NS:
		if (!pattern_accept (path))
			return (0);

		fprintf
		(	stderr,
			"%s: Não consegui obter o estado do arquivo DOS \"%s\"\n",
			cmd_nm, path
		);

		return (0);

		/*
		 ******	Outros tipos de arquivos ****************
		 */
	    default:
		error
		(	"gettree_ftw_function: Tipo de arquivo inválido: %d\n",
			code
		);
		return (0);

	}	/* end switch */

}	/* end gettree_ftw_function */

/*
 ****************************************************************
 *	Cria os diretórios intermediários TROPIX faltando	*
 ****************************************************************
 */
int
sofix_verdir (const char *filenm)
{
	char		*cp;

	/*
	 *	Percorre o nome dado analisando todos os diretórios
	 */
	for (cp = (char *)filenm + 1; *cp != '\0'; cp++)
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

}	/* end sofix_verdir */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_gettree_help (void)
{
	if (cmd_cflag)
	{
		fprintf
		(	stderr,
			"%s - compara e copia árvores DOS para o sistema %s\n"
			"\nSintaxe:\n"
			"\t%s [-ui.s] [-p \"<p>\"] [-P \"<P>\"] <árvore>\n",
			cmd_nm, sys_nm, cmd_nm
		);

		fprintf
		(	stderr,
			"\nOpções:"
			"\t-u: Atualiza a árvore %s\n"
			"\t-i: Pede a confirmação para cada arquivo a atualizar\n",
			sys_nm
		);
	}
	else
	{
		fprintf
		(	stderr,
			"%s - copia árvores DOS para o sistema %s\n"
			"\nSintaxe:\n"
			"\t%s [-ifv.s] [-p \"<p>\"] [-P \"<P>\"] <árvore>\n",
			cmd_nm, sys_nm, cmd_nm
		);
		fprintf (stderr, "\nOpções:%s", options_ifv);
	}

	fprintf
	(	stderr,
		"\t-.: Processa também arquivos ocultos (\"hidden\" e \".old\" ...)\n"
		"\t-s: Adiciona os padrões dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	fprintf (stderr, somewhere_expand, "DOS");

}	/* end do_gettree_help */
