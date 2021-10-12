/*
 ****************************************************************
 *								*
 *			tr0.c					*
 *								*
 *	Teste do TRACE -- PAI					*
 *								*
 *	Versão	1.0.0, de 11.02.88				*
 *		3.0.0, de 18.08.95				*
 *								*
 *	Módulo: KERNEL						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	*pgname;	/* Nome do programa */

int		vflag;		/* Verbose */

/*
 ******	Protótipos de funções ***********************************
 */
int	help (void);

/*
 ****************************************************************
 *	Programa de teste do TRACE				*
 ****************************************************************
 */
main (int argc, register const char *argv[])
{
	int		opt;
	int		pid, wpid;
	int		status;
	register int	i;

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

	/*
	 *	Cria o filho
	 */
	if ((pid = fork ()) != 0)
	{
		int		data;

		/*
		 *	Pai
		 */
		wpid = wait (&status);

		printf
		(	"PAI: retornou de wait: pid = %d, status = 0x%04X\n",
			wpid,
			status
		);

/********************************
		data = ptrace (1, pid, 0, 0);

		printf ("PAI: recebi o valor 0x%08X\n", data);
********************************/

		for (;;)
		{
			char		buf[30];
			int		req, addr, data;

			printf ("Req: ");
			fflush (stdout);

			if (gets (buf) != buf)
				exit (0);

			req = strtol (buf, (const char **)NULL, 0);

			switch (req)
			{
			    case 1:
			    case 2:
			    case 3:
				printf ("Addr: ");
				fflush (stdout);

				if (gets (buf) != buf)
					exit (0);

				addr = strtol (buf, (const char **)NULL, 0);

				data = ptrace (req, pid, addr, 7777);

				printf
				(	"PAI: Addr = 0x%06X, data = 0x%08X\n",
					addr ,data
				);
				break;

			    case 4:
			    case 5:
			    case 6:
				printf ("Addr: ");
				fflush (stdout);

				if (gets (buf) != buf)
					exit (0);

				addr = strtol (buf, (const char **)NULL, 0);

				printf ("Data: ");
				fflush (stdout);

				if (gets (buf) != buf)
					exit (0);

				data = strtol (buf, (const char **)NULL, 0);

				data = ptrace (req, pid, addr, data);

				printf
				(	"PAI: Addr = 0x%06X, data = 0x%08X\n",
					addr, data
				);
				break;

			    case 7:
			    case 9:
				data = ptrace (req, pid, 1, 0);

				printf
				(	"PAI: Addr = 0x%06X, data = 0x%08X\n",
					addr, data
				);
				break;

			    case 8:
				data = ptrace (req, pid, 1, 0);

				printf
				(	"PAI: Addr = 0x%06X, data = 0x%08X\n",
					addr, data
				);

				wpid = wait (&status);

				printf
				(	"PAI: retornou de wait: pid = %d, status = 0x%04X\n",
					wpid,
					status
				);

				break;

			    case 16:
				for (i = 0; i < 4096; i++)
				{
				    if (ptrace (6, pid, i, 0) >= 0)
				    {
					printf
					(	"Consegui escrever: %d\n",
						i
					);
				    }
				}
				break;

			    default:
				printf ("PAI: Req inválido\n");
				continue;

			}	/* end switch */

		}	/* end for EVER */

		data = ptrace (7, pid, 0, 0);

		printf ("PAI: mandei executar; recebi o valor 0x%08X\n", data);
	}
	else
	{
		/*
		 *	Filho
		 */
		ptrace (0, 0, 0, 0);

		printf ("FILHO: vai dar exec\n");

		execl ("tr1", "tr1", (char *)NULL);

		printf ("FILHO: Erro no exec\n");
		perror ("FILHO");
	}
	
	exit (0);

}	/* end tr0 */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
int
help (void)
{
	fprintf (stderr, "%s - programa.\n", pgname);
	fprintf (stderr, "\tVersão: 1.0 de x.88.\n");
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-opções] arg\n", pgname);
	fprintf (stderr, "\nOpções:\t-x: opção.\n");
	fprintf (stderr, "\t-y: opção.\n");
	fprintf (stderr, "\nObs.:\tobservação.\n");
	exit (2);

}	/* end help */
