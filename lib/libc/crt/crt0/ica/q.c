#include <signal.h>

char	env0[] = "Seria o HOME=......";

char	*envp[] = { env0, (char *)0 };

main (argc, argv)
char **argv;
{
	int		status, pid, w;
	register int	(*istat) (), (*qstat) ();

	if ((pid = fork ()) == 0)
	{
		execve ("t", argv, envp);
		printf ("Nao consegui executar 't'\n");
		exit (-1);
	}
 
	istat = signal (SIGINT,  SIG_IGN);
	qstat = signal (SIGQUIT, SIG_IGN);

	while ((w = wait (&status)) != pid && w != -1)
		;

	if (w == -1)
		status = -1;

	signal (SIGINT,  istat);
	signal (SIGQUIT, qstat);

	exit (status);

}	/* end r */
