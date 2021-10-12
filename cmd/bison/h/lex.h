/*
 ****************************************************************
 *								*
 *			lex.h					*
 *								*
 *	Token type definitions for bison's input reader		*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Token type definitions for bison's input reader		*
 ****************************************************************
 */
#define	ENDFILE		0
#define	IDENTIFIER	1
#define	COMMA		2
#define COLON		3
#define	SEMICOLON	4
#define	BAR		5
#define LEFT_CURLY	6
#define TWO_PERCENTS	7
#define PERCENT_LEFT_CURLY	8
#define	TOKEN		9
#define	NTERM		10
#define GUARD	       11
#define	TYPE	       12
#define	UNION	       13
#define START	       14
#define LEFT	       15
#define RIGHT	       16
#define NONASSOC       17
#define PREC	       18
#define SEMANTIC_PARSER 19
#define PURE_PARSER    20
#define TYPENAME       21
#define NUMBER	       22
#define EXPECT	       23
#define	ILLEGAL	       24

#define	MAXTOKEN	1024
