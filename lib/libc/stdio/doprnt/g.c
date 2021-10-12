#include <stdio.h>
#include <stdlib.h>

print (double d)
{
		printf ("%%g		\"%g\"\n", d);
		printf ("%%G		\"%G\"\n", d);
		printf ("%%.0G		\"%.0G\"\n", d);
		printf ("%%.1G		\"%.1G\"\n", d);
		printf ("%%.2G		\"%.2G\"\n", d);
		printf ("%%.15G		\"%.15G\"\n", d);
		printf ("%%015G		\"%015G\"\n", d);
		printf ("\n");
}

main ()
{
	print (-1.2345E+300);
	print (-1.2345E+200);
	print (-1.2345E+123);
	print (-1.2345);
	print (-1.0000);
	print (-1.2345E-123);
	print (-1.2345E-200);
	print (-1.2345E-300);

	print (0.0);

	print (+1.2345E-300);
	print (+1.2345E-200);
	print (+1.2345E-123);
	print (+1.0000);
	print (+1.2345);
	print (+1.2345E+123);
	print (+1.2345E+200);
	print (+1.2345E+300);

}
