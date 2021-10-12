/*
 ****************************************************************
 *								*
 *			do_puttree.c				*
 *								*
 *	Copia árvores TROPIX para o sistema DOS			*
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
#include <ftw.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Variáveis deste comando *********************************
 */
extern int	not_first;	/* Já escreveu o nome de um diretório */

/*
 ******	Protótipos de funções ***********************************
 */
int		puttree_ftw_function (const char *, STAT *, int);
int		dos_verdir (const char *);
void		do_puttree_help (void);

extern int	cmp_one_file (const char *, int, STAT *, DOSSTAT *);

/*
 ****************************************************************
 *	Copia árvores TROPIX para o sistema DOS			*
 ****************************************************************
 */
void
do_puttree (int argc, const char *argv[])
{
	char		*tree;
	const char	*str;
	int		opt;
	STAT		old_s;

	/*
	 *	Inicializações
	 */
	not_first = 0;

	/*
	 *	Analisa as opções
	 */
	if (streq (cmd_nm, "cmpputtree"))
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
				{ do_puttree_help (); return; }
			break;

		    case 'P':		/* Padrões de exclusão */
			if (put_p_option (cmd_optarg, exc_pat, &exc_pati) < 0)
				{ do_puttree_help (); return; }
			break;

		    case 'H':			/* Help */
			do_puttree_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_puttree_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_puttree_help (); return; }

	/*
	 *	Obtém o estado do diretório fonte TROPIX
	 */
	tree = path_can ((char *)argv[0]);

	if (stat (tree, &old_s) < 0)
	{
		printf
		(	"%s: Não achei o arquivo %s \"%s\" (%s)\n",
			cmd_nm, sys_nm, tree, strerror (errno)
		);
		return;
	}

	if (!S_ISDIR (old_s.st_mode))
	{
		printf
		(	"%s: O arquivo %s \"%s\" não é um diretório\n",
			cmd_nm, sys_nm, tree
		);
		return;
	}

	/*
	 *	Percorre árvore TROPIX
	 */
	ftw (tree, puttree_ftw_function);

}	/* end do_puttree */

/*
 ****************************************************************
 *	Obtém um arquivo					*
 ****************************************************************
 */
int
puttree_ftw_function (const char *path, STAT *sp, int code)
{
	DOSSTAT		z;
	int		fd;
	const char	*nm;

	/*
	 *	Verifica se houve um <^C>
	 */
	if (received_intr)
		return (-1);

	/*
	 *	Obtém o estado do <destino> DOS
	 */
	if (dos_stat (path, &z) < 0)
		z.z_mode = Z_IFMT;		/* Meio deselegante! */

	nm = last_nm (path);

	/*
	 *	Identifica o <fonte> (a partir de ftw)
	 */
	switch (code)
	{
		/*
		 ******	Diretório *******************************
		 */
	    case FTW_D:		/* Diretórios ".old": pula toda subárvore */
		if (!cmd_dotflag)
		{
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
		if (Z_ISDIR (z.z_mode))
			return (0);

		/*
		 *	Se não for um diretório ...
		 */
		if (z.z_mode == Z_IFMT)		/* Meio deselegante! */
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
						"DOS"
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
					if (z.z_mode & Z_RO) fprintf
					(	stderr,
						"\t*** O arquivo \"%s\" só "
						"permite leituras\n", path
					);

					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						"DOS"
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

				if (z.z_mode & Z_RO) fprintf
				(	stderr,
					"\t*** O arquivo \"%s\" só "
					"permite leituras\n", path
				);

				fprintf
				(	stderr,
					"\t*** Atualiza no %s? (n): ",
					"DOS"
				);

				if (askyesno () <= 0)
					return (1);
			}

			if (dos_unlink (&z, 1 /* trunca */) < 0)
				return (1);
		}

		/*
		 *	Cria o diretório
		 */
		if (dos_verdir (path) < 0)
		{
			printf
			(	"%s: Erro na criação dos diretórios "
				"intermediários de \"%s\" (%s)\n",
				cmd_nm, path, strerror (errno)
			);
			return (1);
		}

		if (dos_mkdir (path) < 0)
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
	    case FTW_F:
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
		if (!S_ISREG (sp->st_mode))
		{
			printf
			(	"\t*** O arquivo %s \"%s\" não é regular\n",
				sys_nm, path
			);
			return (0);
		}

		/*
		 *	Examina o nome novo
		 */
		switch (z.z_mode & Z_IFMT)
		{
		    case Z_IFMT:	/* Arquivo ainda não existe no DOS */
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
						"DOS"
					);

					if (askyesno () <= 0)
						return (0);
				}
			}
			break;

		    case Z_DIR:
			printf
			(	"\t*** \"%s\" já existe e é um diretório\n",
				path
			);
			return (0);
	
		    case Z_REG:
			if   (cmd_cflag)
			{
				if ((fd = inopen (sp->st_dev, sp->st_ino)) < 0)
				{
					printf
					( 	"%s: Não consigo abrir \"%s\" (%s)\n",
						cmd_nm, path, strerror (errno)
					);
					return (0);
				}

				if (cmp_one_file (path, fd, sp, &z) == 0)
					{ close (fd); return (0); }

				close (fd);

				if (!cmd_uflag)
					return (0);

				if (cmd_iflag)
				{
					if (z.z_mode & Z_RO) printf
					(	"\t*** O arquivo \"%s\" só "
						"permite leituras\n", path
					);

					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						"DOS"
					);

					if (askyesno () <= 0)
						return (0);
				}
			}
			elif (!cmd_fflag)
			{
				fprintf
				(	stderr,
					"\t*** \"%s\" já existe%s. "
					"Apaga/reescreve? (n): ",
					path,
					(z.z_mode & Z_RO) ? " e só permite leituras" : ""
				);

				if (askyesno () <= 0)
					return (0);
			}
			goto old_unlink;
	
		    default:
			if (cmd_cflag)
			{
				printf
				(	"\t*** O arquivo %s \"%s\" "
					"já existe e não é regular\n",
					"DOS", path
				);

				if (!cmd_uflag)
					return (0);

				if (cmd_iflag)
				{
					if (z.z_mode & Z_RO) fprintf
					(	stderr,
						"\t*** O arquivo \"%s\" só "
						"permite leituras\n", path
					);

					fprintf
					(	stderr,
						"\t*** Atualiza no %s? (n): ",
						"DOS"
					);

					if (askyesno () <= 0)
						return (0);
				}
			}

		    old_unlink:
			if (dos_unlink (&z, 1 /* trunca */) < 0)
				return (0);
	
		}	/* end switch (já existe o arquivo DOS (dst)) */ 
	
		/*
		 *	Copia o contéudo
		 */
		if (put_one_file (path, 1 /* complete path */) < 0)
		{
			/* As mensagens já foram dadas */
		}

		return (0);

		/*
		 ******	Arquivo ilegível ************************
		 */
	    case FTW_DNR:
	    case FTW_NS:
		if (!pattern_accept (path))
			return (0);

		fprintf
		(	stderr,
			"%s: Não consegui obter o estado do arquivo %s \"%s\"\n",
			cmd_nm, sys_nm, path
		);

		return (0);

		/*
		 ******	Outros tipos de arquivos ****************
		 */
	    default:
		error
		(	"puttree_ftw_function: Tipo de arquivo inválido: %d\n",
			code
		);
		return (0);

	}	/* end switch */

}	/* end puttree_ftw_function */

/*
 ****************************************************************
 *	Cria os diretórios intermediários DOS faltando		*
 ****************************************************************
 */
int
dos_verdir (const char *filenm)
{
	char		*cp;
	DOSSTAT		z;

	/*
	 *	Percorre o nome dado analisando todos os diretórios
	 */
	for (cp = (char *)filenm + 1; *cp != '\0'; cp++)
	{
		if (*cp == '/')
		{
			*cp = '\0';

			if (dos_stat (filenm, &z) < 0)
			{
				if (dos_mkdir (filenm) < 0)
					{ *cp = '/'; return (-1); }
			}

			*cp = '/';

		}	/* end if */

	}	/* end for */

	return (0);

}	/* end dos_verdir */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_puttree_help (void)
{
	if (cmd_cflag)
	{
		fprintf
		(	stderr,
			"%s - compara e copia árvores %s para o sistema DOS\n"
			"\nSintaxe:\n"
			"\t%s [-ui.s] [-p \"<p>\"] [-P \"<P>\"] <árvore>\n",
			cmd_nm, sys_nm, cmd_nm
		);

		fprintf
		(	stderr,
			"\nOpções:"
			"\t-u: Atualiza a árvore DOS\n"
			"\t-i: Pede a confirmação para cada arquivo a atualizar\n"
		);
	}
	else
	{
		fprintf
		(	stderr,
			"%s - copia árvores %s para o sistema DOS\n"
			"\nSintaxe:\n"
			"\t%s [-ifv.s] [-p \"<p>\"] [-P \"<P>\"] <árvore>\n",
			cmd_nm, sys_nm, cmd_nm
		);
		fprintf (stderr, "\nOpções:%s", options_ifv);
	}

	fprintf
	(	stderr,
		"\t-.: Processa também arquivos ocultos (\".old\" ...)\n"
		"\t-s: Adiciona os padrões dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	fprintf (stderr, somewhere_expand, sys_nm);

}	/* end do_puttree_help */
