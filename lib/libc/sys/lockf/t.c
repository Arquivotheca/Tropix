#include <sys/syscall.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

main ()
{
	register int	fd;
	int		fun;
	int		do_begin, do_end;

	fd = open ("../a.c", O_RDWR);

	for (;;)
	{
		printf ("função, do_begin, do_end => ");

		if (scanf ("%d%d%d", &fun, &do_begin, &do_end) < 0)
			exit (0);

		printf ("%d %d %d\t", fun, do_begin, do_end);

		if (do_end == 0)		/* do_end == 0 => sz = 0 */
			do_end = do_begin;

		lseek (fd, do_begin, 0);

		if (lockf (fd, fun, do_end-do_begin) < 0)
		{
			printf ("Erro no lockf\n");
			perror ("t");
		}
		else
		{
			printf ("Lockf executado\n");
		}
	}
}
