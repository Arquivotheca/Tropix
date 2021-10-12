#include <sys/types.h>
#include <sys/times.h>

main ()
{
	MUTM		t, u;
	int		r, s, i;

	for (i = 0; /* vazio */; i++)
	{
		r = mutime (&t);
		s = mutime (&u);

		if (u.mu_utime - t.mu_utime < 0)
		{
			printf
			(	"%6d %d %d %06d %06d %d %d %d\n",
				i, t.mu_time, u.mu_time, t.mu_utime, u.mu_utime,
				u.mu_time - t.mu_time,
				u.mu_utime - t.mu_utime,
				1000000 + u.mu_utime - t.mu_utime
			);
		}
	}

}
