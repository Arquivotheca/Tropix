/*
 ****************************************************************
 *								*
 *			h/ed.h					*
 *								*
 *	Definições Gerais para todo ED				*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
 *		3.0.0, de 26.04.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	ED_H

/*
 ******	Macros comuns *******************************************
 */
#define	EVER	;;
#define	elif	else if

/*
 ****** O Booleano **********************************************
 */
typedef enum { false, true } bool;

/*
 ******	Tamanhos ************************************************
 */
#define LSIZE	256		/* Linha do texto */
#define NSIZE	 64		/* Nome de arquivo */
#define RSIZE	128		/* Cadeia para Substituicão */

/*
 ******	Lista Circular de linhas mantidas por ED ****************
 */
typedef	struct	base		BASE;

struct	base
{
	BASE	*ant;	/* Base anterior */
	BASE	*pos;	/* Base posterior */
	char	*ch;	/* Cadeia de caracteres da linha */
};

#define		NOBASE		(BASE *)0

/*
 ******	Tipo Tamanho ********************************************
 */
typedef	struct
{
	int	s_nc;	/* No. de caracteres */
	int	s_nl;	/* No. de linhas */

}	SIZE;

/*
 ******	Caracteristicas do Texto em edicão **********************
 */
typedef	struct
{
	SIZE	t_size;
	int	t_flag;		/* Indicadores, abaixo */
	char	t_name[NSIZE+1];

}	TEXT;

#define   T_CHANGED	1	/* O Texto já foi alterado */
#define   T_RDONLY	2	/* O Texto é apenas para leitura */

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

/*
 ******	 Expressões Regulares: Automatos ************************
 */
#define	AUTOSZ	400

typedef	char	AUTO[AUTOSZ];
