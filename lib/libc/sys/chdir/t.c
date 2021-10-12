#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	err, ret;
	char		s[30];
	char		buf[80];

	while (gets (s) == s)
	{
		ret = chdir (s);

		err = errno;

		printf ("ret = %d, getcwd = %s\n",
			ret, getcwd (buf, 80) );
		if (ret < 0)
			printf ("t: %s\n", strerror (err) );

	}

	ret = chdir ((char *)0x560000);

	err = errno;

	printf ("ret = %d, getcwd = %s\n",
		ret, getcwd (buf, 80) );
	if (ret < 0)
		printf ("t: %s\n", strerror (err) );

} 
