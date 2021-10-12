#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

main (int argc, register char *argv[])
{
	int	rgid = -1, egid = -1;

	argc--; argv++;

	printf ("No momento:\trgid = %d, egid = %d\n",  getgid (), getegid ());

	if (argc > 0 && !streq (argv[0], "-"))
		rgid = atoi (argv[0]);

	if (argc > 1 && !streq (argv[1], "-"))
		egid = atoi (argv[1]);

	printf ("Valores dados:\trgid = %d, egid = %d\n",  rgid, egid);

	if (setrgid (rgid) < 0)
		printf ("Erro em SETRUID (%s)\n", strerror (errno));

	if (setegid (egid) < 0)
		printf ("Erro em SETEUID (%s)\n", strerror (errno));

	printf ("No momento:\trgid = %d, egid = %d\n",  getgid (), getegid ());

	if (setegid (0) < 0)
		printf ("Não consegui voltar a ser SUPER (EFETIVO) (%s)\n", strerror (errno));

	printf ("No momento:\trgid = %d, egid = %d\n",  getgid (), getegid ());

	if (setrgid (0) < 0)
		printf ("Não consegui voltar a ser SUPER (REAL) (%s)\n", strerror (errno));

	printf ("No momento:\trgid = %d, egid = %d\n",  getgid (), getegid ());
}
