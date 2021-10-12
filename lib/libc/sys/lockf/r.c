#include <sys/syscall.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

main ()
{
	register int	fd;
	char		buf[50];
	int		do_wait;
	int		the_begin, the_end;

	printf ("test: 0, wait: 1 => "); scanf ("%d", &do_wait);

	fd = open ("../a.c", do_wait ? O_RDONLY : O_RDONLY|O_NDELAY);

	for (;;)
	{
		printf ("início, final => ");
		if (scanf ("%d%d", &the_begin, &the_end) < 0)
			exit (0);
		printf ("%d %d\n", the_begin, the_end);

		lseek (fd, the_begin, 0);

		if (read (fd, buf, the_end-the_begin) < 0)
		{
			printf ("Erro no read\n");
			perror ("r");
		}
		else
		{
			printf ("\n\"");
			fflush (stdout);

			write (1, buf, the_end-the_begin);

			printf ("\"\n");
			fflush (stdout);
		}
	}
}
