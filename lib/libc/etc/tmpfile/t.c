#include <stdio.h>

#define	NOSTR	(char *)NULL

main (int argc, register char *argv[])
{
	register FILE	*fp1, *fp2;
	char		buf[128];

	fp1 = tmpfile ();
	fp2 = tmpfile ();

	fprintf (fp1, "Linha de teste 1\n"); 
	fprintf (fp2, "**** Linha de teste 2\n"); 

	rewind (fp1);
	rewind (fp2);

	fngets (buf, 128, fp1);
	puts (buf);

	fngets (buf, 128, fp2);
	puts (buf);
}
