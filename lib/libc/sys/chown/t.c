
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	STAT		statb;
	register	n, err, ret;
	char		s[30];

	while (gets (s) == s)
	{
		n = atoi (s);

		ret = chown ("t.c", n, n+3);

		err = errno;

		printf ("n = %d, ret = %d", n, ret); 
		if (ret < 0)
			printf (" %s\n", strerror (err) );
		else
			putchar ('\n');

		stat ("t.c", &statb);

		printf ("uid = %d, gid = %d\n", statb.st_uid, statb.st_gid);

	}
} 
