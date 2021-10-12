/*
 ****************************************************************
 *								*
 *			src/command.c				*
 *								*
 *	Tratamento do comando filho				*
 *								*
 *	Versão	3.0.0, de 25.12.97				*
 *		4.8.0, de 18.03.05				*
 *								*
 *	Módulo: cmd/xterm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2005 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <init.h>
#include <termio.h>
#include <utmp.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/keysym.h>

#include "../h/feature.h"
#include "../h/command.h"
#include "../h/misc.h"
#include "../h/main.h"
#include "../h/screen.h"
#include "../h/scrollbar.h"
#include "../h/xdefaults.h"

/*
 ****************************************************************
 *	Macros							*
 ****************************************************************
 */
#define	EVER		;;

#define	ENVSZ		6
#define CMDBUFSZ	1024
#define ESC_ARGS	32
#define MULTICLICK_TIME	500
#define MOUSE_THRESHOLD	50

/*
 ****************************************************************
 *	Variáveis locais					*
 ****************************************************************
 */
static const char	utmpf[]		= "/var/adm/utmp";
static const char	wtmpf[]		= "/var/adm/wtmp";
static const char	globalenv[]	= "/etc/globalenv";
static const char	grave_dev[]	= "/dev/grave";

static char		*envp[ENVSZ+1];	/* Novo ambiente */
static char		*cmd_name;	/* Nome do comando */
static int		cmd_pid;	/* PID do comando */

static int		grave_fd;	/* fd da tumba */

static int		refresh_count,
			refresh_type  = SLOW_REFRESH;

static Atom		wmDeleteWindow;

/*
 ******	Informações sobre o pseudo-terminal alocado *************
 */
static PTYIO		ptyio;
static int		pty_slot;
static STAT		pty_stat;
static TERMIO		pty_termio;

/*
 ******	Modos Privados (DEC) ************************************
 */
#define PrivMode_132		(1 <<  0)
#define PrivMode_132OK		(1 <<  1)
#define PrivMode_rVideo		(1 <<  2)
#define PrivMode_relOrigin	(1 <<  3)
#define PrivMode_Screen		(1 <<  4)
#define PrivMode_Autowrap	(1 <<  5)
#define PrivMode_aplCUR		(1 <<  6)
#define PrivMode_aplKP		(1 <<  7)
#define PrivMode_BackSpace	(1 <<  8)
#define PrivMode_ShiftKeys	(1 <<  9)
#define PrivMode_VisibleCursor	(1 << 10)
#define PrivMode_MouseX10	(1 << 11)
#define PrivMode_MouseX11	(1 << 12)
#define PrivMode_MouseX11Track	(1 << 13)
#define PrivMode_scrollBar	(1 << 14)
#define PrivMode_menuBar	(1 << 15)

#define PrivMode_Default	(PrivMode_Autowrap|PrivMode_ShiftKeys|PrivMode_VisibleCursor)
#define PrivMode_mouse_report	(PrivMode_MouseX10|PrivMode_MouseX11)

#define PrivMode(test,bit)			\
	do					\
	{					\
		if (test)			\
			PrivateModes |= (bit);	\
		else				\
			PrivateModes &= ~(bit);	\
	} while (0)


static ulong		PrivateModes = PrivMode_Default;
static ulong		SavedModes   = PrivMode_Default;

#undef PrivMode_Default

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int		get_pty (void);
int		set_standard_fds (void);
char		get_next_char_from_command (void);

void		clean_exit (void);
void		mourn_death_of_son (void);

void		handle_signals (int, ...);
void		handle_key_event (XEvent *);
void		handle_x_event (void);
void		handle_escape_seq (void);
void		handle_csi_seq (void);
void		handle_xterm_seq (void);
void		handle_sgr_mode (unsigned int, int[]);

void		make_utmp_entry (void);
void		clear_utmp_entry (void);
int		close_x_connection (Display *);
void		murder_dearest_son (void);
void		read_global_environment (void);

void		handle_graphics (void);
void		handle_terminal_mode (int, int, unsigned int, int[]);

extern void	XDisconnectDisplay (Display *);

/*
 ****************************************************************
 *	Executa o comando dado					*
 ****************************************************************
 */
int
exec_command (const char *argv[])
{
	/* Instala a função de término civilizado */

	atexit (clean_exit);

	/* Tenta abrir a tumba */

	if ((grave_fd = open (grave_dev, O_RDWR)) < 0)
		{ msg ("Não consegui abrir o dispositivo \"%s\"", grave_dev); return (-1); }

	/* Obtém um pseudo-terminal disponível */

	if (get_pty () < 0)
		{ msg ("Não consegui obter um pseudo-terminal"); return (-1); }

	/* Retira o "xterm" do grupo do terminal onde ele foi executado */

	settgrp (0);

	wmDeleteWindow = XInternAtom (dpy, "WM_DELETE_WINDOW", False);

	XSetWMProtocols (dpy, TermWin.parent, &wmDeleteWindow, 1);

	XSetIOErrorHandler (close_x_connection);

	/* add Backspace value */

	SavedModes |= (PrivateModes & PrivMode_BackSpace);

	/* add value for scrollBar */

	if (scrollbar_visible ())
	{
		PrivateModes |= PrivMode_scrollBar;
		SavedModes   |= PrivMode_scrollBar;
	}

	PrivMode ((pty_termio.t_erase == '\b'), PrivMode_BackSpace);

	signal (SIGHUP,  handle_signals);
	signal (SIGINT,  handle_signals);
	signal (SIGQUIT, handle_signals);
	signal (SIGTERM, handle_signals);

	/*
	 *	Cria o processo filho para executar o comando.
	 */
	cmd_name = argv == NULL ? "shell" : argv[0];

	if ((cmd_pid = fork ()) < 0)
		{ msg ("Não consegui dar 'fork'"); return (-1); }

	if (cmd_pid == 0)
	{
		/*
		 *	O comando filho é líder de grupo.	
		 *	Volta o "nice" ao default.
		 */
		setpgrp ();	nice (saved_nice - nice (0));

		/*
		 *	Estabelece os arquivos 0, 1 e 2 para o filho.
		 */
		if (set_standard_fds () < 0)
			exit (1);

		signal (SIGINT,  SIG_DFL);
		signal (SIGQUIT, SIG_DFL);

		if (home_directory != NOSTR)
			chdir (home_directory);

		if (argv != NULL)
		{
			if (streq (argv[0], "login"))
			{
				char	slot_name[4];

				/* Executa o "/bin/login" */

				read_global_environment ();

				snprintf (slot_name, sizeof (slot_name), "%d", pty_slot);

				execl
				(	"/bin/login", "login", slot_name,
					ptyio.t_nm_client + 5, "xterm", NOSTR
				);
			}
			else
			{
				/* Executa o comando dado */

				setgid (getgid ()); setuid (getuid ());

				execvp (argv [0], argv);
			}
		}
		else
		{
			const char	*shell, *home;
			char		arg[20], *cp;

			/* Executa a "shell" */

			if ((shell = getenv ("SHELL")) == NOSTR || *shell == '\0')
				shell = "/bin/sh";

			cp = arg;

			if (home_directory == NOSTR)
				*cp++ = '-';
			
			strcpy (cp, basename (shell));

			setgid (getgid ());
			setuid (getuid ());

			if (home_directory == NOSTR && (home = getenv ("HOME")) != NOSTR)
				chdir (home);

			execlp (shell, arg, NULL);
		}

		exit (1);
	}

	/*
	 *	Se necessário, desvia as mensagens de erro do kernel para o pty servidor.
	 */
	if (Options & Opt_console)
	{
		if (ioctl (ptyio.t_fd_server, TC_KERNEL_PTY_ON) < 0)
			msg ("TC_KERNEL_PTY_ON falhou: %s", strerror (errno));
	}

	make_utmp_entry ();

	return (0);

}	/* end exec_command */

/*
 ****************************************************************
 *	Processa os caracteres vindos do comando		*
 ****************************************************************
 */
void
main_loop (void)
{
	char		ch;

	for (EVER)
	{
		switch (ch = get_next_char_from_command ())
		{
		    case '\a':
			scr_bell ();
			break;

		    case '\b':
			scr_backspace ();
			break;

		    case 013:
		    case '\f':
			scr_index (UP);
			break;

		    case '\e':
			handle_escape_seq ();
			break;

		    case '\n':
			refresh_count = 0x7FFFFFFF;
			scr_putchar (ch);
			break;

		    default:
			if (ch >= ' ' || ch == '\r' || ch == '\t')
			{
				refresh_count++;
				scr_putchar (ch);
			}
			break;

#if (0)	/*******************************************************/
		    case 005:
			cmd_printf (VT100_ANS);
			break;

		    case 016:
			scr_charset_choose (1);
			break;

		    case 017:
			scr_charset_choose (0);
			break;
#endif	/*******************************************************/
		}
	}

}	/* end main_loop */

/*
 ****************************************************************
 *	Obtém um pseudo-terminal				*
 ****************************************************************
 */
int
get_pty (void)
{
	const INITAB	*initp;
	char		*win_title;
	const char	*pty_name;

	pty_slot = -1;

	/*
	 *	Obtém um pty disponível e sua posição na "initab"
	 */
	if (getpty (&ptyio) < 0)
		return (-1);

	if ((initp = getinitnam (ptyio.t_nm_client + sizeof ("/dev/") - 1)) == NOINITAB)
		return (-1);

	endinitent ();

	if (fstat (ptyio.t_fd_client, &pty_stat) < 0)
		return (-1);

	pty_slot = initp->it_slot;

	msg ("Usando o pseudo-terminal %s (%d, %d)", ptyio.t_nm_client, ptyio.t_fd_server, ptyio.t_fd_client);

	/*
	 *	Estabelece o título da janela
	 */
	pty_name  = basename (ptyio.t_nm_client);
	win_title = alloca (strlen (rs_title) + strlen (pty_name) + 5);

	sprintf (win_title, "%s (%s)", rs_title, pty_name);

	XStoreName   (dpy, TermWin.parent, win_title);
	XSetIconName (dpy, TermWin.parent, win_title);

	/*
	 *	Fecha o pty cliente, que não é usado pelo "xterm"
	 *	(ele será reaberto entre o "fork" e o "exec" do comando filho)
	 */
	close (ptyio.t_fd_client);

	return (ioctl (ptyio.t_fd_server, TCGETS, &pty_termio));

}	/* end get_pty */

/*
 ****************************************************************
 *	Estabelece os arquivos 0, 1 e 2 para o processo filho	*
 ****************************************************************
 */
int
set_standard_fds (void)
{
	int	fd;

	/*
	 *	Fecha todos os arquivos eventualmente abertos
	 */
	for (fd = NUFILE - 1; fd >= 0; fd--)
		close (fd);

	/*
	 *	Reabre o pty cliente, que terá fd = 0
	 */
	if ((ptyio.t_fd_client = open (ptyio.t_nm_client, O_RDWR)) != 0)
		return (-1);

	/*
	 *	Acerta as dimensões da janela
	 */
	pty_termio.t_nline	= TermWin.nrow;
	pty_termio.t_ncol	= TermWin.ncol;
	pty_termio.t_page	= 0;

	if (ioctl (ptyio.t_fd_client, TCSETNR, &pty_termio) < 0)
		goto bad;

	if (chown (ptyio.t_nm_client, getuid (), getgid ()) < 0)
		goto bad;

	/*
	 *	Cria os arquivos 1 e 2
	 */
	if (dup (dup (ptyio.t_fd_client)) < 0)
		goto bad;

	return (0);

    bad:
	close (ptyio.t_fd_client);
	return (-1);

}	/* end set_standard_fds */

/*
 ****************************************************************
 *	Captura um sinal					*
 ****************************************************************
 */
void
handle_signals (int signo, ...)
{
	msg ("Recebi o sinal %s", sigtostr (signo));

	signal (signo, SIG_IGN);

	murder_dearest_son ();

	XDisconnectDisplay (dpy);

	exit (1);

}	/* end handle_signals */

/*
 ****************************************************************
 *	Encerra o programa elegantemente			*
 ****************************************************************
 */
void
clean_exit (void)
{
	chmod (ptyio.t_nm_client, pty_stat.st_mode);
	chown (ptyio.t_nm_client, pty_stat.st_uid, pty_stat.st_gid);

	if (pty_slot >= 0)
		clear_utmp_entry ();

	if (ptyio.t_fd_server <= 0)
		return;

	ioctl (ptyio.t_fd_server, TCFLSH, 2);

	if (Options & Opt_console)
		ioctl (ptyio.t_fd_server, TC_KERNEL_PTY_OFF);

	close (ptyio.t_fd_server);

}	/* end clean_exit */

/*
 ****************************************************************
 *	Cria as entradas no UTMP e no WTMP			*
 ****************************************************************
 */
void
make_utmp_entry (void)
{
	int		fd;
	UTMP		utmp;
	const char	*user_name;

	if (pty_slot < 0)
		return;

	if ((user_name = getenv ("USER")) == NOSTR)
		return;

	memset (&utmp, 0, sizeof (utmp));

	strscpy (utmp.ut_name, user_name, sizeof (utmp.ut_name));
   	strscpy (utmp.ut_line, ptyio.t_nm_client + 5, sizeof (utmp.ut_line));
	memset  (utmp.ut_node, '\0', sizeof (utmp.ut_node));
	time    (&utmp.ut_time);

	if ((fd = open (utmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, pty_slot * sizeof (UTMP), SEEK_SET);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	if ((fd = open (wtmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, 0, SEEK_END);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

}	/* end make_utmp_entry */

/*
 ****************************************************************
 *	Limpa a entrada no UTMP					*
 ****************************************************************
 */
void
clear_utmp_entry (void)
{
	int		fd;
	UTMP		utmp;

	memset (&utmp, 0, sizeof (utmp));

	if ((fd = open (utmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, pty_slot * sizeof (UTMP), SEEK_SET);
		write (fd, &utmp, sizeof (utmp));
		close (fd);
	}

	time    (&utmp.ut_time);
	strscpy (utmp.ut_line, ptyio.t_nm_client + 5, sizeof (utmp.ut_line));

	if ((fd = open (wtmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, 0, SEEK_END);
		write (fd, &utmp, sizeof (utmp));
		close (fd);
	}

}	/* end clear_utmp_entry */

/*
 ****************************************************************
 *	Redimensiona o tamanho da janela no TERMIO		*
 ****************************************************************
 */
void
stty_resize (void)
{
	/*
	 *	É preciso obter novamente as características, que podem
	 *	ter mudado com um "stty". Observe que "ptyio.t_fd_server"
	 *	não pode ser 0!
	 */
	if (ptyio.t_fd_server <= 0 || ioctl (ptyio.t_fd_server, TCGETS, &pty_termio) < 0)
		return;

	pty_termio.t_ncol  = TermWin.ncol;
	pty_termio.t_nline = TermWin.nrow;

	if (pty_termio.t_page > 0)
		pty_termio.t_page = pty_termio.t_nline - 1;

	ioctl (ptyio.t_fd_server, TCSETNR, &pty_termio);

	/* Avisa ao grupo do filho que a janela mudou */

	kill (-cmd_pid, SIGWINCH);

}	/* end stty_resize */

/*
 ****************************************************************
 *	Processa dados recebidos do teclado			*
 ****************************************************************
 */
void
handle_key_event (XEvent *ev)
{
	static int		numlock_state = 0;
	static XComposeStatus	compose = {NULL, 0};
	static char		kbuf[16];
	int			ctrl, meta, shft, len;
	KeySym			keysym;

	/*
	 *	use Num_Lock to toggle Keypad on/off.  If Num_Lock is off, allow an
	 *	escape sequence to toggle the Keypad.
	 *	Always permit `shift' to override the current setting
	 */
	shft = (ev->xkey.state & ShiftMask);
	ctrl = (ev->xkey.state & ControlMask);
	meta = (ev->xkey.state & Mod1Mask);

	if (numlock_state || (ev->xkey.state & Mod5Mask))
	{
		numlock_state = (ev->xkey.state & Mod5Mask);	/* numlock toggle */
		PrivMode ((!numlock_state), PrivMode_aplKP);
	}

	len = XLookupString (&ev->xkey, kbuf, sizeof(kbuf), &keysym, &compose);

	/*
	 *	have unmapped Latin[2-4] entries -> Latin1
	 *	good for installations  with correct fonts, but without XLOCAL
	 */
	if (!len && (keysym >= 0x0100) && (keysym < 0x0400))
	{
		len = 1;
		kbuf [0] = (keysym & 0xFF);
	}

	if (shft)
	{
		if (keysym >= XK_F1 && keysym <= XK_F10)
		{
			/*
			 *	Shift + F1 - F10 generates F11 - F20
			 */
			keysym += (XK_F11 - XK_F1);
			shft = 0;		/* turn off Shift */
		}
		else if (!ctrl && !meta && (PrivateModes & PrivMode_ShiftKeys))
		{
			switch (keysym)
			{
				/* normal XTerm key bindings */
			    case XK_Prior:	/* Shift+Prior = scroll back */
				if (TermWin.saveLines)
				{
					scr_page (UP, TermWin.nrow * 4/5);
					return;
				}
				break;

			    case XK_Next:	/* Shift+Next = scroll forward */
				if (TermWin.saveLines)
				{
					scr_page (DN, TermWin.nrow * 4/5);
					return;
				}
				break;

			    case XK_Insert:	/* Shift+Insert = paste mouse selection */
				selection_request
				(	ev->xkey.time,
					ev->xkey.x,
					ev->xkey.y
				);
				return;

				/*
				 *	xterm extras
				 */
			    case XK_KP_Add:	/* Shift+KP_Add = bigger font */
				change_font (0, FONT_UP);
				return;

			    case XK_KP_Subtract:/* Shift+KP_Subtract = smaller font */
				change_font (0, FONT_DN);
				return;

			    case XK_KP_0:
			    case XK_KP_1:
			    case XK_KP_2:
			    case XK_KP_3:
			    case XK_KP_4:
			    case XK_KP_5:
			    case XK_KP_6:
			    case XK_KP_7:
			    case XK_KP_8:
			    case XK_KP_9:
				len = 1;
				kbuf[0] = '0' + (keysym - XK_KP_0);
				goto out;
			}
		}
	}

	if (keysym >= 0xFF00 && keysym <= 0xFFFF)
	{
#ifdef KEYSYM_RESOURCE
		if (!(shft|ctrl) && KeySym_map [keysym - 0xFF00] != NULL)
		{
			const char	*xbuf;

			xbuf = (KeySym_map [keysym - 0xFF00]);
			len = *xbuf++;

			if (meta)
				cmd_write ("\e", 1);

			cmd_write (xbuf, len);
			return;
		}
#endif

		switch (keysym)
		{
		    case XK_BackSpace:
		    case XK_Delete:
		    case XK_KP_Delete:
			len = 1;
			kbuf [0] = '\b';
			break;

		    case XK_Tab:
			if (shft)
			{
				len = 3;
				strcpy (kbuf,"\e[Z");
			}
			break;

		    case XK_Home:
		    case XK_KP_Home:
			len = strlen (strcpy (kbuf, "\e[7~"));
			break;

		    case XK_KP_Left:		/* "\e[D" */
		    case XK_KP_Up:		/* "\e[A" */
		    case XK_KP_Right:		/* "\e[C" */
		    case XK_KP_Down:		/* "\e[B" */
			keysym -= (XK_KP_Left - XK_Left);
			/* FALL THROUGH */

		    case XK_Left:		/* "\e[D" */
		    case XK_Up:			/* "\e[A" */
		    case XK_Right:		/* "\e[C" */
		    case XK_Down:		/* "\e[B" */
			len = 3;
			strcpy (kbuf,"\e[@");
			kbuf [2] = ("DACB" [keysym - XK_Left]);

			if (PrivateModes & PrivMode_aplCUR)
			{
				kbuf [1] = 'O';
			}
			else if (shft)		/* do Shift first */
			{
				kbuf [2] = ("dacb" [keysym - XK_Left]);
			}
			else if (ctrl)
			{
				kbuf [1] = 'O';
				kbuf [2] = ("dacb" [keysym - XK_Left]);
			}
			break;

		    case XK_Prior:
		    case XK_KP_Prior:
			len = 4;
			strcpy (kbuf, "\e[5~");
			break;

		    case XK_Next:
		    case XK_KP_Next:
			len = 4;
			strcpy (kbuf, "\e[6~");
			break;

		    case XK_End:
		    case XK_KP_End:
			len = strlen (strcpy (kbuf, "\e[8~"));
			break;

		    case XK_Select:
			len = 4;
			strcpy (kbuf, "\e[4~");
			break;

		    case XK_Insert:
		    case XK_KP_Insert:
			len = 4;
			strcpy (kbuf, "\e[2~");
			break;

#if (0)	/*******************************************************/
		    case XK_Execute:
			len = 4;
			strcpy (kbuf,"\e[3~");
			break;

		    case XK_Menu:
			len = 5;
			strcpy (kbuf, "\e[29~");
			break;

		    case XK_Find:
			len = 4;
			strcpy (kbuf, "\e[1~");
			break;

		    case XK_Help:
			len = 5;
			strcpy (kbuf, "\e[28~");
			break;

		    case XK_KP_F1:	/* "\eOP" */
		    case XK_KP_F2:	/* "\eOQ" */
		    case XK_KP_F3:	/* "\eOR" */
		    case XK_KP_F4:	/* "\eOS" */
			len = 3;
			strcpy (kbuf,"\eOP");
			kbuf [2] += (keysym - XK_KP_F1);
			break;
#endif	/*******************************************************/

		    case XK_KP_Enter:
			/* allow shift to override */
			if ((PrivateModes & PrivMode_aplKP) ? !shft : shft)
			{
				len = 3;
				strcpy (kbuf,"\eOM");
			}
			else
			{
				len = 1;
				kbuf [0] = '\r';
			}
			break;

#define FKEY(n,fkey)	{ len = 5; sprintf (kbuf, "\e[%02d~", (int)((n) + (keysym - fkey))); }

		    case XK_F1:		/* "\e[11~" */
		    case XK_F2:		/* "\e[12~" */
		    case XK_F3:		/* "\e[13~" */
		    case XK_F4:		/* "\e[14~" */
		    case XK_F5:		/* "\e[15~" */
			FKEY (11, XK_F1);
			break;

		    case XK_F6:		/* "\e[17~" */
		    case XK_F7:		/* "\e[18~" */
		    case XK_F8:		/* "\e[19~" */
		    case XK_F9:		/* "\e[20~" */
		    case XK_F10:	/* "\e[21~" */
			FKEY (17, XK_F6);
			break;

		    case XK_F11:	/* "\e[23~" */
		    case XK_F12:	/* "\e[24~" */
			FKEY (23, XK_F11);
			break;
#undef FKEY
		}
	}
	else if (ctrl && keysym == XK_minus)
	{
		len = 1;
		kbuf [0] = '\037';	/* Ctrl-Minus generates ^_ (31) */
	}
	else
	{
		/* vazio */;
	}

	if (len <= 0)
		return;	/* not mapped */

	/*
	 * these modifications only affect the static keybuffer
	 * pass Shift/Control indicators for function keys ending with `~'
	 *
	 * eg,
	 *	Prior = "ESC[5~"
	 *	Shift+Prior = "ESC[5~"
	 *	Ctrl+Prior = "ESC[5^"
	 *	Ctrl+Shift+Prior = "ESC[5@"
	 */
	if (kbuf [0] == '\e' && kbuf [1] == '[' && kbuf [len-1] == '~')
		kbuf [len-1] = (shft ? (ctrl ? '@' : '$') : (ctrl ? '^' : '~'));

	/*
	 *	escape prefix
	 */
	if (meta)
	{
		const char ch = '\e';
		cmd_write (&ch, 1);
	}

#undef DEBUG_KEY
#ifdef DEBUG_KEY
{
	char	*p;
	int	i;

	fprintf (stderr, "key 0x%04X [%d]: `", (unsigned int)keysym, len);

	for (i = 0, p = kbuf; i < len; i++, p++)
	{
		fprintf
		(	stderr,
			(*p >= ' ' && *p < '\177' ? "%c" : "\\%03o"),
			*p
		);
	}

	fprintf (stderr, "'\n");
	fflush (stderr);
}
#endif

    out:
	cmd_write (kbuf, len);

}	/* end handle_key_event */

/*
 ****************************************************************
 *	Processa os Eventos do Servidor				*
 ****************************************************************
 */
void
handle_x_event (void)
{
	static Time	buttonpress_time;
	static int	clicks = 0;
	static int	bypass_keystate = 0;
	int		reportmode;
	XEvent		ev;

	XNextEvent (dpy, &ev);

	switch (ev.type)
	{
	    case KeyPress:
		handle_key_event (&ev);
		break;

	    case ClientMessage:
		if (ev.xclient.format == 32 && ev.xclient.data.l[0] == wmDeleteWindow)
		{
			murder_dearest_son ();
			XDisconnectDisplay (dpy);
			exit (0);
		}

		break;

	    case MappingNotify:
		XRefreshKeyboardMapping (&ev.xmapping);
		break;

	    case VisibilityNotify:
		switch (ev.xvisibility.state)
		{
		    case VisibilityUnobscured:
			refresh_type = FAST_REFRESH;
			break;

		    case VisibilityPartiallyObscured:
			refresh_type = SLOW_REFRESH;
			break;

		    default:
			refresh_type = NO_REFRESH;
			break;
		}
		break;

	    case FocusIn:
		if (!TermWin.focus)
			TermWin.focus = 1;
		break;

	    case FocusOut:
		if (TermWin.focus)
			TermWin.focus = 0;
		break;

	    case ConfigureNotify:
		resize_window ();
		break;

	    case SelectionClear:
		selection_clear ();
		break;

	    case SelectionNotify:
		selection_paste (ev.xselection.requestor, ev.xselection.property, True);
		break;

	    case SelectionRequest:
		selection_send (&ev.xselectionrequest);
		break;

	    case GraphicsExpose:
	    case Expose:
		if (ev.xany.window == TermWin.vt)
		{
			scr_expose
			(	ev.xexpose.x, ev.xexpose.y,
				ev.xexpose.width, ev.xexpose.height
			);
		}
		else
		{
			XEvent unused_xevent;

			while (XCheckTypedWindowEvent (dpy, ev.xany.window, Expose, &unused_xevent))
				/* vazio */;

			while (XCheckTypedWindowEvent (dpy, ev.xany.window, GraphicsExpose, &unused_xevent))
				/* vazio */;

			if (isScrollbarWindow (ev.xany.window))
				{ scrollbar_setNone (); scrollbar_show (0); }
		}
		break;

	    case ButtonPress:
		bypass_keystate = ev.xbutton.state & (Mod1Mask|ShiftMask);
		reportmode	= bypass_keystate ? 0 : (PrivateModes & PrivMode_mouse_report);

		if (ev.xany.window == TermWin.vt)
		{
			if (ev.xbutton.subwindow == None)
			{
				if (reportmode)
				{
					if (reportmode & PrivMode_MouseX10)
					{
						/* no state info allowed */
						ev.xbutton.state = 0;
					}
#ifdef MOUSE_REPORT_DOUBLECLICK
					if (ev.xbutton.button == Button1)
					{
						if (ev.xbutton.time - buttonpress_time < MULTICLICK_TIME)
							clicks++;
						else
							clicks = 1;
					}
#else
					clicks = 1;
#endif	/* MOUSE_REPORT_DOUBLECLICK */
					mouse_report (&ev.xbutton);
				}
				else
				{
					switch (ev.xbutton.button)
					{
					    case Button1:
						if (ev.xbutton.time - buttonpress_time < MULTICLICK_TIME)
							clicks++;
						else
							clicks = 1;

						selection_click (clicks, ev.xbutton.x, ev.xbutton.y);
						break;

					    case Button3:
						selection_extend (ev.xbutton.x, ev.xbutton.y);
						break;

					    case Button4:
						scr_page (UP, 1);
						return;

					    case Button5:
						scr_page (DN, 1);
						return;
					}

					buttonpress_time = ev.xbutton.time;
					return;
				}
			}
		}

		if (isScrollbarWindow (ev.xany.window))
		{
			scrollbar_setNone ();

			/*
			 * Rxvt-style scrollbar:
			 * move up if mouse if above slider
			 * move dn if mouse if below slider
			 *
			 * XTerm-style scrollbar:
			 * Move display proportional to pointer location
			 * pointer near top -> scroll one line
			 * pointer near bot -> scroll full page
			 */
#ifndef NO_SCROLLBAR_REPORT
			if (reportmode)
			{
				/*
				 * Mouse report disabled scrollbar:
				 * arrow buttons - send up/down
				 * click on scrollbar - send pageup/down
				 */
				if (scrollbar_upButton (ev.xbutton.y))
				{
					cmd_printf ("\e[A");
				}
				else if (scrollbar_dnButton (ev.xbutton.y))
				{
					cmd_printf ("\e[B");
				}
				else
				{
					switch (ev.xbutton.button)
					{
					    case Button2:
						cmd_printf ("\014");
						break;
					    case Button1:
						cmd_printf ("\e[6~");
						break;
					    case Button3:
						cmd_printf ("\e[5~");
						break;
					}
				}
			}
			else
#endif	/* NO_SCROLLBAR_REPORT */
			{
				if (scrollbar_upButton (ev.xbutton.y))
				{
					/* I would like continuous scrolling */
					if (scr_page (UP, 1))
						scrollbar_setUp ();
				}
				else if (scrollbar_dnButton (ev.xbutton.y))
				{
					if (scr_page (DN, 1))
						scrollbar_setDn ();
				}
				else
				{
					switch (ev.xbutton.button)
					{
					    case Button2:
						if
						(	scrollbar_above_slider (ev.xbutton.y) ||
							scrollbar_below_slider (ev.xbutton.y)
						)
						scr_move_to
						(	scrollbar_position (ev.xbutton.y),
							scrollbar_size ()
						);
						scrollbar_setMotion ();
						break;

					    case Button1:
						/* continua abaixo */

					    case Button3:
						if (scrollbar_above_slider (ev.xbutton.y))
							scr_page (UP, TermWin.nrow / 4);
						else if (scrollbar_below_slider (ev.xbutton.y))
							scr_page (DN, TermWin.nrow / 4);
						else
							scrollbar_setMotion ();
						break;
					}
				}
			}
			return;
		}
		break;

	    case ButtonRelease:
		reportmode = bypass_keystate ? 0 : (PrivateModes & PrivMode_mouse_report);

		if (scrollbar_isUpDn ())
			{ scrollbar_setNone (); scrollbar_show (0); }

		if (ev.xany.window == TermWin.vt)
		{
			if (ev.xbutton.subwindow == None)
			{
				if (reportmode)
				{
					switch (reportmode & PrivMode_mouse_report)
					{
					    case PrivMode_MouseX10:
						break;

					    case PrivMode_MouseX11:
						ev.xbutton.state = bypass_keystate;
						ev.xbutton.button = AnyButton;
						mouse_report (&ev.xbutton);
						break;
					}
					return;
				}

				switch (ev.xbutton.button)
				{
				    case Button1:
					if
					(	bypass_keystate && clicks <= 1 &&
						(PrivateModes & PrivMode_mouse_report)
					)
						selection_extend (ev.xbutton.x, ev.xbutton.y);

					/* continua abaixo */

				    case Button3:
					selection_make (ev.xbutton.time);
					break;

				    case Button2:
					selection_request
					(	ev.xbutton.time,
						ev.xbutton.x, ev.xbutton.y
					);
					break;
				}
			}
		}
		break;

	    case MotionNotify:
	    {
		Window		unused_win;
		int		unused_int;
		unsigned int	unused_mask;

		if (!bypass_keystate && (PrivateModes & PrivMode_mouse_report))
			break;

		if
		(	ev.xany.window == TermWin.vt && clicks <= 1 &&
			(ev.xbutton.state & (Button1Mask|Button3Mask))
		)
		{
			while (XCheckTypedWindowEvent (dpy, TermWin.vt, MotionNotify, &ev))
				/* vazio */;

			XQueryPointer
			(	dpy, TermWin.vt,
				&unused_win, &unused_win, &unused_int, &unused_int,
				&ev.xbutton.x, &ev.xbutton.y, &unused_mask
			);

			if ((ev.xmotion.time - buttonpress_time) > MOUSE_THRESHOLD)
				selection_extend (ev.xbutton.x, ev.xbutton.y);

			break;
		}

		if ((ev.xany.window == scrollBar.win) && scrollbar_isMotion ())
		{
			while (XCheckTypedWindowEvent (dpy, scrollBar.win, MotionNotify, &ev))
				/* vazio */;

			XQueryPointer
			(	dpy, scrollBar.win,
				&unused_win, &unused_win, &unused_int, &unused_int,
				&ev.xbutton.x, &ev.xbutton.y, &unused_mask
			);

			scr_move_to (scrollbar_position (ev.xbutton.y), scrollbar_size ());

			scr_refresh (refresh_type);
			refresh_count = 0;
			scrollbar_show (1);
		}

		break;
	    }
	}

}	/* end handle_x_event */

/*
 ****************************************************************
 *	Envia "count" caracteres para o comando			*
 ****************************************************************
 */
void
cmd_write (const char *buf, unsigned int count)
{
	int		n;

	while (count > 0)
	{
		if ((n = write (ptyio.t_fd_server, buf, count)) > 0)
			{ count -= n; buf += n; }
	}

}	/* end cmd_write */

/*
 ****************************************************************
 *	Saída formatada para o comando				*
 ****************************************************************
 */
void
cmd_printf (const char *fmt, ...)
{
	char		buf[256];
	va_list		arg_ptr;
	int		len;

	va_start (arg_ptr, fmt);

	len = vsnprintf (buf, sizeof (buf), fmt, arg_ptr);

	va_end (arg_ptr);

	cmd_write (buf, len);

}	/* end cmd_printf */

/*
 ****************************************************************
 *	Processa seqüências de ESC				*
 ****************************************************************
 */
void
handle_escape_seq (void)
{
	switch (get_next_char_from_command ())
	{
	    case 'D':
		scr_index (UP);
		break;

	    case 'E':
		scr_putchar ('\n');
		scr_putchar ('\r');
		break;

	    case 'M':
		scr_index (DN);
		break;

	    case '[':
		handle_csi_seq ();
		break;

	    case ']':
		handle_xterm_seq ();
		break;

	    case 'c':
		scr_poweron ();	
		break;

	    case 'H':
		scr_set_tab (1);
		break;

	    case '(':
		scr_charset_set (0, get_next_char_from_command ());
		break;

	    case ')':
		scr_charset_set (1, get_next_char_from_command ());
		break;

	    case '*':
		scr_charset_set (2, get_next_char_from_command ());
		break;

	    case '+':
		scr_charset_set (3, get_next_char_from_command ());
		break;

	    case '7':
		scr_cursor (SAVE);
		break;

	    case '8':
		scr_cursor (RESTORE);
		break;

	    case '#':
		if (get_next_char_from_command () == '8')
			scr_E ();
		break;

	    case '=':
		PrivMode (1, PrivMode_aplKP);
		break;

	    case '>':
		PrivMode (0, PrivMode_aplKP);
		break;

	    case '@':
		get_next_char_from_command ();
		break;

#if (0)	/*******************************************************/
	    case 'G':
		handle_graphics ();
		break;

    /*case 'N':	scr_single_shift (2);	break;*/
    /*case 'O':	scr_single_shift (3);	break;*/

	    case 'Z':
		cmd_printf (ESCZ_ANSWER);
		break;	/* steal obsolete ESC [ c */
#endif	/*******************************************************/

	    case 'n':
		scr_charset_choose (2);
		break;

	    case 'o':
		scr_charset_choose (3);
		break;
	}

}	/* end handle_escape_seq */

/*
 ****************************************************************
 *	Processa as seqüências ESC[...				*
 ****************************************************************
 */
void
handle_csi_seq (void)
{
	char		ch, priv;
	int		arg[ESC_ARGS], nargs;

	nargs   = 0;
	arg [0] = 0;
	arg [1] = 0;

	priv = 0;

	if ((ch = get_next_char_from_command ()) >= '<' && ch <= '?')
	{
		priv = ch;
		ch = get_next_char_from_command ();
	}

	/* read any numerical arguments */
	do
	{
		int n;
		for (n = 0; isdigit (ch); ch = get_next_char_from_command ())
			n = n * 10 + (ch - '0');

		if (nargs < ESC_ARGS)
			arg [nargs++] = n;

		if (ch == '\b')
		{
			scr_backspace ();
		}
		else if (ch == '\e')
		{
			handle_escape_seq ();
			return;
		}
	     	else if (ch < ' ')
		{
			scr_putchar (ch);
			return;
		}

		if (ch < '@')
			ch = get_next_char_from_command ();

	}	while (ch >= ' ' && ch < '@');

	if (ch == '\e')
	{
		handle_escape_seq ();
		return;
	}
	else if (ch < ' ')
		return;

	switch (ch)
	{
	    case 'A':
	    case 'e':		/* up <n> */
		scr_gotorc ((arg[0] ? -arg[0] : -1), 0, RELATIVE);
		break;
	    case 'B':			/* down <n> */
		scr_gotorc ((arg[0] ? +arg[0] : +1), 0, RELATIVE);
		break;
	    case 'C':
	    case 'a':		/* right <n> */
		scr_gotorc (0, (arg[0] ? +arg[0] : +1), RELATIVE);
		break;
	    case 'D':			/* left <n> */
		scr_gotorc (0, (arg[0] ? -arg[0] : -1), RELATIVE);
		break;
	    case 'E':			/* down <n> & to first column */
		scr_gotorc ((arg[0] ? +arg[0] : +1), 0, R_RELATIVE);
		break;
	    case 'F':			/* up <n> & to first column */
		scr_gotorc ((arg[0] ? -arg[0] : -1), 0, R_RELATIVE);
		break;
	    case 'G':
	    case '`':		/* move to col <n> */
		scr_gotorc (0, (arg[0] ? +arg[0] : +1), R_RELATIVE);
		break;
	    case 'd':			/* move to row <n> */
		scr_gotorc ((arg[0] ? +arg[0] : +1), 0, C_RELATIVE);
		break;
	    case 'H':
	    case 'f':		/* position cursor */
		switch (nargs)
		{
		    case 0:
			scr_gotorc (0, 0, 0);
			break;
		    case 1:
			scr_gotorc ((arg[0] ? arg[0]-1 : 0), 0, 0);
			break;
		    default:
			scr_gotorc (arg[0]-1, arg[1]-1, 0);
			break;
		}
		break;

	    case 'I':
		scr_tab (arg[0] ? +arg[0] : +1);
		break;
	    case 'Z':
		scr_tab (arg[0] ? -arg[0] : -1);
		break;
	    case 'J':
		scr_erase_screen (arg[0]);
		break;
	    case 'K':
		scr_erase_line (arg[0]);
		break;
	    case '@':
		scr_insdel_chars ((arg[0] ? arg [0] : 1), INSERT);
		break;
	    case 'L':
		scr_insdel_lines ((arg[0] ? arg [0] : 1), INSERT);
		break;
	    case 'M':
		scr_insdel_lines ((arg[0] ? arg [0] : 1), DELETE);
		break;
	    case 'X':
		scr_insdel_chars ((arg[0] ? arg [0] : 1), ERASE);
		break;
	    case 'P':
		scr_insdel_chars ((arg[0] ? arg [0] : 1), DELETE);
		break;
#if (0)	/*******************************************************/
	    case 'c':
		cmd_printf (VT100_ANS);
		break;
#endif	/*******************************************************/
	    case 'm':
		handle_sgr_mode (nargs, arg);
		break;

#if (0)	/*******************************************************/
	    case 'n':			/* request for information */
		switch (arg[0])
		{
		    case 5:
			cmd_printf ("\e[0n");
			break;	/* ready */
		    case 6:
			scr_report_position ();
			break;
#if defined (ENABLE_DISPLAY_ANSWER)
		    case 7:
			cmd_printf ("%s\n", display_name);
			break;
#endif
		    case 8:
			xterm_seq (XTerm_title, APL_NAME "-" VERSION);
			break;
		}
		break;
#endif	/*******************************************************/

	    case 'r':			/* set top and bottom margins */
		if (priv != '?')
		{
			if (nargs < 2 || arg[0] >= arg[1])
				scr_scroll_region (0, 10000);
			else
				scr_scroll_region (arg[0] - 1, arg[1] - 1);
			break;
		}
		/* drop */

	    case 's':
	    case 't':
	    case 'h':
	    case 'l':
#if (0)	/*******************************************************/
		handle_terminal_mode (ch, priv, nargs, arg);
#endif	/*******************************************************/
		break;

	    case 'g':
		switch (arg[0])
		{
		    case 0:
			scr_set_tab (0);
			break;	/* delete tab */
		    case 3:
			scr_set_tab (-1);
			break;	/* clear all tabs */
		}
		break;

	    case 'W':
		switch (arg [0])
		{
		    case 0:
			scr_set_tab (1);
			break;	/* = ESC H */
		    case 2:
			scr_set_tab (0);
			break;	/* = ESC [ 0 g */
		    case 5:
			scr_set_tab (-1);
			break;	/* = ESC [ 3 g */
		}
		break;
	}

}	/* end handle_csi_seq */

/*
 ****************************************************************
 *	Processa as seqüências ESC] Ps ; Pt BEL			*
 ****************************************************************
 */
void
handle_xterm_seq (void)
{
	char	ch, string[512];
	int	arg;

	ch = get_next_char_from_command ();

	for (arg = 0; isdigit (ch); ch = get_next_char_from_command ())
		arg = arg * 10 + (ch - '0');

	if (ch == ';')
	{
		int n = 0;

		while ((ch = get_next_char_from_command ()) != 007)
		{
			if ((n < sizeof (string) - 1))
			{
				/* silently translate whitespace to space char */
				if (isspace (ch))
					ch = ' ';

				if (ch >= ' ')
					string [n++] = ch;
			}
		}

		string [n] = '\0';
		xterm_seq (arg, string);
	}

}	/* end handle_xterm_seq */

/*
 ****************************************************************
 *	Processa seqüências "sgr"				*
 ****************************************************************
 */
void
handle_sgr_mode (unsigned int nargs, int arg [])
{
	int		i;

	if (nargs == 0)
	{
		scr_rendition (0, ~RS_None);
		return;
	}

	for (i = 0; i < nargs; i++)
	{
		switch (arg [i])
		{
		    case 0:
			scr_rendition (0, ~RS_None);
			break;
		    case 1:
			scr_rendition (1, RS_Bold);
			break;
		    case 4:
			scr_rendition (1, RS_Uline);
			break;
		    case 5:
			scr_rendition (1, RS_Blink);
			break;
		    case 7:
			scr_rendition (1, RS_RVid);
			break;
		    case 22:
			scr_rendition (0, RS_Bold);
			break;
		    case 24:
			scr_rendition (0, RS_Uline);
			break;
		    case 25:
			scr_rendition (0, RS_Blink);
			break;
		    case 27:
			scr_rendition (0, RS_RVid);
			break;

		    case 30: case 31:		/* set fg color */
		    case 32: case 33:
		    case 34: case 35:
		    case 36: case 37:
			scr_color (minColor + (arg [i] - 30), RS_Bold);
			break;
		    case 39:			/* default fg */
			scr_color (restoreFG, RS_Bold);
			break;

		    case 40: case 41:		/* set bg color */
		    case 42: case 43:
		    case 44: case 45:
		    case 46: case 47:
			scr_color (minColor + (arg [i] - 40), RS_Blink);
			break;
		    case 49:			/* default bg */
			scr_color (restoreBG, RS_Blink);
			break;
		}
	}

}	/* end handle_sgr_mode */

/*
 ****************************************************************
 *	Retorna o próximo caractere enviado pelo comando	*
 ****************************************************************
 */
char
get_next_char_from_command (void)
{
	static int	refreshed = 0;
	static int	fd_vet[3], nfd = -1, index;
	static char	cmd_area_base[CMDBUFSZ], *cmd_area_ptr,	*cmd_area_endp;
	int		n;

	/*
	 *	Atualiza a janela, se necessário
	 */
	if (refresh_count >= TermWin.ncol * TermWin.nrow / 2)
		{ scr_refresh (refresh_type); refresh_count = 0; refreshed = 1; }

	/*
	 *	Há caracteres disponíveis: não precisa ler
	 */
	if (cmd_area_ptr < cmd_area_endp)
		goto has_char;

	if (nfd < 0)
	{
		/* Só da primeira vez que é chamada */

		fd_vet[0] = XConnectionNumber (dpy);
		fd_vet[1] = ptyio.t_fd_server;
		fd_vet[2] = grave_fd;

		nfd	  = 3;
		index	  = -1;
	}

	for (EVER)
	{
		while (XPending (dpy))
		{
			/* Processa os eventos pendentes */

			refreshed = 0; handle_x_event ();

			/* Certas ações relacionadas ao "mouse" produzem caracteres */

			if (cmd_area_ptr < cmd_area_endp)
				goto has_char;
		}

		index = attention (nfd, fd_vet, index, refreshed ? 0 : -1);

		switch (index)
		{
		    case 0:
			/* Já foi processado acima */
			break;

		    case 1:
			/* Há dados em "ptyio.t_fd_server" (escritos pelo comando) */

			if ((n = ioctl (ptyio.t_fd_server, TCNREAD)) <= 0)
				msg ("Attention prematura: n = %d\n", n);

			if ((n = read (ptyio.t_fd_server, cmd_area_base, sizeof (cmd_area_base) - 1)) > 0)
			{
				cmd_area_ptr  = cmd_area_base;
				cmd_area_endp = cmd_area_base + n;

				goto has_char;
			}
			break;

		    case 2:
			/* Morreu um filho */

			mourn_death_of_son ();
			break;

		    default:
			/* Nenhum descritor tem dados disponíveis */

			index = -1;

			if (refreshed == 0)
			{
				scr_refresh (refresh_type);
				scrollbar_show (1);

				refresh_count = 0; refreshed = 1;
			}
			break;
		}
	}

    has_char:
	refreshed = 0;
	return (*cmd_area_ptr++);

}	/* end get_next_char_from_command */

/*
 ****************************************************************
 *	Tratamento de erros (chamada pela Xlib)			*
 ****************************************************************
 */
int
close_x_connection (Display *dp)
{
	murder_dearest_son ();

	if (errno == EPIPE || errno == NOERR)
	{
#if (0)	/****************************************************/
		msg ("O servidor X (\"%s\") encerrou a conexão.", DisplayString (dpy));
#endif	/****************************************************/

		XDisconnectDisplay (dp);
		exit (0);
	}

	msg ("Erro na conexão com servidor X (\"%s\"): %s.", DisplayString (dp), strerror (errno));

	XDisconnectDisplay (dp);

	exit (1);

	return (0);	/* to make "lint" happy... */

}	/* end close_x_connection */

/*
 ****************************************************************
 *	Mata o filho (chorando muito)				*
 ****************************************************************
 */
void
murder_dearest_son (void)
{
	msg ("Vou enviar o SIGKILL ao grupo do filho \"%s\", PID = %d", cmd_name, cmd_pid);

	kill (-cmd_pid, SIGKILL);

	mourn_death_of_son ();

	msg ("Não recebi notícias do filho morto");

}	/* end murder_dearest_son */

/*
 ****************************************************************
 *	Processa o arquivo "/etc/globalenv"			*
 ****************************************************************
 */
void
read_global_environment (void)
{
	int		i;
	FILE		*pp;
	char		sysname[32], buf[32];
	UTSNAME		uts;

	/*
	 *	A primeira variável do ambiente é o nome do Sistema Operacional
	 */
	uname (&uts); snprintf (sysname, sizeof (sysname), "SYS=%s", uts.uts_sysname);

	if ((envp[0] = strdup (sysname)) == NOSTR)
		return;

	environ = (const char **)envp;

	/*
	 *	O restante do ambiente é lido de "/etc/globalenv"
	 */
	if ((pp = fopen (globalenv, "r")) == NOFILE)
	{
		msg ("Não consegui abrir \"%s\"", globalenv);
		return;
	}
	
	for
	(	i = 1;
		i < ENVSZ && fngets (buf, sizeof (buf), pp) == buf;
		i++
	)
	{
		if (strchr (buf, '=') == NOSTR)
		{
			msg ("Linha inválida em \"%s\": \"%s\"", globalenv, buf);
			continue;
		}

		if ((envp[i] = strdup (buf)) == NOSTR)
			break;
	}

	fclose (pp);

}	/* end read_global_environment */

/*
 ****************************************************************
 *	Prateia mansamente sobre o filho morto			*
 ****************************************************************
 */
void
mourn_death_of_son (void)
{
	int	status, pid;

	if ((pid = wait (&status)) == cmd_pid)
	{
		msg ("Morreu o filho \"%s\", PID = %d, status = %02X", cmd_name, pid, status);

		XDisconnectDisplay (dpy);

		exit (0);
	}

	msg ("Morreu o processo não-filho %d, status = %02X", pid, status);

}	/* end mourn_death_of_son */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Processa o modo gráfico (do Rob Nations)		*
 ****************************************************************
 */
void
handle_graphics (void)
{
	char		ch, cmd;

	cmd = get_next_char_from_command ();

	if (cmd == 'Q')			/* query graphics */
	{
		cmd_printf ("\eG0\n");		/* no graphics */
		return;
	}

	/* swallow other graphics sequences until terminating ':' */
	do
	{
		ch = get_next_char_from_command ();

	}	while (ch != ':');

}	/* end handle_graphics */

/*
 ****************************************************************
 *	Processa as seqüências ESC [ ? Ps mode			*
 ****************************************************************
 */
/*
 * mode can only have the following values:
 *	'l' = low
 *	'h' = high
 *	's' = save
 *	'r' = restore
 *	't' = toggle
 * so no need for fancy checking
 */
#define PrivCases(bit)						\
	if (mode == 't')					\
		state = !(PrivateModes & bit);			\
	else							\
		state = mode;					\
								\
	switch (state)						\
	{							\
	    case 's':						\
		SavedModes |= (PrivateModes & bit);		\
		continue;					\
		break;						\
	    case 'r':						\
		state = (SavedModes & bit) ? 1 : 0;		\
		/*drop*/					\
	    default:						\
		PrivMode (state, bit);				\
	}

void
handle_terminal_mode (int mode, int priv,
		       unsigned int nargs, int arg[])
{
	unsigned int	i;
	int		state;

	if (nargs == 0)
		return;

	switch (mode)			/* make lo/hi boolean */
	{
	    case 'l':
		mode = 0;
		break;
	    case 'h':
		mode = 1;
		break;
	}

	switch (priv)
	{
	    case 0:
		if (mode && mode != 1)
			return;		/* only do high/low */

		for (i = 0; i < nargs; i++)
		{
			switch (arg [i])
			{
			    case 4:
				scr_insert_mode (mode);
				break;
			    case 36:
				PrivMode (mode, PrivMode_BackSpace);
				break;
			}
		}
		break;

	    case '?':
		for (i = 0; i < nargs; i++)
		{
			switch (arg [i])
			{
			    case 1:		/* application cursor keys */
				PrivCases (PrivMode_aplCUR);
				break;

			/*  case 2:	- reset charsets to USASCII */

			    case 3:		/* 80/132 */
				PrivCases (PrivMode_132);
				if (PrivateModes & PrivMode_132OK)
					set_width (state ? 132 : 80);
				break;

			/*  case 4:	- smooth scrolling */

			    case 5:		/* reverse video */
				PrivCases (PrivMode_rVideo);
				scr_rvideo_mode (state);
				break;

			    case 6:		/* relative/absolute origins  */
				PrivCases (PrivMode_relOrigin);
				scr_relative_origin (state);
				break;

			    case 7:		/* autowrap */
				PrivCases (PrivMode_Autowrap);
				scr_autowrap (state);
				break;

			/*  case 8:	- auto repeat, can't do on a per window basis */

			    case 9:		/* X10 mouse reporting */
				PrivCases (PrivMode_MouseX10);
				/* orthogonal */
				if (PrivateModes & PrivMode_MouseX10)
					PrivateModes &= ~(PrivMode_MouseX11);
				break;
#ifdef scrollBar_esc
			    case scrollBar_esc:
				PrivCases (PrivMode_scrollBar);
				map_scrollBar (state);
				break;
#endif
			    case 25:		/* visible/invisible cursor */
				PrivCases (PrivMode_VisibleCursor);
				scr_cursor_visible (state);
				break;

			    case 35:
				PrivCases (PrivMode_ShiftKeys);
				break;

			    case 36:
				PrivCases (PrivMode_BackSpace);
				break;

			    case 40:		/* 80 <--> 132 mode */
				PrivCases (PrivMode_132OK);
				break;

			    case 47:		/* secondary screen */
				PrivCases (PrivMode_Screen);
				scr_change_screen (state);
				break;

			    case 66:		/* application key pad */
				PrivCases (PrivMode_aplKP);
				break;

			    case 1000:		/* X11 mouse reporting */
				PrivCases (PrivMode_MouseX11);
				/* orthogonal */
				if (PrivateModes & PrivMode_MouseX11)
					PrivateModes &= ~(PrivMode_MouseX10);
				break;

#if (0)	/****************************************************/
			    case 1001:
				break;	/* X11 mouse highlighting */
#endif	/****************************************************/
			}
			break;
		}
	}

}	/* end handle_terminal_mode */

#undef PrivCases
#endif	/*******************************************************/
