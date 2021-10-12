/*
 ****************************************************************
 *								*
 *			refresh.c				*
 *								*
 *	Atualiza o conteúdo de uma janela no terminal		*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 10.03.95				*
 *								*
 *	Módulo: refresh						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/local.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define		OTIM

/*
 ******	Contadores **********************************************
 */
static int	nrefreshs = 0;
static int	nwatchs = 0;
static int	nupdates = 0;
static int	ndomoves = 0;
static int	nparmexecs = 0;

/*
 ******	Definições locais de macros *****************************
 */
#define		TAB(x)	(((x) & (~7)) + 8)
#define		TABD(x)	(((x) & (~7)) + 16)
#define		TABT(x)	(((x) & (~7)) + 24)
#define		TABQ(x)	(((x) & (~7)) + 32)

#define		sattr(y,x)	(screen[y][x] & A_ATTRIBUTES)
#define		stext(y,x)	(screen[y][x] & A_CHARTEXT)

#define		DEL	0x7F	/* Caractere ISO <del> */

/*
 ******	Variáveis locais ****************************************
 */
static WINDOW	*win;		/* Janela sendo Atualizada */
static TERM	*term;		/* Terminal da Janela */
static INFO	*info;		/* Informações sobre o Terminal */
static FILE	*outfp;		/* Arquivo de saída para o terminal */
static chtype	**screen;	/* Tela do Terminal */
static int	wflags;		/* win->w_flags */

static int	lines,		/* Tamanho do Vídeo */
		cols;

static char	w;
static int	curx,		/* Posição corrente do cursor */
		cury;

static bool	clean;		/* A linha em atualização está limpa no vídeo */
static chtype	*wline;		/* Área para linha realizada */

static char	*cr,		/* Movimentos rápidos para o cursor */
		*cud1,
		*cub1,
		*cup,
		*ht;

/*
 ******	Protótipos de funções ***********************************
 */
static bool	inswindow (void);
static bool	delwindow (void);
static void	update (int, int, int);
static bool	begining (int, int);
static void	ending (int, int);
static int	realize (int);
static void	movecur (int, int);
static void	clearscreen (void);
static void	delscreen (int, int, int);
static void	insscreen (int, int, int);
static int	coln (chtype *, int);
static void	retrocede (int);
static bool	cursor (int *, int *);

/*
 ****************************************************************
 *	Atualiza o conteúdo de uma janela no terminal		*
 ****************************************************************
 */
void
wrefresh (register WINDOW *wp)
{
	register int	l;
	register int	sy;
	bool		invisible;
	int		y, x;

nrefreshs++;

	if (win != wp)
	{
		win = wp; w = wp - _windows;

		if (check (wp->w_flags, A_REAL))
		{
			if (wline)
				free (wline);

			wline = malloc ((wp->w_maxc + 1) * sizeof (chtype));
		}
	}

	if (term != (TERM *)(wp->w_term))
	{
		term = (TERM *)(wp->w_term);
		info = &term->t_info;

		lines = term->t_lines;
		cols = term->t_cols;
		outfp = term->t_outfp;
		screen = term->t_screen;

		curx = term->t_curx;
		cury = term->t_cury;

		cr = info->i_strings[s_cr];
		cub1 = info->i_strings[s_cub1];
		cud1 = info->i_strings[s_cud1];
		cup = info->i_strings[s_cup];
		ht = info->i_strings[s_ht];
	}

	wflags = wp->w_flags;
	clean = false;
	wp->w_newlines = 0;

	/*
	 *	Cursor no vídeo
	 */
	if (check (wflags, R_CURSOR))
	{
		/*
		 *	Se programada, chama função do usuário e retorna
		 */
		if (!cursor (&y, &x))
		{
			if
			(	check (wflags, R_MOVE)  &&
				wp->w_move != (void (*) (int, int))SIG_IGN &&
				wp->w_move != (void (*) (int, int))SIG_DFL &&
				wp->w_move != (void (*) (int, int))SIG_ERR
			)
			{
				(*wp->w_move) (y - wp->w_miny, x - wp->w_minx);

				if (cursor (&y, &x))
					movecur (y, x);

				return;
			}
		}
	}

	/*
	 *	Verifica se a janela aparece no vídeo
	 */
	if (wp->w_minx + wp->w_maxc < 0 || wp->w_minx >= cols)
		return;

	/*
	 *	Limpa o vídeo se necessário
	 */
	if (check (term->t_flags, T_CLEAR) || check (wflags, R_CLEAR))
	{
		fputs (info->i_strings[s_clear], outfp);
		clearscreen ();

		reset (term->t_flags, T_CLEAR);
		reset (wp->w_flags, R_CLEAR);
	   /***	reset (wflags, R_CLEAR); ***/

		/*
		 *	Atualiza todo o vídeo
		 */
		for (wp = term->t_last;	wp != NOWINDOW;	wp = wp->w_queue)
		{
			for (l = wp->w_maxl; l >= 0; l--)
				wp->w_changed[l] = 1;

			wrefresh (wp);
		}
		return;
	}

	/*
	 *	Cursor Invisível
	 */
	if
	(	check (wflags, R_INVIS) &&
		info->i_strings[s_civis] &&
		info->i_strings[s_cnorm]
	)
	{
		fputs (info->i_strings[s_civis], outfp);
		invisible = true;
	}
	else
	{
		invisible = false;
	}

	/*
	 *	Força o posicionamento do cursor
	 */
	if (check (wflags, R_FORCE))
		fputs (parmexec (cup, cury, curx), outfp);

	/*
	 *	Observação da Entrada
	 */
	if (check (wflags, R_WATCH))
	{
		/*
		 *	Se o usuário teclou algo,
		 *	atualiza apenas a linha corrente
		 */
		if (remaining (term->t_infp))
		{
			if (check (wflags, R_CURSOR))
			{
				l = wp->w_curl;
				sy = wp->w_miny + l;

				if (sy >= 0 && sy < lines)
					update (l, sy, sy);

				if (wp->w_dispcount == 0)
					wp->w_changed[l] = 0;
			}
nwatchs++;
			goto done;
		}
	}

	/*
	 *	Deslocamento de linhas
	 */
	if (checkall (wflags, W_WIDE|W_TOP) && wp->w_dispcount != 0)
	{
		if
		(	wp->w_miny + wp->w_displine < lines - 1 &&
			wp->w_miny + wp->w_maxl > 0 &&
			wp->w_displine + wp->w_dispcount < wp->w_maxl &&
			wp->w_displine - wp->w_dispcount < wp->w_maxl
		)
		{
			register bool	s;

			if (wp->w_dispcount > 0)
				s = inswindow ();
			else
				s = delwindow ();

			if (s)
				wp->w_dispcount = 0;
		}
	}

	/*
	 *	Atualização das linhas alteradas
	 */
	for (l = 0, sy = wp->w_miny; l <= wp->w_maxl; l++, sy++)
	{
		if
		(	wp->w_changed[l] ||
			(wp->w_dispcount && l >= wp->w_displine)
		)
		{
			if (sy >= 0 && sy < lines)
				update (l, sy, sy);

			wp->w_changed[l] = 0;
		}
	}

	wp->w_dispcount = 0;


	/*
	 *	Se necessário, Posiciona o Cursor
	 */
    done:
	if (check (wflags, R_CURSOR))
	{
		if (y >= 0 && y < lines && x >= 0 && x < cols)
			movecur (y, x);
	}


	/*
	 *	Cursor Invisível: reestabelece
	 */
	if (invisible)
		fputs (info->i_strings[s_cnorm], outfp);

	/*
	 *	Descarrega o Buffer de Saída
	 */
	if (check (wflags, R_HOLD))
		set (term->t_flags, T_FLUSH);
	else
		fflush (outfp);

	/*
	 *	Restaura a informação sobre a posição do cursor
	 */
	term->t_curx = curx;
	term->t_cury = cury;

}	/* end wrefresh */

/*
 ****************************************************************
 *	Inserção de Linhas					*
 ****************************************************************
 */
static bool
inswindow (void)
{
	register int	l;
	register int	sy;
	int		ty;

	register int	start;	/* Linha da Janela onde correu a inserção */
	int		count;	/* Contagem */
	bool		scrolling;

	register int	miny;	/* win->w_miny */
	int		maxl;

	char		*inscmd;
	int		insy = 0;

	char		*delcmd;
	int		dely = 0;

	/*
	 *	Inicializações
	 */
	if (check (win->w_flags, W_BOX))
		maxl = win->w_maxl - 1;
	else
		maxl = win->w_maxl;

	miny = win->w_miny;
	start = win->w_displine;
	count = win->w_dispcount;
	scrolling = false;

#ifdef	DEBUG
	fprintf (stderr, "inswindow (%d, %d)\n", start, count);
#endif	DEBUG

	/*
	 *	Decide se haverá um rolamento
	 */
	if (miny + start > 0 || miny + maxl < lines - 1)
	{
		scrolling = (info->i_strings[s_csr] != NOSTR) &&
				(info->i_strings[s_ri] != NOSTR);
	}
	else
	{
		scrolling = (info->i_strings[s_ri] != NOSTR);
	}

	/*
	 *	Examina os parâmetros
	 */
	if (scrolling)
	{
		/*
		 *	Parâmetros para o rolamento
		 */
		inscmd = info->i_strings[s_ri];
		insy = miny + start;

		if (insy < 0)
			insy = 0;

		delcmd = NOSTR;
		dely = miny + maxl;

		if (dely >= lines)
			dely = lines - 1;
	}
	else
	{
		/*
		 *	Parâmetros para inserção e remoção de linhas
		 */
		inscmd = NOSTR;

		if (miny + start <= 0)
		{
			inscmd = info->i_strings[s_ri];
			insy = 0;
		}

		if (inscmd == NOSTR)
		{
			inscmd = info->i_strings[s_il1];
			insy = miny + start;

			if (insy < 0)
				insy = 0;
		}

		if (inscmd == NOSTR)
			return (false);

		/*
		 *	Calcula delcmd e dely
		 */
		if (miny + maxl >= lines - 1)
		{
			delcmd = NOSTR;
		}
		elif ((delcmd = info->i_strings[s_dl1]) != NOSTR)
		{
			dely = miny + maxl;
		}
		else
		{
			return (false);
		}
	}


	/*
	 *	Altera a representação do vídeo
	 */
	insscreen (miny + start, count, miny + maxl);

	/*
	 *	Altera o Vídeo
	 */
	ty = miny + start;

	if (ty < 0)
		ty = 0;

	if (scrolling && (insy != 0 || dely != lines - 1))
	{
		fputs (parmexec (info->i_strings[s_csr], insy, dely), outfp);
		cury = 0;
		curx = 0;
#ifdef	DEBUG
		fprintf (stderr, "início: csr: %d, %d\n", insy, dely);
#endif	DEBUG
	}

	for
	(	l = start + count - 1, sy = miny + l;
		l >= start;
		l--, sy--
	)
	{
		if (sy < 0 || sy >= lines)
			continue;

		if (delcmd != NOSTR)
		{
			if (cury != dely)
				movecur (dely, 0);

			fputs (delcmd, outfp);
#ifdef	DEBUG
			fprintf (stderr, "del1 em %d\n", dely);
#endif	DEBUG
		}

		if (inscmd != NOSTR)
		{
			if (cury != insy)
				movecur (insy, 0);

			fputs (inscmd, outfp);
#ifdef	DEBUG
			fprintf (stderr, "ins1 em %d\n", insy);
#endif	DEBUG
		}

		clean = true;
		update (l, sy, ty);
		win->w_changed[l] = 0;
	}

	if (scrolling && (insy != 0 || dely != lines - 1))
	{
		fputs (parmexec (info->i_strings[s_csr], 0, lines - 1), outfp);
		cury = 0;
		curx = 0;
#ifdef	DEBUG
		fprintf (stderr, "final: csr: %d, %d\n", insy, dely);
#endif	DEBUG
	}

	return (true);

}	/* end inswindow */

/*
 ****************************************************************
 *	Remoção de linhas					*
 ****************************************************************
 */
static bool
delwindow (void)
{
	register int	l;
	register int	sy;
	int		ty;

	register int	start;
	int		count;
	bool		scrolling;

	register int	miny;	/* win->w_miny */
	int		maxl;

	char		*inscmd;
	int		insy;

	char		*delcmd;
	int		dely = 0;

	/*
	 *	Inicializações
	 */
	if (check (win->w_flags, W_BOX))
		maxl = win->w_maxl - 1;
	else
		maxl = win->w_maxl;

	miny = win->w_miny;
	start = win->w_displine;
	count = - win->w_dispcount;
	scrolling = false;

#ifdef	DEBUG
	fprintf
	(	stderr,
		"delwindow (%d, %d): Janela [%d, %d]\n",
		start, count, win->w_miny, win->w_maxl
	);
#endif	DEBUG

	/*
	 *	Decide se haverá um rolamento
	 */
	if (miny + start > 0 || miny + maxl < lines - 1)
	{
		scrolling = (info->i_strings[s_csr] != NOSTR) &&
				(info->i_strings[s_ind] != NOSTR);
	}
	else
	{
		scrolling = (info->i_strings[s_ind] != NOSTR);
	}

	/*
	 *	Analisa os parâmetros
	 */
	if (scrolling)
	{
		/*
		 *	Parâmetros para o rolamento
		 */
		delcmd = info->i_strings[s_ind];
		dely = miny + maxl;

		if (dely >= lines)
			dely = lines - 1;

		inscmd = NOSTR;
		insy = miny + start;

		if (insy < 0)
			insy = 0;
	}
	else
	{
		/*
		 *	Parâmetros para inserção e remoção de linhas
		 */
		delcmd = NOSTR;

		if (miny + start <= 0)
		{
			delcmd = info->i_strings[s_ind];
			dely = lines - 1;
		}

		if (delcmd == NOSTR)
		{
			delcmd = info->i_strings[s_dl1];
			dely = miny + start;

			if (dely < 0)
				dely = 0;
		}

		if (delcmd == NOSTR)
			return (false);

		insy = miny + maxl;

		if (insy >= lines - 1)
		{
			inscmd = NOSTR;
		}
		elif ((inscmd = info->i_strings[s_il1]) == NOSTR)
		{
			return (false);
		}
	}

	/*
	 *	Altera a representação do Vídeo
	 */
	delscreen (miny + start, count, miny + maxl);

	/*
	 *
	 */
	ty = miny + maxl;

	if (ty >= lines)
		ty = lines - 1;

	if (scrolling && (insy != 0 || dely != lines - 1))
	{
		fputs (parmexec (info->i_strings[s_csr], insy, dely), outfp);
		cury = 0;
		curx = 0;
#ifdef	DEBUG
		fprintf (stderr, "início: csr: %d, %d\n", insy, dely);
#endif	DEBUG
	}

	for
	(	l = maxl - count + 1, sy = miny + l;
		l <= maxl;
		l++, sy++
	)
	{
		if (sy < 0 || sy >= lines)
			continue;

		if (delcmd != NOSTR)
		{
			if (cury != dely)
				movecur (dely, 0);
			fputs (delcmd, outfp);
#ifdef	DEBUG
			fprintf (stderr, "del1 em %d\n", dely);
#endif	DEBUG
		}

		if (inscmd != NOSTR)
		{
			if (cury != insy)
				movecur (insy, 0);
			fputs (inscmd, outfp);
#ifdef	DEBUG
			fprintf (stderr, "ins1 em %d\n", insy);
#endif	DEBUG
		}

		clean = true;
		update (l, sy, ty);
		win->w_changed[l] = 0;
	}

	if (scrolling && (insy != 0 || dely != lines - 1))
	{
		fputs (parmexec (info->i_strings[s_csr], 0, lines - 1), outfp);
		cury = 0;
		curx = 0;
#ifdef	DEBUG
		fprintf (stderr, "final: csr: %d, %d\n", insy, dely);
#endif	DEBUG
	}
	return (true);

}	/* end delwindow */

/*
 ****************************************************************
 *	Atualiza uma linha da janela				*
 ****************************************************************
 */
static void
update (int l /* janela */, int sy /* screen */, int ty /* video */)
{
	register int	x;
	register int	maxx;
	register char	*map;
	register chtype	*onscreen;
	register chtype	*onwin;
	register int	ch;
	register FILE	*fp;
	int		lastcol;
	chtype		*line;

chtype	nattr, oattr;

nupdates++;

	fp = outfp;

	/*
	 *	Modo Real
	 */
	if (check (wflags, A_REAL))
	{
		lastcol = realize (l); line = wline;

		if (clean)
		{
			if (lastcol < 0)
				return;
		}
		else
		{
			lastcol = win->w_maxc;
		}
	}
	else
	{
		line = win->w_text[l];
		lastcol = win->w_maxc;
	}

	clean = false;

	/*
	 *	Inicializações
	 */
	if (win->w_minx <= 0)
	{
		map = &term->t_map[sy][0];
		onscreen = &screen[sy][0];
		onwin = &line[-win->w_minx];
		x = 0;
	}
	else
	{
		map = &term->t_map[sy][win->w_minx];
		onscreen = &screen[sy][win->w_minx];
		onwin = &line[0];
		x = win->w_minx;
	}

	if ((maxx = win->w_minx + lastcol) >= cols)
		maxx = cols - 1;

	/*
	 *	Não é possível escrever nas últimas linha e coluna
	 *	em terminais com margem automática
	 */
	if (maxx == cols - 1 && ty == lines - 1 && info->i_booleans[b_am])
		maxx--;

	/*
	 *	Atualização da Linha: enquanto x <= maxx
	 */
	for (EVER)
	{
		/*
		 *	Pula trecho em que a atualização não é necessária
		 */
		while (*onscreen == *onwin || *map != w)
		{
			map++;
			onwin++;
			onscreen++;

			if (++x > maxx)
				goto done;
		}

		oattr = (*onscreen) & A_ATTRIBUTES;
		nattr = (*onwin) & A_ATTRIBUTES;

		/*
		 *	Posiciona o cursor
		 */
		if (x == curx + 1 && ty == cury)
		{
			ch = *(onscreen - 1);

			if ((ch & A_ATTRIBUTES) == A_NORMAL)
			{
			   /***	ch = ch & A_CHARTEXT; ***/
				putc (ch, fp);
			   /***	curx = x; ***/
			}
			else
			{
				movecur (ty, x);
			}
		}
		else
		{
			movecur (ty, x);
		}

		/*
		 *	Trecho atualizando: atributos nulos
		 */
		if (nattr == A_NORMAL && oattr == A_NORMAL)
		{
			/*
			 *	Escreve caracteres
			 */
			do
			{
				*onscreen++ = ch = *onwin++;
				putc (ch, fp);
				map++;

				if (++x > maxx)
					goto done_check;
			}
			while
			(	(*onscreen & A_ATTRIBUTES) == A_NORMAL
				&& (*onwin & A_ATTRIBUTES) == A_NORMAL
				&& *onscreen != *onwin
				&& *map == w
			);

			/*
			 *	Atualiza a posição do cursor
			 */
			curx = x;
		}
		else
		{
			/*
			 *	Trecho atualizando: alterando atributos
			 */
			bool	repeat = true;

			/*
			 *	Escreve caracteres com atributos
			 */
			do
			{
				if (repeat)
					repeat = begining (nattr, oattr);

				ch = (*onwin) & A_CHARTEXT;
				putc (ch, fp);
				*onscreen++ = *onwin++;
				map++;

				if ((curx = ++x) > maxx)
					goto done_check;
			}
			while
			(	(*onscreen & A_ATTRIBUTES) == oattr
				&& (*onwin & A_ATTRIBUTES) == nattr
				&& *onscreen != *onwin
				&& *map == w
			);

			/*
			 *	Termina atributo
			 */
			ending (nattr, oattr);
		}
	}

	/*
	 *	Fim da atualização da linha
	 *
	 *	Verifica se o cursor passou
	 *	para a próxima linha do vídeo
	 */
    done_check:
	if (x == cols)
	{
		if (info->i_booleans[b_am])
		{
			curx = 0;
			cury++;
		}
		else
		{
			curx = cols - 1;
		}
	}
	else
	{
		curx = x;
	}

    done:
	/* vazio */;

}	/* end update */

/*
 ****************************************************************
 *	Inicia um campo com atributo				*
 ****************************************************************
 */
static bool
begining (int attr, int oattr)
{
	switch (attr)
	{
	    case A_BLINK:
		if (info->i_strings[s_blink] != NOSTR)
		{
			retrocede (info->i_numbers[n_xmc]);
			fputs (info->i_strings[s_blink], outfp);
		}
		return (false);

	    case A_BOLD:
		if (info->i_strings[s_bold] != NOSTR)
		{
			retrocede (info->i_numbers[n_xmc]);
			fputs (info->i_strings[s_bold], outfp);
		}
		return (false);

	    case A_UNDERLINE:
		if (info->i_strings[s_smul] != NOSTR)
		{
			retrocede (info->i_numbers[n_xmc]);
			fputs (info->i_strings[s_smul], outfp);
			return (false);
		}
		elif (info->i_strings[s_uc] != NOSTR)
		{
			fputs (info->i_strings[s_uc], outfp);
			return (true);
		}

	    case A_REVERSE:
		if (info->i_strings[s_rev] != NOSTR)
		{
			retrocede (info->i_numbers[n_xmc]);
			fputs (info->i_strings[s_rev], outfp);
		}
		return (false);

	    case A_ALTCHARSET:
		if (info->i_strings[s_smacs] && info->i_strings[s_rmacs])
		{
			fputs (info->i_strings[s_smacs], outfp);
			return (false);
		}
		elif (info->i_strings[s_ac] != NOSTR)
		{
			fputs (info->i_strings[s_ac], outfp);
			return (true);
		}

	    case A_STANDOUT:
		if (info->i_strings[s_smso] && info->i_strings[s_rmso])
			fputs (info->i_strings[s_smso], outfp);
		return (false);

	    case A_NORMAL:
		if (oattr != A_ALTCHARSET && info->i_strings[s_sgr0])
		{
			retrocede (info->i_numbers[n_xmc]);
			fputs (info->i_strings[s_sgr0], outfp);
		}
		return (false);
	}

	return (false);		/* ? */

}	/* end begining */

/*
 ****************************************************************
 *	Termina um campo com atributo				*
 ****************************************************************
 */
static void
ending (int attr, int oattr)
{
	/*
	 *	Envia comando
	 */
	switch (attr)
	{
	    case A_NORMAL:
		break;

	    case A_STANDOUT:
		if (info->i_strings[s_smso] && info->i_strings[s_rmso])
			fputs (info->i_strings[s_rmso], outfp);
		break;

	    case A_ALTCHARSET:
		if (info->i_strings[s_smacs] && info->i_strings[s_rmacs])
			fputs (info->i_strings[s_rmacs], outfp);
		break;

	    default:
		fputs (info->i_strings[s_sgr0], outfp);
		curx += info->i_numbers[n_xmc];
		break;
	}

}	/* end ending */

/*
 ****************************************************************
 *	Modo real						*
 ****************************************************************
 */
static int
realize (int l)
{
	register chtype	*wp, *rp;
	register chtype	*last;
	register int	chtext;
	register int	chattr;
	register int	b;
	chtype		*rline;

	rline = win->w_text[l];
	last = wline + win->w_maxc;
	rp = rline;
	wp = wline;
	b = check (wflags, W_BOX) ? 1 : 0;

	while (wp <= last && *rp)
	{
		chtext = *rp & A_CHARTEXT;

		if (isgraph (chtext) || chtext == ' ')
		{
			/*
			 *	Caso mais comum
			 */
			*wp++ = *rp++;
		}
		elif (chtext == '\t')
		{
			/*
			 *	Tabulação
			 */
			register int	c;
			register int	t;

			chattr = *rp & A_ATTRIBUTES;
			t = ((wp - wline + b) & (~7)) + 8 + b;

			if (wline + t <= last)
			{
				for (c = wp - wline; c < t; c++)
					*wp++ = ' ' | chattr;
			}
			else
			{
				while (wp <= last)
					*wp++ = ' ' | chattr;
			}
			rp++;
		}
		else
		{
			/*
			 *	Caracteres de Controle
			 */
			chattr = *rp & A_ATTRIBUTES;
			*wp++ = '^' | chattr;

			if (wp <= last)
			{
				*wp = (chtext == DEL) ?  '?' : chtext + '@';
				*wp++ |= chattr;
			}
			rp++;
		}
	}

	/*
	 *	Restaura Contorno
	 */
	if (check (wflags, W_BOX))
	{
		/*
		 *	Preenche até o contorno
		 */
		while (wp < last)
			*wp++ = ' ';

		/*
		 *	Redesenha o contorno
		 */
		if   (l == win->w_maxl)
			*wp++ = ACS_LRCORNER;
		elif (l == 0)
			*wp++ = ACS_URCORNER;
		else
			*wp++ = ACS_VLINE;
	}

	/*
	 *	Retorna a última coluna diferente de espaço
	 */
	if (!clean)
	{
		while (wp <= last)
			*wp++ = ' ';
	}

	return (wp - wline - 1);

}	/* end realize */

/*
 ****************************************************************
 *	Escolhe e executa movimento para o Cursor		*
 ****************************************************************
 */
static void
movecur (int y, register int x)
{
	register char	*movecmd;
	register int	c;
	register FILE	*fp;
	char		string[25];

	if (curx == x && cury == y)
		return;

ndomoves++;

	movecmd = string;

#ifdef	OTIM
	/*
	 *	Verifica opções mais rápidas
	 */
	if (y == cury)
	{
		if
		(	x == curx + 1 &&
			sattr (cury, curx) == A_NORMAL
		)
		{
			movecmd[0] = stext (cury, curx);
			movecmd[1] = '\0';
		}
		elif (((x & 7) == 0) && ht)
		{
			if (x == TAB (curx))
			{
				strcpy (movecmd, ht);
			}
			elif (x == TABD (curx))
			{
				strcpy (movecmd, ht);
				strcat (movecmd, ht);
			}
			elif (x == TABT (curx))
			{
				strcpy (movecmd, ht);
				strcat (movecmd, ht);
				strcat (movecmd, ht);
			}
			elif (x == TABQ (curx))
			{
				strcpy (movecmd, ht);
				strcat (movecmd, ht);
				strcat (movecmd, ht);
				strcat (movecmd, ht);
			}
			else
			{
				movecmd = NOSTR;
			}
		}
		elif
		(	x == curx + 2 &&
			sattr (cury, curx) == A_NORMAL &&
			sattr (cury, curx + 1) == A_NORMAL
		)
		{
			movecmd[0] = stext (cury, curx);
			movecmd[1] = stext (cury, curx + 1);
			movecmd[2] = '\0';
		}
		elif (x == curx - 1)
		{
			movecmd = cub1;
		}
		elif (x == 0)
		{
			movecmd = cr;
		}
		else
		{
			movecmd = NOSTR;
		}
	}
	elif (y == cury + 1 && cud1)
	{
		if   (x == curx)
		{
			movecmd = cud1;
		}
		elif ((x & 7) == 0 && cr)
		{
			if (x == 0)
			{
				strcpy (movecmd, cud1);
				strcat (movecmd, cr);
			}
			elif ((x == 8) && ht)
			{
				strcpy (movecmd, cud1);
				strcat (movecmd, cr);
				strcat (movecmd, ht);
			}
			elif ((x == 16) && ht)
			{
				strcpy (movecmd, cud1);
				strcat (movecmd, cr);
				strcat (movecmd, ht);
				strcat (movecmd, ht);
			}
			else
			{
				movecmd = NOSTR;
			}
		}
		elif (x == curx - 1 && cub1)
		{
			strcpy (movecmd, cud1);
			strcat (movecmd, cub1);
		}
		elif
		(	x == 1 &&
			sattr (y, 0) == A_NORMAL
		)
		{
			movecmd[0] = '\r';
			movecmd[1] = '\n';
			movecmd[2] = stext (y, 0);
			movecmd[3] = '\0';
		}
		else
		{
			movecmd = NOSTR;
		}
	}
	elif (y == 0 && x == 0)
	{
		movecmd = info->i_strings[s_home];
	}
	else
	{
		movecmd = NOSTR;
	}

#else	OTIM
	movecmd = NOSTR;
#endif	OTIM

	if (movecmd == NOSTR)
	{
		movecmd = parmexec (cup, y, x);
nparmexecs++;
	}

	fp = outfp;

	while ((c = *movecmd++) != '\0')
		putc (c, fp);

	/*
	 *	Atualiza posição interna do Cursor 
	 */
	cury = y;
	curx = x;

}	/* end movecur */

/*
 ****************************************************************
 *	Limpa a Representação do Vídeo				*
 ****************************************************************
 */
static void
clearscreen (void)
{
	register int	l;
	register chtype	**scr;

	scr = screen;

	for (l = lines - 1; l >= 0; l--)
		memsetw ((short *)scr[l], ' ', cols);

	cury = 0;
	curx = 0;

}	/* end clearscreen */

/*
 ****************************************************************
 *	Remove linhas em Screen					*
 ****************************************************************
 */
static void
delscreen (int start, register int count, int End)
{
	register int	l;
	register chtype	**scr;
	register chtype	**s;
	register int	round;
	register int	Lines;

#ifdef	DEBUG
	fprintf (stderr, "delscreen (%d, %d, %d)\n", start, count, End);
#endif	DEBUG

	End += 1;
	scr = screen;
	s = malloc (lines * (sizeof (chtype *)));

	/*
	 *	Copia Screen
	 */
	for (l = lines - 1; l >= 0; l--)
		s[l] = scr[l];

	Lines = End - count;
	round = (End - start) - count;

	/*
	 *	Rotação
	 */
	for (l = Lines; l < End; l++)
		scr[l] = s[l - round];

	/*
	 *	Deslocamento
	 */
	for (l = start; l < Lines; l++)
		scr[l] = s[l + count];

	/*
	 *	Novas linhas
	 */
	for (l = Lines; l < End; l++)
		memsetw ((short *)scr[l], ' ', cols);

	free (s);

}	/* end delscreen */

/*
 ****************************************************************
 *	Insere linhas em Screen					*
 ****************************************************************
 */
static void
insscreen (int start, register int count, int End)
{
	register int	l;
	register chtype	**scr;
	register chtype	**s;
	register int	round;

#ifdef	DEBUG
	fprintf (stderr, "insscreen (%d, %d, %d)\n", start, count, End);
#endif	DEBUG

	End += 1;
	scr = screen;
	s = malloc (lines * (sizeof (chtype *)));

	/*
	 *	Copia Screen
	 */
	for (l = lines - 1; l >= 0; l--)
		s[l] = scr[l];

	round = (End - start) - count;

	/*
	 *	Deslocamento
	 */
	for (l = start + count; l < End; l++)
		scr[l] = s[l - count];

	/*
	 *	Rotação
	 */
	for (l = start + count - 1; l >= start; l--)
		scr[l] = s[l + round];

	/*
	 *	Novas linhas
	 */
	for (l = start + count - 1; l >= start; l--)
		memsetw ((short *)scr[l], ' ', cols);

	free (s);

}	/* end insscreen */

/*
 ****************************************************************
 *	Calcula a coluna para o modo Real			*
 ****************************************************************
 */
static int
coln (chtype *rline, int order)
{
	register int	i;
	register int	chtext;
	register chtype	*linep, *endp;

	i = 0;

	for (linep = rline, endp = linep + order; linep <= endp; linep++)
	{
		if (*linep == 0)
			return (i);

		chtext = *linep & A_CHARTEXT;

		if   (isgraph (chtext) || chtext == ' ')
			i++;
		elif (chtext == '\t')
			i = (i & ~7) + 8;
		else
			i += 2;
	}

	return (i - 1);

}	/* end coln */

/*
 ****************************************************************
 *	Retrocede o Cursor					*
 ****************************************************************
 */
static	void
retrocede (register int n)
{
	while (n-- > 0)
		fputs (info->i_strings[s_cub1], outfp);

}	/* end retrocede */

/*
 ****************************************************************
 *	Calcula coordenadas para o cursor no vídeo		*
 ****************************************************************
 */
static bool
cursor (int *yp, int *xp)
{
	int	y, x;

	y = win->w_curl;

	if (check (wflags, A_REAL))
		x = coln (win->w_text[y], win->w_curc);
	else
		x = win->w_curc;

	y += win->w_miny;
	x += win->w_minx;

	*yp = y;
	*xp = x;

	return (y >= 0 && y < lines && x >= 0 && x < cols);

}	/* end cursor */
