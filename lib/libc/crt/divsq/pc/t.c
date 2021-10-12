
int
main (int argc, const char *argv[])
{
	long long	z, d, q;
	int		i;
	long		*p;

#if (0)	/*******************************************************/
	for (z = 1000000000000000000ll, d = 100000000000ll; d < 1000000000000000000ll; d++)
	{
		q = z / d;

		if (q * d > z)
			printf ("Erro: z = %qu, d = %qu, q = %qu, produto = %qu\n", z, d, q, q * d);
	}
#endif	/*******************************************************/

	for (z = 1, i = 0; i < 19; i++)
	{
		printf ("%qd %qd\n", z, z / 10);

		z *= 10;
	}

	printf ("\n");
	p = (long *)&d;

	for (z = 10, d = 1, i = 0; i < 18; i++)
	{
		printf ("divisor = %P %P\t", p[1], p[0]);
		printf ("%qd / %qd => %lld\n", z, d, z / d);

		z *= 10;
		d *= 10;
	}

	printf ("\n");
	p = (long *)&d;

	for (z = -10, d = 1, i = 0; i < 18; i++)
	{
		printf ("divisor = %P %P\t", p[1], p[0]);
		printf ("%qd / %qd => %lld\n", z, d, z / d);

		z *= 10;
		d *= 10;
	}

	printf ("\n");
	p = (long *)&d;

	for (z = 10, d = -1, i = 0; i < 18; i++)
	{
		printf ("divisor = %P %P\t", p[1], p[0]);
		printf ("%qd / %qd => %lld\n", z, d, z / d);

		z *= 10;
		d *= 10;
	}

	printf ("\n");
	p = (long *)&d;

	for (z = -10, d = -1, i = 0; i < 18; i++)
	{
		printf ("divisor = %P %P\t", p[1], p[0]);
		printf ("%qd / %qd => %lld\n", z, d, z / d);

		z *= 10;
		d *= 10;
	}

	return (0);
}
