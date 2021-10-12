#include <sys/types.h>
#include <stdio.h>
#include <stat.h>
#include <fcntl.h>
#include <errno.h>

#define ALLOC	0
#define GET	1

main ()
{
	int i, j;
	int nsemas, fd, initval;
	int semadescr[30];
	int cr;

	printf ("no. de semáforos? ");
	scanf ("%d", &nsemas);
	initval = 2;

	fd = open ("/sync/sema", O_RDONLY);
	printf ("fd = %d\n", fd);

	if ((cr = semactl (ALLOC, fd, nsemas)) == -1)
	{
		printf ("Não consegui alocar os semáforos\n");
		perror ("musemas");
		exit (1);
	}
	printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);

	for (i = 0; i < nsemas ; i++)
	{
	if ((cr = semactl (GET, fd, i, initval)) == -1)
	{
		printf ("Não consegui obter o descritor do semáforo%d\n",
			 i);
		perror ("musemas");
		exit (1);
	}
	printf ("Descritor do primeiro semáforo = %08X, %d\n", cr, cr);
	semadescr[i] = cr;
	}

	for (j = 0; j < 10; j++)
	{
	for (i = 0; i < nsemas; i++)
	{
		cr = semalock (semadescr[i]);

		printf (" processo %d recebeu o recurso e ainda tinham %d\n",
				getpid (), cr);
	}

		sleep (5);

	for (i = 0; i < nsemas; i++)
	{
		cr = semafree (semadescr[i]);

		printf (" processo %d liberou o recurso e agora tinham %d\n",
				getpid (), cr);
	}
	}

	close (fd);
}
