/*
 ****************************************************************
 *								*
 *			data.c					*
 *								*
 *	Definição das variáveis globais de "curses"		*
 *								*
 *	Versão	2.0.0, de 15.03.88				*
 *		3.0.0, de 02.11.95				*
 *								*
 *	Módulo: data						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <termio.h>
#include <stdio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/acento.h"
#include "../h/local.h"

/*
 ****************************************************************
 *	Dimensões do terminal					*
 ****************************************************************
 */
entry int	LINES = 0;
entry int	COLS = 0;

/*
 ****************************************************************
 *	Janela padrão						*
 ****************************************************************
 */
entry WINDOW	*stdwin = NOWINDOW;

/*
 ****************************************************************
 *	Caracteres semi-gráficos				*
 ****************************************************************
 */
entry chtype	ACS_ULCORNER = 0,
		ACS_LLCORNER = 0,
		ACS_URCORNER = 0,
		ACS_LRCORNER = 0,
		ACS_RTEE = 0,
		ACS_LTEE = 0,
		ACS_BTEE = 0,
		ACS_TTEE = 0,
		ACS_HLINE = 0,
		ACS_VLINE = 0,
		ACS_PLUS = 0,
		ACS_S1 = 0,
		ACS_S9 = 0,
		ACS_DIAMOND = 0,
		ACS_CKBOARD = 0,
		ACS_DEGREE = 0,
		ACS_PLMINUS = 0,
		ACS_BULLET = 0,
		ACS_LARROW = 0,
		ACS_RARROW = 0,
		ACS_DARROW = 0,
		ACS_UARROW = 0,
		ACS_BOARD = 0,
		ACS_LANTERN = 0,
		ACS_BLOCK = 0;

/*
 ****************************************************************
 *	Lista de terminais (interna)				*
 ****************************************************************
 */
entry TERM	*_firstterm = NOTERM;	/* Terminal 0 */
entry TERM	*_curterm = NOTERM;	/* Terminal Corrente */

/*
 ****************************************************************
 *	Pool de janelas	(interno)				*
 ****************************************************************
 */
entry WINDOW	*_windows;

/*
 ****************************************************************
 *	Tabela de acentos (interno)				*
 ****************************************************************
 */
entry const ACENTO	Acentos[] =
{
	OR ('~', 'A'),	0xC3,	/* Ã */
	OR ('~', 'a'),	0xE3,	/* ã */
	OR ('~', 'O'),	0xD5,	/* Õ */
	OR ('~', 'o'),	0xF5,	/* õ */
	OR (',', 'C'),	0xC7,	/* Ç */
	OR (',', 'c'),	0xE7,	/* ç */
	OR (',', 'z'),	0xDF,	/* ß */
	OR ('\'','A'),	0xC1,	/* Á */
	OR ('\'','a'),	0xE1,	/* á */
	OR ('\'','E'),	0xC9,	/* É */
	OR ('\'','e'),	0xE9,	/* é */
	OR ('\'','I'),	0xCD,	/* Í */
	OR ('\'','i'),	0xED,	/* í */
	OR ('\'','O'),	0xD3,	/* Ó */
	OR ('\'','o'),	0xF3,	/* ó */
	OR ('\'','U'),	0xDA,	/* Ú */
	OR ('\'','u'),	0xFA,	/* ú */
#if (0)	/*******************************************************/
	OR ('\'','C'),	0xC7,	/* Ç */
	OR ('\'','c'),	0xE7,	/* ç */
	OR ('\'','z'),	0xDF,	/* ß */
#endif	/*******************************************************/
	OR ('^', 'A'),	0xC2,	/* Â */
	OR ('^', 'a'),	0xE2,	/* â */
	OR ('^', 'E'),	0xCA,	/* Ê */
	OR ('^', 'e'),	0xEA,	/* ê */
	OR ('^', 'I'),	0xCE,	/* Î */
	OR ('^', 'i'),	0xEE,	/* î */
	OR ('^', 'O'),	0xD4,	/* Ô */
	OR ('^', 'o'),	0xF4,	/* ô */
	OR ('^', 'U'),	0xDB,	/* Û */
	OR ('^', 'u'),	0xFB,	/* û */
	OR ('`', 'A'),	0xC0,	/* À */
	OR ('`', 'a'),	0xE0,	/* à */
	OR ('`', 'E'),	0xC8,	/* È */
	OR ('`', 'e'),	0xE8,	/* è */
	OR ('`', 'I'),	0xCC,	/* Ì */
	OR ('`', 'i'),	0xEC,	/* ì */
	OR ('`', 'O'),	0xD2,	/* Ò */
	OR ('`', 'o'),	0xF2,	/* ò */
	OR ('`', 'U'),	0xD9,	/* Ù */
	OR ('`', 'u'),	0xF9,	/* ù */
	OR (':', 'A'),	0xC4,	/* Ä */
	OR (':', 'a'),	0xE4,	/* ä */
	OR (':', 'E'),	0xCB,	/* Ë */
	OR (':', 'e'),	0xEB,	/* ë */
	OR (':', 'I'),	0xCF,	/* Ï */
	OR (':', 'i'),	0xEF,	/* ï */
	OR (':', 'O'),	0xD6,	/* Ö */
	OR (':', 'o'),	0xF6,	/* ö */
	OR (':', 'U'),	0xDC,	/* Ü */
	OR (':', 'u'),	0xFC,	/* ü */
#if (0)	/*******************************************************/
	OR ('=', 'a'),	0xAA,	/* ª */
	OR ('=', 'o'),	0xBA,	/* º */
#endif	/*******************************************************/
	0,		0
};
