#include <stdio.h>
#include <string.h>

char	buf[125];

main ()
{
	register char	*cp;
	register int	c;

	for (;;)
	{
		if (gets (buf) != buf)
			return (0);

		printf ("'%s' => '", buf );
		for (cp = buf; *cp; cp++)
		{
			c = isotoabitb[*cp];
			putchar (c);
		}
		printf ("'\n");
	}
}
