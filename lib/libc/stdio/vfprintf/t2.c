#include <stdarg.h>
#include <stdio.h>

typedef struct
{
	int	a, b, c;

}	TIPO;

void f (char *name, const char *format, ...);

main ()
{
	TIPO	tipo;

	tipo.a = 3;
	tipo.b = 4;
	tipo.c = 5;

	f ("Total", "%s: %d, (%d, %d, %d), %d\n", 2, tipo, 6); 
}

void f (char *name, const char *format, ...)
{
	TIPO	t;
	int	x, y;

	register va_list ap;

	va_start (ap, format);

	x = va_arg (ap, int);
	t = va_arg (ap, TIPO);
	y = va_arg (ap, int);

	va_end (ap);

	fprintf (stderr, format, name, x, t.a, t.b, t.c, y);
} 
