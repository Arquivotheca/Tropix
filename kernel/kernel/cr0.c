#include <sys/syscall.h>
#include <sys/kcntl.h>
#include <stdio.h>

int
main ()
{
	printf ("cr0 = %P\n", kcntl (K_CR0, 0));

	return (0);
}
