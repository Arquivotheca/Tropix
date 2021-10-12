#include <sys/types.h>

#include <stdio.h>
#include <pwd.h>

main ()
{
	printf ("Senha lida = \"%s\"\n", getpass ("Que Bonito Papel: "));

}	/* end t */
