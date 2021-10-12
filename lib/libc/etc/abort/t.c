#include <stdio.h>
#include <signal.h>

void	catch (int);

main (argc, argv)
char	**argv;
{
	int	ret;

	if      (argc == 2)
		signal (SIGABRT, SIG_IGN);
	else if (argc == 3)
		signal (SIGABRT, catch);

	ret = abort ();

	printf ("retorno = %d\n", ret);
}

void
catch (int signo)
{
	printf ("Recebi o SINAL %d\n", signo);
	fflush (stdout);
}
