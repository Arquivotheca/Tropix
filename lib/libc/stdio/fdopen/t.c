#include <stdio.h>

main (argc, argv)
char	**argv;
{
	register	 fd;
	register FILE	*fp;
	char		buf[120];

	if ((fd = open (argv[1], 0)) < 0)
	{
		fprintf (stderr, "Erro no open de %s\n", argv[1]);
		perror (argv[0]);
		exit (1);
	}

	if ((fp = fdopen (fd, "r")) == NOFILE)
	{
		fprintf (stderr, "Erro no fdopen de %s\n", argv[1]);
		perror (argv[0]);
		exit (1);
	}

	status (fp);

	while (fngets (buf, 120, fp) == buf)
		printf ("%s\n", buf);

}	/* end t */
