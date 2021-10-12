/*
 ****************************************************************
 *								*
 *			lex.h					*
 *								*
 *	Definições e estruturas usadas pelo scanner		*
 *								*
 *	Versão	1.0.0, de 14.02.86				*
 *		3.0.0, de 26.02.94				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Categorias léxicas **************************************
 */
#define	INV	0		/* Caractere Invalido */
#define	SEPAR	1		/* Separador */
#define	LETTER	2		/* Letra */
#define	DIGIT	3		/* Digito */
#define	APOST	4		/* Apostrofo */
#define	QUOTE	5		/* Aspas */
#define	USYM	6		/* Simbolo Unario (de um caractere) */
#define	BSYM	7		/* Simbolo Unario ou Duplo */

/*
 ******	Macros para obtenção da categoria & índice **************
 */
#define	CATEG(t)	(categtb[*(t)])

#define	CINDEX(t)	(indextb[*(t)])

/*
 ******	Vetor de símbolos unários *******************************
 */
typedef	struct
{
	MAJOR	u_major;	/* major do delimitador */
	MINOR	u_minor;	/* minor do delimitador */

}	UNSYM;

/*
 ******	Definições globais **************************************
 */
extern const char	categtb[];	/* Tabela de categorias dos catacteres */
extern const char	indextb[];	/* Tabela de índices dos caracteres */
extern const UNSYM	unsym[];	/* Vetor de Simbolos Unarios */

