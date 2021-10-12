#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

int	intro;

void	intr ();

int	p;

jmp_buf	env;

main ()
{
	setbuf (stdout, 0);

	if (signal (SIGHUP, intr) == SIG_ERR)
		printf ("erro no signal intr\n");

	if (setjmp (env) == 0)
	{
		printf ("entrando em pause\n");
		pause ();
		printf ("cheguei apos o pause\n");
		exit (0);
	}

	printf ("Retorno do setjmp\n");

	exit (0);

}

void
intr (int signo, void *pc, void *fa)
{
	printf
	(	"peguei uma Interrupcao: signal = %d, pc = %X, fa = %x\n",
		signo, pc, fa
	);
	intro++;
	signal (SIGHUP, intr);
	longjmp (env, 1);
}
