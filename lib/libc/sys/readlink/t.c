#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	char		area[512];
	int		value;

	if (argv[1] == NOSTR)
		return (0);

	area[0] = '\0';

	if ((value = readlink (argv[1], area, 512)) < 0)
		error ("*Errno no \"readlink\"");

	printf ("value = %d\n", value);

	if (area[0] != '\0')
		printf ("%s\n", area);

	return (0);
}
