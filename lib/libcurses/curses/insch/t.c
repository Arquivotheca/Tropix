/*
 ****************************************************************
 *								*
 *		t.c					*
 *								*
 *	X
 *								*
 *	Versão 1.0, de 08.05.87					*
 *								*
 *	Rotinas:						*
 *		x
 *								*
 *	Modulo: libcurses/curses				*
 *		Biblioteca Especial				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <signal.h>
#include <curses.h>

WINDOW	*win1, *win2, *win3;

int	onint ();
int	fswitch ();

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
main (argc, argv)
char	**argv;
{
	char	str[80];
	int	x, y;
	int	ch;

	signal (SIGINT, onint);

	if (incurses (argv[0]) == ERR)
	{
		printf ("Não consegui entrar em Curses\n");
		exit (1);
	}
	setswitch (fswitch);

	if ((win1 = newwin (LINES - 8, COLS, 8, 0)) == NOWINDOW)
	{
		printf ("Não consegui criar janela 1\n");
		outcurses ();
		exit (1);
	}

	if ((win2 = newwin (LINES - 4, COLS - 24 , 4, 14)) == NOWINDOW)
	{
		printf ("Não consegui criar janela 2\n");
		outcurses ();
		exit (1);
	}

	if ((win3 = newwin (12, 20, 0, 40)) == NOWINDOW)
	{
		printf ("Não consegui criar janela 3\n");
		outcurses ();
		exit (1);
	}


	/*
	 *	Programação de WIN1
	 */
	wgeton  (win1, G_IGXONXOFF);
	wgeton  (win1, G_ECHO|G_INSERT);
	wrefon (win1, R_WATCH|R_HOLD);
	waddon (win1, A_WRAP|A_SCROLL);

	wbox (win1, "WIN1", '|'|A_REVERSE, 0);

	wmove (win1, 10, 10);
	if (waddstr (win1, "En	tre:  ") == ERR)
	{
		printf ("Erro escrevendo 'Entre: '\n");
		outcurses ();
		exit (0);
	}

	/*
	 *	Programação de WIN2
	 */
	wgetoff (win2, G_ECHO|G_EDIT);
	wrefoff (win2, R_CURSOR);
	wrefon	(win2, R_WATCH|R_HOLD);
	wbox (win2, "WIN2", 0, 0);
	wmove (win2, 2, 20);
wattron (win2, A_REVERSE);
	waddstr (win2, "TROPIX CURSES");
	wmove (win2, 13, 2);
	if (waddstr (win2, "Sisco") == ERR)
	{
		printf ("Erro escrevendo 'Telebras '\n");
		outcurses ();
		exit (0);
	}
	wmove (win2, 20, 10);
	if (waddstr (win2, "TROPIX") == ERR)
	{
		printf ("Erro escrevendo 'TROPIX '\n");
		outcurses ();
		exit (0);
	}
	wmove (win2, 10, 10);

	/*
	 *	Programação de WIN3
	 */
	wbox (win3, "WIN3", 0, 0);
	wrefon  (win3, R_WATCH);

	for ( ; ; )
	{
		wmove (win1, 10, 13);
		touchwin (win1);
		wgetstr (win1, str);
		wgetstr (win1, str);
		wgetstr (win1, str);

		touchwin (win2);
		wgetch (win2);

/*
		wscroll (win1, 1);
		wrefresh (win1);
*/

		y = 0;	x = 40;
		mvwin (win3, y, x);
		touchwin (win3);
		wmove (win3, 1,1);
		waddstr (win3, "Entre:");
		wrefresh (win3);
		wgetstr (win3, str);
		waddstr (win3, "Posicione-me");
		wrefresh (win3);

		wrefoff (win3, R_CURSOR);
		wgetoff (win3, G_ECHO);
		wgeton (win3, G_KEYPAD);
		do
		{
			switch (ch = wgetch (win3))
			{
			    case KEY_DOWN:
				if (1 || y < 12)
					y++;
				else
					beep ();
				break;

			    case KEY_UP:
				if (1 || y > 0)
					y--;
				else
					beep ();
				break;

			    case KEY_LEFT:
				if (1 || x > 0)
					x--;
				else
					beep ();
				break;

			    case KEY_RIGHT:
				if (1 || x < 60)
					x++;
				else
					beep ();
				break;

			    case 'q':
				outcurses ();
				exit (0);
			}
			wmove (win3, 10, 2);
			wprintw (win3, "(%2d,%2d)", y, x);
			mvwin (win3, y, x);
		}
		while (ch != '\n');

		wgeton (win3, G_ECHO);
		wgetoff (win3, G_KEYPAD);
		wrefon (win3, R_CURSOR);

		untouchwin (win3);

		wgetch (win2);
	}

	outcurses ();

	exit (0);

}	/* end main */
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
onint ()
{
	outcurses ();
	printf ("Good bye!\n");
	exit (0);
}

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
fswitch ()
{
	mvwin (win3, 10, 10);
	touchwin (win3);
	wmove (win3, 1, 1);
	waddstr (win3, "SWITCH: entre uma linha");
	wrefresh (win3);
	for ( ; wgetch (win3) != '\n'; )
		;
	untouchwin (win3);
	werase (win3);
}
