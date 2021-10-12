#include <stdio.h>

char	buf[120];

char	*gets (), *fgets (), *fngets ();

extern	errno;

main (argc, argv)
char	**argv;
{
	register char	*cp;
	register FILE	*fp;


	if ((fp = fopen (argv[1], "r")) == NOFILE)
	{
		printf ("Nao consegui abrir %s\n", argv[1]);
		printf ("t: %s\n", errtostr (errno) );
		exit (1);
	}

	while (fngets (buf, 120, fp) == buf)
		puts (buf);

	status (fp);

}	/* end t */
