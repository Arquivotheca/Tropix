#include <math.h>

main ()
{
	double		a, b, c, d;

	printf
	(	"arco-seno de 0.5 = %G graus\n",
		180 / M_PI * asin (0.5)
	);

	printf
	(	"arco-coseno de 0.5 = %G graus\n",
		180 / M_PI * acos (0.5)
	);

	printf ("Entre com o seno\n");

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);

		b = asin (a); 
		c = acos (a);

		printf ("a = %25.20G\n", a);
		printf ("b = asin (a) = %25.20G graus\n", b/M_PI*180);
		printf ("c = acos (a) = %25.20G graus\n", c/M_PI*180);
/*
		printf ("b = asin (a) = %25.20G graus\n", b);
		printf ("b = %25.20G radianos\n", b);
*/
	}
}

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

	return (0);
}
