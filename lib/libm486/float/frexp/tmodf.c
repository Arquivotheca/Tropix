main ()
{
	double a, b, in;
	int	i, exp;
	double	modf (), frexp (), ldexp ();

	for (;;)
	{
	scanf("%d %lg", &i, &a);

	b = modf (a, &in);
	printf ("a, b, in = %g %g %g \n", a, b, in);

	b = frexp (a, &exp);
	printf ("a, b, exp = %g %g %d \n", a, b, exp);

	b = ldexp (a, &i);
	printf ("a, b, i = %g %g %d\n", a, b, i);
	}
}
