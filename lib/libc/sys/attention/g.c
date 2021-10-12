
#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>

const char *pgname;

main (int argc, register const char *argv[])
{
	register int	r, fd, index = -1;
	const char	*fnm1 = "/dev/tty";
	const char	*fnm2 = "/dev/grave";
	int		fd_vector[20];
	int		pidfilho;
	char		area[80];

	pgname = argv[0];

	if (argv[1])
	{
		fnm1 = argv[1];

		if (argv[2])
			fnm2 = argv[2];
	}

	if ((fd_vector[0] = open (fnm1, O_RDONLY)) < 0)
	{
		printf
		(	"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, fnm1, strerror (errno)
		);
		exit (1);
	}

	if ((fd_vector[1] = open (fnm2, O_RDONLY)) < 0)
	{
		printf
		(	"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, fnm2, strerror (errno)
		);
		exit (1);
	}

	printf ("fnm1 = \"%s\", fd[0] = %d\n", fnm1, fd_vector[0]);
	printf ("fnm2 = \"%s\", fd[1] = %d\n", fnm2, fd_vector[1]);

	/*
	 *	Cria um filho
	 */
	if ((pidfilho = fork ()) < 0)
	{
		printf
		(	"%s: Não consegui criar um filho (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	O filho
	 */
	if (pidfilho == 0)
	{
		printf ("FILHO executando\n");
		sleep (5);
		printf ("FILHO terminando\n");
		exit (0);
	}
	else
	{
		printf ("Criei o FILHO %d\n", pidfilho);
	}


   /***	sleep (5); ***/

	/*
	 *	Dá o ATTENTION
	 */
	for (;;)
	{
		if ((index = attention (2, fd_vector, index, 0)) < 0)
		{
			printf ("Erro no attention\n");
			perror (argv[0]);
			exit (1);
		}

		fd = fd_vector[index];

		printf ("attention retornou index = %d, fd = %d\n", index, fd);

		if (index == 0)
		{
			if ((r = read (fd, area, 80)) < 0)
			{
				printf
				(	"%s: Não consegui ler \"%s\" (%s)\n",
					pgname, fnm1, strerror (errno)
				);
				exit (1);
			}

			if (r == 0)
				{ printf ("EOF\n"); exit (0); }

			write (1, area, r);
		}
		else	/* GRAVE */
		{
			printf ("Vou dar o WAIT\n");
			pidfilho = wait ((int *)NULL);
			printf ("Dei o WAIT, filho = %d\n", pidfilho);
		}

	}	/* end for EVER */

}
