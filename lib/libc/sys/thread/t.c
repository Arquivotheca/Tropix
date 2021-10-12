#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define	NTHREADS	3
#define	NDIM		3

int	a[3][3] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
int	b[3][3] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
int	c[3][3];
int	th[3];

main ()
{
	int	i, j, k;
	int	thid;

	for (i = 0; i < NTHREADS; i++)
	{
		if ((th[i] = thread ()) == 0)
		{
			for (j = 0; j < NDIM; j++)
			{
				c[i][j] = 0;

				for (k = 0; k < 3; k++)
					c[i][j] += a[i][k]*b[k][j]; 
			}

			if (i == 0)
				system ("data");

			if (i == 1)
				system ("uptime");

			_exit (0);
		}

		if (th[i] < 0)
			printf ("Não consegui criar thread %d\n", i);

		if (th[i] > 0)
			printf ("Consegui criar thread no. %d\n", th[i]);
	}

	printf ("\n");
	for (i = 0; i < NTHREADS; i++)
	{
		thid = wait (0);
		printf ("Terminou thread no. %d\n", thid);
	}

	printf ("\n");
	printf ("Produto das matrizes:\n");

	for (i = 0; i < NDIM; i++)
	{
		for (j = 0; j < 3; j++)
			printf ("%d ", c[i][j]);
		printf ("\n");
	}	
}
