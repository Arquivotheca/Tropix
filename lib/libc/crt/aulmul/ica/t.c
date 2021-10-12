main ()
{
	unsigned long a, b, c, d;

	for (;;)
	{
	scanf("%d %d", &a, &b);

	d = a;
	c = a * b;
	d *= b;

	printf ("a, b, c = %d %d %d %d\n", a, b, c, d);
	}
}
