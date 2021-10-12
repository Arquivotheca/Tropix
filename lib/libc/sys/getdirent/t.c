#include <sys/types.h>
#include <sys/sysdirent.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define	COUNT	33

int
main (int argc, const char *argv[])
{
	int		fd, n, count;
	int		eof = 0;
	DIRENT		dirent[COUNT], *dp;
	const char	*file_nm;

	file_nm = ".";
	count	= COUNT;

	if (argv[1] != NOSTR)
	{
		file_nm = argv[1];

		if (argv[2] != NOSTR)
			count = atol (argv[2]);
	}

#if (0)	/*******************************************************/
	if (argv[1])
		file_nm = argv[1];
	else
		file_nm = ".";
#endif	/*******************************************************/

	fd = open (file_nm, O_RDONLY);

	error ("sizeof (dirent) = %d", sizeof (dirent));

	for (;;)
	{
		if ((n = getdirent (fd, dirent, count * sizeof (DIRENT), &eof)) < 0)
			error ("$*Erro em getdirent");

		if (n == 0)
			break;

		error
		(	"getdirent retornou %d, %d elementos, %d residual, eof = %d\n",
			n, n / sizeof (DIRENT), n % sizeof (DIRENT), eof
		);

		for (dp = dirent; n >= sizeof (DIRENT); dp++, n -= sizeof (DIRENT))
			printf ("%6d, %3d, \"%s\"\n", dp->d_ino, dp->d_namlen, dp->d_name);
	}

	close (fd);

	return (0);
}
