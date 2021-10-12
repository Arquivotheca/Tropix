/*
 ****************************************************************
 *								*
 *			xcoord.c				*
 *								*
 *	Imprime as coordenadas do click do mouse		*
 *								*
 *	Versão	4.4.0, de 19.03.03				*
 *								*
 *	Módulo: lib/X11/tst/xcoord.c				*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.4.0, de 19.03.03";

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
entry	int	x = 100,
		y = 100,
		dx = 500,
		dy = 500;

entry	char	*fontname;

/*
 ****************************************************************
 *	Protótipos de Funções Locais				*
 ****************************************************************
 */
static	void		help (int);
entry	void		msg (char *, ...);
entry	void		get_geometry (const char *);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	Display		*dp;
	Window		win;
	int		screen;
	ulong		fg, bg;
	GC		gc;
	XEvent		ev;
	Font		font;
	KeySym		key;
	char		text[80];
	int		doneflag, len, opt, exitval;

	/*
	 *	Inicializa opções.
	 */
	exitval = 0;

	/*
	 *	Analisa as opções de execução.
	 */
	while ((opt = getopt (argc, argv, "G:f:MH")) != EOF)
	{
		switch (opt)
		{
		    case 'M':
			exit (0);

		    case 'H':
			help (0);
			break;

		    case 'f':
			fontname = (char *)optarg;
			break;

		    case 'G':
			get_geometry (optarg);
			break;

		    default:
			putc ('\n', stderr);
			help (2);

		}	/* end switch (opt) */

	}	/* while ((opt = getopt (...))) */

	/*
	 *	Conecta-se ao servidor.
	 */
	if ((dp = XOpenDisplay (NULL)) == (Display *)NULL)
		msg ("$Não consegui conectar-me ao servidor");

	screen = DefaultScreen (dp);

	bg = WhitePixel (dp, screen);
	fg = BlackPixel (dp, screen);

	win =	XCreateSimpleWindow
		(	dp,
			DefaultRootWindow (dp), 
			x, y, dx, dy,
			2,
			fg, bg			/* cores: frente e fundo */
		);

	gc = DefaultGC (dp, screen);

	if (fontname != NOSTR)
	{
		if (font = XLoadFont (dp, fontname))
			XSetFont (dp, gc, font);
	}

	XSelectInput (dp, win, ButtonPressMask|KeyPressMask|ExposureMask);

	XMapRaised (dp, win);

	for (doneflag = 0; doneflag == 0; /* sem incremento */)
	{
		XNextEvent (dp, &ev);

		switch (ev.type)
		{
		    case Expose:
			break;

		    case ButtonPress:
			sprintf
			(	text,
#if (0)	/****************************************************/
				"(%d, %d), (%d, %d)",
				ev.xbutton.x_root, ev.xbutton.y_root,
#endif	/****************************************************/
				"(%d, %d)",
				ev.xbutton.x, ev.xbutton.y
			);

			XDrawImageString
			(	dp, win, gc,
				ev.xbutton.x, ev.xbutton.y,
				text, strlen (text)
			);

			break;

		    case KeyPress:
			len = XLookupString (&ev.xkey, text, 10, &key, 0);

			if (len == 1 && text[0] == 'q')
				doneflag++;
			break;
		}
	}


	XDestroyWindow (dp, win);
	XCloseDisplay (dp);

	exit (0);

}	/* end main */

/*
 ****************************************************************
 *	Extrai a geometria desejada				*
 ****************************************************************
 */
void
get_geometry (const char *geo)
{
	sscanf (geo, "%d,%d,%d,%d", &x, &y, &dx, &dy);

}	/* end get_geometry */

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
static void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - imprime as coordenadas onde o mouse foi clicado\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-G <x>,<y>,<dx>,<dy>] [-f <fonte>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-G: descreve a geometria da janela\n"
		"\t-f: usa a <fonte> de caracteres\n"
	);

	exit (exitval);

}	/* end help */
