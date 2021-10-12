#include <sys/common.h>
#include <sys/dir.h>
#include <sys/uerror.h>
#include <sys/uproc.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int	errno;

main (int argc, register const char *argv[])
{
	UPROC		u;
	char		mmu[5 * 4096];

	if (kcntl (K_GETMMU, &u, &mmu) == -1)
		printf ("Erro: %s\n", strerror (errno )); 

}
