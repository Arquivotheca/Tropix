#include <stdio.h>

main ()
{
	register FILE	*fp;

	fp = fopen ("/etc/rc", "r");

	fstatus (fp, "\nPonto 1: ");

	getc (fp);

	fstatus (fp, "\nPonto 2: ");

	fstatus (stdin, "\nSTDIN: ");

	fstatus (stdout, "\nSTDOUT: ");

	fstatus (stderr, "\nSTDERR: ");

}
