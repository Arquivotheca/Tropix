#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register FILE	*fp;
	register 	c;
	char		buf[120];

	if ((fp = fopen (argv[1], "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"%s\" (%s)\n",
			argv[1], strerror (errno)
		);
		exit (1);
	}

	c = getc (fp); fprintf (stderr, "\"%c\"\n", c); c = ungetc (c, fp);
	fprintf (stderr, "ungetc retornou \"%c\"\n", c);
	fngets (buf, 120, fp); fprintf (stderr, "%s\n", buf);
	fstatus (fp, "\nPonto 1: ");

	while (getc (fp) != EOF)
		/* vazio */;
	fstatus (fp, "\nPonto 2: ");

	c = ungetc ('1', fp);
	fprintf (stderr, "ungetc retornou \"%c\"\n", c);
	fstatus (fp, "\nPonto 3: ");

	c = ungetc ('2', fp);
	fprintf (stderr, "ungetc retornou \"%c\"\n", c);
	fstatus (fp, "\nPonto 4: ");

	fngets (buf, 120, fp); fprintf (stderr, "%s\n", buf);
	fstatus (fp, "\nPonto 5: ");

}	/* end t */
