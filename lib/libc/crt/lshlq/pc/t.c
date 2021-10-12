main ()
{
	unsigned long long	qu;
	long long		qs;
	long			*p = (long *)&qu;
	int			i;

	p = (long *)&qu; qu = 0x1ll;

	for (i = 64; i > 0; i--)
	{
		printf ("%P %P\n", p[1], p[0]);

		qu <<= 1;
	}

	printf ("\n");

	p = (long *)&qs; qs = 0x1ll;

	for (i = 64; i > 0; i--)
	{
		printf ("%P %P\n", p[1], p[0]);

		qs <<= 1;
	}
}
