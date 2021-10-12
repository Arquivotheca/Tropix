
#include <stdlib.h>

void f1 (void);
void f2 (void);
void f3 (void);

main (int argc, register char *argv[])
{
	sleep (2);

	atexit (f1);
	atexit (f2);
	atexit (f3);

	exit (0);
}

void
f1 (void)
{
	printf ("Mensagem 1\n");
}

void
f2 (void)
{
	printf ("Mensagem 2\n");
}

void
f3 (void)
{
	printf ("Mensagem 3\n");
}
