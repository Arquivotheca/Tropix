/*
 ****************************************************************
 *								*
 *			tty.c					*
 *								*
 *	Imprime o nome do terminal/vídeo			*
 *								*
 *	Versão	2.0.0, de 20.01.87				*
 *		3.0.0, de 13.07.97				*
 *								*
 *	Módulo: tty						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0, de 13.07.97";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	sflag;		/* Silence */
entry int	vflag;		/* Verboso */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Imprime o nome do terminal/vídeo			*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	const char	*tty_name;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "svH")) != EOF)
	{
		switch (opt)
		{
		    case 's':			/* Modo silencioso */
			sflag++;
			break;

		    case 'v':			/* Verboso */
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
	 *	Modo silencioso
	 */
	if (sflag)
	{
		int	code = isatty (0) ? 0 : 1;

		if (vflag)
			printf ("Código de retorno = %d\n", code);

		exit (code);
	}

	/*
	 *	Obtém o nome do terminal
	 */
	if ((tty_name = ttyname (2)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Terminal/vídeo desconhecido\n",
			pgname
		);
		exit (1);
	}

	printf ("%s\n", tty_name);

	return (0);

}	/* end tty */

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
		"%s - imprime o nome do terminal/vídeo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-s]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: Fornece apenas o código de retorno\n"
		"\t    (0 se \"stdin\" é um terminal, 1 em caso contrário)\n"
	);

	exit (2);

}	/* end help */
