#include <stdio.h>
#include <stdlib.h>

print (int i)
{
		printf ("%%x		\"%x\"\n", i);
		printf ("%%X		\"%X\"\n", i);
		printf ("%%#x		\"%#x\"\n", i);
		printf ("%%#X		\"%#X\"\n", i);
		printf ("%%hX		\"%hX\"\n", i);
		printf ("%%+X		\"%+X\"\n", i);
		printf ("%%-X		\"%-X\"\n", i);
		printf ("%% X		\"% X\"\n", i);
		printf ("%%15X		\"%15X\"\n", i);
		printf ("%%015X		\"%015X\"\n", i);
		printf ("%%15.0X		\"%15X\"\n", i);
		printf ("%%15.3X		\"%15.3X\"\n", i);
		printf ("%%015.3X		\"%015.3X\"\n", i);
		printf ("%%-15.3X		\"%-15.3X\"\n", i);
		printf ("%%-015.3X	\"%-015.3X\"\n", i);
		printf ("%%+15.3X		\"%+15.3X\"\n", i);
		printf ("%%+015.3X	\"%+015.3X\"\n", i);
		printf ("%%-+15.3X	\"%-+15.3X\"\n", i);
		printf ("%%-+015.3X	\"%-+015.3X\"\n", i);
		printf ("%% 15.3X		\"% 15.3X\"\n", i);
		printf ("%% 015.3X	\"% 015.3X\"\n", i);
}

main ()
{
	print (0);
	print (0x1A);
	print (0x123BC);
	print (0x1234DE);
	print (0x3456ABCD);
}

