#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>

main ()
{
	puts (getlogin ());
}
