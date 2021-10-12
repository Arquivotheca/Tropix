#include <sys/types.h>
#include <sys/times.h>

main ()
{
	MUTM	t;
	int	r;

	r = mutime (&t);

	printf ("retorno = %d, seg = %d, micro = %d\n",
		r, t.mu_time, t.mu_utime);
}
