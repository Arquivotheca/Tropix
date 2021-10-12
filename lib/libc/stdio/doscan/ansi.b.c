#include <stdio.h>

main ()
{
	/*
	 *	Teste ANSI, pg. 134
	 */
	int	i, r;
	float	x;
	char	name[50];

	r = fscanf (stdin, "%2d%f%*d %[0123456789]", &i, &x, name);

	printf
	(	"r = %d, i = %d, x = %f, name = \"%s\", nexchar = '%c'\n",
		r, i, x, name, getchar ()
	);

	exit (0);
}
