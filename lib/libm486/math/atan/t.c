#include <math.h>

main ()
{
	double		a, b, c;
	int		i;

	printf
	(	"arco-tan de 1.0 = %G graus\n",
		180 / M_PI * atan (1.0)
	);

	printf ("Dê o valor da tangente\n");

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);
		b = atan (a);
		printf ("a = %25.20G \n", a);
		printf ("atan (a) = %25.20G graus\n", b/M_PI*180);
	}
}
