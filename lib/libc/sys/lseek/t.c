#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register 	fd;
	register	n;
	char		buf[120];

	if ((fd = open (argv[1], 0)) < 0)
	{
		printf ("Não consegui abrir %s\n", argv[1]);
		printf ("t: %s\n", strerror (errno) );
		exit (1);
	}

	for (;;)
	{
		n = strtol (gets (buf), (const char **)NULL, 0);

		if (n < 0)
			break;

		if (lseek (fd, n, L_SET) < 0)
		{
			printf ("Erro no LSEEK em %d\n", n);
			printf ("s: %s\n", strerror (errno) );
		}
		else
		{
			printf ("Inicio = %d\n", n);
			read (fd, buf, 80);
			write (1, buf, 80);
		}
	}

}	/* end t */
