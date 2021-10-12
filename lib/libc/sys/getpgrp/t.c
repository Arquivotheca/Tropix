#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	printf ("prgp = %d\n", getpgrp () );
}
