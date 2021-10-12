#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	int		fd[2];
	char		b[8];
	int		pidfilho, i = -1;
	int		fd_vec[1];

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
#if (0)	/*******************************************************/
#endif	/*******************************************************/
		write (fd[1], "ATENÇÃO\n", 8);
	}
	else
	{
		/* Filho */

		close (fd[1]);
		fd_vec[0] = fd[0];

#if (0)	/*******************************************************/
		sleep (3);
#endif	/*******************************************************/

		if ((i = attention (1, fd_vec, i, 0)) < 0)
		{
			fprintf
			(	stderr,
				"Erro na ATTENTION (%s)\n",
				strerror (errno)
			);
			exit (1);
		}

		printf
		(	"ATTENTION retornou %d, _attention_index = %d\n",
			i, _attention_index
		);

		read  (fd_vec[i], b, 8);
		write (1, b, 8);
	}

	exit (0);
}
