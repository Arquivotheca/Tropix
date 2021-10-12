#include <stdio.h>

main ()
{
	/*
	 *	Teste ANSI, pg. 134
	 */
	int	i, n;
	float	x;
	char	*line;
	char	name[50];

#undef	SSCANF
#ifdef	SSCANF
	line = "25  54.32e-1 thompson\n";
	strcpy (name, "---- indefinido -----");
#endif	SSCANF

#ifdef	SSCANF
	n = sscanf (line, "%d%f%s", &i, &x, name);
#else
	n = fscanf (stdin, "%d%f%s", &i, &x, name);
#endif	SSCANF

	printf
	(	"n = %d, i = %d, x = %f, name = \"%s\"\n",
		n, i, x, name
	);

	exit (0);
}
