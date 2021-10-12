#include <sys/syscall.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define EVER	;;

long		getint (const char *str);

main ()
{
	char		line[80];
	long		area, size;
	const char	*str;
	const void	*r;

	for (EVER)
	{
		printf ("area = ");
		if (gets (line) == NOSTR)
			exit (0);
		area = getint (line);

		printf ("size = ");
		if (gets (line) == NOSTR)
			exit (0);
		size = getint (line);

		printf ("phys (%d, %d, O_RDONLY) retornou: ", area, size);

		r = phys ((void *)area, size, O_RDONLY);

		if ((int)r == -1)
			printf ("ERRO (%s)\n", strerror (errno));
		else
			printf ("%P\n", r);
	}
}

/*
 ****************************************************************
 *	Converte um número dado					*
 ****************************************************************
 */
long
getint (const char *str)
{
	register long 	n;
	const char	*ptr;

	n = strtol (str, &ptr, 0);

	switch (*ptr)
	{
	    case 'w':
	    case 'W':
		n *= 2;
		break;

	    case 'l':
	    case 'L':
		n *= 4;
		break;

	    case 'k':
	    case 'K':
		n *= 1024;
		break;

	    case 'm':
	    case 'M':
		n *= 1024 * 1024;
		break;

	    case 'b':
	    case 'B':
		n *= 51;
		break;

	    case '\0':
		break;
		
	    default:
		break;

	}	/* end switch (*ptr) */

	return (n);

}	/* end getint */

