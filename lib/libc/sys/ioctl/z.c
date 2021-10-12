#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register int	fd;

	if ((fd = open (argv[1], 0)) < 0)
	{
		printf ("Erro no open: %s\n", strerror (errno) );
		fd = 18;
	}

	if (ioctl (fd, ZIP_IS_ZIP) < 0)
	{
		printf ("Errno no ioctl: %s\n", strerror (errno) );
		exit (1);
	}

	printf ("É ZIP\n");
}
