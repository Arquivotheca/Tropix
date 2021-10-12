#include <sys/types.h>
#include <sys/sync.h>
#include <sys/ipc.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <fcntl.h>

#define ALLOC	0
#define GET	1

main ()
{
	int nevents, fd, eventid, eventordno, loop;
	int cr, i;
	int	event0, event1;

	loop = 12;
	nevents = 4;
	eventid = 0;
	eventordno = 1;

	fd = open ("/sync/event", O_RDONLY);

	if ((cr = eventctl (ALLOC, fd, nevents)) == -1)
	{
		printf ("Não consegui alocar os eventos\n");
		perror ("t");
		exit (1);
	}
	printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);
	sleep (1);

	if ((event0 = eventctl (GET, fd, eventid)) == -1)
	{
		printf ("Não consegui obter o descritor do evento %d\n",
			 eventid);
		exit (1);
	}
	printf ("Descritor do primeiro evento = %08X, %d\n", event0, event0);
	sleep (1);
	eventid++;
	if ((event1 = eventctl (GET, fd, eventid)) == -1)
	{
		printf ("Não consegui obter o descritor do evento %d\n",
			 eventid);
		exit (1);
	}
	printf ("Descritor do segundo evento = %08X, %d\n", event1, event1);
	sleep (1);

	for (i = 0; i < loop; i++)
	{
		printf ("processo %d detectou %d eventos já ocorridos\n",
			 getpid (), eventtest (event0));

		cr = eventwait (event0, eventordno);

		printf (" processo %d recebeu evento %d\n",
				getpid (), cr);

		sleep (1);

		cr = eventdone (event1);

		printf (" processo %d evento %d\n",
				getpid (), cr);

		eventordno++;
	}

	close (fd);
}
