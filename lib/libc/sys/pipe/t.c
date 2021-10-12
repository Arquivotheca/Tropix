#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	int		fd[2];
	char		b[8];
	int		pidfilho;

	if (pipe (fd) < 0)
		printf ("Erro no PIPE: %s\n", strerror (errno ));

	if ((pidfilho = fork ()) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui dar criar um processo\n"
		);
		exit (1);
	}

	if (pidfilho > 0)
	{
		/* Pai */

		close (fd[0]);
		sleep (3);
		write (fd[1], "ATENÇÃO\n", 8);
	}
	else
	{
		/* Filho */

		close (fd[1]);
		read  (fd[0], b, 8);
		write (1, b, 8);
	}

	exit (0);
}
