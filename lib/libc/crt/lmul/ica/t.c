main ()
{
	unsigned long a, b, c, d;
	long e, f, g, h;
	register int	i;


	scanf("%d %d", &a, &b);
	c = a * b;
	e = a;
	f = b;
	for (i = 0; i < 100000; i++)
/*
	for (;;)
*/
	{

	g = e * f;

	}
	printf ("a, b, c, d = %d %d %d %d\n", a, b, c, d);
	printf ("e, f, g, h = %d %d %d %d\n", e, f, g, h);
}
