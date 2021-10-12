/*
 ****************************************************************
 *								*
 *			lex.h					*
 *								*
 *	Definições peculiares ao analisador léxico		*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 22.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Categorias Léxicas					*
 ****************************************************************
 */
#define	INV	0		/* Caractere Inválido */
#define	SEPAR	1		/* Separador */
#define	LETTER	2		/* Letra */
#define	DIGIT	3		/* Dígito */
#define	QUOTE	4		/* Aspas */
#define	USYM	5		/* Simbolo Unário (de um caractere) */
#define	BSYM	6		/* Simbolo Unário ou Duplo */
#define	APOST	7		/* Apóstrofe */

/*
 ****************************************************************
 *	Macros para Obtenção da Categoria & Índice		*
 ****************************************************************
 */
#define	CATEG(cp)	categ[*(cp)]

#define	CINDEX(cp)	cindex[*(cp)]

extern const char	categ[];	/* Tabela de categorias */

/*
 ****************************************************************
 *	Vetor com Símbolos Unários				*
 ****************************************************************
 */
typedef	struct
{
	MAJOR	u_major;	/* MAJOR do Símbolo */
	MINOR	u_minor;	/* MINOR do Símbolo */

}	UNSYM;
