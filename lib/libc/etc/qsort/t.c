
#include	<stdio.h>

#define	NL	1000
#define	NC	100

typedef struct
{
	char	c[NC];

}	LINHA;

main ()
{
	static LINHA	matriz[NL];
	register	int	i, j;
	register	int	c, k;
	register	LINHA	*lp;
	int		compara ();

	lp = &matriz[0];

	i = 0;

	while ((c = getchar ()) != EOF)
	{
		if (c == '\n')
		{
			lp->c[i] = 0;
			lp++;
			i = 0;
		}
		else
		{
			lp->c[i] = c;
			i++;
		}
	}

	j = lp-matriz;

	fprintf (stderr, "No. de elementos = %d\n", j);

	qsort (matriz, j, NC, compara);

	fprintf (stderr, "Fim do sort\n");

	for (lp = matriz, i = 0; i < j; lp++, i++)
	{
		printf ("%s\n", &lp->c[0]);
	}
}
