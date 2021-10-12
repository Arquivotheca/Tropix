#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Imprime o Environment					*
 ****************************************************************
 */
int
main (int argc, const char *argv[], const char *envp[])
{
	const char	**rp, *cp;

	if ((rp = environ) == NULL)
	{
		printf ("nao tem environ\n");
		exit (1);
	}

	if (envp == NULL)
	{ 
		printf ("nao tem envp\n");
	}

	if (envp != rp)
	{
		printf ("envp e _environ sao diferentes\n");
	}

	while (cp = *rp++)
	{
		printf ("%s\n", cp);
	}

	return (0);

}	/* end printenv */
