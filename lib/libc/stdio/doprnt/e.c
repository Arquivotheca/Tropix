#include <stdio.h>
#include <stdlib.h>

print (double d)
{
		printf ("%%e		\"%e\"\n", d);
		printf ("%%E		\"%E\"\n", d);
		printf ("%%+E		\"%+E\"\n", d);
		printf ("%%-+30E		\"%-+30E\"\n", d);
		printf ("%%030E		\"%030E\"\n", d);
		printf ("%%.15E		\"%.15E\"\n", d);
		printf ("%%30.15E		\"%30.15E\"\n", d);
		printf ("%%30.15E		\"%30.15E\"\n", d);
		printf ("%%.0E		\"%.0E\"\n", d);
		printf ("%%.1E		\"%.1E\"\n", d);
		printf ("%%.2E		\"%.2E\"\n", d);
		printf ("\n");
}

main ()
{
	print (-1.2345E+300);
	print (-1.2345E+200);
	print (-1.2345E+123);
	print (-1.2345);
	print (-1.2345E-123);
	print (-1.2345E-200);
	print (-1.2345E-300);

	print (0.0);

	print (+1.2345E-300);
	print (+1.2345E-200);
	print (+1.2345E-123);
	print (+1.2345);
	print (+1.2345E+123);
	print (+1.2345E+200);
	print (+1.2345E+300);

}
