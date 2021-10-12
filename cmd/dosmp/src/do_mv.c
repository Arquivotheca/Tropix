/*
 ****************************************************************
 *								*
 *			do_mv.c					*
 *								*
 *	Move (troca o nome) de arquivos DOS			*
 *								*
 *	Versão	3.0.0, de 19.11.93				*
 *		3.0.0, de 10.07.95				*
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
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		do_one_mv (const char *, const char *);
void		do_mv_help (void);

/*
 ****************************************************************
 *	Move (troca o nome) de arquivos DOS			*
 ****************************************************************
 */
void
do_mv (int argc, const char *argv[])
{
	const char	*dir_nm;
	char		*nm_point = NOSTR;
	char		*path = NOSTR;
	int		opt;
	DOSSTAT		z;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "ifvH")) >= 0)
	{
		switch (opt)
		{
		    case 'i':			/* Interativo */
			cmd_iflag++;
			break;

		    case 'f':			/* Força */
			cmd_fflag++;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_mv_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mv_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc < 2)
		{ do_mv_help (); return; }

	/*
	 *	Examina se o último argumento é um diretório
	 */
	if (dos_stat (dir_nm = argv[argc-1], &z) >= 0 && Z_ISDIR (z.z_mode))
	{
		path = alloca (strlen (dir_nm) + 20);

		strcpy (path, dir_nm); 		strcat (path, "/");
		nm_point = strend (path);

		while (nm_point[-2] == '/')
			nm_point--;
	}

	/*
	 *	Se foram dados mais de dois arquivos,
	 *	o último deve ser um diretório
	 */
	if (argc > 2  &&  path == NOSTR)
	{
		printf
		(	"%s: O arquivo \"%s\" não é um diretório\n\n",
			cmd_nm,	dir_nm
		);
		do_mv_help ();
		return;
	}

	/*
	 *	Processa os argumentos
	 */
	if   (path == NOSTR)
	{
		/*
		 *	Primeiro caso: "mv a b"
		 */
		do_one_mv (argv[0], argv[1]);
	}
	else
	{
		/*
		 *	Segundo caso: "mv a b c ... dir"
		 */
		argv[argc-1] = NOSTR;

		for (/* vazio */; *argv; argv++)
		{
			if (received_intr)
				break;

			strcpy (nm_point, last_nm (*argv));

			do_one_mv (*argv, path);
		}
	}

}	/* end do_mv */

/*
 ****************************************************************
 *	Move um arquivo						*
 ****************************************************************
 */
void
do_one_mv (const char *old_path, const char *new_path)
{
	DOSSTAT		z;

	/*
	 *	Se necessário, pede confirmação do usuario
	 */
	if   (cmd_iflag)
	{
		fprintf (stderr, "%s? (n) : ", old_path);

		if (askyesno () <= 0)
			return;
	}
	elif (cmd_vflag)
	{
		printf ("%s:\n", old_path);
	}

	/*
	 *	Obtém o estado do arquivo original
	 */
	if (dos_stat (old_path, &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, old_path, strerror (errno)
		);
		return;
	}

	/*
	 *	Certifica-se de que é não é um diretório
	 */
	if (Z_ISDIR (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" é um diretório\n",
			cmd_nm, old_path
		);
		return;
	}

	/*
	 *	Verifica se  é nome de VOLUME
	 */
	if (Z_ISVOL (z.z_mode))
	{
		if (dos_unlink (&z, 1 /* trunca */) < 0)
			return;

		mk_one_vol (new_path);
	}
	else
	{
		if (dos_link (&z, new_path) < 0)
		{
			printf
			(	"%s: Não consegui criar o nome \"%s\" (%s)\n",
				cmd_nm, new_path, strerror (errno)
			);
			return;
		}

		if (dos_unlink (&z, 0 /* Não trunca */) < 0)
			return;
	}

}	/* end do_one_mv */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mv_help (void)
{
	fprintf
	(	stderr,
		"%s - move (troca o nome) de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-ifv] <arquivo1> <arquivo2>\n"
		"\t%s [-ifv] <arquivo1> [<arquivo2> ...] <diretório>\n",
		cmd_nm, cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: Pede a confirmação para cada arquivo a mover\n"
		"\t-f: Os arquivos destino existentes são removidos "
			"silenciosamente\n"
		"\t-v: Lista o nome dos arquivos movidos\n"
	);

}	/* end do_mv_help */
