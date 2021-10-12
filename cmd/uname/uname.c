/*
 ****************************************************************
 *								*
 *			uname.c					*
 *								*
 *	Imprime a identificação do sistema			*
 *								*
 *	Versão	1.0.0, de 09.02.86				*
 *		3.0.0, de 13.07.97				*
 *								*
 *	Módulo: uname						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 13.07.97";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Imprime a identificação do sistema			*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	UTSNAME		uts;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
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
	 *	Pequena consistência
	 */
	if (argc > 0)
		help ();

	/*
	 *	Obtém a identificação do sistema
	 */
	if (uname (&uts) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter a identificação do sistema (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Imprime a identificação do sistema (prosa)
	 */
	if (!vflag)
	{
		printf
		(	"%s %s - Versão %s de %s às %s em um %s (%s)\n",
			uts.uts_sysname, 	uts.uts_systype,
			uts.uts_version, 	uts.uts_date,
			uts.uts_time, 		uts.uts_machine,
			uts.uts_nodename
		);

#ifdef	ICA
		printf
		(	"\nNo. de série: %s,   Uso autorizado para: %s, %s\n",
			uts.uts_sysserno, 	uts.uts_customer,
			uts.uts_depto
		);
#endif	ICA

		exit (0);
	}

	/*
	 *	Imprime a identificação do sistema (estrutura)
	 */
	printf
	(	"sysname 	= %s\n"	/* Nome do Sistema */
		"systype 	= %s\n"	/* Tipo do Sistema */
		"nodename	= %s\n"	/* Nome do nó */
		"version 	= %s\n"	/* Versão */
		"date		= %s\n"	/* Data */
		"time		= %s\n"	/* Tempo */
		"machine 	= %s\n"	/* Computador */
		"customer	= %s\n"	/* Nome do Cliente */
		"depto		= %s\n"	/* Nome do Departamento */
		"sysserno	= %s\n",/* Numero de serie */

		uts.uts_sysname,	/* Nome do Sistema */
		uts.uts_systype,	/* Tipo do Sistema */
		uts.uts_nodename,	/* Nome do nó */
		uts.uts_version,	/* Versão */
		uts.uts_date,		/* Data */
		uts.uts_time,		/* Tempo */
		uts.uts_machine,	/* Computador */
		uts.uts_customer,	/* Nome do Cliente */
		uts.uts_depto,		/* Nome do Departamento */
		uts.uts_sysserno	/* Numero de serie do Sistema */
	);

	return (0);

}	/* end uname */

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
	 	"%s - imprime a identificação do sistema\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Formato da estrutura \"uts\"\n"
	);

	exit (2);

}	/* end help */
