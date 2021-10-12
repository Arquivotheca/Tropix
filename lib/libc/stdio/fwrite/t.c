#include <stdio.h>
#include <errno.h>

main (argc, argv)
char	**argv;
{
	register char	*cp;
	register FILE	*fpin, *fpout;
	char		buf[450];

	if ((fpin = fopen (argv[1], "r")) == NOFILE)
	{
		printf
		(	"Nao consegui abrir \"%s\" (%s)\n",
			argv[1], strerror (errno)
		);
		exit (1);
	}

	if ((fpout = fopen (argv[2], "w")) == NOFILE)
	{
		printf
		(	"Nao consegui abrir \"%s\" (%s)\n",
			argv[2], strerror (errno)
		);
		exit (1);
	}

	printf ("fread: %d\n", fread (buf, 10, 45, fpin) );

	printf ("fwrite: %d\n", fwrite (buf, 10, 45, fpout) );

}	/* end t */
