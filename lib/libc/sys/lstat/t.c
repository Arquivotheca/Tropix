#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	STAT		s, ls;
	const char	*file_nm;

	if (argc > 1)
		file_nm = argv[1];
	else
		file_nm = "../t.c";

	if (stat (file_nm, &s) < 0)
		error ("$*Erro no stat");

	if (lstat (file_nm, &ls) < 0)
		error ("$*Erro no stat");

	printf ("stat:  %c: size = %d\n", (modetostr (s.st_mode))[0], s.st_size);
	printf ("lstat: %c: size = %d\n", (modetostr (ls.st_mode))[0], ls.st_size);

	return (0);

}	/* end mail */
