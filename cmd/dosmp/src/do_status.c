/*
 ****************************************************************
 *								*
 *			do_status.c				*
 *								*
 *	Diversas funções de mudança de estado			*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.5, de 12.02.97				*
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
#include <string.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_iso_help (void);
void	do_bin_help (void);
void	do_ro_help (void);
void	do_rw_help (void);
void	do_lfn_help (void);
void	do_stat_help (void);

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
entry int	bin_mode = 0;	/* Modo binário ao invés ISO */
entry int	lfn_mode = 1;	/* == 0 => 8.3 * != 0 => 255 (W 95) */

/*
 ****************************************************************
 *	Altera a representação de dados para ISO		*
 ****************************************************************
 */
void
do_iso (int argc, const char *argv[])
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
			do_iso_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_iso_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_iso_help (); return; }

	bin_mode = 0;

}	/* end do_iso */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_iso_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o modo da representação de dados para ISO (ASCII)\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_iso_help */

/*
 ****************************************************************
 *	Altera a representação de dados para BINÁRIO		*
 ****************************************************************
 */
void
do_bin (int argc, const char *argv[])
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
			do_bin_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_bin_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_bin_help (); return; }

	bin_mode = 1;

}	/* end do_bin */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_bin_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o modo da representação de dados para BINÁRIO\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_bin_help */

/*
 ****************************************************************
 *	Permite apenas leituras no sistema de arquivos		*
 ****************************************************************
 */
void
do_ro (int argc, const char *argv[])
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
			do_ro_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_ro_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_ro_help (); return; }

	if (fs_status == FS_MOUNT_RW)
	{
	   /***	put_fat (); ***/

		fs_status = FS_MOUNT_RO;
	}

}	/* end do_ro */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_ro_help (void)
{
	fprintf
	(	stderr,
		"%s - permite apenas leituras no sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_ro_help */

/*
 ****************************************************************
 *	Permite leituras e escritas no sistema de arquivos	*
 ****************************************************************
 */
void
do_rw (int argc, const char *argv[])
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
			do_rw_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_rw_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_rw_help (); return; }

	if (rflag)
	{
		printf
		(	"%s: Não posso permitir escritas, "
			"pois foi dada a opção \"-r\"\n",
			cmd_nm
		);
		return;
	}

	if (dev_ro)
	{
		printf
		(	"%s: Não posso permitir escritas, "
			"pois \"%s\" está com proteção de escrita\n",
			cmd_nm, dev_nm
		);
		return;
	}

	if (fs_status == FS_MOUNT_RO)
		fs_status = FS_MOUNT_RW;

}	/* end do_rw */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_rw_help (void)
{
	fprintf
	(	stderr,
		"%s - permite leituras e escritas no sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_rw_help */

/*
 ****************************************************************
 *	Altera a criação de nomes de arquivos			*
 ****************************************************************
 */
void
do_lfn (int argc, const char *argv[])
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
			do_lfn_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_lfn_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_lfn_help (); return; }

	if   (streq (argv[0], "off") || streq (argv[0], "8.3"))
		lfn_mode = 0;
	elif (streq (argv[0], "on") || streq (argv[0], "255"))
		lfn_mode = 1;
	elif (streq (argv[0], "W95") || streq (argv[0], "w95"))
		lfn_mode = 1;
	else
		{ do_lfn_help (); return; }

}	/* end do_lfn */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_lfn_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o modo de criação dos nomes DOS\n"
		"\nSintaxe:\n"
		"\t%s [off|8.3] [on|255|w95|W95]\n",
		cmd_nm, cmd_nm
	);

}	/* end do_lfn_help */

/*
 ****************************************************************
 *	Informa a representação de dados corrente		*
 ****************************************************************
 */
void
do_stat (int argc, const char *argv[])
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
			do_stat_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_stat_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_stat_help (); return; }

	/*
	 *	Escreve o estado
	 */
	switch (fs_status) 
	{
	    case FS_CLOSED:
		printf
		(	"Não há dispositivo DOS disponível\n"
		);
		break;

	    case FS_OPEN:
		printf
		(	"O sistema de arquivos \"%s\" NÃO está montado\n",
			dev_nm
		);
		break;

	    case FS_MOUNT_RO:
	    case FS_MOUNT_RW:
		printf
		(	"Sistema de arquivos \"%s\" montado %s\n",
			dev_nm, fs_status == FS_MOUNT_RO ?
			"somente para LEITURAS" : "para leituras e ESCRITAS"
		);

		printf
		(	"Opção \"-r\" %s",
			rflag  ? "DADA\n" : "NÃO dada, "
		);

		if (!rflag) printf
		(	"dispositivo \"%s\" %s proteção de escrita\n",
			dev_nm, dev_ro ?  "COM" : "SEM"
		);

#if (0)	/*******************************************************/
		printf
		(	"Tabela de endereços dos clusters (FAT) %s\n",
			dirty_fat ? "NÃO atualizada" : "ATUALIZADA"
		);
#endif	/*******************************************************/

		break;

	    default:
		error ("Estado inválido: %d", fs_status);
		break;

	}	/* end switch */

	/*
	 *	Representação de dados
	 */
	printf
	(	"Modo de representação de dados %s\n",
		bin_mode ? "BINÁRIO" : "ISO (ASCII)"
	);

	/*
	 *	Criação de nomes nos diretórios
	 */
	printf
	(	"Modo de criação dos nomes DOS: Estilo %s\n",
		lfn_mode ? "Windows 95" : "tradicional (8.3)"
	);

}	/* end do_stat */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_stat_help (void)
{
	fprintf
	(	stderr,
		"%s - informa o estado do sistema de arquivos e a "
			"repres. de dados corrente\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_stat_help */
