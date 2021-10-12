#include <stdio.h>
#include <signal.h>
#include <curses.h>

getalarm ()
{
	signal (SIGALRM, getalarm);
}

main ()
{
	chtype	ch;

	incurses ("");
	refon (R_INPUT);
	getset (G_KEYPAD|G_TONL);

	do
	{
		ch = getch ();

		switch (ch)
		{
		    case KEY_HOME:
			addstr ("KEY_HOME\n");
			break;

		    case KEY_END:
			addstr ("KEY_END\n");
			break;

		    case KEY_PPAGE:
			addstr ("KEY_PPAGE\n");
			break;

		    case KEY_NPAGE:
			addstr ("KEY_NPAGE\n");
			break;

		    case KEY_LEFT:
			addstr ("KEY_LEFT\n");
			break;

		    case KEY_RIGHT:
			addstr ("KEY_RIGHT\n");
			break;

		    case KEY_UP:
			addstr ("KEY_UP\n");
			break;

		    case KEY_DOWN:
			addstr ("KEY_DOWN\n");
			break;

		    case KEY_F1:
			addstr ("KEY_F1\n");
			break;

		    case KEY_F2:
			addstr ("KEY_F2\n");
			break;

		    case KEY_F3:
			addstr ("KEY_F3\n");
			break;

		    case KEY_F4:
			addstr ("KEY_F4\n");
			break;

		    case KEY_F5:
			addstr ("KEY_F5\n");
			break;

		    case KEY_F6:
			addstr ("KEY_F6\n");
			break;

		    case KEY_F7:
			addstr ("KEY_F7\n");
			break;

		    case KEY_F8:
			addstr ("KEY_F8\n");
			break;

		    case KEY_F9:
			addstr ("KEY_F9\n");
			break;

		    case KEY_F0:
			addstr ("KEY_F0\n");
			break;

		    default:
			addch (ch);
		}

	}
	while (ch != 'Q');

	outcurses ();
	exit (0);
}
