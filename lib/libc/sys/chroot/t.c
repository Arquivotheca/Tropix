#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	err, ret;

	chdir ("/bin");

	ret = chroot (argv[1]);

	err = errno;

	printf ("ret = %d\n", ret);

	if (ret < 0)
		printf ("%s: %s\n", argv[1], strerror (err) );

	execl ("sh", "sh", 0);

	printf ("Erro no execl\n");

} 
