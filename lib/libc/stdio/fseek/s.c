#include <stdio.h>

char	buf[120];

char	a[] = "abcdefghij";
char	n[] = "0123456789";
char	A[] = "ABCDEFGHIJ";

char	*gets (), *fgets (), *fngets ();

extern	errno;

main (argc, argv)
char	**argv;
{
	register char	*cp;
	register FILE	*fp;

	if ((fp = fopen (argv[1], "w")) == NOFILE)
		error ("$*Nao consegui abrir \"%s\"", argv[1]);

	fstatus (fp, NOSTR);

	memset (buf, ':', 120);

	fseek (fp, 0, 0);
	fwrite (buf, 48, 1, fp); 

	fseek (fp, 16, 0);
	fwrite (a, 8, 1, fp); 

	fseek (fp, 0, 0);
	fwrite (n, 8, 1, fp); 

	fseek (fp, 32, 0);
	fwrite (A, 8, 1, fp); 

	fstatus (fp, NOSTR);

	exit (0);

}	/* end s */
