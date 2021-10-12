#include <stdio.h>

extern	errno;

main (argc, argv)
char	**argv;
{
	register char	*cp;
	register FILE	*fp;
	register	c;

	if ((fp = fopen (argv[1], "r")) == NOFILE)
	{
		printf ("Nao consegui abrir %s\n", argv[1]);
		printf ("t: %s\n", errtostr (errno) );
		exit (1);
	}

	while ((c = fgetc (fp)) != EOF)
		fputc (c, stdout);

}	/* end t */
