/*
 ****************************************************************
 *								*
 *			commands.h				*
 *								*
 *	Definição da estrutura para edição por linha		*
 *								*
 *	Versão	2.0.0, de 12.05.88				*
 *		3.0.0, de 23.03.95				*
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

/*
 *	Os seguintes arquivos devem ser incluídos antes deste:
 *
 *		<curses.h>
 *		"../h/defs.h"
 *		"../h/text.h"
 */

#define	H_COMMANDS	/* Para declarar as funções */

/*
 ******	Substituições *******************************************
 */
#define  RSSZ	20	/* Replacement specials size */
#define  RTSZ	255	/* Replacement text size */

typedef	struct
{
	char	*r_specials[RSSZ];
	char	r_text[RTSZ];

}	REPL;

#define	NOREPL	(REPL *)0

/*
 ******	Estrutura para edição por linhas ************************
 */
typedef	struct
{
	BASE	*c_cur;

	char	*c_search;
	char	*c_ssearch;
	char	*c_subst;

	REPL	*c_repl;

}	COMM;

#define	NOCOMM	(COMM *)0

/*
 ******	Valores retornados pela função "Commands" ***************
 */
#define	COM_QUIT	(-1)
#define	COM_NORMAL	0
