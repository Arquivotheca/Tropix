
#include <stdio.h>
#include <curses.h>

char	pgname[] = "teste";

main ()
{
	/*
	 *	Iniciando operação com Curses
	 */
	if (incurses (pgname) == ERR)
	{
		perror (pgname);
		fprintf
		(	stderr,
			"%s: Atribua à variável \"TERM\" ",
			"o tipo de seu terminal\n",
			pgname
		);
		exit (1);
	}

	outcurses ();

#if (0)	/*************************************/
	/*
	 *	x
	 */
	Comm = newcomm ();
	View = newview ();

	/*
	 *	Janelas
	 */
	ready = newwin (1, COLS, LINES - 2, 0);
	waddstr (ready, "Aperte uma tecla para voltar ao modo Visual");
	wrefon (ready, R_HOLD);
	wgetset (ready, 0);

	bigwin = stdwin;
	smallwin = newwin (3, COLS, LINES - 3, 0);

	wrefon (bigwin, R_HOLD);
	wrefon (View->v_win, R_HOLD);
	wrefon (View->v_err, R_HOLD);


	Comm->c_cur = FIRST ();
	View->v_base = FIRST ();
#endif	/*************************************/

}	/* end main */
