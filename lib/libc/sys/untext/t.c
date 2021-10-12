#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	if (untext (argv[1]) < 0)
		printf ("Erro: %s\n", strerror (errno ));
}
