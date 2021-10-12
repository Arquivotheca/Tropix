#include <stdio.h>

main ()
{
	int	ch;

	system ("stty raw -echo\n");

	do
	{
		ch = getchar ();
		switch (ch)
		{
		    case ' ':
		    case 'q':
			putchar ('\r');
			putchar ('\n');
			break;

		    default:
			printf ("%d ", ch);
			break;
		}
	}
	while (ch != 'q');

	putchar ('\n');

	system ("stty -raw echo\n");
	exit (0);
}
