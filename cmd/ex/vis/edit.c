/*
 ****************************************************************
 *								*
 *			edit.c					*
 *								*
 *	Edições no modo visual					*
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
#include <ctype.h>
#include <curses.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/visual.h"
#include "../h/obj.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		delete (BASE *, int, int);
void		insert (BASE *, int, int);

/*
 ****************************************************************
 *	Substituição						*
 ****************************************************************
 */
void
vis_change (VIS *vp)
{
	int		start, End;
	BASE		*first, *last;
	BASE		*base;
	int		count, order;

	if (Debug)
		fprintf (stderr, "change:  '%c'\n", vp->v_obj.o_obj);

	/*
	 *	Contagem
	 */
	if (vp->v_count == 0)
		count = 1;
	else
		count = vp->v_count;

	/*
	 *	Computa a abrangência da substituição
	 */
	switch (object (&vp->v_obj, count, true, false, &base, &order))
	{
		/*
		 *	Avante em linhas inteiras
		 */
	    case OBJ_FLINE:
		first = View->v_base;
		last = base;
		goto changeln;

		/*
		 *	Para trás em linhas inteiras
		 */
	   case OBJ_BLINE:
		first = base;
		last = View->v_base->ant;

		/*
		 *	Trecho comum para OBJ_FLINE e OBJ_BLINE
		 */
	    changeln:
		base = first->ant;
		txt_lsave (first->ant, last->pos);
		txt_new (base, 1);
		txt_remove (first, last);
		base = base->pos;
		order = 0;
		break;

		/*
		 *	Avante
		 */
	    case OBJ_FCHAR:
		first = View->v_base;
		last = base;
		start = View->v_curc;
		End = order;
		goto changechars;

		/*
		 *	Para trás
		 */
	    case OBJ_BCHAR:
		first = base;
		last = View->v_base;
		start = order;
		End = View->v_curc - 1;

		/*
		 *	Trecho comum para OBJ_FCHAR e OBJ_BCHAR
		 */
	    changechars:
		txt_lsave (first->ant, last->pos);

		if (last == first)
		{
			delete (first, start, End);
		}
		else
		{
			delete (first, start, strlen (first->ch) - 1);

			if (last != first->pos)
				txt_remove (first->pos, last->ant);

			if (End >= 0)
				delete (last, 0, End);

			txt_join (first, last, 0);
		}

		base = last;
		order = start;
		break;

	    case OBJ_ERROR:
	    case OBJ_NULL:
	    default:
		goto error;
	}

	insert (base, order, 0);
	return;

    error:
	beep ();
	vis_error ("Erro na substituição", NOSTR);

}	/* end vis_change */

/*
 ****************************************************************
 *	Remoção e/ou armazenamento de texto			*
 ****************************************************************
 */
void
vis_delete (VIS *vp)
{
	int		start, End;
	BASE		*first, *last;
	BASE		*base;
	int		count, order;
	int		buf;
	bool		Remove;

	Remove = (vp->v_fun == F_DELETE);

	if (vp->v_buf)
		buf = vp->v_buf;
	else
		buf = '?';

	if (Debug)
	{
		if (Remove)
			fprintf (stderr, "delete: obj:'%c', buf:'%c'\n", vp->v_obj.o_obj, buf);
		else
			fprintf (stderr, "yank:   obj:'%c', buf:'%c'\n", vp->v_obj.o_obj, buf);
	}

	/*
	 *	Contagem
	 */
	if (vp->v_count == 0)
		count = 1;
	else
		count = vp->v_count;

	/*
	 *	Computa a abrangência da remoção
	 */
	switch (object (&vp->v_obj, count, true, false, &base, &order))
	{
		/*
		 *	Avante em linhas inteiras
		 */
	    case OBJ_FLINE:
		first = View->v_base;
		last = base;

		if (!buf_lput (first, last, buf))
			goto error;

		if (Remove)
		{
			txt_lsave (first->ant, last->pos);
			base = first->ant;
			txt_remove (first, last);

			if (!ISLAST (base) || ISFIRST (base->pos))
				base = base->pos;

			order = str_nonwhite (base->ch);
		}
		break;

		/*
		 *	Para trás em linhas inteiras
		 */
	    case OBJ_BLINE:
		if (ISFIRST (base))
			goto error;

		first = base;
		last = View->v_base->ant;

		if (!buf_lput (first, last, buf))
			goto error;

		if (Remove)
		{
			base = View->v_base;
			order = View->v_curc;
			txt_lsave (first->ant, last->pos);
			txt_remove (first, last);
		}
		break;

		/*
		 *	Avante
		 */
	    case OBJ_FCHAR:
		first = View->v_base;
		last = base;
		start = View->v_curc;
		End = order;
		goto deletechars;

		/*
		 *	Para trás
		 */
	    case OBJ_BCHAR:
		first = base;
		last = View->v_base;
		start = order;
		End = View->v_curc - 1;

		/*
		 *	Trecho comum para OBJ_FCHAR e OBJ_BCHAR
		 */
	    deletechars:
		if (Remove)
			txt_lsave (first->ant, last->pos);

		if (last == first)
		{
			buf_sput (first->ch, start, End, buf);

			if (Remove)
				delete (first, start, End);
		}
		else
		{
			buf_sput (first->ch, start, strlen (first->ch) -1, buf);

			if (Remove)
				delete (first, start, strlen (first->ch) - 1);

			if (last != first->pos)
			{
				if (!buf_lput (first->pos, last->ant, 0))
					goto error;

				if (Remove)
					txt_remove (first->pos, last->ant);
			}

			if (End >= 0)
			{
				buf_sput (last->ch, 0, End, '\0');

				if (Remove)
					delete (last, 0, End);
			}

			if (Remove)
			{
				txt_join (first, last, 0);
				base = last;
			}
		}

		order = start;
		break;

	    case OBJ_ERROR:
	    case OBJ_NULL:
	    default:
		goto error;

	}

	if (Remove)
	{
		/*
		 *	Atualiza a Janela
		 */
		pg_update ();
		pg_move (base, order, true);
	}

	return;

    error:
	beep ();
	vis_error ("Erro na remoção", NOSTR);

}	/* end vis_delete */

/*
 ****************************************************************
 *	Inserção interativa de texto				*
 ****************************************************************
 */
void
vis_insert (VIS *vp)
{
	BASE		*base;
	int		order;
	int		count;

	if (Debug)
		fprintf (stderr, "insert:  '%c'\n", vp->v_obj.o_obj);

	/*
	 *	Contagem
	 */
	if (vp->v_count == 0)
		count = 1;
	else
		count = vp->v_count;

	/*
	 *	Computa a abrangência e localização da inserção
	 */
	if (vp->v_obj.o_obj == '+')
	{
		/*
		 *	Abre linhas abaixo
		 */
		txt_fsave (View->v_base);
		txt_new (View->v_base, count);
		base = View->v_base->pos;
		order = 0;
		count -= 1;
	}
	elif (vp->v_obj.o_obj == '-')
	{
		/*
		 *	Abre linhas acima
		 */
		txt_bsave (View->v_base);
		base = View->v_base->ant;
		txt_new (View->v_base, -count);
		base = base->pos;
		order = 0;
		count -= 1;
	}
	else
	{
		switch (object (&vp->v_obj, count, false, true, &base, &order))
		{
			/*
			 *	Inserção na linha corrente
			 */
		    case OBJ_BCHAR:
		    case OBJ_NULL: 	/* Inserção na posição corrente */
		    case OBJ_FCHAR:
			txt_lsave (base->ant, base->pos);
			base = View->v_base;
			count = 0;
			break;

		    case OBJ_ERROR:
		    default:
			goto error;
		}
	}

	insert (base, order, count);
	return;

    error:
	beep ();
	vis_error ("Erro na inserção", NOSTR);

}	/* end vis_insert */

/*
 ****************************************************************
 *	Join							*
 ****************************************************************
 */
void
vis_join (VIS *vp)
{
	BASE		*first, *last;
	int		order;
	int		count;

	/*
	 *	Contagem
	 */
	if (vp->v_count > 0)
		count = vp->v_count;
	else
		count = 1;

	/*
	 *	Computa abrangência da alteração
	 */
	first = View->v_base;
	last = txt_walk (first, count);

	txt_lsave (first->ant, last->pos);
	order = txt_join (first, last, 1);

	/*
	 *	Atualiza a Janela
	 */
	pg_update ();
	pg_move (last, order, true);

}	/* end vis_join */

/*
 ****************************************************************
 *	Cópia de um buffer para o texto				*
 ****************************************************************
 */
void
vis_put (VIS *vp)
{
	int		buf;
	bool		forward;
	BASE		*base;
	int		order;
	BASE		*anterior, *posterior;

	base = View->v_base;
	order = View->v_curc;
	forward = (vp->v_com == 'p');

	/*
	 *	Escolha do Buffer
	 */
	if (vp->v_buf)
		buf = vp->v_buf;
	else
		buf = '?';

	/*
	 *	Local da cópia
	 */
	anterior = base->ant;
	posterior = base->pos;

	txt_lsave (anterior, posterior);
	buf_get (base, order, forward, buf);

	/*
	 *	Atualiza a Janela
	 */
	pg_update ();
	pg_move (base, order, true);

}	/* end vis_put */

/*
 ****************************************************************
 *	Substituição de um caractere				*
 ****************************************************************
 */
void
vis_replace (VIS *vp)
{
	BASE		*base;
	int		order;
	static int	lastch = ' ';

#if (0)	/*******************************************************/
	vp;				/* CUIDADO */
#endif	/*******************************************************/

	if (!Redo)
		lastch = wgetch (View->v_win);

	base = View->v_base;
	order = View->v_curc;

	txt_lsave (base->ant, base->pos);

	if (base->ch[order] == '\0')
	{
		base->ch = realloc (base->ch, order + 2);
		base->ch[order + 1] = '\0';
	}

	base->ch[order] = lastch;

	pg_update ();
	pg_move (base, order, true);

}	/* end vis_replace */

/*
 ****************************************************************
 *	Identação						*
 ****************************************************************
 */
bool
vis_shift (VIS *vp)
{
	BASE		*first, *last;
	int		count;
	char		*line;
	BASE		*base;
	int		order;

	/*
	 *	Contagem
	 */
	if (vp->v_count == 0)
		count = 1;
	else
		count = vp->v_count;

	/*
	 *	Computa a abrangência da indentação
	 */
	switch (object (&vp->v_obj, count, true, false, &base, &order))
	{
		/*
		 *	Avante em linhas inteiras
		 */
	    case OBJ_FLINE:
		first = View->v_base;
		last = base;
		break;

		/*
		 *	Para trás em linhas inteiras
		 */
	    case OBJ_BLINE:
		first = base;
		last = View->v_base->ant;
		break;

	    case OBJ_ERROR:
	    default:
		return (false);
	}

	/*
	 *	Salva linhas antes da indentação
	 */
	txt_lsave (first->ant, last->pos);

	/*
	 *	Realiza a indentação em cada uma das linhas
	 */
	for (base = first; /* teste abaixo */; base = base->pos)
	{
		if (vp->v_com == '>')
		{
			/*
			 *	Indentação à direita
			 */
			if ((line = malloc (strlen (base->ch) + 2)) == NOSTR)
				return (false);

			line[0] = '\t';
			strcpy (&line[1], base->ch);
			free (base->ch);
			base->ch = line;
		}
		elif (base->ch[0] == '\t')
		{
			/*
			 *	Indentação à esquerda
			 */
			if ((line = malloc (strlen (base->ch))) == NOSTR)
				return (false);

			strcpy (line, &(base->ch[1]));
			free (base->ch);
			base->ch = line;
		}

		/*
		 *	Condição de término do loop
		 */
		if (base == last)
			break;
	}

	/*
	 *	Atualização da Janela e Reposicionamento
	 */
	pg_update ();
	pg_move (View->v_base, str_nonwhite (View->v_base->ch), true);

	return (true);

}	/* end vis_shift */

/*
 ****************************************************************
 *	Undo							*
 ****************************************************************
 */
void
vis_undo (VIS *vp)
{
	BASE		*base;
	int		order;

#if (0)	/*******************************************************/
	vp;			/* CUIDADO */
#endif	/*******************************************************/

	if ((base = txt_restore ()) == NOBASE)
	{
		beep ();
		vis_error ("Não há nada a ser desfeito", NOSTR);
		return;
	}

	/*
	 *	Atualiza a Janela
	 */
	order = str_nonwhite (base->ch);
	pg_update ();
 	pg_move (base, order, true);

}	/* end vis_undo */

/*
 ****************************************************************
 *	Remoção de caracteres de uma linha de Texto		*
 ****************************************************************
 */
void
delete (BASE *base, int start, int End)
{
	if (base->ch[End] != '\0')
		End++;

	if (start >= End || base->ch[start] == '\0')
		return;

	strcpy (&base->ch[start], &base->ch[End]);

	base->ch = realloc (base->ch, strlen (base->ch) + 1);

}	/* end delete */

/*
 ****************************************************************
 *	Inserção interativa de texto 	 			*
 ****************************************************************
 */
void
insert (BASE *base, int order, int count)
{
	int		ch;
	char		*p;
	char		*left, *right;
	int		leftlen = 0, rightlen = 0;	/* CUIDADO */
	int		buf;
	char		text[LSIZE];

	/*
	 *	Inicializações
	 */
	buf = '!';

	/*
	 *	Repetição da Última inserção
	 */
	if (Redo)
	{
		if (order > 0)
			buf_get (base, order - 1, true, buf);
		else
			buf_get (base, 0, false, buf);

		pg_update ();
		pg_move (base, order, true);
		return;
	}

	/*
	 *	Atualiza o vídeo e posiciona o cursor
	 */
	pg_update ();
	pg_move (base, order, false);

	/*
	 *	Reprograma E/S da Janela
	 */
	wgeton (View->v_win, G_TONL|G_ECHO|G_EDIT|G_INSERT);
	waddon (View->v_win, A_SCROLL);

	if (count > 0)
		wgeton (View->v_win, G_ADDNL);

	/*
	 *	Guarda parte anterior da linha corrente
	 */
	if (order > 0)
	{
		leftlen = order;
		left = malloc (leftlen + 1);
		strncpy (left, base->ch, leftlen);
		left[leftlen] = '\0';
	}
	else
	{
		left = NOSTR;
	}

	/*
	 *	Guarda parte posterior da linha corrente
	 */
	if (strlen (base->ch) > order)
	{
		rightlen = strlen (base->ch) - order;
		right = malloc (rightlen + 1);
		strcpy (right, &base->ch[order]);
	}
	else
	{
		right = NOSTR;
	}

	for (EVER)
	{
		/*
		 *	Lê uma cadeia
		 */
		p = text;

		while
		(
			((ch = wgetch (View->v_win)) != '\n') &&
			(ch != EOL) &&
			(ch != EOT)
		)
		{
			if (p < &(text[LSIZE-1]))
				*p++ = ch;
			else
				beep ();
		}

		*p = '\0';

		/*
		 *	Guarda a cadeia em "text"
		 */
		buf_sput (text, 0, (p - text) - 1, buf);

		/*
		 *	Insere a cadeia no texto
		 */
		if (left != NOSTR)
		{
			free (base->ch);
			base->ch = malloc (leftlen + (p - text) + 1);
			strcpy (base->ch, left);
			strcpy (&(base->ch[leftlen]), text);
			free (left);
			left = NOSTR;
		}
		else
		{
			free (base->ch);
			base->ch = malloc ((p - text) + 1);
			strcpy (base->ch, text);
		}

		/*
		 *	Examina a linha corrente
		 */
		if (ch == '\n')
		{
			/*
			 *	Término da linha corrente
			 */
			if (count == 0)
			{
				/*
			 	 *	Insere uma nova linha no texto
				 */
				txt_insert (base, right);
			}
			else
			{
				/*
				 *	Passa para a próxima linha do texto
				 */
				if (count == 1)
					wgetoff (View->v_win, G_ADDNL);

				count--;
			}

			/*
			 *	Reinicialização
			 */
			base = base->pos;
			buf = '\0';
		}
		else
		{
			/*
			 *	Encerra inserção de texto
			 */
			order = strlen (base->ch);

			if (right != NOSTR)
			{
				char	*line;

				/*
				 *	Copia parte à direita
				 */
				if ((line = malloc (strlen (base->ch) + rightlen + 1)) == NOSTR)
					break;

				strcpy (line, base->ch);
				strcat (line, right);

				free (base->ch);
				base->ch = line;

				free (right);
				right = NOSTR;
			}
			break;
		}

	}	/* for (EVER)	*/

	/*
	 *	Retorna programação de E/S da janela
	 */
	wgetoff (View->v_win, G_TONL|G_ECHO|G_EDIT|G_INSERT|G_ADDNL);
	waddoff (View->v_win, A_SCROLL);

	/*
	 *	Atualização do vídeo
	 */
	pg_remap (base);
	pg_move (base, order, true);

}	/* end insert */
