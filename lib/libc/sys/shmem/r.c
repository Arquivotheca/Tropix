#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

main ()
{
	int	nevents, event_fd, shmem_fd, size;
	int	event0, event1;
	int	*regaddr0;
	int	i;

	nevents = 4; size = 4096; i = 0;

	if ((event_fd = open ("eventfile", O_RDWR)) < 0)
	{
		printf ("Não pude abrir \"eventfile\"\n");
		perror ("receive");
		exit (1);
	}

	if ((shmem_fd = open ("shmemfile", O_RDWR)) < 0)
	{
		printf ("Não pude abrir \"shmemfile\"\n");
		perror ("receive");
		exit (1);
	}

	if (eventctl (UE_ALLOC, event_fd, nevents) == -1)
	{
		sleep (2);
		printf ("Não consegui alocar os eventos\n");
		perror ("receive");
		exit (1);
	}

	if ((int)(regaddr0 = shmem (shmem_fd, size)) == -1)
	{
		printf ("r: Não consegui obter a memória compartilhada (%s)\n", strerror (errno));
		exit (1);
	}

	if ((event0 = eventctl (UE_GET, event_fd, 0)) == -1)
	{
		sleep (2);
		printf ("Não consegui obter o descritor do evento 0\n");
		exit (1);
	}

	if ((event1 = eventctl (UE_GET, event_fd, 1)) == -1)
	{
		sleep (2);
		printf ("Não consegui obter o descritor do evento 1\n");
		exit (1);
	}
	sleep (2);
	printf ("Endereço do início da região 0 = %P\n", regaddr0);
	sleep (2);

	printf ("Lendo da região \n");
	sleep (2);

	for (;;)
	{
		eventwait (event0, ++i);

		printf ("Na região tinha %08X\n", *regaddr0);

		sleep (1);
	
		eventdone (event1);

	}

	sleep (1);
	close (event_fd);
	close (shmem_fd);
}
