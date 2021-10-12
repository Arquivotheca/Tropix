/*
 ****************************************************************
 *								*
 *			visual.c				*
 *								*
 *	Modo visual						*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/visual.h"
#include "../h/obj.h"
#include "../h/char.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry bool	Redo;		/* Repetindo a Última Edição */

/*
 ****************************************************************
 *	Máscaras Sintáticas para formação do Comando Visual	*
 ****************************************************************
 */
#define	S_BUF	(001)
#define	S_COUNT	(002)
#define	S_COM	(004)
#define	S_OBJ	(010)

/*
 ****************************************************************
 *	Variáveis Locais					*
 ****************************************************************
 */
static VIS	ZEROVIS;
static VIS	v;		/* Comando do modo Visual */
static bool	moving = false;

/*
 ******	Protótipos de funções ***********************************
 */
static void	read_command (void);
static void	synonym (int, VIS *);

/*
 ****************************************************************
 *	Sinônimos						*
 ****************************************************************
 */
typedef	struct
{
	char	s_ch;
	char	s_fun;
	short	s_obj;

}	SYN;

static SYN	syntab[] =
{
    /* s_ch */	/* s_fun */	/* s_obj */

	'A',	F_INSERT,	'$', 		/* $i */
	'a',	F_INSERT,	'l',		/* li */
	'I',	F_INSERT,	'^',		/* ^i */
	'O',	F_INSERT,	'-',		/* -i */
	'o',	F_INSERT,	'+',		/* +i */

	'D',	F_DELETE,	'$',		/* d$ */
	'x',	F_DELETE,	'l',		/* dl */
	'X',	F_DELETE,	'h',		/* dh */

	'C',	F_CHANGE,	'$',		/* c$ */
	's',	F_CHANGE,	'l',		/* cl */
	'S',	F_CHANGE,	'^',		/* cc */

	'Y',	F_YANK,		'^',		/* yy */
	'P',	F_PUT,		'-',		/* -p */

	'\0',	F_NULL,		0
};

/*
 ****************************************************************
 *	Modo visual						*
 ****************************************************************
 */
int
Visual (void)
{
	static VIS	last_edition;
	int		returnvalue;

	if (Mdebug)
		chkalloc ();

	/*
	 *	Configuração da Janela com o texto
	 */
	waddset (View->v_win, A_REAL);
	wgetoff (View->v_win, G_ECHO|G_EDIT|G_TONL|G_KEYPAD|G_NODELAY);
	wgeton  (View->v_win, G_SIGRET|G_IGXONXOFF);
	wrefset	(View->v_win, R_CURSOR|R_HOLD|R_WATCH|R_MOVE);
#ifdef	TIMING
	wrefset	(View->v_win, R_CURSOR|R_HOLD);
#endif	TIMING

	/*
	 *	Configuração da Janela para mensagens de Erro
	 */
	waddset (View->v_err, A_REAL);
	wgetset (View->v_err, G_IGXONXOFF|G_ECHO|G_EDIT|G_TOISO|G_TONL);
	wrefset	(View->v_err, R_HOLD);

	/*
	 *	Torna visível ou atualiza a Janela
	 */
	if (check (Text->t_flags, T_NEW) || pg_present (View->v_base) < 0)
	{
		pg_display (View->v_base);
		reset (Text->t_flags, T_NEW);
	}
	else
	{
		pg_move (View->v_base, View->v_curc, true);
	}

	touchwin (View->v_win);

	/*
	 *	Escreve mensagem procedente do modo Comandos
	 */
	if (Message[0] != '\0')
	{
		vis_message (Message);
		Message[0] = '\0';
	}

	/*
	 *	Interpretação e Execução dos comandos do Modo Visual
	 */
	for (EVER)
	{
		if (Mdebug)
			chkalloc ();

		wrefresh (View->v_win);

		read_command ();

		if (Interruption)
		{
			vis_message ("Interrupção");
			Interruption = false;
			continue;
		}

		if (Redo = (v.v_com == '.'))
			memmove (&v, &last_edition, sizeof (VIS));

		txt_usave ();

		switch (v.v_fun)
		{
		    case F_WINDOW:
			vis_window (&v);
			break;

		    case F_MOVE:
			vis_move (&v);
			break;

		    case F_MARK:
			mrk_define (v.v_arg, View->v_base, View->v_curc);
			break;

		    case F_INSERT:
			vis_insert (&v);
			memmove (&last_edition, &v, sizeof (VIS));
			break;

		    case F_DELETE:
			memmove (&last_edition, &v, sizeof (VIS));
			vis_delete (&v);
			break;

		    case F_YANK:
			vis_delete (&v);
			vis_error (NOSTR, NOSTR);
			break;

		    case F_CHANGE:
			vis_change (&v);
			memmove (&last_edition, &v, sizeof (VIS));
			break;

		    case F_REPLACE:
			memmove (&last_edition, &v, sizeof (VIS));
			vis_replace (&v);
			break;

		    case F_JOIN:
			memmove (&last_edition, &v, sizeof (VIS));
			vis_join (&v);
			break;

		    case F_UNDO:
			vis_undo (&v);
			break;

		    case F_SHIFT:
			memmove (&last_edition, &v, sizeof (VIS));

			if (!vis_shift (&v))
				beep ();
			break;

		    case F_PUT:
			memmove (&last_edition, &v, sizeof (VIS));
			vis_put (&v);
			break;

		    case F_EXTRA:
			if (v.v_com == ':')
			{
				returnvalue = VIS_QUICK;
				goto done;
			}
			elif (v.v_com == 'Q')
			{
				returnvalue = VIS_STAY;
				goto done;
			}
			elif (v.v_com == '!')
			{
				/*
				 *	Comando para a Shell
				 */
				int	nextch;

				vis_getstr (temp_buffer, ESC, true);

				if (temp_buffer[0] != '!' || System == NOSTR)
				{
					if (System)
						free (System);

					System = malloc (strlen (temp_buffer) + 1);
					strcpy (System, temp_buffer);
				}

				outcurses ();
				system (System);

				nextch = incurses (NOSTR);
				vis_ungetch (nextch);

				touchwin (View->v_win);
			}
			elif (v.v_com == CTLG)
			{
				sprintf
				(
					temp_buffer,
					"Linha %d, de um total de %d",
					txt_order (View->v_base),
					txt_order (LAST ())
				);
				vis_message (temp_buffer);
			}
			else
				vis_error ("Comando não implementado", NOSTR);
			break;

		    case F_NULL:
			vis_error ("Comando NULO", NOSTR);
			break;

		    default:
			vis_error ("Comando estranho", NOSTR);

		}	/* switch v.v_fun */
	}

    done:
	if (Mdebug)
		chkalloc ();

	return (returnvalue);

}	/* end Visual */

/*
 ****************************************************************
 *	Rotina de "switch"					*
 ****************************************************************
 */
void
vis_switch (int signo, ...)
{
	if (!moving)
	{
		moving = true;

		if (View->v_toleft)
		{
			mvwin (View->v_win, 0, COLS - View->v_cols);
			View->v_toleft = false;
		}
		else
		{
			mvwin (View->v_win, 0, 0);
			View->v_toleft = true;
		}

		moving = false;
	}

}	/* end vis_switch */

/*
 ****************************************************************
 *	Rotina do cursor					*
 ****************************************************************
 */
void
vis_cursor (int y, int x)
{
	if (!moving)
	{
		moving = true;

		if (x < VIEW_WIDTH)
		{
			if (View->v_toleft)
			{
				mvwin (View->v_win, 0, COLS - View->v_cols);
				View->v_toleft = false;
			}
			else
			{
				mvwin (View->v_win, 0, 0);
				View->v_toleft = true;
			}
		}
		else
		{
			vis_message ("Linha muito longa. Retorne o cursor");
		}

		moving = false;
	}

}	/* end vis_cursor */

/*
 ****************************************************************
 *	Leitura do comando visual				*
 ****************************************************************
 */
static void
read_command (void)
{
	char		ch;
	int		expected;

	vis_initget ();

	expected = S_BUF | S_COUNT | S_COM | S_OBJ;
	memmove (&v, &ZEROVIS, sizeof (VIS));

	while (expected)
	{
		ch = vis_chget();

	    again:
		if (ch == ERR)
			return;

		switch (SCLASS(ch))
		{
		    case L_BUFFER:
			if (!(expected & S_BUF))
			{
				vis_error
				(	
					"Especificação de buffer inesperada",
					NOSTR
				);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			ch = vis_chget();

			if (islower (ch))
			{
				v.v_buf = ch;
			}
			else
			{
				vis_error
				(
					"Use somente buffers de 'a' até 'z'",
					NOSTR
				);

				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			expected &= ~S_BUF;
			break;

		    case L_COUNT:
			if (!(expected & S_COUNT))
			{
				vis_error ("Contagem inesperada", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_count = ch - '0';

			while (ch = vis_chget(), isdigit(ch))
			{
				v.v_count *= 10;
				v.v_count += ch - '0';
			}

			vis_ungetch (ch);

			expected &= ~S_COUNT;
			break;

		    case L_SYNONYM:
			if (!(expected & S_COM))
			{
				if (expected & S_OBJ)
					vis_error ("Objeto esperado", NOSTR);
				else
					vis_error ("Comando inesperado", NOSTR);

				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			synonym (ch, &v);
			expected = 0;
			break;

		    case L_COMPLETE:
			if (!(expected & S_COM))
			{
				if (expected & S_OBJ)
					vis_error ("Objeto esperado", NOSTR);
				else
					vis_error ("Comando inesperado", NOSTR);

				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_com = ch;
			v.v_fun = FCLASS(ch);
			expected = 0;
			break;

		    case L_OPERATOR:
			if (expected & S_COM)
			{
				v.v_com = ch;
				v.v_fun = FCLASS (ch);
				expected &= ~S_COM & ~S_BUF;
			}
			elif (expected & S_OBJ)
			{
				v.v_obj.o_obj = ch;
				expected = 0;
			}
			else
			{
				vis_error ("Operador inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}
			break;

		    case L_OBJECT:
			if (!(expected & S_OBJ))
			{
				vis_error ("Objeto inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_obj.o_obj = ch;

			if (expected & S_COM)
				v.v_fun = F_MOVE;

			expected = 0;
			break;

		    case L_SEARCH:
			if (!(expected & S_OBJ))
			{
				vis_error ("Objeto inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			vis_getstr (temp_buffer, ch, false);

			if (temp_buffer[0] != '\0')
			{
				re_compile (temp_buffer, &Search);

				if (Esearch)
					free (Esearch);

				Esearch = strdup (temp_buffer);
			}

			v.v_obj.o_obj = ch;

			if (expected & S_COM)
				v.v_fun = F_MOVE;

			expected = 0;
			break;

		    case L_FIND:
			if (!(expected & S_OBJ))
			{
				vis_error ("Objeto inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_obj.o_obj = ch;
			v.v_obj.o_arg = vis_chget();

			if (expected & S_COM)
				v.v_fun = F_MOVE;

			expected = 0;
			break;

		    case L_DEFMARK:
			if (!(expected & S_COM))
			{
				vis_error ("Comando inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_com = ch;
			v.v_fun = FCLASS(ch);
			ch = vis_chget();

			if (!islower(ch))
			{
				vis_error
				(
					"Marcas são sempre letras minusculas",
					NOSTR
				);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_arg = ch;
			expected = 0;
			break;

		    case L_MARK:
			if (!(expected & S_OBJ))
			{
				vis_error ("Objeto inesperado", NOSTR);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_obj.o_obj = ch;

			if (expected & S_COM)
				v.v_fun = F_MOVE;

			ch = vis_chget();

			if (!islower(ch) && ch != '\'')
			{
				vis_error
				(
					"Marcas são sempre letras minusculas",
					NOSTR
				);
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				continue;
			}

			v.v_obj.o_arg = ch;
			expected = 0;
			break;

		    case L_ESCAPE:
			if   ((ch = vis_chget ()) == '\e')
			{
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				beep ();
				vis_initget ();
				break;
			}
			elif (ch != '[')
			{
				vis_initget ();
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				memmove (&v, &ZEROVIS, sizeof (VIS));
				goto again;
			}
#if (0)	/*******************************************************/
			if (vis_chget () != '[')
			{
				expected = S_BUF | S_COUNT | S_COM | S_OBJ;
				beep ();
				vis_initget ();
				break;
			}
#endif	/*******************************************************/

			switch (vis_chget ())
			{
			    case 'A':	/* Seta para cima */
				v.v_obj.o_obj = 'k';
				v.v_fun = F_MOVE;
				expected = 0;
				break;

			    case 'B':	/* Seta para baixo */
				v.v_obj.o_obj = 'j';
				v.v_fun = F_MOVE;
				expected = 0;
				break;

			    case 'C':	/* Seta para direita */
				v.v_obj.o_obj = 'l';
				v.v_fun = F_MOVE;
				expected = 0;
				break;

			    case 'D':	/* Seta para esquerda */
				v.v_obj.o_obj = 'h';
				v.v_fun = F_MOVE;
				expected = 0;
				break;

			    case '2':	/* Insert */
				vis_chget ();
				v.v_com = 'i';
				v.v_fun = F_INSERT;
				expected = 0;
				break;

			    case '5':	/* Page up */
				vis_chget ();
				v.v_com = CTLU;
				v.v_fun = F_WINDOW;
				expected = 0;
				break;

			    case '6':	/* Page down */
				vis_chget ();
				v.v_com = CTLD;
				v.v_fun = F_WINDOW;
				expected = 0;
				break;

			    case '7':	/* Home */
				vis_chget ();
				v.v_com = 'g';
				v.v_fun = F_WINDOW;
				v.v_count = 1;
				expected = 0;
				break;

			    case '8':	/* End */
				vis_chget ();
				v.v_com = 'g';
				v.v_fun = F_WINDOW;
				expected = 0;
				break;
			}

			break;

		    case L_NULL:
			beep ();
			vis_error ("Comando inexistente", NOSTR);
			expected = S_BUF | S_COUNT | S_COM | S_OBJ;
			continue;

		}	/* switch SCLASS */

	}	/* while expected */

	vis_endget ();

}	/* end read_command */

/*
 ****************************************************************
 *	Sinônimos						*
 ****************************************************************
 */
static void
synonym (int ch, VIS *vp)
{
	SYN		*sp;

	for (sp = syntab; sp->s_ch != '\0'; sp++)
	{
		if (sp->s_ch == ch)
		{
			vp->v_fun = sp->s_fun;
			vp->v_obj.o_obj = sp->s_obj;
			break;
		}
	}

}	/* end synonym */
