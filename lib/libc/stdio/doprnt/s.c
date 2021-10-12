#include <stdio.h>
#include <stdlib.h>

print (char *str)
{
		printf ("%%s:		\"%s\"\n", str);
		printf ("%%15s:		\"%15s\"\n", str);
		printf ("%%015s:		\"%015s\"\n", str);
		printf ("%%15.6s:		\"%15.6s\"\n", str);
		printf ("%%015.6s:	\"%015.6s\"\n", str);
		printf ("%%-15.6s:	\"%-15.6s\"\n", str);
		printf ("%%-015.6s:	\"%-015.6s\"\n", str);
		printf ("%%+15.6s:	\"%+15.6s\"\n", str);
		printf ("%%+015.6s:	\"%+015.6s\"\n", str);
		printf ("%% 15.6s:	\"% 15.6s\"\n", str);
		printf ("%% 015.6s:	\"% 015.6s\"\n", str);
}

main ()
{
	print ("");
	print ("Curta");
	print ("Cadeia média");
	print ("Cadeia um pouco mais longa");
}
