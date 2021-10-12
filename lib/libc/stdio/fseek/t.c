#include <stdio.h>

char	buf[120];

char	*gets (), *fgets (), *fngets ();

extern	errno;

main (argc, argv)
char	**argv;
{
	register char	*cp;
	register FILE	*fp;
	register	n;

	if ((fp = fopen (argv[1], "r")) == NOFILE)
		error ("$*Nao consegui abrir \"%s\"", argv[1]);

	for (;;)
	{

		n = strtol (gets (buf), (char **)NULL, 0);

		if (n < 0)
			break;

		if (fseek (fp, n, 0) < 0)
		{
			error ("*Erro no FSEEK em %d", n);
		}
		else
		{
			fstatus (fp, NOSTR);
			printf ("Inicio = %d\n", n);
			fngets (buf, 120, fp);
			printf ("%s\n", buf);
		}
	}

	fstatus (stdin, NOSTR);
	fstatus (stdout, NOSTR);
	fstatus (stderr, NOSTR);
	fstatus (fp, NOSTR);

}	/* end t */
