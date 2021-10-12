/*
 ****************************************************************
 *								*
 *			do_touch.c				*
 *								*
 *	Atribui a data atual a arquivos				*
 *								*
 *	Versão	3.0.0, de 22.11.93				*
 *		3.0.0, de 10.06.95				*
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
#include <errno.h>
#include <time.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		simple_touch (const char *);
void		do_touch_help (void);

/*
 ****************************************************************
 *	Atribui a data atual a arquivos				*
 ****************************************************************
 */
void
do_touch (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_touch_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_touch_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc == 0)
		{ do_touch_help (); return; }

	/*
	 *	Processa os arquivos
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			break;

		simple_touch (*argv);
	}

}	/* end do_touch */

/*
 ****************************************************************
 *	Atribui a data atual a um arquivo			*
 ****************************************************************
 */
void
simple_touch (const char *path)
{
	DOSSTAT		z;

	/*
	 *	Obtém o estado do arquivo
	 */
	if (dos_stat (path, &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, path, strerror (errno)
		);
		return;
	}

	/*
	 *	Pôe a data atual
	 */
	put_dos_file_time (time ((time_t *)NULL), &z.z_d);

	/*
	 *	Reescreve o diretório modificado
	 */
	dos_dir_write (&z);

}	/* end simple_touch */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_touch_help (void)
{
	fprintf
	(	stderr,
		"%s - atribui a data atual a arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s <arquivo> ...\n",
		cmd_nm, cmd_nm
	);

}	/* end do_touch_help */
