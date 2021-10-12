#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	M, m;
	const char	*p;
	char		id[16];
	char		buf[80];

	while (gets (buf) == buf)
	{
		M = strtol (buf, &p, 0);
		m = strtol (p+1, (const char **)0, 0);

		if (kcntl (K_GETDEVNM, (M << 8)|m, id) == -1)
			printf ("Erro: %s\n", strerror (errno )); 

		printf ("(%d, %d): %s\n", M, m, id);

	}
}

