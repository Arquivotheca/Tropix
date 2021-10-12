/*
 ****************************************************************
 *								*
 *			getch.c					*
 *								*
 *	Entrada de caracteres do teclado			*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 09.03.95				*
 *								*
 *	Módulo: getch						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>
#include <ctype.h>
#include <errno.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/acento.h"

/*
 ****************************************************************
 *	Tratamento de Acentuação				*
 ****************************************************************
 */
#define	ALARM

extern const	ACENTO	Acentos[];	/* Tabela de Acentuação */

/*
 ****************************************************************
 *	Fila de Caracteres Lidos				*
 ****************************************************************
 */
typedef	struct
{
	char	c_chr;		/* Caractere */
	char	c_space;	/* Espaço Ocupado */

}	CHAR;

#define	QUEUESZ	BLSZ			/* No. Máx. de Caracteres na Fila */
#define NOCHAR	(CHAR *)NULL

static CHAR	*queue;			/* Área para a Fila */
static CHAR	*qhp;			/* Header Pointer */
static CHAR	*qtp;			/* Tail Pointer */

/*
 *	Estado da Fila
 */
#define	empty()		(qtp == qhp)		/* Fila completamente Vazia */
#define	full()		qtp >= &queue[QUEUESZ-1]/* Fila Cheia ao Máximo */

/*
 *	Operações na fila
 */
#define	start()		qtp = qhp = &queue[0]	/* Inicializa a Fila */

#define	put(c,s)	qtp->c_chr = (c),	\
			(qtp++)->c_space = (s)	/* Põe um caractere na Fila */

#define	get()		(qhp++)->c_chr	/* Tira um '' no Início da Fila */

#define	remove()	qtp--		/* Tira um '' no Fim da Fila */

/*
 *	Informações sobre o último caractere na fila
 */
#define	space()		((qtp - 1)->c_space)
#define	chr()		((qtp - 1)->c_chr)

/*
 ****************************************************************
 *	Variáveis Locais					*
 ****************************************************************
 */
static INFO	*info;		/* Descrição do terminal */
static TERM	*term;		/* Terminal */
static WINDOW	*win;		/* Janela */
static int	flags;		/* Flags da Janela */
static TERMIO	*termio;	/* Modo de Comunicação do Terminal */
static FILE	*infp;		/* Arquivo de Entrada */
static bool	insertion;	/* Inserção de Caracteres */

/*
 ****************************************************************
 *	Constantes Locais					*
 ****************************************************************
 */
#define		XON		0x11		/* <^Q> */
#define		XOFF		0x13		/* <^S> */
#define		ESCAPE		0x16		/* <^V> */

#define		NOMATCH		0
#define		PARTIAL		1

/*
 ******	Protótipos de funções ***********************************
 */
static void	readln (void);
static chtype	readkey (void);
static chtype	match (chtype, int);
static int	readch (void);
static void	bclear (int);
static bool	isswitch (int);
static void	doswitch (bool);
static void	save (CHAR **);
static void	restore (CHAR **);
static void	retrocede (void);
static void	set_locals (WINDOW *);
static void	getalarm (int, ...);

/*
 ****************************************************************
 *	Leitura e echo de um caractere				*
 ****************************************************************
 */
int
wgetch (WINDOW *window)
{
	register int	ch;

	/*
	 *	Inicializações
	 */
	if (queue == NOCHAR && (queue = malloc (QUEUESZ * sizeof (CHAR))) == NOCHAR)
		return (ERR);

	set_locals (window);

	if (empty ())
	{
		/*
		 *	Atualização
		 */
		if (check (flags, R_INPUT))
			wrefresh (win);

		/*
		 *	Descarrega atualizações
		 */
		if (check (term->t_flags, T_FLUSH))
		{
			fflush (term->t_outfp);
			reset (term->t_flags, T_FLUSH);
		}
	
		/*
		 *	Paginação: reinicia contagem das linhas
		 */
		if (check (flags, A_PAGE))
			win->w_newlines = 0;
	}

	/*
	 *	Analisa a ação
	 */
	if (check (flags, G_KEYPAD)) /* Reconhecimento de teclas especiais */
	{
		ch = readkey ();
	}
	elif (checkall (flags, G_EDIT|G_ECHO))	/* Edição da entrada */
	{
		while (empty ())
			readln ();

		ch = get ();
	}
	elif (!empty ()) /* Ainda há caracteres resultantes de edição anterior */
	{
		ch = get ();
	}
	else	/* Leitura individual de caracteres: troca de contexto */
	{
		while (isswitch (ch = readch ()))
			doswitch (false);

		/*
		 *	Eco
		 */
		if (ch != ERR && check (flags, G_ECHO))
		{
			if (!insertion ||(ch == '\n' && check (flags, G_ADDNL)))
				waddch (win, ch);
			else
				winsch (win, ch);

			wrefresh (win);
		}
	}

	return (ch);

}	/* end wgetch */

/*
 ****************************************************************
 *	Leitura editada de uma linha				*
 ****************************************************************
 */
static void
readln (void)
{
	register int		ch;
	register char		lastl;	/* Linha na Janela do Último Caract. Teclado */
	register char		lastc;	/* Coluna  ...  */
	register bool		converte; /* Conversão para acentuados */
	register const ACENTO	*ap;
	char			savel, savec;
	char			sp;
	int			update;
	int			oldchar; /* Caractere de Acentuacao Pendente */
	int			och;
	int			orc;
	bool			endofline;
	bool			endoffile;

	/*
	 *	Inicializações
	 */
	ch = 0;
	converte = check (flags, G_TOISO);
	oldchar = 0;
	endofline = false;
	endoffile = false;

	/*
	 *	Inicia Fila
	 */
	start ();

	/*
	 *	Se inserção, abre um espaço
	 */
	if (insertion)
		{ winsch (win, ' '); retrocede (); wrefresh (win); }

	/*
	 *	Edição
	 */
	for (EVER)
	{
		ch = readch ();

		/*
		 *	Caractere de Escape
		 */
		if (ch == ESCAPE)
			{ ch = getc (infp); oldchar = 0; goto normal; }

		/*
		 *	Casos de terminação
		 */
		if   (ch == '\n')
		{
			break;
		}
		elif (ch == ERR) /* Entrada não bloqueada ou recepção de um sinal */
		{
			break;
		}
		elif (ch == termio->t_eol) /* Fim de linha alternativo */
		{
			endofline = true;
			break;
		}
		elif (ch == termio->t_eof) /* Fim de arquivo */
		{
			endoffile = true;
			break;
		}

		/*
		 *	Troca de contexto e Alterações na linha
		 */
		if (isswitch (ch))
		{
			/*
			 *	Troca de Contexto
			 */
			doswitch (true);
		}
		elif (ch == termio->t_erase || ch == termio->t_aerase)
		{
			/*
			 *	Cancelamento do Último Caractere
			 */
			if (empty ())
				{ beep (); continue; }

			bclear (space ());
			remove ();
			oldchar = 0;
		}
		elif (ch == termio->t_word)
		{
			/*
			 *	Cancelamento da Última Palavra
			 */
			if (empty ())
				{ beep (); continue; }

			if (isspace (chr ()))
			{
				while (!empty () && isspace (chr ()))
					{ bclear (space ()); remove (); }
			}
			else
			{
				while (!empty () && !isspace (chr ()))
					{ bclear (space ()); remove (); }
			}

			oldchar = 0;
		}
		elif (ch == termio->t_kill)
		{
			/*
			 *	Cancelamento da Última Linha
			 */
			if (empty ())
				{ beep (); continue; }

			while (!empty ())
				{ bclear (space ()); remove (); }

			oldchar = 0;
		}
		elif (ch == termio->t_retype)
		{
			/*
			 *	Reescreve o Vídeo
			 */
			set (term->t_flags, T_CLEAR);
			wrefresh (win);
		}
		else
		{
			/*
			 *	Caractere Normal
			 *
			 *	Tratamento de ACENTOS:
			 *
			 *	Verifica a Existência de um Acento Pendente
			 */
			if (oldchar)
			{
				och = oldchar; oldchar = 0;

				/*
				 *	Ignora a acentuação
				 */
				if (ch == '\\')
					goto getnextchar;

				orc = OR (och, ch); 

				for (ap = Acentos; ap->a_fonte; ap++)
				{
					if (ap->a_fonte == orc)
					{
						/*
						 *	Achou o acentuado:
						 *	Cancela o acento
						 *	Substitui pelo acentuado
						 *	
						 */
						bclear (space ());
						remove ();
						ch = (ushort)ap->a_destino;
						goto normal;
					}
				}

				/*
				 *	O acentuado não existe
				 */

				/*
				 *	Se acento, fica pendente
				 */
				if (isaccent (ch))
					oldchar = ch;

				goto normal;

			}	/* if (oldchar) */

			/*
			 *	Se acento, insere e fica pendente
			 */
			if (isaccent (ch) && converte)
				oldchar = ch;

			/*
			 *	Processamento NORMAL
			 *
			 *	Verifica se ainda cabe na Fila
			 */
		    normal:
			if (full())
			{
				/* ------ */
				beep ();
				continue;
			}

			/*
			 *	Ecoa Caractere
			 */
			savel = win->w_curl;
			savec = win->w_curc;

			if (insertion)
				update = winsch (win, ch);
			else
				update = waddch (win, ch);

			/*
			 *	Se não foi possível ecoar caractere
			 *	não o acrescenta na fila
			 */
			if (update == ERR)
				{ beep (); continue; }

			/*
			 *	Coloca Caractere na Fila
			 */
			lastl = savel;
			lastc = savec;

			if (win->w_curl == lastl && win->w_curc > lastc)
			{
				sp = win->w_curc - lastc;
			}
			else
			{
				sp = win->w_curc + win->w_maxc - lastc;

				if (check (flags, W_BOX))
					sp -= 2;
			}

			put (ch, sp);
		}

	    getnextchar:
		wrefresh (win);

	}	/* Edição */

	/*
	 *	Põe último caractere na fila
	 */
	if   (endoffile)
	{
		put (EOT, 0);
	}
	elif (endofline)
	{
		put (EOL, 0);
	}
	elif (ch == ERR)
	{
		put (ERR, 0);
	}
	else
	{
		put ('\n', 0);

		if (!insertion || check (flags, G_ADDNL))
			waddch (win, ch);
		else
			winsch (win, ch);

		wrefresh (win);
	}

	/*
	 *	Se inserção, remove o espaço
	 */
	if (insertion)
		wdelch (win);

}	/* end readln */

/*
 ****************************************************************
 *	Leitura de uma tecla especial				*
 ****************************************************************
 */
static chtype
readkey (void)
{
	register int	i, ch, key;
	register bool	alarmset;
	static	char	ahead[20];

	/*
	 *	Examina cada um dos próximos caracteres
	 *	enquanto houver reconhecimento parcial
	 */
	alarmset = false;

	for (i = 0; /* sem teste */; i++)
	{
		key = NOMATCH;

		/*
		 *	Prepara o próximo caractere na lista;
		 *	se for o final da lista lê um caractere
		 */
		if (ahead[i] == 0)
		{
#ifdef	ALARM
			/*
			 *	Garante contagem do tempo
			 *	após a leitura do primeiro caractere
			 */
			if (!alarmset && i > 0)
			{
				signal (SIGALRM, getalarm);
				alarm (1);
				alarmset = true;
			}
#endif	ALARM
			/*
			 *	Lê caractere
			 */
			if ((ch = readch ()) == ERR)
			{
				/*
				 *	Expiração do tempo limite
				 */
				alarmset = false;

				if (i == 0)
				{
					/*
					 *	Ainda aguardamos o primeiro caractere.
					 *	Reinicia leitura
					 */
					i = -1;
					continue;
				}
				else
				{
					/*
					 *	Reconhecimento parcial invalidado.
					 *	Encerra leitura
					 */
					break;
				}
			}
			else
			{
				/*
				 *	Caractere válido
				 */
				ahead[i] = ch;
				ahead[i+1] = 0;
			}
		}

		/*
		 *	Verifica reconhecimento deste caractere
		 */
		key = match (ahead[i], i);

		/*
		 *	Se há reconhecimento parcial
		 *	então examina o próximo caractere
		 */
		if (key != PARTIAL)
			break;
	}

	/*
	 *	Cancela contagem de tempo
	 */
	if (alarmset)
		{ alarm (0); alarmset = false; }

	/*
	 *	Se não há reconhecimento, retira da lista
	 *	o primeiro caractere, que será retornado
	 */
	if (key == NOMATCH)
	{
		key = ahead[0];
		strmove (&ahead[0], &ahead[1]);
	}
	else
	{
		strmove (&ahead[0], &ahead[i+1]);
	}

	return (key);
	
}	/* end readkey */

/*
 ****************************************************************
 *	Verifica a tecla especial				*
 ****************************************************************
 */
static chtype
match (chtype ch, int i)
{
	register chtype	key;
	register KEY	*kp;
	register KEY	*keys;
	register int	nkeys;

	keys = term->t_keys;
	nkeys = term->t_nkeys;

	/*
	 *	Iniciando novo match
	 */
	if (i == 0)
	{
		for (kp = &keys[nkeys-1]; kp >= &keys[0]; kp--)
			kp->k_partial = (kp->k_string != NOSTR);
	}

	/*
	 *	Verifica match para cada tecla especial
	 */
	key = NOMATCH;

	for (kp = &keys[nkeys-1]; kp >= &keys[0]; kp--)
	{
		if (kp->k_partial)
		{
			if (ch == kp->k_string[i])
			{
				if (kp->k_string[i+1] == 0)
				{
					kp->k_partial = false;
					key = kp->k_key;
					break;
				}
				else
				{
					key = PARTIAL;
				}
			}
			else
			{
				kp->k_partial = false;
			}
		}
	}

	return (key);

}	/* end match */

/*
 ****************************************************************
 *	Leitura de um caractere com tratamento de sinais	*
 ****************************************************************
 */
static int
readch (void)
{
	register int	ch;

	/*
	 *	Descarrega atualizações
	 */
	if (check (term->t_flags, T_FLUSH))
	{
		fflush (term->t_outfp);
		reset (term->t_flags, T_FLUSH);
	}
	
	/*
	 *	Leitura do caractere
	 */
    read_again:
	switch (ch = getc (infp))
	{
	    case EOF:
		if (errno == EINTR && !check (flags, G_SIGRET))
			{ errno = NOERR; clearerr (infp); goto read_again; }

		/*
		 *	Entrada Não Bloqueada ou Recepção de um Sinal
		 */
		ch = ERR;
		break;

	    case XON:
	    case XOFF:
		if (check (flags, G_IGXONXOFF) || info->i_booleans[b_xon])
			goto read_again;
		break;
	}

	/*
	 *	Mapeamento <cr> -> <nl>
	 */
	if (check (flags, G_TONL) && ch == '\r')
		ch = '\n';

	return (ch);

}	/* end readch */

/*
 ****************************************************************
 *	Limpa o espaço ocupado pelo último caractere		*
 ****************************************************************
 */
static void
bclear (int sp)
{
	register int	i;

	for (i = 0; i < sp; i++)
	{
		if (insertion)
		{
			retrocede ();
			wdelch (win);
		}
		else
		{
			retrocede ();
			waddch (win, ' ');
			retrocede ();
		}
	}

}	/* end bclear */

/*
 ****************************************************************
 *	Verifica a troca de contexto				*
 ****************************************************************
 */
static bool
isswitch (int ch)
{
	return
	(
		(ch == termio->t_swtch) &&
		(term->t_switch != SIG_IGN) &&
		(term->t_switch != SIG_DFL) &&
		(term->t_switch != SIG_ERR)
	);

}	/* end isswitch */

/*
 ****************************************************************
 *	Troca de Contexto					*
 ****************************************************************
 */
static void
doswitch (bool savequeue)
{
	register WINDOW	*w;
	CHAR		*q;
	register int	l, c;

	/*
	 *	Salva o contexto
	 */
	w = win;
	l = w->w_curl;
	c = w->w_curc;

	if (savequeue)
		save (&q);

	/*
	 *	Chama a função do usuário
	 */
	(*term->t_switch) (0, 0);

	/*
	 *	Restaura o contexto
	 */
	if (savequeue)
		restore (&q);

	w->w_curc = c;
	w->w_curl = l;
	set_locals (w);

}	/* end doswitch */

/*
 ****************************************************************
 *	Salva o contexto					*
 ****************************************************************
 */
static void
save (CHAR **wqueue)
{
	int		size;

	put ('\0', 1);

	size = (qtp - queue) * sizeof (CHAR);

	if ((*wqueue = (CHAR *)malloc (size)) == (CHAR *)0)
		return;

	memmove (*wqueue, queue, size);

	start ();

}	/* end save */

/*
 ****************************************************************
 *	Restaura o contexto					*
 ****************************************************************
 */
static void
restore (CHAR **wqueue)
{
	register CHAR	*c;

	if (*wqueue == (CHAR *)0)
		return;

	start ();

	for (c = *wqueue; c->c_chr != '\0'; c++)
		put (c->c_chr, c->c_space);

	free (*wqueue);

	*wqueue = (CHAR *)0;

}	/* end restore */

/*
 ****************************************************************
 *	Retrocede uma posição					*
 ****************************************************************
 */
static void
retrocede (void)
{
	register int	b;

	b = check (flags, W_BOX) ? 1 : 0;

	if (win->w_curc > b)
	{
		win->w_curc--;
	}
	elif (win->w_curl > b)
	{
		win->w_curc = win->w_maxc - b;
		win->w_curl--;
	}

}	/* end retrocede */

/*
 ****************************************************************
 *	Inicializações						*
 ****************************************************************
 */
static void
set_locals (WINDOW *window)
{
	win = window;
	term = (TERM *)win->w_term;
	info = &(term->t_info);
	termio = &(term->t_termio);
	infp = term->t_infp;
	flags = win->w_flags;
	insertion = checkall (flags, G_INSERT|G_ECHO);

	/*
	 *	Leitura não bloqueada
	 */
	if (check (flags, G_EDIT))
	{
		if (check (term->t_oflags, O_NDELAY))
		{
			reset (term->t_oflags, O_NDELAY);
			fcntl (fileno (infp), F_SETFL, term->t_oflags);
		}
	}
	else
	{
		if (check (flags, G_NODELAY) != check (term->t_oflags,O_NDELAY))
		{
			if (check (flags, G_NODELAY))
				set (term->t_oflags, O_NDELAY);
			else
				reset (term->t_oflags, O_NDELAY);

			fcntl (fileno (infp), F_SETFL, term->t_oflags);
		}
	}

}	/* end set_locals */

/*
 ****************************************************************
 *	Recebe um alarme e não faz nada				*
 ****************************************************************
 */
static void
getalarm (int signo, ...)
{
	/* vazio */

}	/* end getalarm */
