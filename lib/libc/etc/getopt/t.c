
#include <stdio.h>

char		*pgname;	/* Nome do programa */

int		vflag;		/* Verbose */

extern int	optind;		/* Incremento do argv (em getopt) */

extern char	*optarg;	/* Argumento (de getopt) */

main (argc, argv)
char *argv[];
{
	register int	opt;

	pgname = argv[0];

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "vf:H")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Arquivo */
			printf ("optarg = \"%s\"\n", optarg);
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    case '?':			/* Inválido */
			printf ("Opção com algum erro : \"%c\"\n", *optarg);
			help ();

		    default:			/* Erro */
			help ();
		}

	}

	argv += optind;

	printf ("Primeiro argumento = \"%s\"\n", *argv);

}	/* end x */

help ()
{
	fprintf (stderr, "%s - programa\n", pgname);
	exit (2);

}	/* end help */
