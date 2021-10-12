#include <stdio.h>
#include <errno.h>

main (argc, argv)
char	**argv;
{
	register FILE	*fp;
	char		buf[120];

	if ((fp = fopen (argv[1], "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"Erro no fopen de \"%s\" (%s)\n",
			argv[1], strerror (errno)
		);
		exit (1);
	}

	status (fp);

	while (fngets (buf, 120, fp) == buf)
		printf ("%s\n", buf);

}	/* end t */
