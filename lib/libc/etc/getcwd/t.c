#include <string.h>
#include <errno.h>

#define	NOSTR	(char *)0

char	buf[256];

main ()
{
	char		*path;

#if (0)	/*************************************/
	if ((path = getcwd (buf, 256)) == NOSTR)
		printf ("%s\n", strerror (errno));
	else
		printf ("%s\n", path);

	chdir ("/src");

#endif	/*************************************/
	printf ("%s\n", getcwd (buf, 256) );

	chdir ("/src/xwin/cmd/xterm/src");

	printf ("%s\n", getcwd (buf, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
	chdir ("..");
	printf ("%s\n", getcwd ((char *)0, 256) );
#if (0)	/*******************************************************/
#endif	/*******************************************************/
}
