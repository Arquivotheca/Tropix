/*
 ****************************************************************
 *								*
 *			nice.c					*
 *								*
 *	Executa um comando com prioridade modificada		*
 *								*
 *	Versão	1.0.0, de 30.07.87				*
 *		3.0.0, de 01.06.97				*
 *								*
 *	Módulo: nice						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 01.06.97";

#define	DEFINC	10		/* Incremento padrão */
#define	MAXINC	20		/* Incremento máximo */

#define	elif	else if

#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Executa um comando com prioridade modificada		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register const char	*cp;
	const char		*inc_str = NOSTR;
	register int		incr = DEFINC;
	int			plus = 0;
	const char		*str;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "p:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'p':			/* Incremento dado */
			if (inc_str)
				help ();
			inc_str = optarg;
			break;

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
	 *	Analisa o incremento dado
	 */
	if ((cp = inc_str) != NOSTR)
	{
		if   (cp[0] == '+')
			{ plus++; cp++; }
		elif (cp[0] == '-')
			{ cp++; }

		incr = strtol (cp, &str, 0);

		if (*str != '\0' || (unsigned)incr > MAXINC)
		{
			fprintf
			(	stderr,
				"Incremento inválido: \"%s\"\n\n",
				inc_str
			);
			help ();
		}

		if (plus)
			incr = -incr;

	}	/* end if (inc_str != NOSTR) */

	/*
	 *	Altera a prioridade
	 */
	if (argv[0] == NOSTR)
		help ();

	if (nice (incr) < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro na chamada ao sistema \"nice\" (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Executa o comando dado
	 */
	execvp (argv[0], &argv[0]);

	fprintf
	(	stderr,
		"%s: Não consegui executar \"%s\" (%s)\n",
		pgname, argv[0], strerror (errno)
	);

	return (2);

}	/* end nice */

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
		"%s - executa um comando com prioridade modificada\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-p [+]<inc>] <comando> [<opções>] [<argumentos>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-p: Decrementa a prioridade de <inc> "
			"(ao invés de " XSTR (DEFINC) ").\n"
		"\t    Usando o \"+\", a prioridade será "
			"incrementada de <inc>\n"
		"\t    (somente para superusuários)\n"
		"\nObs.:\tO valor de <inc> deve ser no máximo " XSTR (MAXINC)
			"\n"
	);

	exit (2);

}	/* end help */
