#include <sys/types.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	TMS	t;
	time_t	r;

	for (r = 0; r < 100000; r++)
		;

	r = times (&t);

	printf ("ret = %d\n", r);
	printf ("%d, %d, %d, %d\n",
		t.tms_utime,
		t.tms_stime,
		t.tms_cutime,
		t.tms_cstime);
}
