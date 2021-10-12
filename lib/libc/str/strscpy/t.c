#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[12];
char	cbuf[12];

int
main (int argc, register char *argv[])
{
	int		size = argv[1] ? strtol (argv[1], 0, 0) : 3;
	register char	*cp;
	register int	i;

	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (buf) != buf)
			return (0);

		memset (cbuf, '+', sizeof cbuf);

		cp = strscpy (cbuf, buf, size);

		if (cp != cbuf)
			printf ("*** Não retornou o destino\n");

		for (i = 0; i < 12; i++)
		{
			if (cp[i])
				printf ("%c", cp[i]);
			else
				printf ("%c", 0xA4 /* == ¤ */);
		}

		printf ("\n");
	}

	return (0);
}
