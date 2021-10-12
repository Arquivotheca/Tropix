/*
 ****************************************************************
 *								*
 *			do_show.c				*
 *								*
 *	Mostra um arquivo na tela do terminal			*
 *								*
 *	Versão	3.0.0, de 19.11.93				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		show_file (const char *);
void		do_show_help (void);

/*
 ****************************************************************
 *	Mostra um arquivo na tela do terminal			*
 ****************************************************************
 */
void
do_show (int argc, const char *argv[])
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
			do_show_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_show_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_show_help (); return; }

	show_file (*argv);

}	/* end do_show */

/*
 ****************************************************************
 *	Mostra um arquivos na tela do terminal			*
 ****************************************************************
 */
void
show_file (const char *path)
{
	int		n, fd;
	const char	*nm, *tmp_nm;
	char		area[BLSZ];
	DOSSTAT		z;
	DOSFILE		f;

	/*
	 *	Abre o arquivo
	 */
	if (dos_stat (nm = path, &z) < 0)
	{
		printf
		(	"%s: Não achei o arquivo DOS \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return;
	}

	if (!Z_ISREG (z.z_mode))
	{
		printf
		(	"%s: O arquivo DOS \"%s\" não é regular\n",
			cmd_nm, nm
		);
		return;
	}

	if (dos_open (&f, &z) < 0)
		return;

	/*
	 *	Obtém um nome temporário
	 */
	if ((tmp_nm = mktemp ("/tmp/dosmp.XXXXXX")) == NOSTR)
	{
		error ("Não consegui obter nome de arquivo temporário");
		return;
	}

	/*
	 *	Cria o arquivo
	 */
	if ((fd = creat (tmp_nm, 0666)) < 0)
	{
		printf
		(	"%s: Não consegui criar \"%s\" (%s)\n",
			cmd_nm, tmp_nm, strerror (errno)
		);
		free ((char *)tmp_nm); 		return;
	}

	/*
	 *	Percorre os blocos do arquivo
	 */
	while ((n = dos_read (&f, area, BLSZ)) > 0)
	{
		if (write (fd, area, n) != n)
		{
			fprintf
			(	stderr,
				"%s: Erro na escrita de \"%s\" (%s)\n",
				cmd_nm, tmp_nm, strerror (errno)
			);

			close (fd); 	unlink (tmp_nm);
			free ((char *)tmp_nm); 	return;
		}
	}

	close (fd);

	/*
	 *	Executa o "show"
	 */
	sprintf (area, "show %s", tmp_nm);

	system (area);

	/*
	 *	Remove o temporário
	 */
	unlink (tmp_nm); 	free ((char *)tmp_nm);

}	/* end show_file */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_show_help (void)
{
	fprintf
	(	stderr,
		"%s - mostra um arquivo DOS na tela do terminal\n"
		"\nSintaxe:\n"
		"\t%s <arquivo>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_show_help */
