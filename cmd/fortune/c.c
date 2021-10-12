#include <stdio.h>

#define	elif	else if

main ()
{
	register int	c;
	int		nl_seen = 0;
	int		percent_written = 0;
	int		line = 1;

	while ((c = getchar ()) >= 0)
	{
		if (c != '\n')
		{
			percent_written = 0;

			while (nl_seen > 0)
				{ putchar ('\n'); nl_seen--; }

			putchar (c); nl_seen = 0;
		}
		else /* é <nl> */
		{
			line++;

			if (percent_written)
				printf ("Mais do que 2 linhas em brancos seguidas, linha %d\n", line - 1);

			switch (++nl_seen)
			{
			    case 1:
			    case 2:
				break;

			    case 3:
				putchar ('\n');
				putchar ('%');
				putchar ('\n');
				nl_seen = 0;
				percent_written++;

			}	/* end switch */
		}
	}

	if (nl_seen)
		putchar ('\n');
}
