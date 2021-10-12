
int
main (int argc, const char *argv[])
{
	unsigned long long	z, d, q, r;
	int			i;
	long			*p;

	for (z = 3, i = 0; i < 19; i++)
	{
		printf ("%qd %qd\n", z, z % 10);

		z = z * 10 + 3;
	}

	printf ("\n");

	for (z = 13, d = 10, i = 0; i < 18; i++)
	{
		p = (long *)&d;
		printf ("Dividendo = %P %P\t", p[1], p[0]);

		r = z % d;

		printf ("%qu, %qu => %llu\n", z, d, r);
#if (0)	/*******************************************************/
		p = (long *)&r;
		printf ("Resto = %P %P\n", p[1], p[0]);
#endif	/*******************************************************/

		z = (z - 3) * 10 + 3;
		d *= 10;
	}

	printf ("\n");

	for (z = 130ll, d = 100ll, i = 0; i < 17; i++)
	{
		p = (long *)&d;
		printf ("Dividendo = %P %P\t", p[1], p[0]);

		r = z % d;

		printf ("%qu, %qu => %llu\n", z, d, r);

		z *= 10;
		d *= 10;
	}

	return (0);
}
