#include <sys/types.h>
#include <sys/syscall.h>

#include <stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int
main (int argc, const char *argv[])
{
	const char	*file_nm;
	DIR		*ddp;
	DIRENT		*dp;
	off_t		offset = 0;
	STAT		s;
	int		i;

	if (argv[1])
		file_nm = argv[1];
	else
		file_nm = ".";

	if ((ddp = opendir (file_nm)) == NODIR)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	fstat (ddp->d_fd, &s);

	while (dp = readdir (ddp))
	{
		printf ("%6d, %3d, \"%s\", eof = %d\n", dp->d_ino, dp->d_namlen, dp->d_name, ddp->d_eof);

		if (streq (dp->d_name, ".."))
		{
			offset = telldir (ddp);
			printf ("telldir após \"..\" = %d\n", offset);
		}
	}

	printf ("\ntelldir = %d\n\n", telldir (ddp));

	/*
	 *	Teste de "seekdir"
	 */
	printf ("Teste de \"seekdir\", após \"..\":\n\n");

	if (seekdir (ddp, offset) < 0)
		error ("$*Erro no \"seekdir\"");

	while (dp = readdir (ddp))
		printf ("%6d, %3d, \"%s\"\n", dp->d_ino, dp->d_namlen, dp->d_name);

	if (closedir (ddp) < 0)
		error ("$*Não consegui fechar \"%s\"", file_nm);

	/*
	 *	x
	 */
	printf ("\nTeste de \"inopendir\":\n\n");

	if ((ddp = inopendir (s.st_dev, s.st_ino)) == NODIR)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	for (i = 4; i > 0 && (dp = readdir (ddp)); i--)
		printf ("%6d, %3d, \"%s\"\n", dp->d_ino, dp->d_namlen, dp->d_name);

	/*
	 *	Teste de "rewinddir"
	 */
	printf ("\nTeste de \"rewinddir\":\n\n");

	if (rewinddir (ddp) < 0)
		error ("$*Erro no \"seekdir\"");

	for (i = 4; i > 0 && (dp = readdir (ddp)); i--)
		printf ("%6d, %3d, \"%s\"\n", dp->d_ino, dp->d_namlen, dp->d_name);

	if (closedir (ddp) < 0)
		error ("$*Não consegui fechar \"%s\"", file_nm);

	return (0);
}
