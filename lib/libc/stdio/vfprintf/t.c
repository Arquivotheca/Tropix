#include <stdarg.h>
#include <stdio.h>

void error (char *function_name, char *format, ...);

main ()
{
	error ("função", "Um argumento:      %d\n",  1);
	error ("função", "Dois argumentos:   %d, %d\n", 1, 2);
	error ("função", "Tres argumentos:   %d, %d, %d\n", 1, 2, 3);
	error ("função", "Quatro argumentos: %d, %d, %d, %d\n", 1, 2, 3, 4);
	error ("função", "Cinco argumentos:  %d, %d, %d, %d, %d\n", 1, 2, 3, 4, 5);
	error ("função", "Seis argumentos:   %d, %d, %d, %d, %d, %d\n", 1, 2, 3, 4, 5, 6);
}

void error (char *function_name, char *format, ...)
{
	register va_list args;

	va_start (args, format);

	fprintf (stderr, "ERROR in \"%s\": ", function_name);

	vfprintf (stderr, format, args);

	va_end (args);
}
