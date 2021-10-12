/*
 ****************************************************************
 *								*
 *			xpaint.c				*
 *								*
 *	Exibe imagens em uma janela				*
 *								*
 *	Versão	3.0.0, de 15.03.98				*
 *		4.4.0, de 17.04.03				*
 *								*
 *	Módulo: cmd/xpaint					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <errno.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "../h/xpaint.h"

/*
 ****************************************************************
 *	Definições Globais					*
 ****************************************************************
 */
#define	INCR	32			/* Incremento para alocação */

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.4.0, de 17.04.03";

entry	int		save_argc;	/* Para XSetWMProperties */
entry	const char	**save_argv;

entry	int		vflag,		/* Verboso */
			Rflag;		/* Mostra como fundo da janela-mãe */

entry	int		screen;
entry	XClassHint	classhint;
entry	XWMHints	wmhint;
entry	GC		gc;

entry	IMAGE		*images,	/* Tabela de Imagens */
			*next_image,
			*end_images;

entry	int		default_factor;	/* Fator de Ampliação/Redução */

/*
 ****************************************************************
 *	Os diversos formatos suportados				*
 ****************************************************************
 */
entry	const FORMAT	formats[]	=
{
	{ "GIF",	is_a_GIF_file,	load_GIF_file	},
	{ "JPG",	is_a_JPG_file,	load_JPG_file	},
	{ "BMP",	is_a_BMP_file,	load_BMP_file	},
	{ "XPM",	is_a_XPM_file,	load_XPM_file	},
	{ "JPEG",	is_a_JPG_file,	load_JPG_file	}
};

entry	const FORMAT  *end_formats = &formats [sizeof (formats) / sizeof (FORMAT)];

/*
 ****************************************************************
 *	Protótipos de funções					*
 ****************************************************************
 */
const char	**read_args_from_stdin (int *);
int		add_image_to_table (const char *, STAT *, int);
Window		create_window (Display *);
void		convert_extension (const char *, char *, int);
int		draw_window (Display *, Window, XImage *, IMAGE *);
KeySym		wait_for_events (Display *, Window, int);
int		set_background (Display *, Window, XImage *, const char *);
void		help (int);

extern const char *basename (const char *);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	XStandardColormap stdcmap, *cmp;
	Display		*dpy = NULL;
	Window		root, win;
	XGCValues	gcv;
	int		depth;

	FILE		*fp;
	XImage		*ximage = NULL;
	IMAGE		*last_image, *ip, *old_ip;
	int		nimages, times;

	const char	*display = NOSTR;
	int		time_interval = 0;
	int		opt, exit_val = 0;
	int		Nflag = 0;

	Atom		map;
	KeySym		key;

	save_argc = argc;	save_argv = argv;

	/*
	 *	Analisa as opções de execução.
	 */
	while ((opt = getopt (argc, argv, "d:f:HMNRs:v")) != EOF)
	{
		switch (opt)
		{
		    case 'd':
			display = optarg;
			break;

		    case 'f':
			if (strchr (optarg, '/') != NOSTR)
			{
				int	num, den;

				sscanf (optarg, "%d/%d", &num, &den);

				if (num != 1 || den <= 0 || den > 8)
					msg ("$Fator redutivo inválido");

				default_factor = -den + 1;
			}
			else
			{
				sscanf (optarg, "%d", &default_factor);

				if (default_factor <= 0 || default_factor > 8)
					msg ("$Fator de ampliação inválido");

				default_factor--;
			}
			break;

		    case 'H':
			help (0);
			break;

		    case 'M':
			return (0);

		    case 'N':
			Nflag++;
			break;

		    case 'R':
			Rflag++;
			break;

		    case 's':
			time_interval = atol (optarg);

			if (time_interval < 1 || time_interval > 60)
			{
				msg ("Argumento inválido para opção '-s'");
				help (1);
			}

			break;

		    case 'v':
			vflag++;
			break;

		    default:
			putc ('\n', stderr);
			help (2);

		}	/* end switch (opt) */

	}	/* end while (analisando opções) */

	argv += optind;
	argc -= optind;

	/*
	 *	Consistência final das Opções.
	 */
	if (Nflag)
	{
		if (argc > 0 || *argv != NOSTR)
			msg ("$Com a opção '-N' NÃO podem ser dados nomes de arquivos");

		argv = read_args_from_stdin (&argc);
	}

	if (Rflag && time_interval > 0)
		msg ("$As opções '-R' e '-s' são incompatíveis");

	/*
	 *	Constrói a Tabela de Imagens.
	 */
	for (/* acima */; *argv != NULL; argv++)
	{
		if (ftw (*argv, add_image_to_table) < 0)
			msg ("$*Erro em \"ftw (%s)\"", *argv);
	}

	if ((nimages = next_image - images) <= 0)
		msg ("$Não foram encontrados arquivos contendo imagens");

	last_image = next_image - 1;

	if (Rflag && nimages > 1)
	{
		/* Escolhe aleatoriamente uma imagem */

		images += (time (NULL) % nimages); last_image = images;

		msg ("Imagem a a janela-mãe: \"%s\"", images->i_name);
	}

	/*
	 *	Abre a conexão com o servidor.
	 */
	if ((dpy = XOpenDisplay (display)) == NULL)
	{
		msg
		(	"$Não consegui conectar-me ao servidor \"%s\"",
			XDisplayName (display)
		);
	}

	screen = DefaultScreen (dpy);
	depth  = DefaultDepth (dpy, screen);
	root   = RootWindow (dpy, screen);

	/*
	 *	Obtém o mapa de cores de acordo com a profundidade.
	 */
	cmp = &stdcmap;

	map = (depth <= 8) ? XA_RGB_DEFAULT_MAP : XA_RGB_BEST_MAP;

	if (!XGetStandardColormap (dpy, root, cmp, map))
	{
		cmp = NULL;
		msg ("Mapa de cores padrão para profundidade %d não disponível", depth);
	}

	/*
	 *	Decide se cria ou não uma janela para exibir as imagens.
	 */
	win = Rflag ? root : create_window (dpy);

	gcv.foreground = WhitePixel (dpy, screen);
	gcv.background = BlackPixel (dpy, screen);

	gc = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);

	/*
	 *	Mostra as imagens.
	 */
	for (times = 0, old_ip = NOIMAGE, ip = images; ip != NOIMAGE; times++)
	{
		if (vflag && (times & 15) == 0 && nimages > 0)
		{
			printf ("\n NUM  FMT   LARG x  ALT   FATOR   CORES   TAMANHO  ARQUIVO\n");
			printf ("--------------------------------------------------------------\n");
		}

		if (old_ip != ip)
		{
			/* Abre o arquivo que contém a imagem */

			if ((fp = fdopen (inopen (ip->i_dev, ip->i_ino), "r")) == NOFILE)
			{
				msg ("*Não consegui abrir o arquivo \"%s\"", ip->i_name);
				exit_val++; break;
			}

			/* Lê a imagem para a memória */

			if ((ximage = (*ip->i_format->f_load) (fp, 1, dpy, cmp, ip)) == NULL)
			{
				msg ("Não consegui ler imagens do arquivo \"%s\"", ip->i_name);
				fclose (fp); exit_val++; break;
			}

			fclose (fp);

			/* Imprime, se necessário, algumas informações sobre a imagem */

			if (vflag)
			{
				char	factor[8];

				if (ip->i_factor >= 0)
					snprintf (factor, sizeof (factor), "%d",    ip->i_factor + 1);
				else
					snprintf (factor, sizeof (factor), "1/%d", -ip->i_factor + 1);

				printf
				(	"%4d  %s   %4d x %4d   %4s %8d  %8d  %s\n",
					ip - images + 1, ip->i_format->f_extension,
					ip->i_width,	 ip->i_height,  factor,
					ip->i_ncolors,	 ip->i_file_sz, ip->i_name
				);
			}

			/* Mostra a imagem como fundo da janela */

			if (draw_window (dpy, win, ximage, ip) < 0)
				break;

		}	/* end if (nova imagem) */

		/* Aguarda eventos e analisa o retorno */

		key = wait_for_events (dpy, win, time_interval);

		old_ip = ip;

		switch (key)
		{
		    case 'q':
		    case 'Q':
		    case XK_Escape:
			ip = NOIMAGE;		/* Fim do programa */
			break;

		    case XK_Up:
		    case XK_KP_Up:
			if (ip > images)
				ip--;		/* Retrocede uma imagem */
			break;

		    case XK_Down:
		    case XK_KP_Down:
			if (ip < last_image)
				ip++;		/* Avança uma imagem */
			elif (time_interval > 0)
				ip = images;	/* Volta ao início */
			break;

		    case XK_Home:
		    case XK_KP_Home:
			ip = images;		/* Volta ao início */
			break;

		    case XK_End:
		    case XK_KP_End:
			ip = last_image;	/* Vai para a última imagem */
			break;

		    case XK_Right:
		    case XK_KP_Right:
			ip->i_factor++;		/* Aumenta o fator */
			old_ip = NOIMAGE;
			break;

		    case XK_Left:		/* Diminui o fator */
		    case XK_KP_Left:
			ip->i_factor--;
			old_ip = NOIMAGE;
			break;

		    default:
			break;

		}	/* end switch (tecla retornada) */

		if (ip != old_ip)
			XDestroyImage (ximage);

	}	/* end for (percorrendo imagens) */

	XFreeGC (dpy, gc);

	XCloseDisplay (dpy);

	return (exit_val);

}	/* end main */

/*
 ****************************************************************
 *	Lê os argumentos do "stdin", montando um "argv"		*
 ****************************************************************
 */
const char **
read_args_from_stdin (int *argc_ptr)
{
	char		area[1024];
	const char	**argv, **argp, **end_argv;
	int		argc;

	argv = argp = end_argv = NULL;
	argc = 0;

	while (fngets (area, sizeof (area), stdin) != NOSTR)
	{
		if (argp >= end_argv)
		{
			argv = realloc (argv, (argc + INCR) * sizeof (char *));

			if (argv == NULL)
				msg ("$Memória insuficiente para leitura dos nomes de arquivos");

			argp	 = argv + argc;
			end_argv = argv + (argc + INCR - 1); /* sutil */
		}

		if ((*argp++ = strdup (area)) == NOSTR)
			msg ("$Memória insuficiente para leitura dos nomes de arquivos");

		argc++;
	}

	if (argp != NULL)
		*argp = NOSTR;

	*argc_ptr = argc;

	return (argv);

}	/* end read_args_from_stdin */

/*
 ****************************************************************
 *	Acrescenta uma imagem à tabela				*
 ****************************************************************
 */
int
add_image_to_table (const char *path, STAT *sp, int flag)
{
	IMAGE		*ip;
	const char	*extension;
	char		ext[EXTSZ + 1];
	FILE		*fp;
	const FORMAT	*fmtp;

	/*
	 *	Descarta os tipos inválidos.
	 */
	if ((sp->st_mode & S_IFMT) != S_IFREG)
		return (0);

	/*
	 *	Abre o arquivo e obtém seu estado.
	 */
	if ((fp = fopen (path, "r")) == NOFILE)
		return (0);

	/*
	 *	Analisa a extensão do arquivo, se houver.
	 */
	if ((extension = strrchr (path, '.')) != NOSTR)
	{
		convert_extension (extension + 1, ext, EXTSZ);

		/*
		 *	Procura a extensão na tabela.
		 */
		for (fmtp = formats; /* abaixo */; fmtp++)
		{
			if (fmtp >= end_formats)
				goto no_extension_found;

			if (streq (ext, fmtp->f_extension))
				break;
		}

		/*
		 *	Achou! Verifica se é o formato.
		 */
		if (vflag == 2)
		{
			printf
			(	"*** Tentando o formato \"%s\" "
				"para o arquivo \"%s\"\n",
				ext, path
			);
		}

		if ((*fmtp->f_test) (fp))
			goto valid_file;

		/*
		 *	O arquivo NÃO contém uma imagem no formato
		 *	correspondente à sua extensão.
		 */
		fclose (fp);
		return (0);

	}	/* end if (possui extensão) */

	/*
	 *	O nome não possu extensão: tenta os formatos, um por um.
	 */
    no_extension_found:
	for (fmtp = formats; /* abaixo */; fmtp++)
	{
		if (fmtp >= end_formats)
			break;

		if (vflag == 2)
		{
			printf
			(	"*** Tentando o formato \"%s\" "
				"para o arquivo \"%s\"\n",
				fmtp->f_extension, path
			);
		}

		/*
		 *	Verifica se é o formato.
		 */
		fseek (fp, 0, SEEK_SET);

		if ((*fmtp->f_test) (fp))
			goto valid_file;

	}	/* end for (percorrendo formatos) */

	/*
	 *	O arquivo NÃO contém uma imagem nos formatos conhecidos.
	 */
	fclose (fp);
	return (0);

	/*
	 *	O arquivo contém uma imagem em um formato conhecido.
	 */
    valid_file:
	if ((path = strdup (path)) == NOSTR)
		msg ("$Memória insuficiente para a Tabela de Imagens");

	if (next_image >= end_images)
	{
		IMAGE	*new_images;

		/* Aumenta a tabela */

		end_images += INCR;

		new_images = realloc (images, (end_images - images) * sizeof (IMAGE));

		if (new_images == NOIMAGE)
			msg ("$Memória insuficiente para a Tabela de Imagens");

		next_image = new_images + (next_image - images);
		end_images = new_images + (end_images - images);
		images     = new_images;
	}

	/*
	 *	Preenche as informações.
	 */
	ip = next_image++;

	ip->i_name	= path;
	ip->i_dev	= sp->st_dev;
	ip->i_ino	= sp->st_ino;
	ip->i_file_sz	= sp->st_size;
	ip->i_format	= fmtp;
	ip->i_factor	= default_factor;
   /***	ip->i_width	= ...;	***/
   /***	ip->i_height	= ...;	***/
   /***	ip->i_ncolors	= ...;	***/

	fclose (fp);
	return (0);

}	/* end add_image_to_table */

/*
 ****************************************************************
 *	Cria a janela uma única vez				*
 ****************************************************************
 */
Window
create_window (Display *dpy)
{
	Window		win;

	/*
	 *	Cria a janela onde será mostrada a imagem.
	 */
        win =	XCreateSimpleWindow
                (	dpy, RootWindow (dpy, screen),
			0, 0, 100, 100, 1, 0, 0
		);

	if (!win)
		msg ("$Não consegui criar a janela");

	/*
	 *	Prepara a máscara de eventos e as dicas para o "fvwm".
	 */
	XSelectInput (dpy, win, ExposureMask|KeyPressMask|ButtonPressMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XPaint";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint | StateHint;

	XSetWMProperties
	(	dpy, win, NULL, NULL, (char **)save_argv, save_argc,
		NULL, &wmhint, &classhint
	);

	/*
	 *	Mapeia a janela.
	 */
	XMapWindow (dpy, win);

	return (win);

}	/* end create_window */

/*
 ****************************************************************
 *	Converte a extensão para minúsculas			*
 ****************************************************************
 */
void
convert_extension (const char *from, char *to, int sz)
{
	char	c;

	while (sz-- > 0 && (c = *from++) != '\0')
	{
		if (c >= 'a' && c <= 'z')
			c -= ('a' - 'A');

		*to++ = c;
	}

	*to = '\0';

}	/* end convert_extension */

/*
 ****************************************************************
 *	Desenha o fundo da janela				*
 ****************************************************************
 */
int
draw_window (Display *dpy, Window win, XImage *ximage, IMAGE *ip)
{
	char		title[256];
	static int	old_width, old_height;

	/*
	 *	Pequena consistência.
	 */
	if (ximage->depth != DefaultDepth (dpy, screen))
	{
		msg
		(	"\"%s\": imagem tem profundidade incorreta (%d :: %d)",
			ip->i_name, ximage->depth, DefaultDepth (dpy, screen)
		);

		return (-1);
	}

	/*
	 *	Modifica o fundo, o tamanho e o título da janela.
	 */
	XSetWindowBackgroundPixmap (dpy, win, None);

	if (old_width == ximage->width && old_height == ximage->height)
		XResizeWindow (dpy, win, ximage->width - 1, ximage->height - 1);

	if (set_background (dpy, win, ximage, ip->i_name) < 0)
		return (-1);

	XResizeWindow (dpy, win, old_width = ximage->width, old_height = ximage->height);

	snprintf
	(	title, sizeof (title),
		ip->i_ncolors > 0 ? "%s (%d x %d, %d cores)" : "%s (%d x %d)",
		basename (ip->i_name), ximage->width, ximage->height, ip->i_ncolors
	);

	XStoreName (dpy, win, title);

	XFlush (dpy);

	return (0);

}	/* end draw_window */

/*
 ****************************************************************
 *	Malha de Eventos					*
 ****************************************************************
 */
KeySym
wait_for_events (Display *dpy, Window win, int time_interval)
{
	int		fd;
	XEvent		ev;

	fd = ConnectionNumber (dpy);

	for (EVER)
	{
		if (attention (1, &fd, -1, time_interval) < 0)
			return (XK_Down);

		while (XPending (dpy))
		{
			XNextEvent (dpy, &ev);

			switch (ev.type)
			{
			    case Expose:
				XClearWindow (dpy, win);
				XFlush (dpy);
				break;

			    case ButtonPress:
				return (XK_Escape);

			    case KeyPress:
			    {
				char	text[8];
				KeySym	keysym;

				XLookupString (&ev.xkey, text, sizeof (text), &keysym, NULL);

				return (keysym);
			    }

			    default:
				break;
			}

		}	/* end while (há eventos pendentes) */

	}	/* end for (aguardando eventos) */

}	/* end wait_for_events */

/*
 ****************************************************************
 *	Modifica o fundo da janela				*
 ****************************************************************
 */
int
set_background (Display *dpy, Window win, XImage *ximage, const char *name)
{
	Pixmap		pic;
	int		ret = 0;

	/*
	 *	Cria o Pixmap correspondente à imagem.
	 */
	pic =	XCreatePixmap
		(	dpy, win,
			ximage->width, ximage->height,
			DefaultDepth (dpy, screen)
		);

	if (pic == 0)
	{
		msg ("\"%s\": o servidor X não pode criar o Mapa de Pixels", name);
		return (-1);
	}

	/*
	 *	Envia para o servidor o conteúdo do Pixmap (a imagem).
	 */
	if (XPutImage (dpy, pic, gc, ximage, 0, 0, 0, 0, ximage->width, ximage->height))
	{
		msg ("\"%s\": o servidor X rejeitou a imagem", name);
		XFreePixmap (dpy, pic);
		return (-1);
	}

	/*
	 *	O Pixmap torna-se o fundo da janela.
	 */
	XSetWindowBackgroundPixmap (dpy, win, pic);

	if (Rflag)
	{
		Atom		prop, type;
		uchar		*data;
		ulong		length, after;
		int		format;

		XClearWindow (dpy, win);
		XFlush (dpy);

		prop = XInternAtom (dpy, "_XSETROOT_ID", False);

		(void)XGetWindowProperty
		(	dpy, win, prop, 0, 1, True,
			AnyPropertyType, &type, &format, &length, &after, &data
		);

#if (0)	/*******************************************************/
		/* NÃO SEI BEM PARA QUE SERVIA... */

		if
		(	type == XA_PIXMAP && format == 32 && length == 1 &&
			after == 0
		)
			XKillClient (dpy, *((Pixmap *)data));
#endif	/*******************************************************/

		XChangeProperty
		(	dpy, win, prop, XA_PIXMAP, 32, PropModeReplace,
			(uchar *)&pic, 1
		);

		XSetCloseDownMode (dpy, RetainPermanent);
		XFlush (dpy);

		ret = -1;
	}

	XFreePixmap (dpy, pic);

	return (ret);

}	/* end set_background */

/*
 ****************************************************************
 *	Resumo de Utilização do Programa			*
 ****************************************************************
 */
void
help (int exit_val)
{
	fprintf
	(	stderr,
		"%s - mostra uma imagem (gif, jpeg, bmp ou xpm) em uma janela\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vRN] [-d <display>] [-s <tempo>] <arquivo> ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: modo verboso\n"
		"\t-R: mostra a imagem no fundo da janela-mãe\n"
		"\t-N: lê os nomes de arquivos da entrada padrão\n"
		"\t-d: conecta-se ao servidor <display>\n"
		"\t-s: aguarda um intervalo de <tempo> segundos entre imagens\n"
	);

	exit (exit_val);

}	/* end help */
