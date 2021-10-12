main ()
{
	long	a = 1;
	srand (a);

	for (;;)
	{
		a = rand (a);
		printf ("%10d  %8X\n", a, a);
	}
}
