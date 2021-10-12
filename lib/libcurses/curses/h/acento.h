/*
 ****************************************************************
 *								*
 *			acento.h				*
 *								*
 *	Definição de acentos					*
 *								*
 *	Versão	2.0.0, de 15.03.88				*
 *		3.0.0, de 22.03.95				*
 *								*
 *	Modulo: acento.h					*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definição de acentos ************************************
 */
typedef	struct
{
	int	a_fonte;	/* Fonte: pares */
	int	a_destino;	/* Destino: resultado da conversao */

}	ACENTO;

#define	OR(a, b)	((a << 8) | b)

#define		isaccent(c)	(c == ',' || c == ':' || c == '\'' ||	\
					c == '`' || c == '^' ||		\
					c == '~' || c == '=')
