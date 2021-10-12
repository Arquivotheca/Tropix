#include <stdio.h>
#include <signal.h>


int
main ()
{
	printf ("Estado do sinal HANGUP = %P\n", signal (SIGHUP, SIG_DFL));

	return (0);
}
