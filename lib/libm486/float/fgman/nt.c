#include <math.h>

main ()
{
	double	a, in;
	double b, c;
	int	i, exp;

	printf ("Entre com um inteiro i e um flutuante a\n");

	for (;;)
	{
		scanf("%d %lg", &i, &a);
	
		b = modf (a, &in);
		printf ("a, b, in = %G %G %G \n", a, b, in);
		printf ("a = %08X %08X\n", a);
		printf ("b = %08X %08X\n", b);
		printf ("in = %08X %08X \n", in);
		b = fgfra (a);
		in = fgint (a);
		printf ("a, b, in = %G %G %G \n", a, b, in);
		printf ("a = %08X %08X\n", a);
		printf ("b = %08X %08X\n", b);
		printf ("in = %08X %08X\n", in);
	
		b = frexp (a, &exp);
		printf ("a, b, exp = %G %G %d \n", a, b, exp);
		b = fgman (a);
		exp = fgexp (a);
		printf ("a, b, exp = %G %G %d \n", a, b, exp);
	
		b = ldexp (a, i);
		printf ("a, b, i = %G %G %d\n", a, b, i);
		b = faexp (i, a);
		printf ("a, b, i = %G %G %d\n", a, b, i);
	
		c = fhuge ();
		printf ("fhuge = %G \n",  c);
	}
}
