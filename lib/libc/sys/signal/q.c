#include <sys/syscall.h>

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

int	intro;

int	intr ();

int	p;

jmp_buf	env;

main ()
{
	setbuf (stdout, 0);

	printf ("entrando em pause\n");
	pause ();
	printf ("cheguei apos o pause\n");
	exit (0);

}
