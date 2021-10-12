#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <termio.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register int	fd;
	TERMIO		omode, mode;

	if ((fd = open (argv[1], 0)) < 0)
	{
		printf ("Erro no open: %s\n", strerror (errno) );
		fd = 18;
	}

	if (ioctl (fd, TCGETS, &omode) < 0)
	{
		printf ("Errno no ioctl (1): %s\n", strerror (errno) );
		exit (1);
	}

	mode = omode;

	mode.c_iflag |= IGNCR;

	if (ioctl (fd, TCSETAW, &mode) < 0)
	{
		printf ("Errno no ioctl (2): %s\n", strerror (errno) );
		exit (1);
	}

}
