#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

main (int argc, register char *argv[])
{
	int	ruid = -1, euid = -1;

	argc--; argv++;

	printf ("No momento:\truid = %d, euid = %d\n",  getuid (), geteuid ());

	if (argc > 0 && !streq (argv[0], "-"))
		ruid = atoi (argv[0]);

	if (argc > 1 && !streq (argv[1], "-"))
		euid = atoi (argv[1]);

	printf ("Valores dados:\truid = %d, euid = %d\n",  ruid, euid);

	if (setruid (ruid) < 0)
		printf ("Erro em SETRUID (%s)\n", strerror (errno));

	if (seteuid (euid) < 0)
		printf ("Erro em SETEUID (%s)\n", strerror (errno));

	printf ("No momento:\truid = %d, euid = %d\n",  getuid (), geteuid ());

	if (seteuid (0) < 0)
		printf ("Não consegui voltar a ser SUPER (EFETIVO) (%s)\n", strerror (errno));

	printf ("No momento:\truid = %d, euid = %d\n",  getuid (), geteuid ());

	if (setruid (0) < 0)
		printf ("Não consegui voltar a ser SUPER (REAL) (%s)\n", strerror (errno));

	printf ("No momento:\truid = %d, euid = %d\n",  getuid (), geteuid ());
}
