#include <sys/types.h>
#include <stdio.h>
#include <stat.h>
#include <fcntl.h>

#define ALLOC	0
#define GET	1

main ()
{
	int i, j;
	int nsemas, semaid, initval;
	int fd[10], semadescr[10];
	int cr;
	char buf[100];

	nsemas = 1;
	semaid = 0;
	initval = 2;

	fd[0] = open ("/sync/sema0", O_RDONLY);
	fd[1] = open ("/sync/sema1", O_RDONLY);
	fd[2] = open ("/sync/sema2", O_RDONLY);
	fd[3] = open ("/sync/sema3", O_RDONLY);
	fd[4] = open ("/sync/sema4", O_RDONLY);
	fd[5] = open ("/sync/sema5", O_RDONLY);
	fd[6] = open ("/sync/sema6", O_RDONLY);
	fd[7] = open ("/sync/sema7", O_RDONLY);
	fd[8] = open ("/sync/sema8", O_RDONLY);
	fd[9] = open ("/sync/sema9", O_RDONLY);

	for (i = 0; i <10; i++)
	{
		if ((cr = semactl (ALLOC, fd[i], nsemas)) == -1)
		{
			printf ("Não consegui alocar os semáforos\n");
			exit (1);
		}
		printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);
	}

	for (i = 0; i <10; i++)
	{
		if ((cr = semactl (GET, fd[i], semaid, initval)) == -1)
		{
			printf ("Não consegui obter o descritor do semáforo%d\n",
				 semaid);
			exit (1);
		}
		printf ("Descritor do primeiro semáforo = %08X, %d\n", cr, cr);
		semadescr[i] = cr;
	}

	for (j = 0; j < 5; j++)
	{
		for (i = 0; i <10; i++)
		{
			cr = semalock (semadescr[i]);
	
			printf (" processo %d recebeu o recurso e ainda tinham %d\n",
					getpid (), cr);
		}

		sleep (10);

		for (i = 0; i <10; i++)
		{
			cr = semafree (semadescr[i]);
	
			printf (" processo %d liberou o recurso e agora tinham %d\n",
					getpid (), cr);
		}
	}

	for (i = 0; i <10; i++)
	{
		sleep (1);
		close (fd[i]);
	}
}
