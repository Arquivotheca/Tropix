#include <stdio.h>
#include <stdlib.h>

void		prenv ();

/*
 ****************************************************************
 *	Teste de Putenv						*
 ****************************************************************
 */
main (int argc, register const char *argv[])
{
	register char	*cp;

	if (argv[1] && streq (argv[1], "-n"))
		environ = (const char **)NULL;

	for (;;)
	{
		printf ("\n\"environ\" = %P\n", environ);
		prenv ();
		cp  = malloc (32);
		if (gets (cp) == NOSTR)
			exit (0);

		if (putenv (cp) == 0)
			printf ("OK");
		else
			printf ("ERRO");

		if (chkalloc () < 0)
			printf ("\nErro em CHKALLOC");
		else
			printf ("\nCHKALLOC OK");
	}

}	/* end t */

/*
 ****************************************************************
 *	Imprime o Environment					*
 ****************************************************************
 */
void
prenv ()
{
	register const char	**rp, *cp;

	if ((rp = environ) == NULL)
	{
		printf ("NÃO tem \"environ\"\n");
		return;
	}

	while (cp = *rp++)
	{
		printf ("%s\n", cp);
	}

}	/* end prenv */
