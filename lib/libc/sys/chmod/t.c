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
		n = strtol (s, (const char **)0, 0);

		ret = chmod ("t.c", n);

		err = errno;

		printf ("n = %o, ret = %d", n, ret); 
		if (ret < 0)
			printf (" %s\n", strerror (err) );
		else
			putchar ('\n');

		stat ("t.c", &statb);

		printf ("%s\n", modetostr (statb.st_mode) );

	}
} 
