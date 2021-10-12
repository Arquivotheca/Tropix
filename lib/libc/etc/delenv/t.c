#include <stdio.h>
#include <stdlib.h>

void		prenv ();

/*
 ****************************************************************
 *	Teste de Putenv						*
 ****************************************************************
 */
main ()
{
	register char	*cp;

	for (;;)
	{
		prenv ();
		cp  = malloc (30);
		if (gets (cp) == NOSTR)
			exit (0);

		if (delenv (cp) == 0)
			printf ("\nOK\n\n");
		else
			printf ("\n***** ERRO\n\n");
	}

}	/* end t */

/*
 ****************************************************************
 *	Imprime o Environment					*
 ****************************************************************
 */
void
prenv (void)
{
	register const char **rp, *cp;

	if ((rp = environ) == NULL)
	{
		printf ("nao tem environ\n");
		return;
	}

	while (cp = *rp++)
	{
		printf ("%s\n", cp);
	}

}	/* end prenv */
