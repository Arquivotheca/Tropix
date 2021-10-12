#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	if (unlink ("bobo") < 0)
		printf ("Erro: %s\n", strerror (errno ));
}
