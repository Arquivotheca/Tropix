/*
 ****************************************************************
 *								*
 *			do_sys.c				*
 *								*
 *	Torna um sistema DOS carregável				*
 *								*
 *	Versão	4.6.0, de 13.05.04				*
 *		4.6.0, de 17.05.04				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
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
 ******	Variáveis globais ***************************************
 */
entry char	sys_dir[] = ".";

/*
 ******	Protótipos de funções ***********************************
 */
int		sys_ftw_function (const char *, STAT *, int);
int		dos_verdir (const char *);
void		do_sys_help (void);

extern int	cmp_one_file (const char *, int, STAT *, DOSSTAT *);

/*
 ****************************************************************
 *	Copia árvores TROPIX para o sistema DOS			*
 ****************************************************************
 */
void
do_sys (int argc, const char *argv[])
{
	int		opt, old_bin_mode;
	const char	*dir_str = sys_dir;
	char		*path, *cp_point;
	DOSSTAT		z;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "d:vH")) >= 0)
	{
		switch (opt)
		{
		    case 'd':			/* Diretório dos arquivos */
			dir_str = cmd_optarg;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_sys_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_sys_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_sys_help (); return; }

	/*
	 *	Prepara o caminho
	 */
	path = alloca (strlen (dir_str) + 1 + 8 + 1 + 3 + 1);

	strcpy (path, dir_str); cp_point = strend (path);

	/*
	 *	Copia os arquivos
	 */
	old_bin_mode = bin_mode; bin_mode = 1;

	if (cmd_vflag)
		printf ("/IO.SYS\n");

	strcpy (cp_point, "/IO.SYS");	

	if (put_one_file (path, 0) < 0)
		return;

	if (cmd_vflag)
		printf ("/DRVSPACE.BIN\n");

	strcpy (cp_point, "/DRVSPACE.BIN");

	if (put_one_file (path, 0) < 0)
		return;

	if (cmd_vflag)
		printf ("/MSDOS.SYS\n");

	strcpy (cp_point, "/MSDOS.SYS");

	if (put_one_file (path, 0) < 0)
		return;

	if (cmd_vflag)
		printf ("/COMMAND.COM\n");

	strcpy (cp_point, "/COMMAND.COM");

	if (put_one_file (path, 0) < 0)
		return;

	bin_mode = old_bin_mode;

	/*
	 *	Acerta as proteções
	 */
	if (dos_stat ("/IO.SYS", &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, "IO.SYS", strerror (errno)
		);
	}

	z.z_mode = Z_SYS|Z_HIDDEN|Z_RO;		dos_dir_write (&z);

	if (dos_stat ("/DRVSPACE.BIN", &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, "DRVSPACE.BIN", strerror (errno)
		);
	}

	z.z_mode = Z_SYS|Z_HIDDEN|Z_RO;		dos_dir_write (&z);

	if (dos_stat ("/MSDOS.SYS", &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, "MSDOS.SYS", strerror (errno)
		);
	}

	z.z_mode = Z_SYS|Z_HIDDEN|Z_RO;		dos_dir_write (&z);

	if (dos_stat ("/COMMAND.COM", &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, "COMMAND.COM", strerror (errno)
		);
	}

	z.z_mode = Z_RO;			dos_dir_write (&z);

}	/* end do_sys */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_sys_help (void)
{
	fprintf
	(	stderr,
		"%s - copia os arquivos necessário para tornar o sistema DOS carregável\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-d <dir>]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Escreve os nomes dos arquivos gravados\n"
		"\t-d: Dá o diretório contendo os arquivos (normalmente \"%s\")\n",
		sys_dir
	);

}	/* end do_sys_help */
