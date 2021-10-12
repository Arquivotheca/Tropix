#include <stdio.h>

char	buf[120];

char	*fngets ();

main (argc, argv)
char	**argv;
{
	register int	 i;
	register	 fd;
	register FILE	*fp;

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

#if (0)	/*************************************/
	while (getc (fp) != EOF)
		;

	status (fp);

	for (i = 80; i > 0; i--)
		getc (fp);

	status (fp);

	rewind (fp);

	status (fp);

	fngets (buf, 120, fp);

	printf ("%s\n", buf);
#endif	/*************************************/

	while (fngets (buf, 120, fp) == buf)
	{
		printf ("%s\n", buf);
		rewind (fp);
	}

}	/* end t */
