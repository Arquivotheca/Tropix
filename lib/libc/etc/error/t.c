#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

int
main ()
{
	error ("Texto simples");

	errno = ENOSPC;
	error ("*Texto com código de erro simples");

	error ("A seguir, texto default");
	error (NOSTR);

}
