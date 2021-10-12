#include <stdio.h>
#include <curses.h>

main ()
{
	WINDOW 	*pj1, *pj2, *pj3, *pj4;
	char	str[20];

	incurses ("teste");

	pj1 = newwin (2,80,0,0);
	pj2 = newwin (20,17,3,0);
	pj3 = newwin (20,62,3,18);
	pj4 = newwin (1,80,23,0);

	wattrset (pj3, A_UNDERLINE);

	wbox (pj1, "TITULO", 0, 0);
	wbox (pj2, "HELP", 0, 0);
	wbox (pj3, "TEXTO", 0, 0);

	touchwin (pj1);
	touchwin (pj2);
	touchwin (pj3);
	touchwin (pj4);

/*
	mvwinsstr (pj1, 0, 10, "Titulo: ");
	wrefresh (pj1);
*/
	mvwinsstr (pj2, 4, 10, "Help");
	wrefresh (pj2);
	mvwinsstr (pj3, 4, 18, "Forneca o dado tal: ");
	wrefresh (pj3);

	wgetstr (pj3,str);

	delwin (pj2);
	getch ();

	outcurses ();
	exit (0);
}
