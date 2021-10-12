#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

main ()
{
	int		j;
	int		nsemas, fd, semaid, semadescr, initval;
	int		cr;

	nsemas = 1;
	semaid = 0;
	initval = 2;

	fd = open ("Makefile", O_RDONLY);
	printf ("fd = %d\n", fd);

	if ((cr = semactl (US_ALLOC, fd, nsemas)) == -1)
	{
		printf ("Não consegui alocar os semáforos\n");
		exit (1);
	}
	printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);

	if ((cr = semactl (US_GET, fd, semaid, initval)) == -1)
	{
		printf ("Não consegui obter o descritor do semáforo%d\n",
			 semaid);
		exit (1);
	}
	printf ("Descritor do primeiro semáforo = %08X, %d\n", cr, cr);
	semadescr = cr;

	for (j = 0; j < 10; j++)
	{
		while ((cr = sematestl (semadescr)) < 0)
		{
		printf (" processo %d não consegui o recurso e tinham %d\n",
			getpid (), cr);
			sleep (2);
		}

		printf (" processo %d recebeu o recurso e ainda tinham %d\n",
				getpid (), cr);

		sleep (5);

		cr = semafree (semadescr);

		printf (" processo %d liberou o recurso e agora tinham %d\n",
				getpid (), cr);

	}

	close (fd);
}
