
#include <sys/types.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	time_t	t, r;

	t = time (&r);

	printf ("%d, %d\n", t, r);
}
