#include <sys/syscall.h>

#include <stdio.h>

main (int argc, const char *argv[])
{
	if (argc != 3)
		error ("$Não é assim que se usa!");

	if (rename (argv[1], argv[2]) < 0)
		error ("$*Erro na chamada RENAME");
}
