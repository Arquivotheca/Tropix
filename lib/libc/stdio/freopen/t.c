#include <stdio.h>

main (argc, argv)
char	**argv;
{
	register FILE	*fp;
	char		buf[120];

	if ((fp = freopen (argv[1], "r", stdin)) == NOFILE)
	{
		fprintf (stderr, "Erro no open de %s\n", argv[1]);
		perror (argv[0]);
		exit (1);
	}

	while (fngets (buf, 120, fp) == buf)
		printf ("%s\n", buf);

}	/* end t */
