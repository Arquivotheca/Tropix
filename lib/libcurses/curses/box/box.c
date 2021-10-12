/*
 ****************************************************************
 *								*
 *			box.c					*
 *								*
 *	Desenha o contorno de uma janela			*
 *								*
 *	Versão	2.0.0, de 20.04.88				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: box						*
 *		libcurses/curses				*
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

#include "../../terminfo/h/defs.h"

/*
 ****************************************************************
 *	Desenha o contorno de uma janela			*
 ****************************************************************
 */
int
wbox (register WINDOW *win, const char *name, int vert, int hor)
{
	register int		l, c;
	register const char	*source;
	register chtype		*dest;
	register int		length;

	/*
	 *	Verifica o comprimento do nome
	 */
	if (name != NOSTR)
	{
		length = strlen (name);

		if (length > win->w_maxc - 6)
			return (ERR);
	}
	else
	{
		length = 0;
	}

	/*
	 *	Escreve a linha horizontal superior
	 */
	l = 0; 	c = 0;

	win->w_text[l][c] = hor ? hor : ACS_ULCORNER;

	for (c++; c < win->w_maxc; c++)
		win->w_text[l][c] = hor ? hor : ACS_HLINE;

	win->w_text[l][c] = hor ? hor: ACS_URCORNER;

	/*
	 *	Escreve a linha horizontal inferior
	 */
	l = win->w_maxl; c = 0;

	win->w_text[l][c] = hor ? hor : ACS_LLCORNER;

	for (c++; c < win->w_maxc; c++)
		win->w_text[l][c] = hor ? hor : ACS_HLINE;

	win->w_text[l][c] = hor ? hor : ACS_LRCORNER;

	/*
	 *	Escreve a linha vertical esquerda
	 */
	c = 0;

	for (l = 1; l < win->w_maxl; l++)
		win->w_text[l][c] = vert ? vert : ACS_VLINE;

	/*
	 *	Escreve a linha vertical direita
	 */
	c = win->w_maxc;

	for (l = 1; l < win->w_maxl; l++)
		win->w_text[l][c] = vert ? vert : ACS_VLINE;

	/*
	 *	Marca o vetor de alterações
	 */
	for (l = win->w_maxl; l >= 0; l--)
		win->w_changed[l] = 1;

	/*
	 *	Marca esta janela como tendo contorno
	 */
	set (win->w_flags, W_BOX);

	/*
	 *	Escreve nome na janela
	 */
	if (length > 0)
	{
		source = name;
		dest = &win->w_text[0][3];

		while (*dest++ = (chtype)*source++)
			/* vazio */;

		win->w_text[0][2] = ' ';
		win->w_text[0][length + 3] = ' ';
	}

	/*
	 *	Verifica a posição corrente
	 */
	if   (win->w_curl == 0)
		win->w_curl++;
	elif (win->w_curl == win->w_maxl)
		win->w_curl--;

	if   (win->w_curc == 0)
		win->w_curc++;
	elif (win->w_curc == win->w_maxc)
		win->w_curc--;

	return (ERR);

}	/* end wbox */

/*
 ****************************************************************
 *	Apaga o contorno de uma janela				*
 ****************************************************************
 */
void
wclrbox (register WINDOW *win)
{
	register int	l, c;

	/*
	 *	Escreve a linha horizontal superior
	 */
	l = 0; c = 0;

	win->w_text[l][c] = ' ';

	for (c++; c < win->w_maxc; c++)
		win->w_text[l][c] = ' ';

	win->w_text[l][c] = ' ';

	/*
	 *	Escreve a linha horizontal inferior
	 */
	l = win->w_maxl; c = 0;

	win->w_text[l][c] = ' ';

	for (c++; c < win->w_maxc; c++)
		win->w_text[l][c] = ' ';

	win->w_text[l][c] = ' ';

	/*
	 *	Escreve a linha vertical esquerda
	 */
	c = 0;

	for (l = 1; l < win->w_maxl; l++)
		win->w_text[l][c] = ' ';

	/*
	 *	Escreve a linha vertical direita
	 */
	c = win->w_maxc;

	for (l = 1; l < win->w_maxl; l++)
		win->w_text[l][c] = ' ';

	/*
	 *	Marca o vetor de Alterações
	 */
	for (l = win->w_maxl; l >= 0; l--)
		win->w_changed[l] = 1;

	/*
	 *	Marca esta janela como não tendo contorno
	 */
	reset (win->w_flags, W_BOX);

}	/* end wclrbox */
