main ()
{
	long long	q;
	long		*p = (long *)&q;
	int		i;

	q = 0xAAAAAAAAAAAAAAAAll;

	for (i = 64; i > 0; i--)
	{
		printf ("%P %P\n", p[1], p[0]);

		q >>= 1;
	}
}
