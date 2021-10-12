#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

main ()
{
	int		 nevents, fd, eventid, eventdescr;
	int		 cr;
	char		 buf[100];

	fd = creat ("event", O_RDONLY);

	for (;;)
	{
		printf ("no. de eventos \n");
		nevents = atol (gets (buf));
		printf ("nevents = %d, fd = %d\n", nevents, fd);
		if (nevents == 0)
			break;

		cr = eventctl (UE_ALLOC, fd, nevents);
		printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);

		printf ("qual evento?\n");
		eventid = atol (gets (buf));

		cr = eventctl (UE_GET, fd, eventid);
		printf ("Descritor do primeiro evento = %08X, %d\n", cr, cr);
		eventdescr = cr;

		cr = eventtest (eventdescr);
		printf ("No. de eventos ocorridos = %08X, %d\n", cr, cr);

		printf ("eventdone\n");
		cr = eventdone (eventdescr);
		printf ("No. de eventos ocorridos = %08X, %d\n", cr, cr);

	}

	close (fd); unlink ("event");
}
