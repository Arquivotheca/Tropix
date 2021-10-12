#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/disktb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	M, m;
	const char	*p;
	struct disktb	d;
	char		buf[80];

	while (gets (buf) == buf)
	{
		M = strtol (buf, &p, 0);
		m = strtol (p+1, (const char **)0, 0);

		if (kcntl (K_GET_DEV_TB, (M << 8)|m, &d) < 0)
			printf ("Erro: %s\n", strerror (errno )); 
		else
			printf
			(	"(%d, %d): \"%s\", sz = %d, off = %d, \n",
				M, m, d.p_name, d.p_size, d.p_offset
			);

	}

	exit (0);
}

