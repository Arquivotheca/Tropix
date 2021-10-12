#include <stdio.h>
#include <stdlib.h>

print (int i)
{
		printf ("%%p		\"%p\"\n", i);
		printf ("%%P		\"%P\"\n", i);
		printf ("%%#p		\"%#p\"\n", i);
		printf ("%%#P		\"%#P\"\n", i);
		printf ("%%hP		\"%hP\"\n", i);
		printf ("%%+P		\"%+P\"\n", i);
		printf ("%%-P		\"%-P\"\n", i);
		printf ("%% P		\"% P\"\n", i);
		printf ("%%15P		\"%15P\"\n", i);
		printf ("%%015P		\"%015P\"\n", i);
		printf ("%%15.0P		\"%15P\"\n", i);
		printf ("%%15.3P		\"%15.3P\"\n", i);
		printf ("%%015.3P		\"%015.3P\"\n", i);
		printf ("%%-15.3P		\"%-15.3P\"\n", i);
		printf ("%%-015.3P	\"%-015.3P\"\n", i);
		printf ("%%+15.3P		\"%+15.3P\"\n", i);
		printf ("%%+015.3P	\"%+015.3P\"\n", i);
		printf ("%%-+15.3P	\"%-+15.3P\"\n", i);
		printf ("%%-+015.3P	\"%-+015.3P\"\n", i);
		printf ("%% 15.3P		\"% 15.3P\"\n", i);
		printf ("%% 015.3P	\"% 015.3P\"\n", i);
		printf ("\n");
}

main ()
{
	print (0);
	print (0x1A);
	print (0x123BC);
	print (0x1234DE);
	print (0x3456ABCD);
}

