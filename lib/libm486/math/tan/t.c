#include <math.h>

main ()
{
	double		a, b, c;
	double		mul, div;

	printf ("Cálculo da tangente de a * PI / b\n");
	printf ("Entre com os valores de a e b\n");

	for (;;)
	{
		if (scanf ("%Lg %Lg", &a, &b) < 0)
			exit (0);

		c = tan (a * M_PI / b);
		printf ("a = %25.20G  b = %25.20G\n", a, b);
		printf ("Ângulo de %25.20G\n", a/b*180);
		printf ("tan = %25.20G\n", c);
	}
}
