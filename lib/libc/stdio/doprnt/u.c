#include <stdio.h>
#include <stdlib.h>

print (int i)
{
		printf ("%%u		\"%u\"\n", i);
		printf ("%%hu		\"%hu\"\n", i);
		printf ("%%+u		\"%+u\"\n", i);
		printf ("%%-u		\"%-u\"\n", i);
		printf ("%% u		\"% u\"\n", i);
		printf ("%%15u		\"%15u\"\n", i);
		printf ("%%015u		\"%015u\"\n", i);
		printf ("%%15.0u		\"%15u\"\n", i);
		printf ("%%15.3u		\"%15.3u\"\n", i);
		printf ("%%015.3u		\"%015.3u\"\n", i);
		printf ("%%-15.3u		\"%-15.3u\"\n", i);
		printf ("%%-015.3u	\"%-015.3u\"\n", i);
		printf ("%%+15.3u		\"%+15.3u\"\n", i);
		printf ("%%+015.3u	\"%+015.3u\"\n", i);
		printf ("%%-+15.3u	\"%-+15.3u\"\n", i);
		printf ("%%-+015.3u	\"%-+015.3u\"\n", i);
		printf ("%% 15.3u		\"% 15.3u\"\n", i);
		printf ("%% 015.3u	\"% 015.3u\"\n", i);
}

main ()
{
	print (0);
	print (12);
	print (12345);
	print (123456);
	print (3456789012);
}

