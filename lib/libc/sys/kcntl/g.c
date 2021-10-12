#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	KPSZ		kpsz;

	if (kcntl (K_GETPSZ, &kpsz) == -1)
		printf ("Erro: %s\n", strerror (errno )); 

	printf
	(	"texto = %d, data+bss = %d, stack = %d\n",
		kpsz.k_tsize, kpsz.k_dsize, kpsz.k_ssize
	);
}
