#include <sys/types.h>
#include <sys/times.h>

#include <stdio.h>

main ()
{
	TMS		t;
	MUTMS		mt;
	time_t		r, mr;

	for (r = 0; r < 100000; r++)
		;

	r = times (&t);
	mr = mutimes (&mt);

	printf ("ret = %d\n", r);
	printf ("%d, %d, %d, %d\n",
		t.tms_utime,
		t.tms_stime,
		t.tms_cutime,
		t.tms_cstime
	);


	printf ("mret = %d\n", mr);
	printf ("%d, %d, %d, %d, %d, %d, %d, %d\n",
		mt.mu_utime,
		mt.mu_uutime,
		mt.mu_stime,
		mt.mu_sutime,
		mt.mu_cutime,
		mt.mu_cuutime,
		mt.mu_cstime,
		mt.mu_csutime
	);

}
