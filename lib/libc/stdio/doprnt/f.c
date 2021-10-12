#include <stdio.h>
#include <stdlib.h>

print (double d)
{
		printf ("%%f		\"%f\"\n", d);
		printf ("%%f		\"%f\"\n", d);
		printf ("%%+f		\"%+f\"\n", d);
		printf ("%%-+30f		\"%-+30f\"\n", d);
		printf ("%%030f		\"%030f\"\n", d);
		printf ("%%.15f		\"%.15f\"\n", d);
		printf ("%%30.15f		\"%30.15f\"\n", d);
		printf ("%%30.15f		\"%30.15f\"\n", d);
		printf ("%%.0f		\"%.0f\"\n", d);
		printf ("%%.2f		\"%.2f\"\n", d);
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
