/*
 ****************************************************************
 *								*
 *			local.h					*
 *								*
 *	Declarações Locais 					*
 *								*
 *	Versão	2.0.0, de 16.03.88				*
 *		3.0.0, de 22.03.95				*
 *								*
 *	Modulo: local.h						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 *	Os seguintes arquivos devem ser incluidos antes deste:
 *
 *		<curses.h>
 */

/*
 ******	Janelas disponíveis *************************************
 */
#define		_NWINDOWS	20	/* No. Max. de Janelas */

extern	WINDOW	*_windows;		/* Janelas */
