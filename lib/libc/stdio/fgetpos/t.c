#include <stdio.h>
#include <errno.h>

char	buf[120];

main (argc, argv)
char	**argv;
{
	register FILE	*fp;
	register int	n;
	fpos_t		wpos, rpos;

	if ((fp = fopen (argv[1], "r")) == NOFILE)
	{
		printf ("Nao consegui abrir %s\n", argv[1]);
		printf ("t: %s\n", errtostr (errno) );
		exit (1);
	}

	for (;;)
	{

		n = strtol (gets (buf), (char **)NULL, 0);

		if (n < 0)
			break;

		wpos = n;

		if (fsetpos (fp, &wpos) < 0)
		{
			printf ("Erro no FGETPOS em %d\n", n);
			printf ("s: %s\n", errtostr (errno) );
		}
		else
		{
			if (fgetpos (fp, &rpos) < 0)
				printf ("Erro no FSETPOS em %d\n", n);
			else
				printf ("FSETPOS: %d\n", rpos);
			status (fp);
			printf ("Inicio = %d\n", n);
			fngets (buf, 120, fp);
			printf ("%s\n", buf);
		}
	}

	status (stdin);
	status (stdout);
	status (stderr);
	status (fp);

}	/* end t */
