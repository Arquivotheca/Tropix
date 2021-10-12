#include <stdio.h>
#include <stdlib.h>

print (int i)
{
		printf ("%%o		\"%o\"\n", i);
		printf ("%%#o		\"%#o\"\n", i);
		printf ("%%ho		\"%ho\"\n", i);
		printf ("%%+o		\"%+o\"\n", i);
		printf ("%%-o		\"%-o\"\n", i);
		printf ("%% o		\"% o\"\n", i);
		printf ("%%15o		\"%15o\"\n", i);
		printf ("%%015o		\"%015o\"\n", i);
		printf ("%%15.0o		\"%15o\"\n", i);
		printf ("%%15.3o		\"%15.3o\"\n", i);
		printf ("%%015.3o		\"%015.3o\"\n", i);
		printf ("%%-15.3o		\"%-15.3o\"\n", i);
		printf ("%%-015.3o	\"%-015.3o\"\n", i);
		printf ("%%+15.3o		\"%+15.3o\"\n", i);
		printf ("%%+015.3o	\"%+015.3o\"\n", i);
		printf ("%%-+15.3o	\"%-+15.3o\"\n", i);
		printf ("%%-+015.3o	\"%-+015.3o\"\n", i);
		printf ("%% 15.3o		\"% 15.3o\"\n", i);
		printf ("%% 015.3o	\"% 015.3o\"\n", i);
}

main ()
{
	print (0);
	print (12);
	print (12345);
	print (123456);
	print (3456789012);
}

