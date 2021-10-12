#include <stdio.h>

extern char **environ;

/*
 ****************************************************************
 *	Imprime o Environment					*
 ****************************************************************
 */
main (argc, argv, envp)
char **argv, **envp;
{
	register char **rp, *cp;

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

	printf ("Argumentos:\n");

	while (cp = *argv++)
	{
		printf ("\t%s\n", cp);
	}

	printf ("\nEnviron:\n");

	while (cp = *rp++)
	{
		printf ("\t%s\n", cp);
	}

	exit (0);

}	/* end printenv */
