main ()
{
	long long	q;
	long		*p = (long *)&q;

	q = 1ll; q = -q;
	printf ("%P, %P\n", p[1], p[0]);

	q = 0ll; q = -q;
	printf ("%P, %P\n", p[1], p[0]);

	q = -1ll; q = -q;
	printf ("%P, %P\n", p[1], p[0]);
}
