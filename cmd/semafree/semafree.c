/*
 ****************************************************************
 *								*
 *			semafree.c				*
 *								*
 *	Libera o semáforo pelo qual o processo está esperando	*
 *								*
 *	Versão	2.1.0, de 03.12.88				*
 *		3.0.0, de 30.06.97				*
 *								*
 *	Módulo: semafree					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0, de 30.06.97";

#define	NOSTR	(char *)NULL
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
 *	Libera o semáforo pelo qual o processo está esperando	*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	int		opt, pid;
	const char	*str;

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
	 *	Retira o número do processo
	 */
	if (argc != 1)
		help ();

	if ((pid = strtol (argv[0], &str, 0)) < 0 || *str != '\0')
	{
		fprintf
		(	stderr,
			"%s: Número de processo \"%s\" inválido\n",
			pgname, argv[0]
		);
		exit (1);
	}

	/*
	 *	Tenta liberar o recurso
	 */
	if (kcntl (K_SEMAFREE, pid) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui liberar o recurso (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end semafree */

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
		"%s - libera o semáforo pelo qual o processo está esperando\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] <pid>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: verboso\n"
	);

	exit (2);

}	/* end help */
