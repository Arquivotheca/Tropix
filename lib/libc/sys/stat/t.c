#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	STAT		s;
	const char	*file_nm;

	if (argc > 1)
		file_nm = argv[1];
	else
		file_nm = "../t.c";

	if (stat (file_nm, &s) < 0)
		error ("$*Erro no stat");

	printf ("%c: size = %d\n", (modetostr (s.st_mode))[0], s.st_size);

	return (0);

}	/* end mail */
