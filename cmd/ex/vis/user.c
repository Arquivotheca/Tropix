/*
 ****************************************************************
 *								*
 *			user.c					*
 *								*
 *	Comunicação com o usuário				*
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
#include <string.h>
#include <curses.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/visual.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Locais					*
 ****************************************************************
 */
static int	i;
static char	devolvido = '\0';
static char	comando[80];

/*
 ****************************************************************
 *	Inicia Entrada do Teclado				*
 ****************************************************************
 */
void
vis_initget (void)
{
	strcpy (comando, ": "); i = 2;

	if (Verbose)
		{ werase (View->v_err); waddstr (View->v_err, comando); }

}	/* end vis_initget */

/*
 ****************************************************************
 *	Leitura de um caractere					*
 ****************************************************************
 */
char
vis_chget (void)
{
	int		ch;

	/*
	 *	Pega o prox. caractere
	 */
	if (devolvido)
	{
		comando[i] = ch = devolvido;
		devolvido = '\0';
	}
	else
	{
		if ((ch = wgetch (View->v_win)) == ERR)
			return (ERR);

		comando[i] = ch;
	}

	comando[++i] = '\0';

	/*
	 *	Verbose: acrescenta o caractere na janela
	 */
	if (Verbose)
	{
		waddch (View->v_err, ch);
		touchwin (View->v_err);
		wrefresh (View->v_win);
	}

	return (ch);

}	/* end vis_getch */

/*
 ****************************************************************
 *	Devolve um caractere					*
 ****************************************************************
 */
void
vis_ungetch (int c)
{
	comando[--i] = '\0';
	devolvido = c;

}	/* end vis_ungetch */

/*
 ****************************************************************
 *	Leitura de uma cadeia					*
 ****************************************************************
 */
void
vis_getstr (char *str, char end, bool keep)
{
	char		*s;
	bool		special;

	special = false;

	/*
	 *	Escreve prompt
	 */
	wrefon  (View->v_err, R_CURSOR);
	werase (View->v_err);
	waddstr (View->v_err, comando);

	if (Verbose)
		wrefresh (View->v_err);
	else
		touchwin (View->v_err);

	/*
	 *	Leitura
	 */
	wgetstr (View->v_err, str);
	wrefoff (View->v_err, R_CURSOR);

	/*
	 *	Detecção do fim da cadeia
	 */
	for (s = str; *s; s++)
	{
		if (*s == '\n')
			*s = '\0';

		if (*s == end && !special)
			*s = '\0';

		/*
		 *	Tratamento de '\'
		 */
		if (*s == '\\')
			special = !special;
		else
			special = false;
	}

	strcat (comando, str);

	if (!Verbose && !keep)
		untouchwin (View->v_err);

}	/* end vis_getstr */

/*
 ****************************************************************
 *	Mensagens de erro					*
 ****************************************************************
 */
void
vis_error (char *s2, char *s1)
{
	/*
	 *	Escreve e mostra a mensagem
	 */
	werase (View->v_err);
	wprintw (View->v_err, "%s :", comando);

	if (s2 == NOSTR)
		/* vazio */;
	elif (s1 == NOSTR)
		wprintw (View->v_err, " %s", s2);
	else
		wprintw (View->v_err, " %s \"%s\"", s1, s2);

	touchwin (View->v_err);

	/*
	 *	Retorna o cursor ao texto
	 */
	wrefresh (View->v_win);

	/*
	 *	Lê e guarda o prox. caractere
	 */
	vis_ungetch (wgetch (View->v_win));

	/*
	 *	Retira a mensagem
	 */
	untouchwin (View->v_err);

	vis_initget ();

}	/* end vis_error */

/*
 ****************************************************************
 *	Mensagens						*
 ****************************************************************
 */
void
vis_message (char *s)
{
	/*
	 *	Escreve e mostra a mensagem
	 */
	werase (View->v_err);
	waddstr (View->v_err, s);
	touchwin (View->v_err);

	/*
	 *	Retorna o cursor ao texto
	 */
	wrefresh (View->v_win);

	/*
	 *	Lê e guarda o prox. caractere
	 */
	vis_ungetch (wgetch (View->v_win));

	/*
	 *	Retira a mensagem
	 */
	untouchwin (View->v_err);

}	/* end vis_message */

/*
 ****************************************************************
 *	Termina a entrada do teclado				*
 ****************************************************************
 */
void
vis_endget (void)
{
	if (Verbose)
		untouchwin (View->v_err);

}	/* end vis_endget */
