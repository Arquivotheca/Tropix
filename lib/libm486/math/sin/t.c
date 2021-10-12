#include <math.h>
#include <errno.h>

main ()
{
	double		a, b, c, d;

	printf ("Cálculo de seno e cosseno de (a * PI / b)\n");
	printf ("Dê os valores de a e b\n");

	for (;;)
	{
		if (scanf ("%Lg %Lg", &a, &b) < 0)
			exit (0);

		c = sin (a * M_PI / b);
		d = cos (a * M_PI / b);
		printf ("a = %25.20G	b = %25.20G\n", a, b);
		printf ("Ângulo de %25.20G graus\n", a/b*180);
		printf ("cos (a) = %25.20G   sen (a) = %25.20G\n", d, c);
	}
}

int
matherr (x)
EXCEPTION	*x;
{
	if (x->type == DOMAIN)
		printf ("erro DOMAIN na rotina %s\n", x->name);

	if (x->type == SING)
		printf ("erro SING na rotina %s\n", x->name);

	if (x->type == OVERFLOW)
		printf ("erro OVERFLOW na rotina %s\n", x->name);

	if (x->type == UNDERFLOW)
		printf ("erro UNDERFLOW na rotina %s\n", x->name);

	if (x->type == PLOSS)
		printf ("erro PLOSS na rotina %s\n", x->name);

	if (x->type == TLOSS)
		printf ("erro TLOSS na rotina %s\n", x->name);

	if (errno == EDOM)
		printf ("errno = EDOM na rotina %s\n", x->name);

	if (errno == ERANGE)
		printf ("errno = ERANGE na rotina %s\n", x->name);
}
