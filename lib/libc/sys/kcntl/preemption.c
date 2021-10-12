#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register int	flag, r;
	const char	*p;
	char		buf[80];

	while (gets (buf) == buf)
	{
		flag = strtol (buf, &p, 0);

		if ((r = kcntl (K_SETPREEMPTION, flag, 0 /* CPU = 0 */)) < 0)
			printf ("Erro: %s\n", strerror (errno )); 

		printf ("%d => %d\n", flag, r);

	}
}

