#include <stdio.h>

main ()
{
	int		n;

	printf ("Teste com o FP == stdout");
	n = fflush (stdout); sleep (2);
	printf ("\tRetornou %d\n", n);

	printf ("\nTeste com o FP == NULO");
	n = fflush (NOFILE); sleep (2);
	printf ("\t\tRetornou %d\n", n);
}
