#include <stdio.h>

main ()
{
	register int	c;
	register FILE	*fp;

	fp = fopen ("/dev/tty", "r");

	putc ('=', stderr);
	putc ('>', stderr);
	getc (fp);
	putc ('\n', stderr);
	putc ('\n', stderr);

	while ((c = getchar ()) != EOF)
		putchar (c);

	fstatus (stdout, "Ponto 1: ");
}
