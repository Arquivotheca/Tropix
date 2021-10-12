/*
 ****************************************************************
 *								*
 *			xecho.c					*
 *								*
 *	Ecoa caracteres em uma janela				*
 *								*
 *	Versão	1.0.0, de 04.03.97				*
 *								*
 *	Módulo: programs/xecho/xecho.c				*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1995 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 1.0.0, de 04.03.97";
entry	const char	pgoptions[]	= "G:f:MH";

/*
 ****************************************************************
 *	Macros & Tipos						*
 ****************************************************************
 */
#define	ESC		27		/* O caractere Esc */
#define	MARGIN		5		/* Margem horizontal */

typedef	unsigned int	uint;

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
entry	int	winx = 100,
		winy = 100,
		windx = 500,
		windy = 500;

entry	char		*fontname;
entry	Display		*dp;
entry	Window		win;
entry	GC		gc;
entry	XFontStruct	*fip;
entry	uint		width, height;
entry	int		curx, cury;

/*
 ****************************************************************
 *	Protótipos de Funções Locais				*
 ****************************************************************
 */
static	void		help (int);
entry	void		msg (char *, ...);
entry	void		get_geometry (const char *);
entry	void		get_font_info (void);
entry	void		home (void);
entry	void		clear_window (void);
entry	void		new_line (void);
entry	void		display_text (char *, int);
entry	int		process_string (char *, int);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		screen;
	ulong		fg, bg;
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
	while ((opt = getopt (argc, argv, pgoptions)) != EOF)
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
	if ((dp = XOpenDisplay ("")) == (Display *)NULL)
		msg ("$Não consegui conectar-me ao servidor");

	screen = DefaultScreen (dp);

	bg = WhitePixel (dp, screen);
	fg = BlackPixel (dp, screen);

	win =	XCreateSimpleWindow
		(	dp,
			DefaultRootWindow (dp), 
			winx, winy, windx, windy,
			2,
			fg, bg			/* cores: frente e fundo */
		);

	gc = DefaultGC (dp, screen);

	XSetForeground (dp, gc, fg);
	XSetBackground (dp, gc, bg);

	if (fontname != NOSTR)
	{
		if (font = XLoadFont (dp, fontname))
			XSetFont (dp, gc, font);
	}

	XSelectInput (dp, win, ExposureMask|KeyPressMask|ButtonPressMask);
	XMapRaised (dp, win);
	get_font_info ();
	home ();

	for (doneflag = 0; doneflag == 0; /* sem incremento */)
	{
		XNextEvent (dp, &ev);

		switch (ev.type)
		{
		    case Expose:
			break;

		    case KeyPress:
			len = XLookupString (&ev.xkey, text, 10, &key, 0);
			text[len] = '\0';
			doneflag = process_string (text, len);
			break;

		    case ButtonPress:
			doneflag = 1;
			break;
		}
	}

	XDestroyWindow (dp, win);
	XCloseDisplay (dp);

	exit (0);

}	/* end main */

/*
 ****************************************************************
 *	Formata um texto					*
 ****************************************************************
 */
void
get_font_info (void)
{
	Window		root;
	int		x, y;
	uint		border, depth;

	XGetGeometry (dp, win, &root, &x, &y, &width, &height, &border, &depth);

	fip = XQueryFont (dp, XGContextFromGC (gc));

}	/* end get_font_info */

/*
 ****************************************************************
 *	Posiciona o cursor no início da janela			*
 ****************************************************************
 */
void
home (void)
{
	curx = MARGIN;
	cury = fip->ascent + MARGIN;

}	/* end home */

/*
 ****************************************************************
 *	Limpa a Janela						*
 ****************************************************************
 */
void
clear_window (void)
{
	XClearWindow (dp, win);
	XFlush (dp);
	home ();

}	/* end clear_window */

/*
 ****************************************************************
 *	Muda de linha						*
 ****************************************************************
 */
void
new_line (void)
{
	curx = MARGIN;
	cury += fip->ascent + fip->descent;

}	/* end new_line */

/*
 ****************************************************************
 *	Imprime a cadeia					*
 ****************************************************************
 */
void
display_text (char *text, int len)
{
	XCharStruct	overall;
	int		dir, ascent, descent;

	XTextExtents (fip, text, len, &dir, &ascent, &descent, &overall);

	if (curx + overall.width + MARGIN > width)
		new_line ();

	XDrawImageString (dp, win, gc, curx, cury, text, len);

	curx += overall.width;

	if (curx + MARGIN > width)
		new_line ();

	XFlush (dp);

}	/* end display_text */

/*
 ****************************************************************
 *	Processa uma cadeia					*
 ****************************************************************
 */
int
process_string (char *str, int len)
{
	char	c, *cp;
	int	i;

	for (cp = str; (c = *cp) != '\0'; cp++)
	{
		switch (c)
		{
		    case 1:
			for (i = 0; i < 256; i++)
				display_text ((char *)&i, 1);
			break;

		    case ESC:
			return (1);

		    case '\r':
		    case '\n':
			new_line ();
			break;

		    case '\f':
			clear_window ();
			break;

		    default:
			display_text (&c, 1);
		}
	}

	return (0);

}	/* end process_string */

/*
 ****************************************************************
 *	Extrai a geometria desejada				*
 ****************************************************************
 */
void
get_geometry (const char *geo)
{
	sscanf (geo, "%d,%d,%d,%d", &winx, &winy, &windx, &windy);

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
		"%s - ecoa, em uma janela, o que é teclado\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-G <x>,<y>,<dx>,<dy>] [-f <fonte>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-G: descreve a geometria da janela\n"
		"\t-f: utiliza a <fonte> dada\n"
	);

	exit (exitval);

}	/* end help */
