#include <stdio.h>
#include <stdlib.h>

print (int c)
{
		printf ("%%c:		\"%c\"\n", c);
		printf ("%%15c:		\"%15c\"\n", c);
		printf ("%%015c:		\"%015c\"\n", c);
		printf ("%%15.6c:		\"%15.6c\"\n", c);
		printf ("%%015.6c:	\"%015.6c\"\n", c);
		printf ("%%-15.6c:	\"%-15.6c\"\n", c);
		printf ("%%-015.6c:	\"%-015.6c\"\n", c);
		printf ("%%+15.6c:	\"%+15.6c\"\n", c);
		printf ("%%+015.6c:	\"%+015.6c\"\n", c);
		printf ("%% 15.6c:	\"% 15.6c\"\n", c);
		printf ("%% 015.6c:	\"% 015.6c\"\n", c);
}

main ()
{
	print ('1');
	print ('12');
	print ('123');
	print ('1234');
}
