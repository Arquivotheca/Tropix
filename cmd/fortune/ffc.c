/*
 ****************************************************************
 *								*
 *			ffc.c					*
 *								*
 *	Fortune format check			 		*
 *								*
 *	Versão	2.0.0, de 10.06.88				*
 *		2.3.0, de 07.09.89				*
 *								*
 *	Módulo: Fortune						*
 *		Utilitários absolutamente supérfluos		*
 *		Categoria B					*
 *								*
 *	PLURIX: Sistema Operacional Multiprocessado		*
 *		Um desenvolvimento do NCE/UFRJ			*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL

char		*pgname;	/* Nome do programa */

char		*pgversion =  "Versão:	2.3.0, de 07.09.89";

int		vflag;		/* Verbose */

extern int	optind;		/* Incremento do argv (em getopt) */

extern char	*optarg;	/* Argumento (de getopt) */

/*
 ****************************************************************
 *	Estado do automata					*
 ****************************************************************
 */
typedef enum
{
	IDLE,		/* Começo */
	INFOR,		/* A linha é no interior de uma FORTUNE */
	NL1,		/* A linha anterior é <nl> */
	NL2		/* Viu dois ou mais <nl> */

}	STATE;

/*
 ****************************************************************
 *	Programa de Analisar o formato do arquivo FORTUNE	*
 ****************************************************************
 */
main (argc, argv)
char *argv[];
{
	register int	opt;
	register FILE	*fp;
	register STATE	state;
	char		line[128];
	int		count;
	int		lineno;

	pgname = argv[0];

	/*
	 *	Analisa as opções.
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
		}

	}

	argv += optind;
	argc -= optind;

	if ((fp = fopen (argv[0], "r")) == NOFILE)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", argv[0]);
		exit (1);
	}

	state = IDLE;
	count = 0;
	lineno = 0;

	while (fngets (line, 128, fp) != NOSTR)
	{
		lineno++;

		if (line[0] != '\0')
		{
			/*
			 *	Linha de FORTUNE
			 */ 
			switch (state)
			{
			    case IDLE:
			    case NL2:
				count++;
				state = INFOR;
				break;

			    case INFOR:
				break;

			    case NL1:
				state = INFOR;
				break;

			}	/* end switch */
		}
		else
		{
			/*
			 *	Linha Vazia
			 */
			switch (state)
			{
			    case IDLE:
			    case INFOR:
				state = NL1;
				break;

			    case NL1:
				state = NL2;
				break;

			    case NL2:
				printf
				(	"Mais de 2 <nl> seguidos, linha %d\n",
					lineno
				);
				break;

			}	/* end switch */

		}	/* end if */

	}	/* end while */

	printf ("Count = %d\n", count);

}	/* end ffc */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
help ()
{
	fprintf (stderr, "%s - fortune format check\n", pgname);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-v] <fortunes fonte>\n", pgname);
	fprintf (stderr, "\nOpções:\t-v: verboso\n");
	fprintf (stderr, "\nObs.:\tobservação\n");
	exit (2);

}	/* end help */
