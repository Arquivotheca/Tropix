#include	<stdio.h>

char	buf[100];
char	padrao[20];

main (argc, argv)
char	**argv;
{
	register i;

	i = -1;
	printf ("Padrao = ");
	gets (padrao);

	freopen ("/etc/rc", "r", stdin);

	while (gets (buf) != NULL)
	{
		if (patmatch (buf, padrao))
			printf ("%s\n", buf);
	}

}	/* end main */
