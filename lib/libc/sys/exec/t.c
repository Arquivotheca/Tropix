#include <sys/syscall.h>

#include <stdio.h>

main (int argc, register const char *argv[], const char *envp[])
{
	register const char	**rp, *cp;

	rp = argv; printf ("Argumentos:\n");

	while (cp = *rp++)
	{
		printf ("%s\n", cp);
	}

	printf ("\n");

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

	printf ("Ambiente:\n");

	while (cp = *rp++)
	{
		printf ("%s\n", cp);
	}

	exit (0);

}	/* end printenv */
