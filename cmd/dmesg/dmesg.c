/*
 ****************************************************************
 *								*
 *			dmesg.c					*
 *								*
 *	Imprime as mensagens iniciais de carga do núcleo	*
 *								*
 *	Versão	4.5.0, de 21.10.03				*
 *		4.5.0, de 24.03.04				*
 *								*
 *	Módulo: dmesg						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.5.0, de 24.03.04";

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
 *	Imprime as mensagens iniciais de carga do núcleo	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, size, count;
	char		*area;

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
			fprintf (stderr, "\n");
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Obtém e imprime as mensagens
	 */
	size = kcntl (K_DMESG, NOSTR, 0);

	area = alloca (size);

	if ((count = kcntl (K_DMESG, area, size))  < 0)
		error ("$*Erro ao ler a área de mensagens");

	if (vflag)
		fprintf (stderr, "A área contém %d caracteres dos quais %d preenchidos\n", size, count);

	write (1, area, count);

	return (0);

}	/* end dmesg */

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
		"%s - imprime as mensagens iniciais de carga do núcleo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
