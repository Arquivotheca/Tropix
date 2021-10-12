#include <sys/types.h>
#include <stdio.h>
#include <stat.h>
#include <fcntl.h>

#define ALLOC	0
#define GET	1
#define AVAIL	2
#define	WAIT	3

main ()
{
	int nsemas, fd, semaid, semadescr, initval;
	int cr;
	char buf[100];
	int	i;

	fd = open ("/sync/sema", O_RDONLY);

again:
	printf ("no. de semáforos \n");
	nsemas = atol (gets (buf));
	printf ("nsemas = %d, fd = %d\n", nsemas, fd);

	cr = semactl (ALLOC, fd, nsemas);
	printf ("Código de retorno na alocação = %08X, %d\n", cr, cr);
	if (cr < 0)
		goto again;

	for (;;)
	{

	if (nsemas == 0)
		break;

	printf ("qual semáforo?\n");
	semaid = atol (gets (buf));
	printf ("qual valor inicial?\n");
	initval = atol (gets (buf));


	cr = semactl (GET, fd, semaid, initval);
	printf ("Descritor do primeiro semáforo = %08X, %d\n", cr, cr);
	semadescr = cr;
	if (cr < 0)
		continue;
/*
	printf ("sematestl\n");
	cr = sematestl (semadescr);
	printf ("No. de recursos ainda disponíveis = %08X, %d\n", cr, cr);
*/
	printf ("semalock\n");
	cr = semalock (semadescr);
	printf ("No. de recursos ainda disponíveis = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;

	cr = semactl (AVAIL, semadescr);
	printf ("No. de recursos ainda disponíveis (AVAIL) = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;
	cr = semactl (WAIT, semadescr);
	printf ("No. de processos esperando (WAIT) = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;

	sleep (1);
/*
	for (i = 0; i < 1000000; i++);
*/
	printf ("semafree\n");
	cr = semafree (semadescr);
	printf ("No. de recursos agora disponíveis = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;

	cr = semactl (AVAIL, semadescr);
	printf ("No. de recursos agora disponíveis (AVAIL) = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;
	cr = semactl (WAIT, semadescr);
	printf ("No. de processos esperando (WAIT) = %08X, %d\n", cr, cr);
	if (cr < 0)
		continue;

	}

	close (fd);
}
