#include <sys/types.h>
#include <sys/stat.h>
#include <sys/kcntl.h>
#include <sys/sysdirent.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define	COUNT	33
#define	EVER	;;

int
main (int argc, const char *argv[])
{
	int		par_ino;
	char		*path, *cp;
	char		dev_nm[16];
	STAT		s;
	DIRENT		de;

	if ((path = (char *)argv[1]) == NOSTR)
		return (0);

	for (EVER)
	{
		if (stat (path, &s) < 0)
			error ("$*erro no STAT");

		if ((par_ino = getdirpar (s.st_dev, s.st_ino, &de)) < 0)
			error ("$*erro no getdirpar");

		if (kcntl (K_GETDEVNM, s.st_dev, dev_nm) < 0)
			error ("$*erro no GETDEVNM");

		printf ("par_dev = %s, par_ino = %d, len = %d, name = \"%s\"\n", dev_nm, par_ino, de.d_namlen, de.d_name);

		if ((cp = strrchr (path, '/')) == NOSTR)
			break;

		cp[0] = '\0';
	}

	return (0);
}
