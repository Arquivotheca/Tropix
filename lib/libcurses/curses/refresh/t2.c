
/* Desloca lateralmente */

#include <stdio.h>
#include <curses.h>

main ()
{
	int	i;

	incurses ("teste lateral");
	setbuf (stdout, NULL);
	refon (R_INPUT|R_INVIS);

	for (i = 3 ; i > 0 ; i--)
	{
		attrset (A_NORMAL);
		move (10, 10);
		addstr ("012345678901234567890123456789");

		move (10, 66);
		getch ();

		attrset (A_NORMAL);
		move (10, 10);
		addstr ("012345678901234567890123456789");
		attrset (A_UNDERLINE);
		move (10, 13);
		addstr ("34567");

		move (10, 66);
		getch ();

		attrset (A_NORMAL);
		move (10, 10);
		addstr ("012345678901234567890123456789");
		attrset (A_UNDERLINE);
		move (10, 23);
		addstr ("34567");

		move (10, 66);
		getch ();

		attrset (A_NORMAL);
		move (10, 10);
		addstr ("012345678901234567890123456789");
		attrset (A_UNDERLINE);
		move (10, 33);
		addstr ("34567");

		move (10, 66);
		getch ();

	}

	outcurses ();
	exit (0);
}
