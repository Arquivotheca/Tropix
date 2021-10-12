#include	<stdio.h>

char	buf[100];
char	padrao[20];

main (argc, argv)
char	**argv;
{
	printf ("Padrao = ");
	gets (padrao);

#if (0)	/*******************************************************/
	freopen ("/etc/rc", "r", stdin);
#endif	/*******************************************************/

	while (gets (buf) != NULL)
	{
		printf ("%-40s", buf);
		printf (" %2d ", strlen (buf) );
		if (patmatch (buf, padrao))
			printf ("\tAceito\n");
		else
			printf ("\tRecusado\t*****\n");

	}

}	/* end main */
