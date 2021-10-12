#include <stdio.h>
#include <curses.h>

main ()
{
	int	i;

	incurses ("");
	refon (R_INPUT|R_INVIS);

	for (i = 3 ; i > 0 ; i--)
	{
		attrset (A_REVERSE);
		move (10, 10);
		addstr ("Opção 1");
	
		attrset (A_NORMAL);
		move (11, 10);
		addstr ("Opção 2");

		attrset (A_NORMAL);
		move (12, 10);
		addstr ("Opção 3");
	
		move (10, 20);
		getch ();
	
		attrset (A_NORMAL);
		move (10, 10);
		addstr ("Opção 1");
	
		attrset (A_REVERSE);
		move (11, 10);
		addstr ("Opção 2");
	
		attrset (A_NORMAL);
		move (12, 10);
		addstr ("Opção 3");
	
		move (11, 20);
		getch ();
	
		attrset (A_NORMAL);
		move (10, 10);
		addstr ("Opção 1");
	
		attrset (A_NORMAL);
		move (11, 10);
		addstr ("Opção 2");
	
		attrset (A_REVERSE);
		move (12, 10);
		addstr ("Opção 3");
	
		move (12, 20);
		getch ();
	}

	outcurses ();
	exit (0);
}
