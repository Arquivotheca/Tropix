#include <sys/common.h>
#include <sys/shlib.h>

#include <stdio.h>
#include <errno.h>

int
main ()
{
	const char	*nm = "/usr/lib/libt.o";

	if (shlib (nm, 0) < 0)
		printf ("Erro em shlib de \"%s\": %s\n", nm, strerror (errno) );

	return (0);
}
