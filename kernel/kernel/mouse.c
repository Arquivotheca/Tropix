#include <sys/types.h>
#include <sys/syscall.h>

#define	USE_SELECT

#ifdef	USE_SELECT
#include <sys/times.h>
#include <sys/select.h>
#endif	USE_SELECT

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>

int	fd;
TERMIO	t;
uchar	buf[1024];

int
main (int argc, const char *argv[])
{
	int	i, nb, err;
#ifdef	USE_SELECT
	fd_set	fdmask;
#endif	USE_SELECT

	if (argc == 1)
		return (0);

	if ((fd = open (argv[1], 0)) < 0)
	{
		printf ("Erro no open\n");
		exit (1);
	}

	if (ioctl (fd, TCGETS, &t) < 0)
	{
		printf ("Erro no TCGETS\n");
		exit (1);
	}

	printf ("fifo = %d\n", ioctl (fd, TCFIFOSZ, 0));

	if (argc > 2)
	{
		if (ioctl (fd, TCFIFOSZ, atol (argv[2])) < 0)
		{
			printf ("Erro no TCFIFOSZ\n");
			exit (1);
		}

		printf ("fifo = %d\n", ioctl (fd, TCFIFOSZ, 0));
	}

	t.c_oflag = 0;
	t.c_lflag = ICOMM;
	t.c_cflag = B1200 | CS7 | CREAD | CLOCAL | HUPCL;

	if (ioctl (fd, TCSETS, &t) < 0)
	{
		printf ("Erro no TCSETS\n");
		exit (1);
	}

#ifdef	USE_SELECT
	FD_ZERO (&fdmask);	FD_SET (fd, &fdmask);

	while ((err = select (fd + 1, &fdmask, NULL, NULL, NULL)) == 1)
#else
	while ((err = attention (1, &fd, -1, 0)) == 0)
#endif	USE_SELECT
	{
		printf ("Avail: %3d | ", ioctl (fd, TCNREAD));

		if ((nb = read (fd, buf, sizeof (buf))) > 0)
		{
			printf ("Read: %3d | ", nb);

			for (i = 0; i < nb; i++)
			{
				printf ("%02X ", buf[i]);

				if (buf[i] == 0x80)
					printf ("?? ");
			}
		}

		putchar ('\n');
	}

	if (err < 0)
		printf ("Erro na attention\n");

	close (fd);

	return (0);
}
