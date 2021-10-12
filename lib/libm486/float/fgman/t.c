#include <math.h>

main ()
{
	double	a, in;
	double	b;
	int		i, exp;
	double		c;

	printf ("Entre com um inteiro i e um flutuante a\n");

	for (;;)
	{
		scanf ("%d %lg", &i, &a);
		printf ("i = %d, a = %E\n", i, a);

#if (0)	/*************************************/
#endif	/*************************************/
		b = modf (a, &in);
		printf ("modf: a, b, in = %G %G %G \n", a, b, in);
		b = fgfra (a);
		in = fgint (a);
		printf ("fgfra, fgint: a, fração, inteiro = %G %G %G \n", a, b, in);

#if (0)	/*************************************/
#endif	/*************************************/
		b = frexp (a, &exp);
		printf ("frexp: a, b, exp = %G %G %d \n", a, b, exp);
		b = fgman (a);
		exp = fgexp (a);
		printf ("fgman, fgexp: a, mantissa, expoente = %G %G %d \n", a, b, exp);

#if (0)	/*************************************/
#endif	/*************************************/
		b = ldexp (a, i);
		printf ("ldexp: a, b, i = %G %G %d\n", a, b, i);
		b = faexp (i, a);
		printf ("faexp: a, b, i = %G %G %d\n", a, b, i);

		c = fhuge ();
		printf ("fhuge = %G \n",  c);
	}
}
