/*
 ****************************************************************
 *								*
 *			xcoremap.c				*
 *								*
 *	Imprime um mapa de alocação da memória			*
 *								*
 *	Versão	1.0.0, de 04.07.86				*
 *		3.2.3, de 14.04.00				*
 *								*
 *	Módulo: coremap						*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/region.h>

#include <sys/kfile.h>
#include <sys/map.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.2.3, de 14.04.00";

#define	elif	else if
#define	EVER	;;

/*
 ******	Definições do video *************************************
 */
#define	GRAIN		256		/* Grão (deve ser fração de KB) */

#define	PERCENT(a, b)	((100 * (a)) / (b))

#define	ROUNDDIV(a, d)	(((a) + (d) - (1)) / (d))

#define	NOGC	NULL

#define	STD_WIDTH	568
#define	STD_HEIGHT	312

#if (0)	/*******************************************************/
#define	SQUARE_SIZE	8
#endif	/*******************************************************/

entry int	SQUARE_SIZE = 10;

#if (0)	/*******************************************************/
#define	SQUARE_SIZE	10
#endif	/*******************************************************/
#define	FONT_WIDTH	6			/* PROVISÓRIO */
#define X_OFFSET	(7 * FONT_WIDTH)	/* PROVISÓRIO */
#define Y_OFFSET	4			/* PROVISÓRIO */

/*
 ******	Definições do Tamanho ***********************************
 */
const char	bottom_msg[] =
{
	"[Quadrado = %d bytes] total (usuário): %d KB, usado: %d KB (%d %%)",
};

#define	BOTTOM_MSG_SZ	(sizeof (bottom_msg) + 6 + 7 + 6 + 2)

#undef	VISUAL_CLASS
/*
 ******	Nomes das classes visuais *******************************
 */
#ifdef	VISUAL_CLASS
entry const char	* const visual_class_nm[] =
{
	"StaticGray",	/* 0 */
	"GrayScale",	/* 1 */
	"StaticColor",	/* 2 */
	"PseudoColor",	/* 3 */
	"TrueColor",	/* 4 */
	"DirectColor",	/* 5 */
};

#define LAST_VISUAL_CLASS ((sizeof (visual_class_nm) / sizeof (char *)) - 1)

#endif	VISUAL_CLASS

/*
 ****** Variáveis diversas **************************************
 */
entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

entry int	periodo = 1;	/* Período da atualização da tela */

entry GC	*tela;		/* O gráfico na tela */

entry int	MAXCOL;		/* Nr. de Caracteres por linha */

entry int	WIDTH, HEIGHT;	/* Tamanho da Janela */

entry int	MAXCHAR;	/* Nr. Max. de caracteres da tela */

entry int	SQUARE_PER_LINE;/* No. de quadrados por linha */

entry int	total_user_bytes;

#if (0)	/*******************************************************/
entry int	MAXLINE;	/* Nr. de linhas */
entry FILE	*log_fp;

entry Atom	wmDeleteWindow;	/* ... */
#endif	/*******************************************************/

/*
 ******	Parâmetros da memória ***********************************
 */
entry int	END_KERMEM;	/* Fim da Memoria Kernel */
entry int	END_KERTB;	/* Fim da Memoria Kernel */

entry int	BEGIN_USRMEM;	/* Inicio da Memoria User */

entry int	END_USRMEM;	/* Final do usuário */

entry int	END_PHYSMEM;	/* Tamanho da mem. fisica em bytes */

entry int	ALPHA;		/* Nr. de KB por linha da tela */

entry int	DELTA;		/* Nr. de BYTES por caracter da tela */

/*
 ******	Geometria ***********************************************
 */
enum { WIDTH_INC = 6, HEIGHT_INC = 13 };


XSizeHints	szhint =
{
	PMinSize|PResizeInc|PBaseSize|PWinGravity,

	0,		0,		/*		x, y */
	STD_WIDTH,	STD_HEIGHT,	/*		width, height */
	STD_WIDTH - 40, 100,		/* PMinSize	min_width, min_height */
	0,		0,		/*		max_width, max_height */
	WIDTH_INC,	HEIGHT_INC,	/* PResizeInc	width_inc, height_inc */
	{1,		1},		/*		min_aspect */
	{0,		0},		/*		max_aspect ratio */
	0,		0,		/* PBaseSize	base_width, base_height */
	NorthWestGravity		/* PWinGravity	gravity */
};

/*
 ******	Variáveis globais ***************************************
 */
entry	Display		*dpy;
entry	Window		win;

/*
 ******	As várias cores *****************************************
 */
entry	GC		text_gc,
			red_gc,
			green_gc,
			blue_gc,
			orange_gc,
			rod_gc;
/*
 ******	Listas de retângulos a reescrever ***********************
 */
#define XRECTANGLE_INC	16

typedef struct
{
	XRectangle	*x_list;
	int		x_sz;
	int		x_cnt;
	GC		x_gc;

}	XRECTANGLE_LIST;

XRECTANGLE_LIST		red_rectangles_list,
			green_rectangles_list,
			blue_rectangles_list,
			orange_rectangles_list,
			rod_rectangles_list;

/*
 ****** Protótipos de funções ***********************************
 */
void		putregion (int, int, int);
void		display_char (int, XRECTANGLE_LIST *);
void		clear_rectangles_lists (void);
void		clear_rectangles_list (XRECTANGLE_LIST *xp);
void		draw_rectangles_lists (const XRECTANGLE_LIST *xp);
void		on_signal (int, ...);
void		quit (int);
void		setparam (int, int);
int		openwin (int, const char *[], const char *, const char *,
		const char *, const char *, const char *, const SCB *);
ulong		get_window_color (Display *, const char *, ulong);
void		geometry_to_size_hints (Display *, const char *, XSizeHints *);
void		help (void);

void		process_size (const SCB *, int, int);
void		draw_fixed_part (const SCB *, int, int);
void		draw_window (const SCB *, const MAP *, MAP *);

extern void	XDisconnectDisplay (Display *);

/*
 ****************************************************************
 *	Imprime um mapa de alocação da memória			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		save_argc = argc;
	const char	**save_argv = argv;
	int		opt;
	long		pid;
	const char	*str;
	int		fd;
	const char	*geo_str = NOSTR;
	const char	*display = NOSTR;
	const char	*fgc = NOSTR, *bgc = NOSTR;
	const char	*font_str = NOSTR;
	MAP		*kernel_coremap, *copy_coremap;
	unsigned int	border, depth, width = STD_WIDTH, height= STD_HEIGHT;
	SCB		scb;
	int		screen;
	Window		root;
	XGCValues	gcv;
	char		text[64];
	XFontStruct	*fip;
	XWMHints	wmhint;
	XClassHint	classhint;
	XEvent		ev;
#ifdef	VISUAL_CLASS
	XVisualInfo	visual_info;
	int		visual_class, bits;
#endif	VISUAL_CLASS
	int		font_width;


	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vg:f:b:d:F:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'g':			/* Geometria */
			geo_str = optarg;
			break;

		    case 'f':			/* Cor de frente */
			fgc = optarg;
			break;

		    case 'b':			/* Cor de fundo */
			bgc = optarg;
			break;

		    case 'd':
			display = optarg;	/* Display */
			break;

		    case 'F':
			font_str = optarg;	/* Fonte */
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
	 *	Verifica se o <período> foi especificado
	 */
	if (argc > 1)
		help ();

	if (argc == 1 && ((periodo = strtol (argv[0], &str, 0)) <= 0 || *str))
		error ("$Período inválido: \"%s\"", argv[0]);

	/*
	 *	Passa para "background"
	 */
	if ((pid = fork ()) > 0)
		exit (0);

	if (pid < 0)
		error ("$*Não consegui criar outro processo");

	/*
	 *	Verifica se o X-Window está ativo
	 */
	if (display == NOSTR && open ("/dev/xconsole", O_RDWR) >= 0)
		error ("$O Sistema X-Window não está ativo");

	/*
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("$*Não consegui obter o SCB");
	/*
	 *	Conecta com o servidor
	 */
	if ((dpy = XOpenDisplay (display)) == NULL)
		error ("$*Não consegui conectar com o servidor \"%s\"", XDisplayName (display));

	screen	= DefaultScreen (dpy);
	root	= RootWindow (dpy, screen);
	fd	= ConnectionNumber (dpy);

	/*
	 *	Instala a fonte pedida, obtendo o tamanho
	 *	de um caractere. Supõe passo fixo.
	 */
	if (font_str == NOSTR)
		font_str = "6x10";
#if (0)	/*******************************************************/
		font_str = "5x8";
		font_str = "6x13";
#endif	/*******************************************************/

	if ((fip = XLoadQueryFont (dpy, font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", font_str);

#if (0)	/*******************************************************/
	fheight		= fip->ascent + fip->descent;
	font_width		= XTextWidth (fip, "MMMMMMMMMM", 10) / 10;
	SQUARE_SIZE = fip->ascent + fip->descent - 3;
	printf ("ascent = %d, descent = %d\n", fip->ascent, fip->descent);
	printf ("sizeof (GC) = %d\n", sizeof (GC));
#endif	/*******************************************************/

	/*
	 *	Calcula o Tamanho Mínimo
	 */
	SQUARE_SIZE = fip->ascent;

	font_width = XTextWidth (fip, "MMMMMMMMMM", 10) / 10;

	szhint.min_width = BOTTOM_MSG_SZ * font_width;

#ifdef	VISUAL_CLASS
	/*
	 *	Obtém as características do vídeo
	 */
	bits = DefaultDepth (dpy, screen);

	for (visual_class = LAST_VISUAL_CLASS; /* abaixo */; visual_class--)
	{
		if (visual_class < 0)
			{ error ("NÃO consegui obter a classe visual"); break; }

		if (XMatchVisualInfo (dpy, screen, bits, visual_class, &visual_info))
		{
			error ("Classe visual \"%s\", %d bits", visual_class_nm[visual_class], bits);
			break;
		}
	}
#endif	VISUAL_CLASS

	/*
	 *	Processa as cores dadas
	 */
	gcv.foreground = get_window_color (dpy, fgc, BlackPixel (dpy, screen));
	gcv.background = get_window_color (dpy, bgc, WhitePixel (dpy, screen));

	geometry_to_size_hints (dpy, geo_str, &szhint);

        win =	XCreateSimpleWindow
                (	dpy, root,
			szhint.x, szhint.y, szhint.width, szhint.height, 5,
			gcv.foreground,
			gcv.background
		);

	sprintf (text, "XCoremap (%s)", scb.y_uts.uts_nodename);
	XStoreName (dpy, win, text);

	XSelectInput (dpy, win, ExposureMask|KeyPressMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XCoremap";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint|StateHint;

	XSetWMProperties
	(	dpy, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

	gcv.font = fip->fid;
	text_gc = XCreateGC (dpy, win, GCForeground|GCBackground|GCFont, &gcv);

	gcv.foreground = get_window_color (dpy, "red", BlackPixel (dpy, screen));
	red_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	gcv.foreground = get_window_color (dpy, "orange", BlackPixel (dpy, screen));
	orange_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	gcv.foreground = get_window_color (dpy, "green", BlackPixel (dpy, screen));
	green_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	gcv.foreground = get_window_color (dpy, "blue", BlackPixel (dpy, screen));
	blue_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	gcv.foreground = get_window_color (dpy, "palegoldenrod", BlackPixel (dpy, screen));
	rod_gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	XMapWindow (dpy, win);

	/*
	 *	Inicializa as cores das listas
	 */
	red_rectangles_list.x_gc	= red_gc;
	green_rectangles_list.x_gc	= green_gc;
	blue_rectangles_list.x_gc	= blue_gc;
	orange_rectangles_list.x_gc	= orange_gc;
	rod_rectangles_list.x_gc	= rod_gc;

	/*
	 *	x
	 */
	total_user_bytes = PGTOBY (scb.y_enducore1 - scb.y_ucore1);

	/*
	 *	Mapeia a tabela da memória
	 */
	if ((int)(kernel_coremap = phys (scb.y_map, scb.y_nmap * sizeof (MAP), O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não foi possível mapear a tabela de memória");

	/*
	 *	Aloca memória para a cópia instantânea da tabela de memória
	 */
	if ((copy_coremap = malloc (scb.y_nmap * sizeof (MAP))) == (MAP *)NULL)
		error ("Não consegui memória para a cópia da tabela");

	/*
	 *	x
	 */
	process_size (&scb, STD_WIDTH, STD_HEIGHT);

#if (0)	/*******************************************************/
	/*
	 *	Prepara os sinais
	 */
	signal (SIGTERM, on_signal);
	signal (SIGHUP, on_signal);
	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
#endif	/*******************************************************/

	/*
	 *	Entra na malha de eventos
	 */
	for (EVER)
	{
		while (XPending (dpy))
		{
			XNextEvent (dpy, &ev);

			switch (ev.type)
			{
				int		len;
				KeySym		key;
	
			    case Expose:
			    case GraphicsExpose:
#if (0)	/*******************************************************/
error ("Recevi um EXPOSE");
#endif	/*******************************************************/
				memsetl (tela, NOGC, MAXCHAR);
				clear_rectangles_lists ();
				draw_fixed_part (&scb, width, height);
				draw_window (&scb, kernel_coremap, copy_coremap);
				break;
	
#if (0)	/*******************************************************/
			    case ClientMessage:
				if
				(	ev.xclient.format == 32 &&
					ev.xclient.data.l[0] == wmDeleteWindow
				)
				{
					XDisconnectDisplay (dpy);
					exit (exit_code);
				}
				break;
#endif	/*******************************************************/
	
			    case KeyPress:
				len = XLookupString (&ev.xkey, text, 10, &key, 0);
	
				if (text[0] != 'q')
					continue;
	
				/* continua */
#if (0)	/*******************************************************/
			    case_Exit:
#endif	/*******************************************************/
				XCloseDisplay (dpy);
				exit (exit_code);
	
			    case ConfigureNotify:
			    {
				XEvent		dummy;
				int		x, y;
	
#if (0)	/*******************************************************/
error ("Recevi um CONFIGURE");
#endif	/*******************************************************/
				while
				(	XCheckTypedWindowEvent
					(	dpy, win,
						ConfigureNotify, &dummy
					)
				)
					/* vazio */;
	
				XGetGeometry
				(	dpy, win, &root,
					&x, &y, &width, &height,
					&border, &depth
				);
	
				/* parent already resized */
	
				process_size (&scb, width, height);
				XClearWindow (dpy, win);
				clear_rectangles_lists ();
				draw_fixed_part (&scb, width, height);
				draw_window (&scb, kernel_coremap, copy_coremap);
				break;
			    }
	
			}	/* end switch */
		}

		attention (1, &fd, -1, 1);

		clear_rectangles_lists ();
		draw_window (&scb, kernel_coremap, copy_coremap);

	}	/* end for (EVER) */

	XCloseDisplay (dpy);

	return (0);

}	/* end coremap */

/*
 ****************************************************************
 *	Prepara para o tamanho da janela			*
 ****************************************************************
 */
void
process_size (const SCB *sp, int width, int height)
{
	int		n, nchars, graos, nlinhas;

	/*
	 *	Ajusta para o múltiplo do tamanho do quadrado
	 */
	WIDTH  = width -  width  % SQUARE_SIZE;
	HEIGHT = height - height % SQUARE_SIZE;

	SQUARE_PER_LINE = (WIDTH - X_OFFSET) / SQUARE_SIZE;

	/*
	 *	Calcula o número de linhas e colunas
	 */
	MAXCHAR = ((WIDTH - X_OFFSET) / SQUARE_SIZE) * ((HEIGHT - 30) / SQUARE_SIZE);

	/*
	 *	Calcula os parametros globais da memoria
	 */
	END_PHYSMEM = (int)sp->y_endpmem - (int)SYS_ADDR;

	if (END_PHYSMEM % KBSZ)
		error ("A memória (%d Bytes) não é múltipla de KBSZ?", END_PHYSMEM);

	/*
	 *	Calcula o no. total de grãos
	 */
	graos = END_PHYSMEM / GRAIN;

	/*
	 *	Calcula o no. de grãos por char
	 */
	n = ROUNDDIV (graos, MAXCHAR);

	/*
	 *	Calcula o no. total de chars
	 */
	nchars = ROUNDDIV (graos, n);

	/*
	 *	Calcula o no. total de linhas
	 */
	nlinhas = ROUNDDIV (nchars, MAXCOL);

	/*
	 *	Calcula DELTA, em bytes/char
	 */
	DELTA = n * GRAIN;

	/*
	 *	Calcula ALPHA, em KB/linha
	 */
	ALPHA = DELTA * MAXCOL / KBSZ;

	/*
	 *	Para imprimir a tabela	
	 */
#if (0)	/*******************************************************/
	if (pr)
	{
		printf
		(	"%d\t\t%d\t\t%d		%d		%d\n",
			BYTOKB (END_PHYSMEM),
			ALPHA,
			DELTA,
			nlinhas,
			nchars
		);
	}
#endif	/*******************************************************/


	/*
	 *	x
	 */
	END_KERMEM   = ((int)sp->y_uproc - (int)SYS_ADDR) / DELTA;
	END_KERTB   = (PGTOBY (sp->y_ucore1) - (int)SYS_ADDR) / DELTA;
	BEGIN_USRMEM = (PGTOBY (sp->y_ucore1) - (int)SYS_ADDR) / DELTA;
	END_USRMEM  = (PGTOBY (sp->y_enducore1) - (int)SYS_ADDR) / DELTA;

#ifdef	DEBUG
	printf ("y_proc = %P, y_ucore1 = %P\n", sp->y_proc, sp->y_ucore1);
	printf
	(	"END_KERMEM = %d, END_KERTB = %d, BEGIN_USRMEM = %d, END_USRMEM = %d\n",
		END_KERMEM, END_KERTB, BEGIN_USRMEM, END_USRMEM
	);
	sleep (5);
#endif	DEBUG

	/*
	 *	Aloca memória para a tela
	 */
	if (tela != (GC *)NULL)
		free (tela);

	if ((tela = malloc (MAXCHAR * sizeof (GC))) == (GC *)NULL)
		error ("Não consegui memória para a tela");

	memsetl (tela, NOGC, MAXCHAR);

#if (0)	/*******************************************************/
	/*
	 *	Aloca memória para os vetores de retângulos
	 */
	size = MAXCHAR * sizeof (XRectangle);

	if (red_rectangles_list.x_list != NULL)
		free (red_rectangles_list.x_list);

	if ((red_rectangles_list.x_list = malloc (size)) == NULL)
		error (NOSTR);

	if (green_rectangles_list.x_list != NULL)
		free (green_rectangles_list.x_list);

	if ((green_rectangles_list.x_list = malloc (size)) == NULL)
		error (NOSTR);

	if (blue_rectangles_list.x_list != NULL)
		free (blue_rectangles_list.x_list);

	if ((blue_rectangles_list.x_list = malloc (size)) == NULL)
		error (NOSTR);

	if (orange_rectangles_list.x_list != NULL)
		free (orange_rectangles_list.x_list);

	if ((orange_rectangles_list.x_list = malloc (size)) == NULL)
		error (NOSTR);

	if (rod_rectangles_list.x_list != NULL)
		free (rod_rectangles_list.x_list);

	if ((rod_rectangles_list.x_list = malloc (size)) == NULL)
		error (NOSTR);
#endif	/*******************************************************/

}	/* end process_size */

/*
 ****************************************************************
 *	Desenha as áreas "imutáveis"				*
 ****************************************************************
 */
void
draw_fixed_part (const SCB *sp, int width, int height)
{
	int		addr, line = 1;
	char		text[80];

	/*
	 *	Imprime a coluna de endereços
	 */
ALPHA = DELTA * SQUARE_PER_LINE / KBSZ;
#if (0)	/*******************************************************/
printf ("BYTOKB (END_PHYSMEM) = %d, ALPHA = %d\n", BYTOKB (END_PHYSMEM), ALPHA);
#endif	/*******************************************************/

	for (addr = 0; addr < BYTOKB (END_PHYSMEM); addr += ALPHA)
	{
		sprintf (text, "%6d", addr);

		XDrawString
		(	dpy,
			win,
			text_gc,
			1, line * SQUARE_SIZE + Y_OFFSET - 2,
			text, strlen (text)
		);

		line++;
	}

	/*
	 *	Preenche a área do kernel com '+'
	 */
	for (addr = 0; addr < END_KERMEM; addr++)
		display_char (addr, &red_rectangles_list);

	for (/* vazio */; addr < END_KERTB; addr++)
		display_char (addr, &orange_rectangles_list);

	/*
	 *	Preenche a área entre o fim do kernel e o começo
	 *	da memória user com 'o'
	 */
	for (/* vazio */; addr < BEGIN_USRMEM; addr++)
		display_char (addr, &orange_rectangles_list);

	/*
	 *	Preenche a área do RAMD com '-'
	 */
	for
	(	addr = ((int)sp->y_ramd0 - (int)SYS_ADDR) / DELTA;
		addr < ((int)sp->y_endramd0 - (int)SYS_ADDR) / DELTA;
		addr++
	)
	{
		display_char (addr, &green_rectangles_list);
	}

	for
	(	addr = ((int)sp->y_ramd1 - (int)SYS_ADDR) / DELTA;
		addr < ((int)sp->y_endramd1 - (int)SYS_ADDR) / DELTA;
		addr++
	)
	{
		display_char (addr, &green_rectangles_list);
	}

}	/* end draw_fixed_part */

/*
 ****************************************************************
 *	Desenha a tela toda					*
 ****************************************************************
 */
void
draw_window (const SCB *sp, const MAP *kernel_coremap, MAP *copy_coremap)
{
	const MAP		*head, *mp;
	unsigned		CORE_DIFF;
	int			last_end;
	int			addr;
	int			used, unused, old_unused = -1;
	char			text[80];
	int			len;

	/*
	 *	Tira o instantâneo da tabela
	 */
	memmove (copy_coremap, kernel_coremap, sp->y_nmap * sizeof (MAP));

	CORE_DIFF = (int)copy_coremap - (int)sp->y_map;

	head = copy_coremap;	/* A cabeça é a primeira entrada */

	/*
	 *	Inicialização
	 */
	last_end = PGTOBY (sp->y_ucore1) - (int)SYS_ADDR; 
	addr = BEGIN_USRMEM;
	unused = 0;

	/*
	 *	Desenha as regiões
	 */
	for
	(	mp = (MAP *)((int)head->a_forw + CORE_DIFF);
		mp != head;
		mp = (MAP *)((int)mp->a_forw + CORE_DIFF)
	)
	{
		long		a_size = PGTOBY (mp->a_size);
		long		a_area = PGTOBY (mp->a_area);

		a_area -= (int)SYS_ADDR;

		unused += a_size;

		putregion (last_end, a_area, a_size);

		last_end = a_area + a_size;
	}

	/*
	 *	Completa as porções restantes com blue_gc);
	 */
	for (addr = last_end / DELTA; addr < END_USRMEM; addr++)
		display_char (addr, &blue_rectangles_list);

	/*
	 *	Escreve as listas de retângulos
	 */
#if (0)	/*******************************************************/
printf ("red = %d, green = %d, blue = %d, orange = %d, rod = %d\n",
red_rectangles_list.x_cnt, green_rectangles_list.x_cnt, blue_rectangles_list.x_cnt,
orange_rectangles_list.x_cnt, rod_rectangles_list.x_cnt);
#endif	/*******************************************************/

	draw_rectangles_lists (&red_rectangles_list);
	draw_rectangles_lists (&green_rectangles_list);
	draw_rectangles_lists (&blue_rectangles_list);
	draw_rectangles_lists (&orange_rectangles_list);
	draw_rectangles_lists (&rod_rectangles_list);

	/*
	 *	Verifica se mudou o total
	 */
	if (unused == old_unused)
		return;

	/*
	 *	Escreve o sumário
	 */
	used = total_user_bytes - unused;

	sprintf
	(	text,
		bottom_msg,
#if (0)	/*******************************************************/
		"[Quadrado = %d bytes] total (usuário): %d KB, usado: %d KB (%d %%)",
#endif	/*******************************************************/
		DELTA,
		BYTOKB (total_user_bytes),
		BYTOKB (used),
		PERCENT (used, total_user_bytes)
	);

	len = strlen (text);

	XDrawImageString
	(	dpy,
		win,
		text_gc,
		(WIDTH - 6 * len) >> 1, HEIGHT - 10,
		text, len
	);

	old_unused = unused;

}	/* end draw_window */

/*
 ****************************************************************
 *	Imprime uma região na tela				*
 ****************************************************************
 */
void
putregion (int last_end, int begin, int sz)
{
	int	i;
	int	addr;

	/*
	 *	Põe o buraco da tabela (memória alocada)
	 */
	for (i = begin - last_end, addr = last_end / DELTA; i > 0; i -= DELTA)
		display_char (addr++, &blue_rectangles_list);

	/*
	 *	Põe a entrada da tabela (memória livre)
	 */
	for (/* vazio */; sz >= DELTA; sz -= DELTA)
		display_char (addr++, &rod_rectangles_list);

}	/* end putregion */

/*
 ****************************************************************
 *	Coloca um  caractere no gráfico 			*
 ****************************************************************
 */
void
display_char (int addr, XRECTANGLE_LIST *xp)
{
	XRectangle	*rp;

	/*
	 *	Prólogo
	 */
	if (tela[addr] == xp->x_gc)
		return;

	tela[addr] = xp->x_gc;

	/*
	 *	Verifica se o tamanho da lista é suficiente
	 */
	if (xp->x_sz <= xp->x_cnt)
	{
		xp->x_sz += XRECTANGLE_INC;

		if ((xp->x_list = realloc (xp->x_list, xp->x_sz * sizeof (XRectangle))) == NULL)
			error (NOSTR);
	}

	/*
	 *	Adiciona o caractere à lista
	 */
	rp = &xp->x_list[xp->x_cnt];

	rp->x	   = (addr % SQUARE_PER_LINE) * SQUARE_SIZE + X_OFFSET;
	rp->y	   = (addr / SQUARE_PER_LINE) * SQUARE_SIZE + Y_OFFSET;

	rp->width  = SQUARE_SIZE - 2;
	rp->height = SQUARE_SIZE - 2;

	xp->x_cnt++;

#if (0)	/*******************************************************/
	/*
	 *	Calcula o endereço
	 */
	x = (addr % SQUARE_PER_LINE) * SQUARE_SIZE + X_OFFSET;
	y = (addr / SQUARE_PER_LINE) * SQUARE_SIZE + Y_OFFSET;

	XFillRectangle
	(	dpy,
		win,
		gp,
	    /*   x	y		dx			dy   */
		x,	y,	SQUARE_SIZE - 2,	SQUARE_SIZE - 2
	);
#endif	/*******************************************************/

}	/* end display_char */

/*
 ****************************************************************
 *	Zera as listas de modificações				*
 ****************************************************************
 */
void
clear_rectangles_lists (void)
{
	clear_rectangles_list (&red_rectangles_list);

	clear_rectangles_list (&green_rectangles_list);

	clear_rectangles_list (&blue_rectangles_list);

	clear_rectangles_list (&orange_rectangles_list);

	clear_rectangles_list (&rod_rectangles_list);

}	/* end clear_rectangles_lists */

/*
 ****************************************************************
 *	Zera uma lista de modificação				*
 ****************************************************************
 */
void
clear_rectangles_list (XRECTANGLE_LIST *xp)
{
	if (xp->x_list != NULL)
		free (xp->x_list);

	xp->x_list = NULL;

	xp->x_sz = 0; xp->x_cnt = 0;

}	/* end clear_rectangles_list */

#if (0)	/*******************************************************/
	int		long_size;

	long_size = MAXCHAR * sizeof (XRectangle) / sizeof (long);

	memsetl (red_rectangles_list.x_list, 0, long_size);
	red_rectangles_list.x_cnt = 0;

	memsetl (green_rectangles_list.x_list, 0, long_size);
	green_rectangles_list.x_cnt = 0;

	memsetl (blue_rectangles_list.x_list, 0, long_size);
	blue_rectangles_list.x_cnt = 0;

	memsetl (orange_rectangles_list.x_list, 0, long_size);
	orange_rectangles_list.x_cnt = 0;

	memsetl (rod_rectangles_list.x_list, 0, long_size);
	rod_rectangles_list.x_cnt = 0;
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Escreve a lista de retângulos				*
 ****************************************************************
 */
void
draw_rectangles_lists (const XRECTANGLE_LIST *xp)
{
	if (xp->x_cnt == 0)
		return;

	XFillRectangles (dpy, win, xp->x_gc, xp->x_list, xp->x_cnt);

}	/* end draw_rectangles_lists */

/*
 ****************************************************************
 *	Analisa uma cor dada					*
 ****************************************************************
 */
ulong
get_window_color (Display *Xdisplay, const char *color_nm, ulong def)
{
	XColor			xcol;
	Colormap		color_map;

	if (color_nm == NOSTR || color_nm[0] == '\0')
		return (def);

	color_map = DefaultColormap (Xdisplay, DefaultScreen (Xdisplay));

	if (!XParseColor (Xdisplay, color_map, color_nm, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

#if (0)	/*******************************************************/
	error
	(	"Banco de dados para \"%s\": (%04X, %04X, %04X)",
		color_nm, xcol.red, xcol.green, xcol.blue
	);
#endif	/*******************************************************/

	if (!XAllocColor (Xdisplay, color_map, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

#if (0)	/*******************************************************/
	error
	(	"Valores alocados para \"%s\": (%04X, %04X, %04X), %P",
		color_nm, xcol.red, xcol.green, xcol.blue, xcol.pixel
	);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	if
	(	!XParseColor (Xdisplay, color_map, color_nm, &xcol) ||
		!XAllocColor (Xdisplay, color_map, &xcol)
	)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar a cor \"%s\"",
			pgname, color_nm
		);
		return (def);
	}
#endif	/*******************************************************/

	return (xcol.pixel);

}	/* end get_window_color */

/*
 ****************************************************************
 *	Analisa a geometria					*
 ****************************************************************
 */
void
geometry_to_size_hints (Display *display, const char *geo_str, XSizeHints *hp)
{
	int		x, y, user_geo_mask, screen;
	unsigned int	width, height;

	if (geo_str == NOSTR)
		return;

	screen = DefaultScreen (display);

	user_geo_mask = XParseGeometry (geo_str, &x, &y, &width, &height);

	if (user_geo_mask & WidthValue)
		{ hp->width = width * WIDTH_INC + hp->base_width; hp->flags |= USSize; }

	if (user_geo_mask & HeightValue)
		{ hp->height = height * HEIGHT_INC + hp->base_height; hp->flags |= USSize; }

	if (user_geo_mask & XValue)
	{
		if (user_geo_mask & XNegative)
		{
			x += DisplayWidth (display, screen) - hp->width;

			hp->win_gravity = NorthEastGravity;
		}

		hp->x = x; hp->flags |= USPosition;
	}

	if (user_geo_mask & YValue)
	{
		if (user_geo_mask & YNegative)
		{
			y += DisplayHeight (display, screen) - hp->height;

			if (hp->win_gravity == NorthEastGravity)
				hp->win_gravity = SouthEastGravity;
			else
				hp->win_gravity = SouthWestGravity;
		}

		hp->y = y; hp->flags |= USPosition;
	}

}	/* end geometry_to_size_hints */

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
		"%s - desenha um mapa de alocação da memória\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-d <display>] [-g <geometria>] "
		"[-f <frente>] [-b <fundo>] [<período>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-d: Conecta-se ao servidor <display>\n"
		"\t-g: Define a <geometria>\n"
		"\t-f: Define a cor de <frente>\n"
		"\t-b: Define a cor de <fundo>\n"
		"\nArgs:\t<período>: Período entre atualizações "
		"(normalmente 1 segundo)\n"
	);

	exit (2);

}	/* end help */
