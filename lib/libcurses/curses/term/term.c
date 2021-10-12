/*
 ****************************************************************
 *								*
 *			term.c					*
 *								*
 *	Manipulação de vários terminais				*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 05.03.95				*
 *								*
 *	Módulo: term						*
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
#include <fcntl.h>
#include <termio.h>
#include <string.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/local.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define		ESC	0x01B		/* */
#define		CTLA	0x001		/* */

static int	_curtd = 0;	/* Descritor do terminal corrente */
static TERM	*ocur;
static int	olines, ocols;

/*
 ******	Protótipos de funções ***********************************
 */
static void	set_globals (TERM *, int, int);
static void	reset_globals (void);
static int	initkeys (char **, KEY **);

/*
 ****************************************************************
 *	Preparação  de um novo Terminal				*
 ****************************************************************
 */
int
newterm (const char *type_nm, FILE *outfp, FILE *infp)
{
	register TERM	*tp;
	register int	n;
	register int	lines, cols;
	register chtype	*screen;
	register char	*map;
	int		td;

#if (0)	/*************************************/
	/*
	 *	Verifica se a entrada e a saída são tty's
	 */
	if (! isatty (fileno (infp)) || ! isatty (fileno (outfp)))
		return (-1);
#endif	/*************************************/

	/*
	 *	Aloca memória para a estrutura TERM
	 */
	if ((tp = malloc (TERMSZ)) == NOTERM)
		return (-1);

	/*
	 *	Pega informações sobre o terminal
	 */
	if (getinfo (type_nm, &tp->t_info) == 0)
		{ free (tp); return (-1); }

	lines = tp->t_info.i_numbers[n_lines];
	cols  = tp->t_info.i_numbers[n_cols];

	/*
	 *	Tenta obter o tamanho da janela
	 */
	ioctl (fileno (outfp), TCGETS, &tp->t_oldtermio);

	if (tp->t_oldtermio.t_nline)
		lines = tp->t_oldtermio.t_nline;

	if (tp->t_oldtermio.t_ncol)
		cols  = tp->t_oldtermio.t_ncol;

	/*
	 *	Cria a janela padrão
	 */
	set_globals (tp, lines, cols);

	if ((tp->t_stdwin = newwin (lines, cols, 0, 0)) == NOWINDOW)
		{ free (tp); reset_globals (); return (-1); }

	reset_globals ();

	/*
	 *	Aloca representação do Vídeo
	 */
	if ((tp->t_screen = malloc (lines * sizeof (chtype *))) == (chtype **)NULL)
		{ free (tp->t_stdwin); free (tp); return (-1); }

	if ((screen = malloc (lines * cols * sizeof (chtype))) == (chtype *)NULL)
	{
		free (tp->t_screen);
		free (tp->t_stdwin);
		free (tp);
		return (-1);
	}

	if ((tp->t_map = malloc (lines * sizeof (char *))) == (char **)NULL)
	{
		free (tp->t_screen);
		free (tp->t_stdwin);
		free (tp);
		free (screen);
		return (-1);
	}

	if ((map = malloc (lines * cols * sizeof (char))) == NOSTR)
	{
		free (tp->t_map);
		free (tp->t_screen);
		free (tp->t_stdwin);
		free (tp);
		free (screen);
		return (-1);
	}

	for (n = 0; n < lines; n++)
	{
		tp->t_screen[n] = screen + n * cols;
		tp->t_map[n]	= map	 + n * cols;
	}

	/*
	 *	Inicializa representação do Vídeo:
	 *
	 *		t_screen: caracteres <sp>
	 *		t_map: stdwin - _windows
	 */
	for (n = lines * cols - 1; n >= 0; n--)
	{
		screen[n] = ' ';
		map[n] = tp->t_stdwin - _windows;
	}

	/*
	 *	Inicia a fila de prioridade de visibilidade
	 */
	tp->t_last = tp->t_stdwin;
	tp->t_first = tp->t_stdwin;
	tp->t_stdwin->w_queue = NOWINDOW;

	/*
	 *	Obtém o modo de comunicação do terminal
	 */
#if (0)	/*******************************************************/
	ioctl (fileno (infp), TCGETS, &tp->t_oldtermio);
#endif	/*******************************************************/

	memmove (&tp->t_termio, &tp->t_oldtermio, sizeof (TERMIO));

	if (tp->t_termio.t_eol == 0)
		tp->t_termio.t_eol = ESC;

	if (tp->t_termio.t_swtch == 0)
		tp->t_termio.t_swtch = CTLA;

	/*
	 *	Garante saída com buffer
	 */
	if ((tp->t_outbuf = malloc (BL4SZ)) == NOSTR)
	{
		free (tp->t_inbuf);
		free (tp->t_map);
		free (tp->t_screen);
		free (tp->t_stdwin);
		free (tp);
		free (screen);
		return (-1);
	}

	setvbuf (outfp, tp->t_outbuf, _IOFBF, BL4SZ);

	/*
	 *	Inicia a tabela de teclas especiais
	 */
	tp->t_nkeys = initkeys (tp->t_info.i_strings, &tp->t_keys);

	/*
	 *	Inicializações diversas
	 */
	tp->t_infp   = infp;
	tp->t_outfp  = outfp;
	tp->t_lines  = lines;
	tp->t_cols   = cols;
#if (0)	/*******************************************************/
	tp->t_lines  = tp->t_info.i_numbers[n_lines];
	tp->t_cols   = tp->t_info.i_numbers[n_cols];
#endif	/*******************************************************/
	tp->t_attr   = A_NORMAL;
	tp->t_switch = (void (*)(int, ...))0;
	tp->t_oflags = fcntl (fileno (tp->t_infp), F_GETFL, 0);
	tp->t_flags  = T_CLEAR;

	if (tp->t_termio.c_lflag & ISOKEY)
		tp->t_flags |= T_ISOKEY;

	/*
	 *	Caracteres semi-gráficos: Valores pré-estabelecidos
	 */
	ACS_ULCORNER = '+';
	ACS_LLCORNER = '+';
	ACS_URCORNER = '+';
	ACS_LRCORNER = '+';
	ACS_RTEE = '+';
	ACS_LTEE = '+';
	ACS_BTEE = '+';
	ACS_TTEE = '+';
	ACS_HLINE = '-';
	ACS_VLINE = '|';
	ACS_PLUS = '+';
	ACS_S1 = '-';
	ACS_S9 = '_';
	ACS_DIAMOND = '+';
	ACS_CKBOARD = ':';
	ACS_DEGREE = '\'';
	ACS_PLMINUS = '#';
	ACS_BULLET = 'o';
	ACS_LARROW = '<';
	ACS_RARROW = '>';
	ACS_DARROW = 'v';
	ACS_UARROW = '^';
	ACS_BOARD = '#';
	ACS_LANTERN = '#';
	ACS_BLOCK = '#';

	/*
	 *	Caracteres semi-gráficos: Valores para o terminal
	 */
	if (tp->t_info.i_strings[s_acsc] != NOSTR)
	{
		register char		*cp, *acsc;

		if (tp->t_info.i_strings[s_enacs] != NOSTR)
			fputs (tp->t_info.i_strings[s_enacs], tp->t_outfp);

		acsc = tp->t_info.i_strings[s_acsc];

		if ((cp = strchr (acsc, 'x')) != NOSTR);
			ACS_VLINE = cp[1] + A_ALTCHARSET;

		if ((cp = strchr (acsc, 'q')) != NOSTR)
			ACS_HLINE = cp[1] + A_ALTCHARSET;

		if ((cp = strchr (acsc, 'l')) != NOSTR)
			ACS_ULCORNER = cp[1] + A_ALTCHARSET;

		if ((cp = strchr (acsc, 'm')) != NOSTR)
			ACS_LLCORNER = cp[1] + A_ALTCHARSET;

		if ((cp = strchr (acsc, 'k')) != NOSTR)
			ACS_URCORNER = cp[1] + A_ALTCHARSET;

		if ((cp = strchr (acsc, 'j')) != NOSTR)
			ACS_LRCORNER = cp[1] + A_ALTCHARSET;
	}

	/*
	 *	Insere o terminal na lista de terminais
	 */
	if (_firstterm == NOTERM)
	{
		_firstterm = tp;
		td = 0;
	}
	else
	{
		register TERM	*tl;

		td = 1;

		for (tl = _firstterm; tl->t_list != NOTERM; tl = tl->t_list)
			td++;

		tl->t_list = tp;
	}

	tp->t_list = NOTERM;

	return (td);

}	/* end newterm */

/*
 ****************************************************************
 *	Estabelecimento do terminal corrente			*
 ****************************************************************
 */
int
setterm (int newtd)
{
	register TERM	*tl;
	register int	td, old;

	old = _curtd;

	/*
	 *	Verifica se existe a lista de terminais
	 */
	if (_firstterm == NOTERM)
		return (-1);

	/*
	 *	Acha o terminal na lista de terminais
	 */
	tl = _firstterm;

	for (td = 0; td < newtd; td++)
	{
		if (tl->t_list != NOTERM)
			tl = tl->t_list;
		else
			return (-1);
	}

	_curterm = tl;

	/*
	 *	Variáveis globais
	 */
	LINES = _curterm->t_lines;
	COLS = _curterm->t_cols;
	stdwin = _curterm->t_stdwin;

	/*
	 *	Iniciando movimento com Cursor
	 */
	if (tl->t_info.i_strings[s_smcup] != NOSTR)
		fputs (tl->t_info.i_strings[s_smcup], tl->t_outfp);

	return (old);

}	/* end setterm	*/

/*
 ****************************************************************
 *	Atribui valores para as variáveis globais		*
 ****************************************************************
 */
static void
set_globals (TERM *cur, int lines, int cols)
{
	/*
	 *	Guarda os valores originais
	 */
	ocur = _curterm; olines = LINES; ocols = COLS;

	/*
	 *	Atribui os novos valores
	 */
	_curterm = cur; LINES = lines; COLS = cols;

}	/* end set_globals */

/*
 ****************************************************************
 *	Restaura os Valores Originais às Variáveis Globais	*
 ****************************************************************
 */
static void
reset_globals (void)
{
	_curterm = ocur; LINES = olines; COLS = ocols;

}	/* end reset_globals */

/*
 ****************************************************************
 *	Inicia Tabela de Caracteres Especiais.			*
 ****************************************************************
 */
static int
initkeys (register char **strings, KEY **pkeys)
{
	register KEY	*keys;
	int		nkeys;

	nkeys = 18; keys = malloc (nkeys * sizeof (KEY));

	keys[0].k_key = KEY_HOME; keys[0].k_string = strings[s_khome];

	keys[1].k_key = KEY_END; keys[1].k_string = strings[s_kend];

	keys[2].k_key = KEY_NPAGE; keys[2].k_string = strings[s_knp];

	keys[3].k_key = KEY_PPAGE; keys[3].k_string = strings[s_kpp];

	keys[4].k_key = KEY_DOWN; keys[4].k_string = strings[s_kcud1];

	keys[5].k_key = KEY_UP; keys[5].k_string = strings[s_kcuu1];

	keys[6].k_key = KEY_LEFT; keys[6].k_string = strings[s_kcub1];

	keys[7].k_key = KEY_RIGHT; keys[7].k_string = strings[s_kcuf1];

	keys[8].k_key = KEY_F0; keys[8].k_string = strings[s_kf0];

	keys[9].k_key = KEY_F1; keys[9].k_string = strings[s_kf1];

	keys[10].k_key = KEY_F2; keys[10].k_string = strings[s_kf2];

	keys[11].k_key = KEY_F3; keys[11].k_string = strings[s_kf3];

	keys[12].k_key = KEY_F4; keys[12].k_string = strings[s_kf4];

	keys[13].k_key = KEY_F5; keys[13].k_string = strings[s_kf5];

	keys[14].k_key = KEY_F6; keys[14].k_string = strings[s_kf6];

	keys[15].k_key = KEY_F7; keys[15].k_string = strings[s_kf7];

	keys[16].k_key = KEY_F8; keys[16].k_string = strings[s_kf8];

	keys[17].k_key = KEY_F9; keys[17].k_string = strings[s_kf9];

	*pkeys = keys; 	return (nkeys);

}	/* end initkeys */
