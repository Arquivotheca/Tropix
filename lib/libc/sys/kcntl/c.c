#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <id.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	i;
	char		buf[80];

	while (gets (buf) == buf)
	{
		if ((i = kcntl (K_GETDEV, buf)) < 0)
			printf ("Erro: ret = %X, %s\n", i, strerror (errno )); 

		printf ("(%d, %d): %s\n", i >> 8, i & 0xFF, buf);
	}
}

