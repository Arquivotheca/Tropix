/*
 ****************************************************************
 *								*
 *			obj.c					*
 *								*
 *	Objetos							*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: VI/EX						*
 *		Editores de texto				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
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
 *	Variáveis Locais					*
 ****************************************************************
 */
static char	findchar;
static char	markid;
static bool	deleting;
static bool	inserting;
static int	usrcolumn;

static BASE	*newbase;
static int	neworder;

/*
 ******	Protótipos de funções ***********************************
 */
static int	f_null (int, int);
static int	f_line (int, int);
static int	f_column (int, int);
static int	f_inline (int, int);
static int	f_word (int, int);
static int	f_find (int, int);
static int	f_search (int, int);
static int	f_page (int, int);
static int	f_mark (int, int);
static int	f_balance (int, int);

static char	advance (BASE **, int *);
static char	retrocede (BASE **, int *);

/*
 ******	Tabela de funções objeto ********************************
 */
static const int	(*func[]) (int, int) =
{
	f_null,
	f_find,
	f_word,
	f_inline,
	f_column,
	f_line,
	f_search,
	f_mark,
	f_page,
	f_balance,
	f_null,		/* f_sentence */
	f_null,		/* f_section */
	f_null		/* f_paragraph */
};

/*
 ****************************************************************
 *	Procura um objeto					*
 ****************************************************************
 */
int
object (OBJ *op, int count, bool del, bool ins, BASE **nbase, int *norder)
{
	int		major, minor;
	bool		relative_move = false;	/* CUIDADO */
	int		ret;
	static int	last_major;

	deleting = del;
	inserting = ins;
	major = MAJOR (op->o_obj);
	minor = MINOR (op->o_obj);

#if (0)	/*************************************/
	wprintw
	(	View->v_err, "obj: '%c': major:%d minor:%d",
		op->o_obj, major, minor
	);
	ter_send ();
	sleep (1);

	wprintw (View->v_err, "obj: count:%d", count);
	ter_send ();
	sleep (1);
#endif	/*************************************/

	if (last_major != M_COLUMN)
		usrcolumn = str_coln (View->v_curc, View->v_base->ch);

	last_major = major;

	switch (major)
	{
	    case M_NULL:
		*nbase = View->v_base;
		*norder = View->v_curc;
		return (OBJ_NULL);

	    case M_SEARCH:
	    case M_PAGE:
	    case M_SENTENCE:
	    case M_SECTION:
	    case M_PARAGRAPH:
		relative_move = false;
		break;

	    case M_WORD:
	    case M_INLINE:
	    case M_LINE:
	    case M_COLUMN:
	    case M_BALANCE:
		relative_move = true;
		break;

	    case M_FIND:
		findchar = op->o_arg;
		relative_move = false;
		break;

	    case M_MARK:
		if (isalpha (op->o_arg))
			markid = op->o_arg;
		else
			markid = ' ';

		relative_move = false;
		break;

	    default:
		vis_error ("major de objeto estranho", NOSTR);
		break;

	}	/* switch major */

	ret = (*func[major]) (minor, count);

	if (ret != OBJ_ERROR)
	{
		*nbase = newbase;
		*norder = neworder;

		if (relative_move)
			mrk_define (' ', newbase, neworder);
	}
	elif (inserting || deleting)
	{
		/* vazio */;
	}
	elif (relative_move)
	{
		beep ();
	}
	elif (major == M_SEARCH)
	{
		vis_error (Esearch, "Não encontrei");
	}
	else
	{
		vis_error ("Objeto não encontrado", NOSTR);
	}

	if (Debug)
		fprintf (stderr, "object: %d\n", ret);

	return (ret);

}	/* end object */

/*
 ****************************************************************
 *	Funções ainda não implementadas				*
 ****************************************************************
 */
static int
f_null (int minor, int count)
{
	newbase = View->v_base;
	neworder = View->v_curc;

	return (OBJ_ERROR);

}	/* end f_null */

/*
 ****************************************************************
 *	Non white						*
 ****************************************************************
 */
static int
f_line (int minor, int count)
{
	BASE		*b;

	switch (minor)
	{
	    case 0:
		if (deleting)
			count -= 1;

		if ((b = txt_advance (View->v_base, count)) != NOBASE)
		{
			newbase = b;
			neworder = 0;
			return (OBJ_FLINE);
		}
		else
		{
			return (OBJ_ERROR);
		}

	    case 1:
		newbase = View->v_base;
		neworder = 0;
		return ((neworder < View->v_curc) ? OBJ_BCHAR : OBJ_FCHAR);

	    case 2:
		if ((b = txt_advance (View->v_base, -count)) != NOBASE)
		{
			newbase = b;
			neworder = 0;
			return (OBJ_BLINE);
		}
		else
		{
			return (OBJ_ERROR);
		}

	}	/* switch (minor) */

	return (OBJ_ERROR);

}	/* end f_line */

/*
 ****************************************************************
 *	Column							*
 ****************************************************************
 */
static int
f_column (int minor, int count)
{
	BASE		*b;

	switch (minor)
	{
	    case 0:
		if ((b = txt_advance (View->v_base, -count)) != NOBASE)
		{
			newbase = b;
			neworder = str_try (usrcolumn, b->ch);
			return (OBJ_BCHAR);
		}
		else
		{
			return (OBJ_ERROR);
		}

	    case 1:
		newbase = View->v_base;
		neworder = str_try (count, View->v_base->ch);
		return ((neworder < View->v_curc) ? OBJ_BCHAR : OBJ_FCHAR);

	    case 2:
		if ((b = txt_advance (View->v_base, count)) != NOBASE)
		{
			newbase = b;
			neworder = str_try (usrcolumn, b->ch);
			return (OBJ_FCHAR);
		}
		else
		{
			return (OBJ_ERROR);
		}

	}	/* switch (minor) */

	return (OBJ_ERROR);

}	/* end f_column */

/*
 ****************************************************************
 *	Line							*
 ****************************************************************
 */
static int
f_inline (int minor, int count)
{
	char		ch;
	int		nextstop;

	newbase = View->v_base;
	neworder = View->v_curc;

	while (count-- > 0) switch (minor)
	{
		/*
		 *	Primeiro caractere
		 */
	    case 0:
		if (neworder == 0)
		{
			if (ISFIRST (newbase))
				return (OBJ_ERROR);
			elif (!deleting && !inserting)
				newbase = newbase->ant;
		}

		neworder = 0;
		break;

		/*
		 *	Retrocede
		 */
	    case 1:
		if ((ch = retrocede (&newbase, &neworder)) == '\0' || ch == '\n')
			return (OBJ_ERROR);
		break;

		/*
		 *	Avança
		 */
	    case 2:
		if ((deleting && count == 0) || (inserting && strlen (newbase->ch) == 0))
			return (OBJ_FCHAR);

		ch = advance (&newbase, &neworder);

		if (ch == '\n')
		{
			if (inserting)
				return (OBJ_FCHAR);
			else
				return (OBJ_ERROR);
		}
		elif (ch == '\0')
		{
			return (OBJ_ERROR);
		}

		break;

		/*
		 *	Último caractere
		 */
	    case 3:
		if
		(	(newbase->ch[neworder] == '\0' ||
			newbase->ch[neworder+1] == '\0') &&
			!inserting && !deleting
		)
		{
			if (ISLAST (newbase))
				return (OBJ_ERROR);
			else
				newbase = newbase->pos;
		}

		neworder = strlen (newbase->ch);
		break;

		/*
		 *	Avança para a próxima posição de Tabulação
		 */
	    case 4:
		nextstop = str_coln (neworder, newbase->ch);
		nextstop = (nextstop + 8) & ~7;

		if (str_try (nextstop, newbase->ch) == neworder)
			neworder = str_try (nextstop + 8, newbase->ch);
		else
			neworder = str_try (nextstop, newbase->ch);
		break;

	}	/* while (count-- > 0) switch (minor) */

	return ((neworder < View->v_curc) ? OBJ_BCHAR : OBJ_FCHAR);

}	/* end f_inline */

/*
 ****************************************************************
 *	Words							*
 ****************************************************************
 */
static int
f_word (int minor, int count)
{
	char		ch;
	bool		forward = false;	/* CUIDADO */

	newbase = View->v_base;
	neworder = View->v_curc;
	ch = View->v_base->ch[View->v_curc];

	while (count-- > 0) switch (minor)
	{
	    case 0:
		forward = true;

		if (ispunct(ch) || iscntrl(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && ispunct(ch));
		}
		elif (isalnum(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && isalnum(ch));
		}

		while (ch != '\0' && isspace (ch))
			ch = advance (&newbase, &neworder);
		break;

	    case 1:
		forward = true;
		ch = advance (&newbase, &neworder);

		while (ch != '\0' && isspace (ch))
			ch = advance (&newbase, &neworder);

		if (ispunct(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && ispunct(ch));
		}
		elif (isalnum(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && isalnum(ch));
		}

		if (ch != '\0')
			ch = retrocede (&newbase, &neworder);
		break;


	    case 2:
		forward = false;
		ch = retrocede (&newbase, &neworder);

		while (ch != '\0' && isspace (ch))
			ch = retrocede (&newbase, &neworder);

		if (ispunct (ch))
		{
			do
				ch = retrocede (&newbase, &neworder);
			while (ch != '\0' && ispunct(ch));
		}
		elif (isalnum(ch))
		{
			do
				ch = retrocede (&newbase, &neworder);
			while (ch != '\0' && isalnum(ch));
		}

		if (ch != '\0')
			ch = advance (&newbase, &neworder);
		break;

	    case 3:
		forward = true;

		if (!isspace(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && !isspace(ch));
		}

		while (ch != '\0' && isspace(ch))
			ch = advance (&newbase, &neworder);
		break;

	    case 4:
		forward = true;
		ch = advance (&newbase, &neworder);

		while (ch != '\0' && isspace(ch))
			ch = advance (&newbase, &neworder);

		if (ch != '\0' && !isspace(ch))
		{
			do
				ch = advance (&newbase, &neworder);
			while (ch != '\0' && !isspace(ch));
		}

		if (ch != '\0')
			ch = retrocede (&newbase, &neworder);
		break;

	    case 5:
		forward = false;
		ch = retrocede (&newbase, &neworder);

		while (ch != '\0' && isspace(ch))
			ch = retrocede (&newbase, &neworder);

		if (!isspace(ch))
		{
			do
				ch = retrocede (&newbase, &neworder);
			while (ch != '\0' && !isspace(ch));
		}

		if (ch != '\0')
			ch = advance (&newbase, &neworder);
		break;

	}	/* while (count-- > 0) switch (minor) */


	if (deleting && (minor == 0 || minor == 3))
	{
		if (ch != '\0')
			ch = retrocede (&newbase, &neworder);

		return (OBJ_FCHAR);
	}

	if (forward)
		return ((ch != '\0') ? OBJ_FCHAR : OBJ_ERROR);
	else
		return ((ch != '\0') ? OBJ_BCHAR : OBJ_ERROR);

}	/* end f_word */

/*
 ****************************************************************
 *	Find							*
 ****************************************************************
 */
static int
f_find (int minor, int count)
{
	static int	lastfminor;
	static char	lastfchar;
	char		f = '\0';	/* CUIDADO */
	char		c;

#if (0)	/*************************************/
	wprintw (View->v_err, "f_find: findchar:'%c'", findchar);
	ter_send();
	sleep (1);
#endif	/*************************************/

	switch (minor)
	{
	    case 0:
	    case 1:
	    case 2:
	    case 3:
		lastfminor = minor;
		f = lastfchar = findchar;
		break;

	    case 4:
		minor = lastfminor;
		f = lastfchar;
		break;

	    case 5:
		minor = (lastfminor +2) & 3;
		f = lastfchar;
		break;

	}	/* switch minor */

	newbase = View->v_base;
	neworder = View->v_curc;

#if (0)	/*************************************/
	wprintw (View->v_err, "f_find: minor:%d, count:%d, f:'%c'", minor, count, f);
	ter_send ();
	sleep(1);
#endif	/*************************************/

	while (count-- > 0) switch (minor)
	{
	    case 0:
		do
			c = advance (&newbase, &neworder);
		while (c != '\0' && c != f);

		return ((c != '\0') ? OBJ_FCHAR : OBJ_ERROR);

	    case 1:
		do
			c = advance (&newbase, &neworder);
		while (c != '\0' && c != f);

		c = retrocede (&newbase, &neworder);

		return ((c != '\0') ? OBJ_FCHAR : OBJ_ERROR);

	    case 2:
		do
			c = retrocede (&newbase, &neworder);
		while (c != '\0' && c != f);

		return ((c != '\0') ? OBJ_BCHAR : OBJ_ERROR);

	    case 3:
		do
			c = retrocede (&newbase, &neworder);
		while (c != '\0' && c != f);

		c = advance (&newbase, &neworder);

		return ((c != '\0') ? OBJ_BCHAR : OBJ_ERROR);

	}	/* while (count-- > 0) switch (minor) */

	return (OBJ_ERROR);

}	/* end f_find */

/*
 ****************************************************************
 *	Busca de expressões regulares				*
 ****************************************************************
 */
static int	
f_search (int minor, int count)
{
	BASE		*base;
	int		order;
	static int	lastsminor;

	switch (minor)
	{
	    case 0:
	    case 1:
		lastsminor = minor;
		break;

	    case 2:
		minor = lastsminor;
		break;

	    case 3:
		minor = (lastsminor + 1) & 1;
		break;

	    case 4:
		break;
	}

	/*
	 *	Posição Inicial
	 */
	base = View->v_base;
	order = View->v_curc;

	if (minor == 1)
	{
		/*
		 *	Busca para a frente: próximo caractere
		 */
		if (advance (&base, &order) == '\0')
			return (OBJ_ERROR);
	}
	elif (minor == 0)
	{
		if (base->ch[order])
		{
			/*
			 *	Busca para trás: se não é o último
			 *	caractere na linha, avança
			 */
			advance (&base, &order);
		}
		elif (ISFIRST (base))
		{
			/*
			 *	Último car. na linha: início da linha anterior
			 */
			base = LAST ();
			order = 0;
		}
		else
		{
			base = base->ant;
			order = 0;
		}
	}

	/*
	 *	Busca count vezes
	 */
	for (/* vazio */; count > 0; count--)
	{
		while ((order = re_execute (Search, base->ch, &base->ch[order])) < 0)
		{
			if (minor == 0)
				base = base->ant;
			else
				base = base->pos;

			if (base == View->v_base)
				return (OBJ_ERROR);

			order = 0;
		}
	}

	newbase = base;

	if (minor == 4)
		neworder = _end_match - base->ch;
	else
		neworder = order;

	if (deleting)
	{
		if (minor == 0)
			advance (&newbase, &neworder);
		else
			retrocede (&newbase, &neworder);
	}

	return (minor == 0 ? OBJ_BCHAR : OBJ_FCHAR);

}	/* end f_search */

/*
 ****************************************************************
 *	Page							*
 ****************************************************************
 */
static int	
f_page (int minor, int count)
{
	switch (minor)
	{
	    case 0:
		newbase = txt_advance (View->v_first, count - 1);

		if (newbase == NOBASE)
			return (OBJ_ERROR);

		neworder = 0;
		return (OBJ_BLINE);

	    case 1:
		newbase = txt_advance (View->v_first, View->v_lines/2);

		if (newbase == NOBASE)
			return (OBJ_ERROR);

		neworder = 0;

		return (txt_inorder (View->v_base, newbase) ? OBJ_FLINE : OBJ_BLINE);

	    case 2:
		newbase = txt_advance (View->v_last, -count + 1);

		if (newbase == NOBASE)
			return (OBJ_ERROR);

		neworder = 0;

		return (txt_inorder (View->v_base, newbase) ? OBJ_FLINE : OBJ_BLINE);

	}	/* switch minor */

	return (OBJ_ERROR);

}	/* end f_page */

/*
 ****************************************************************
 *	Marcas							*
 ****************************************************************
 */
static int
f_mark (int minor, int count)
{
#if (0)	/*******************************************************/
	count;		/* CUIDADO */
#endif	/*******************************************************/

	if (!mrk_pos (markid, &newbase, &neworder))
		return (OBJ_ERROR);

	if (minor == 1)
	{
		return (txt_inorder (View->v_base, newbase) <= 0 ? OBJ_FLINE : OBJ_BLINE);
	}
	else
	{
		switch (txt_inorder (View->v_base, newbase))
		{
		    case -1:
			return (OBJ_FCHAR);

		    case 0:
			if (neworder >= View->v_curc)
				return (OBJ_FCHAR);
			else
				return (OBJ_BCHAR);

		    case 1:
			return (OBJ_BCHAR);
		}
	}

	return (OBJ_ERROR);

}	/* end f_mark */

/*
 ****************************************************************
 *	Balance							*
 ****************************************************************
 */
static int
f_balance (int minor, int count)
{
	char		balance;
	char		original;
	char		ch;
	char		last;
	char		(*next_char) (BASE **, int *);

	if (minor != 0)
		return (OBJ_ERROR);

	original = View->v_base->ch[View->v_curc];

#if (0)	/*************************************/
	wprintw (View->v_err, "f_bal: orginal: '%c'", original);
	ter_send(); sleep(1);
#endif	/*************************************/

	switch (original)
	{
	    case '"':
		balance = original;
		next_char = advance;
		break;

	    case '\'':
		balance = original;
		next_char = advance;
		break;

	    case '(':
		balance = ')';
		next_char = advance;
		break;

	    case ')':
		balance = '(';
		next_char = retrocede;
		break;

	    case '<':
		balance = '>';
		next_char = advance;
		break;

	    case '>':
		balance = '<';
		next_char = retrocede;
		break;

	    case '[':
		balance = ']';
		next_char = advance;
		break;

	    case ']':
		balance = '[';
		next_char = retrocede;
		break;

	    case '{':
		balance = '}';
		next_char = advance;
		break;

	    case '}':
		balance = '{';
		next_char = retrocede;
		break;

	    default:
		balance = '{';
		next_char = retrocede;
		break;

	}	/* switch (original) */

	count = 1;
	last = '\0';
	newbase = View->v_base;
	neworder = View->v_curc;

	while ((ch = (*next_char) (&newbase, &neworder)) != '\0')
	{
		if (ch == balance && last != '\\')
		{
			if (--count == 0)
				break;
		}
		elif (ch == original && last != '\\')
		{
			count++;
		}

		last = ch;
	}

	if (count > 0)
		return (OBJ_ERROR);

	return ((next_char == retrocede) ? OBJ_BCHAR : OBJ_FCHAR);

}	/* end f_balance */

/*
 ****************************************************************
 *	Avanca um caracter no texto				*
 ****************************************************************
 */
static char
advance (BASE **basep, int *orderp)
{
	char		c;

	if ((*basep)->ch[(*orderp)] == '\0')
	{
		if (ISLAST (*basep))
			return ('\0');
		else
		{
			(*basep) = (*basep)->pos;
			(*orderp) = 0;
		}
	}
	else
	{
		(*orderp)++;
	}

	if ((*basep)->ch[(*orderp)] == '\0')
		c = '\n';
	else
		c = (*basep)->ch[(*orderp)];

#if (0)	/*************************************/
	wprintw (View->v_err, "Advance: '%c'", c);
	ter_send ();
	sleep(1);
#endif	/*************************************/

	return (c);

}	/* end advance */

/*
 ****************************************************************
 *	Retrocede um caracter no Texto				*
 ****************************************************************
 */
static char
retrocede (BASE **basep, int *orderp)
{
	char		c;

	if ((*orderp) == 0)
	{
		if (ISFIRST (*basep))
		{
			return ('\0');
		}
		else
		{
			(*basep) = (*basep)->ant;
			(*orderp) = strlen ((*basep)->ch);
			c = '\n';
		}
	}
	else
	{
		c = (*basep)->ch[--(*orderp)];
	}

#if (0)	/*************************************/
	wprintw (View->v_err,  "Retrocede: '%c'", c);
	ter_send ();
	sleep(1);
#endif	/*************************************/

	return (c);

}	/* end retrocede */
