main ()
{
	double	a, in;
	register double b;
	int	i, exp;
	double	modf (), frexp (), ldexp ();

	printf ("Entre com um inteiro i e um flutuante a\n");

	for (;;)
	{
		scanf("%d %lg", &i, &a);
	
		b = ldexp (a, i);
		printf ("a, b, i = %G %G %d\n", a, b, i);
	
		b = frexp (a, &exp);
		printf ("a, mantissa, expoente = %G %G %d \n", a, b, exp);
	
		b = modf (a, &in);
		printf ("a, fração, inteiro = %G %G %G \n", a, b, in);
	}
}
