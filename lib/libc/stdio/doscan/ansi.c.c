#include <stdio.h>

main ()
{
	/*
	 *	Teste ANSI, pg. 134
	 */
	int	count;
	float	quant;
	char	units[21], item[21];

	do
	{
		count = scanf ("%f%20s of %20s", &quant, units, item);
		scanf ("%*[^\n]");

		printf ("count = %d", count);
		if (count > 0)
			printf (", quant = %g", quant);
		if (count > 1)
			printf (", units = \"%s\"", units);
		if (count > 2)
			printf (", item = \"%s\"", item);
		printf ("\n");
	}
	while (count >= 0);

#if (0)	/*************************************/
	while (!feof (stdin) && !ferror (stdin))
	{
		count = fscanf (stdin, "%f%20s of %20s", &quant, units, item);
		fscanf (stdin, "%*[^\n]");
		printf
		(	"count = %d, quant = %f, "
			"units = \"%s\", item = \"%s\"\n",
			count, quant, units, item
		);
	}
#endif	/*************************************/

	exit (0);
}
