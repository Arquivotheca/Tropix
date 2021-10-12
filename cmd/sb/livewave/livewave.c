/*
 ****************************************************************
 *								*
 *			livewave.c				*
 *								*
 *	Toca música PCM através da placa de som SB-LIVE		*
 *								*
 *	Versão	4.4.0, de 21.01.03				*
 *		4.4.0, de 21.01.03				*
 *								*
 *	Módulo: livewave					*
 *		Utilitários Extremamente sofisticados		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/pcntl.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#if (0)	/*******************************************************/
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.4.0, de 21.01.03";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define KBSZ	1024
#define MBSZ	(1024 * 1024)

#define KBSHIFT	10
#define MBSHIFT	20

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	dflag;		/* Tipo do arquivo */
entry int	tflag;		/* Modo texto */

entry int	live_fd;	/* Descritor da placa SB16 */

entry char	xwin_active;	/* X-Window em ação SB16 */

/*
 ******	Estruturas **********************************************
 */
typedef struct
{
	char	r_riff[4];	/* Nome RIFF */
	long	r_size;		/* Tamanho */

}	RIFF;

typedef struct
{
	char	r_name[4];	/* Nome do membro */

}	NAME;

typedef struct
{
	short	f_type;		/* Tipo do som */
	short	f_channels;	/* No. de canais */
	long	f_rate;		/* Freqüência */
	long	f_buf_sz;	/* Tamanho da área recomendada */
	short	f_bytes;	/* No. de bytes por amostra */
	short	f_bits;		/* No. de bits por amostra */

}	FMT;

entry FMT	fmt;

typedef struct
{
	int	w_offset;	/* Deslocamento */
	int	w_size;		/* Tamanho */

}	WAVE;

entry WAVE	wave;		/* Os dados do 1. wave encontrado */

#if (0)	/*******************************************************/
/*
 ******	Variáveis e Definições globais da parte gráfica *********
 */
#define Xcmap		DefaultColormap (Xdisplay, Xscreen)
#define Xscreen		DefaultScreen (Xdisplay)

/*
 ******	Geometria ***********************************************
 */
#define	WIDTH	540	/* Largura total */
#define	OFF_X	10	/* Margem esquerda */

#define	HEIGHT	90	/* Altura total */
#define	TITLE_Y	23	/* Altura do título */
#define	BAR_Y	40	/* Altura do título */
#define	TIMES_Y	73	/* Altura dos tempos */

XSizeHints	szhint =
{
	PMinSize | PResizeInc | PBaseSize | PWinGravity,
	0, 0, WIDTH, HEIGHT,		/* x, y, width, height */
	WIDTH, HEIGHT,			/* Min width, height */
	0, 0,				/* Max width, height - não usado */
	1, 1,				/* incrementos: width, height */
	{1, 1},				/* incrementos: x, y */
	{0, 0},				/* Aspect ratio - não usado */
	0, 0,				/* base size: width, height */
	NorthWestGravity		/* gravity */
};

entry XClassHint	classhint;
entry XWMHints		wmhint;
entry Atom		wmDeleteWindow;

/*
 ******	Variáveis globais ***************************************
 */
entry	Display		*dpy;
entry	Window		win;
entry	GC		text_gc, bar_gc;
entry	XEvent		ev;
#endif	/*******************************************************/

/*
 ****** Protótipos de funções ***********************************
 */
int		analyse_riff (const char *, FILE *);
void		analyse_wave (const char *, FILE *);
void		live_play_file (const char *, FILE *, int, char *);
void		draw_title_and_times (const char *);
const char	*cv_bytes_to_KB_or_MB (unsigned long);
const char	*live_edit_time (int);
int		getpercent (long, long);
#if (0)	/*******************************************************/
int		openwin (int, const char *[]);
ulong		get_window_color (Display *, const char *, ulong);
void		geometry_to_size_hints (Display *, const char *, XSizeHints *);
#endif	/*******************************************************/
void		help (void);

/*
 ****************************************************************
 *	Toca música PCM através da placa de som SB-LIVE		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*file_nm;
	FILE		*file_fp;
	int		size = 0;
	char		*area0 = NOSTR;
#if (0)	/*******************************************************/
	int		opt, fd = -1;
	unsigned long	paddr, offset;
	int		size = 0, dma_size;
#endif	/*******************************************************/

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vdtH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Tipo do arquivo */
			dflag++;
			break;

		    case 't':			/* Modo texto */
			tflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Abre a placa de som
	 */
	if ((live_fd = open ("/dev/live", O_WRONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", "/dev/live");

#if (0)	/*******************************************************/
	/*
	 *	Verifica se o X-Window está ativo
	 */
	if (!tflag && !dflag && (fd = open ("/dev/xconsole", O_RDWR)) < 0 && errno == EBUSY)
		xwin_active++;

	if (fd >= 0)
		close (fd);

	if (xwin_active)
	{
		long		pid;

		if ((pid = fork ()) > 0)
			exit (0);

		if (pid < 0)
			error ("$*Não consegui criar outro processo");

		openwin (argc, argv);
	}
#endif	/*******************************************************/

	/*
	 *	Obtém 2 áreas convenientemente alinhadas
	 */
	size = 32 * KBSZ;

	area0 = malloc (size);

	/*
	 *	Abre e examina o diversos arquivos
	 */
	while ((file_nm = *argv++) != NOSTR)
	{
		if ((file_fp = fopen (file_nm, "r")) == NOFILE)
			{ error ("*Não consegui abrir \"%s\"", file_nm); continue; }

		/*
		 *	Inicialmente examina o arquivo
		 */
		if (analyse_riff (file_nm, file_fp) < 0)
			continue;

		/*
		 *	Verifica se simplesmente quer os
		 *	atributos do arquivo ou tocar o arquivo
		 */
		if (!dflag)
			live_play_file (file_nm, file_fp, size, area0);

		fclose (file_fp);

	}	/* end while */


	return (exit_code);

}	/* end livewave */

/*
 ****************************************************************
 *	Analisa o formato do arquivo				*
 ****************************************************************
 */
int
analyse_riff (const char *file_nm, FILE *file_fp)
{
	RIFF		riff;
	NAME		name;

	/*
	 *	Examina o arquivo
	 */
	if (fread (&riff, sizeof (RIFF), 1, file_fp) != 1)
	{
		error ("*Não consegui ler o cabeçalho RIFF de \"%s\"", file_nm);
		return (-1);
	}

	/*
	 *	Imprime os valores RIFF
	 */
#undef	DEBUG
#ifdef	DEBUG
	printf ("%4.4s %d\n", riff.r_riff,  riff.r_size);
#endif	DEBUG

	/*
	 *	Procura os diversos componentes
	 */
	for (EVER)
	{
		if (fread (&name, sizeof (NAME), 1, file_fp) != 1)
			break;
#ifdef	DEBUG
		printf ("%4.4s\n", name.r_name);
#endif	DEBUG

		if (strncmp (name.r_name, "WAVE", sizeof (name.r_name)))
		{
			error ("*O arquivo \"%s\" arquivo NÃO tem o formato \"wave\"", file_nm);
			return (-1);
		}

		analyse_wave (file_nm, file_fp);

		break;
	}

	/*
	 *	Verifica se encontrou um dado de WAVE
	 */
	if (wave.w_size == 0)
	{
		error ("*O arquivo \"%s\" arquivo NÃO contém o dado de \"wave\"", file_nm);
		return (-1);
	}

	if (!dflag)
		return (0);

	printf
	(	"\nArquivo \"%s\":\n\n"
		"Deslocamento no arquivo		= %d bytes\n"
		"Tamanho				= %d bytes\n"
		"Tipo	 			= %d (%s)\n"
		"Freqüência de amostragem	= %d Hz\n"
		"Tamanho da área recomendada	= %d bytes\n"
		"No. de bytes por amostra	= %d\n"
		"No. bits por amostra		= %d\n"
		"Tempo				= %s\n",

		file_nm,
		wave.w_offset, wave.w_size,

		fmt.f_type,
		fmt.f_channels > 1 ? "Stereo" : "Mono",
		fmt.f_rate,
		fmt.f_buf_sz,
		fmt.f_bytes,
		fmt.f_bits,
		live_edit_time (wave.w_size)
	);

	return (0);

}	/* end analyse_riff */

/*
 ****************************************************************
 *	Analisa a seção WAVE					*
 ****************************************************************
 */
void
analyse_wave (const char *file_nm, FILE *file_fp)
{
	RIFF		riff;
	int		offset;

	offset = ftell (file_fp);

	for (EVER)
	{
		if (fseek (file_fp, offset, SEEK_SET) < 0)
			break;

		if (fread (&riff, sizeof (RIFF), 1, file_fp) != 1)
			break;
#ifdef	DEBUG
		printf ("%4.4s %d\n", riff.r_riff, riff.r_size);
#endif	DEBUG
		if   (!strncmp (riff.r_riff, "fmt ", sizeof (riff.r_riff)))
		{
			if (fread (&fmt, sizeof (FMT), 1, file_fp) != 1)
				error ("$*Não consegui ler o cabeçalho FMT de \"%s\"", file_nm);
		}
		elif (!strncmp (riff.r_riff, "data", sizeof (riff.r_riff)))
		{
			wave.w_offset = offset + sizeof (RIFF);
			wave.w_size   = riff.r_size;
		}
#if (0)	/*******************************************************/
		elif (!strncmp (riff.r_riff, "LIST", sizeof (riff.r_riff)))
		{
			char		*list;

			printf ("Seção LIST: %d bytes\n", riff.r_size);

			list = alloca (riff.r_size);

			if (fread (list, riff.r_size, 1, file_fp) != 1)
				error ("$*Não consegui ler a seção LISTA de \"%s\"", file_nm);

			printf ("\n\"%-*.*s\"\n", riff.r_size, riff.r_size, list);

		}
#endif	/*******************************************************/

		if (riff.r_size & 1)
			riff.r_size++;

		offset += sizeof (RIFF) + riff.r_size;

	}

}	/* end analyse_wave */

/*
 ****************************************************************
 *	Finalmente, toca o arquivo				*
 ****************************************************************
 */
void
live_play_file (const char *file_nm, FILE *file_fp, int size, char *area0)
{
	int		file_fd, count, cnt;
	int		mode[3];
	char		*data_ptr;
	char		even_odd;
	int		offset;
	char		line[128];

	/*
	 *	Verifica se é verboso
	 */
	sprintf
	(	line,
		"%s: %s, %d Hz, %d bits (%s)",
		file_nm,
		fmt.f_channels > 1 ? "Stereo" : "Mono",
		fmt.f_rate, fmt.f_bits,
		xwin_active ? cv_bytes_to_KB_or_MB (wave.w_size) : live_edit_time (wave.w_size)
	);

	if (vflag)
		printf ("%s\n", line);

	/*
	 *	Atribui o modo do arquivo
	 */
	mode[0] = fmt.f_rate;
	mode[1] = fmt.f_bits;
	mode[2] = fmt.f_channels;

#if (0)	/*******************************************************/
	if (ioctl (live_fd, SB_SET_MODE, mode) < 0)
		error ("$*Não consegui atribuir o modo do arquivo \"%s\"", file_nm);
#endif	/*******************************************************/

	/*
	 *	Finalmente processa o arquivo
	 */
	file_fd = fileno (file_fp); lseek (file_fd, wave.w_offset, SEEK_SET);

	if ((cnt = read (file_fd, area0, 32 * KBSZ)) <= 0)
		error ("$*Não consegui ler os dados do arquivo WAVE \"%s\"", file_nm);

printf ("livewave: area[0] = %P, area[32K] = %P\n", ((int *)area0)[0], ((int *)area0)[8191]);

	if (write (live_fd, area0, cnt) <= 0)
		error ("$*Não consegui escrever os dados na placa LIVE");

	pause ();


#if (0)	/*******************************************************/
	/*
	 *	Calcula o tamanho
	 */
	offset = 0;

	if (xwin_active)
		draw_title_and_times (line);

	/*
	 *	Lê em áreas alternativamente
	 */
	for (even_odd = 0, count = wave.w_size; count > 0; count -= cnt)
	{
		int	x_position;

		/*
		 *	Examina os eventos
		 */
		if (xwin_active) while (XPending (dpy))
		{
			XNextEvent (dpy, &ev);

			switch (ev.type)
			{
				char	text[16];
				int	len;
				KeySym	key;
	
			    case Expose:
			    case GraphicsExpose:
				draw_title_and_times (line);
				break;
	
			    case ClientMessage:
				if
				(	ev.xclient.format == 32 &&
					ev.xclient.data.l[0] == wmDeleteWindow
				)
					goto case_Exit;

				break;
	
			    case KeyPress:
				len = XLookupString (&ev.xkey, text, 10, &key, 0);
	
				if (text[0] != 'q')
					continue;
	
				/* continua */
			    case_Exit:
				XCloseDisplay (dpy);
				exit (exit_code);
	
			}	/* end switch */
		}

		/*
		 *	Escreve o bloco
		 */
		if (count >= size)
			cnt = size;
		else
			cnt = count;

		if (even_odd)
			{ data_ptr = area1; even_odd = 0; }
		else
			{ data_ptr = area0; even_odd = 1; }

		if (read (file_fd, data_ptr, cnt) != cnt)
			error ("$*Não consegui ler os dados do arquivo WAVE \"%s\"", file_nm);

		/*
		 *	Se a última área for menor, preenche com silêncio
		 */
		if (cnt < size) switch (fmt.f_bytes)
		{
		    case 1:
			memset (data_ptr + cnt, data_ptr[cnt - 1], size - cnt);
			break;

		    case 2:
			memsetw (data_ptr + cnt, *(short *)(data_ptr + cnt - 2), (size - cnt) / 2);
			break;

		    case 4:
			memsetl (data_ptr + cnt, *(long *)(data_ptr + cnt - 4), (size - cnt) / 4);
			break;
		}

		if (write (live_fd, data_ptr, size) != size)
			error ("$*Não consegui escrever os dados do arquivo WAVE \"%s\"", file_nm);

		x_position  = getpercent (offset, wave.w_size);
		x_position *= (WIDTH - 20);
		x_position /= 10000;

		/*
		 *	Atualiza a barra
		 */
		if (xwin_active)
		{
			XFillRectangle
			(	dpy,
				win,
				bar_gc,
			    /*   x    y      dx        dy   */
				10, BAR_Y, x_position, 10
			);

			XFlush (dpy);
		}

		offset += cnt;
	}

	/*
	 *	Escreve EOF
	 */
	if (write (live_fd, NOSTR, size) != size)
		error ("$*Não consegui escrever EOF do arquivo WAVE \"%s\"", file_nm);
#endif	/*******************************************************/

}	/* end live_play_file */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Escreve o Título e os Tempos				*
 ****************************************************************
 */
void
draw_title_and_times (const char *line)
{
#define		POINTs		5
	int		i, x_step = (WIDTH - 60) / POINTs;
	const char	*cp;

	XClearWindow (dpy, win);	/* Redesenha */

	XDrawString (dpy, win, text_gc, OFF_X, TITLE_Y, line, strlen (line));

	for (i = 0; i <= POINTs; i++)
	{
		if (i == POINTs)
			cp = live_edit_time (wave.w_size);
		else
			cp = live_edit_time (i * wave.w_size / POINTs);

		XDrawString (dpy, win, text_gc, x_step * i + 5, TIMES_Y, cp, strlen (cp));
	}

	XFlush (dpy);

}	/* end draw_title_and_times */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Converte bytes em KB ou MB				*
 ****************************************************************
 */
const char *
cv_bytes_to_KB_or_MB (unsigned long byte_cnt)
{
	static char	area[32];

	if   (byte_cnt < (10*MBSZ))		/* Menor do que 10 MB */
	{
		byte_cnt += (KBSZ/2);		/* Arredonda 0.5 KB */

		sprintf
		(	area,
			"%d KB",
			byte_cnt >> KBSHIFT
		);
	}
	else
	{
		byte_cnt += (MBSZ/(2 * 100)); /* Arredonda 0.005 MB */

		sprintf
		(	area,
			"%d.%02d MB",
			byte_cnt >> MBSHIFT,
			(100 * (byte_cnt & (MBSZ - 1))) >> MBSHIFT
		);
	}

	return (area);

}	/* end cv_bytes_to_KB_or_MB */

/*
 ****************************************************************
 *	Imprime a duração da música				*
 ****************************************************************
 */
const char *
live_edit_time (int size)
{
	int		tempo;
	static char	area[32];

	/*
	 *	Calcula o tempo
	 */
	tempo = 100 * (size / fmt.f_bytes) / fmt.f_rate;

	sprintf
	(	area,
		"%d:%02d.%02d",
		(tempo / 100) / 60,
		(tempo / 100) % 60, tempo % 100
	);

	return (area);

}	/* end live_edit_time */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */


/*
 ****************************************************************
 *	Desenha um mapa de recursos usados no sistema		*
 ****************************************************************
 */
int
openwin (int argc, const char *argv[])
{
	int		save_argc = argc;
	const char	*display = NOSTR, **save_argv = argv;
	int		screen;
	Window		root;
	XGCValues	gcv;
	const char	*geometry = NOSTR;
	const char	*fgc = NOSTR, *bgc = NOSTR;
	char		text[64];

	/*
	 *	Conecta com o servidor
	 */
	if ((dpy = XOpenDisplay (display)) == NULL)
		error ("$Não consegui conectar com o servidor \"%s\"", XDisplayName (display));

	screen = DefaultScreen (dpy);
	root   = RootWindow (dpy, screen);

	/*
	 *	Processa as cores dadas
	 */
	if (fgc == NOSTR)
		fgc = "royalblue4";

	if (bgc == NOSTR)
		bgc = "palegoldenrod";

	gcv.foreground = get_window_color (dpy, fgc, BlackPixel (dpy, screen));
	gcv.background = get_window_color (dpy, bgc, WhitePixel (dpy, screen));

	geometry_to_size_hints (dpy, geometry, &szhint);

        win =	XCreateSimpleWindow
                (	dpy, root,
			szhint.x, szhint.y, szhint.width, szhint.height, 5,
			gcv.foreground,
			gcv.background
		);

	sprintf (text, "Sbwave");
	XStoreName (dpy, win, text);

	XSelectInput (dpy, win, ExposureMask|KeyPressMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "SysMap";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint | StateHint;

	XSetWMProperties
	(	dpy, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

	text_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	gcv.foreground = get_window_color (dpy, "red", BlackPixel (dpy, screen));

	bar_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	XMapWindow (dpy, win);

	XNextEvent (dpy, &ev);

	return (0);

}	/* end openwin */

/*
 ****************************************************************
 *	Analisa uma cor dada					*
 ****************************************************************
 */
ulong
get_window_color (Display *Xdisplay, const char *color, ulong def)
{
	XColor			xcol;

	if (color == NOSTR || *color == '\0')
		return (def);

	if
	(	!XParseColor (Xdisplay, Xcmap, color, &xcol) ||
		!XAllocColor (Xdisplay, Xcmap, &xcol)
	)
	{
		error ("Não consegui alocar a cor \"%s\"", color);
		return (def);
	}

	return (xcol.pixel);

}	/* end get_window_color */

/*
 ****************************************************************
 *	Analisa a geometria					*
 ****************************************************************
 */
void
geometry_to_size_hints (Display *display, const char *geom, XSizeHints *hp)
{
	int		x, y, flags, scr;
	unsigned int	width, height;

	if (geom == NOSTR)
		return;

	scr = DefaultScreen (display);

	flags = XParseGeometry (geom, &x, &y, &width, &height);

	if (flags & WidthValue)
	{
		hp->width = width + hp->base_width;
		hp->flags |= USSize;
	}

	if (flags & HeightValue)
	{
		hp->height = height + hp->base_height;
		hp->flags |= USSize;
	}

	if (flags & XValue)
	{
		if (flags & XNegative)
		{
			x += DisplayWidth (display, scr) - hp->width;
			hp->win_gravity = NorthEastGravity;
		}

		hp->x = x;
		hp->flags |= USPosition;
	}

	if (flags & YValue)
	{
		if (flags & YNegative)
		{
			y += DisplayHeight (display, scr) - hp->height;

			hp->win_gravity = hp->win_gravity == NorthEastGravity ?
				SouthEastGravity : SouthWestGravity;
		}

		hp->y = y;
		hp->flags |= USPosition;
	}

}	/* end geometry_to_size_hints */
#endif	/*******************************************************/

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	char		err, abrt;
	int		save_errno = errno;

	va_start (args, format);

	if (format == NOSTR)
		format = "$*Erro de entrada/saída de TCP";

	if ((abrt = *format) == '$')
		format++;

	if ((err = *format) == '*')
		format++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, format, args);

	if (err == '*' && save_errno != 0)
		fprintf (stderr, " (%s)", strerror (save_errno));

	putc ('\n', stderr);

	fflush (stderr);

	if (abrt == '$')
		exit (1);

	va_end (args);

}	/* end error */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - Toca música PCM através da placa de som SB-LIVE\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vdt] <arquivo> ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Fornece algumas informações sobre o <arquivo>\n"
		"\t-d: Não toca, apenas fornece todas as informações sobre o <arquivo>\n"
		"\t-t: Usa o modo de texto (isto é, NÃO gráfico)\n"
	);

	exit (2);

}	/* end help */
