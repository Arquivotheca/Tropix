#include <stdio.h>
#include <stdlib.h>

print (int i)
{
		printf ("%%d		\"%d\"\n", i);
		printf ("%%hd		\"%hd\"\n", i);
		printf ("%%+d		\"%+d\"\n", i);
		printf ("%%-d		\"%-d\"\n", i);
		printf ("%% d		\"% d\"\n", i);
		printf ("%%15d		\"%15d\"\n", i);
		printf ("%%015d		\"%015d\"\n", i);
		printf ("%%15.0d		\"%15d\"\n", i);
		printf ("%%15.3d		\"%15.3d\"\n", i);
		printf ("%%015.3d		\"%015.3d\"\n", i);
		printf ("%%-15.3d		\"%-15.3d\"\n", i);
		printf ("%%-015.3d	\"%-015.3d\"\n", i);
		printf ("%%+15.3d		\"%+15.3d\"\n", i);
		printf ("%%+015.3d	\"%+015.3d\"\n", i);
		printf ("%%-+15.3d	\"%-+15.3d\"\n", i);
		printf ("%%-+015.3d	\"%-+015.3d\"\n", i);
		printf ("%% 15.3d		\"% 15.3d\"\n", i);
		printf ("%% 015.3d	\"% 015.3d\"\n", i);
}

main ()
{
	print (-123456);
	print (-12345);
	print (-12);
	print (0);
	print (12);
	print (12345);
	print (123456);
}

