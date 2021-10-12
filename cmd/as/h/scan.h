/*
 ****************************************************************
 *								*
 *			scan.h					*
 *								*
 *	Definições Usadas pelo Scanner				*
 *								*
 *	Versão	1.0.0, de 21.01.86				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Macros **************************************************
 */
#define	LINESZ	1024		/* Tamanho da Linha */

/*
 ******	Minors dos tokens ***************************************
 *
 *	Especifica qual das varias alternativas de um Major
 *	Este valor é retornado em token.l_minor
 *	Os valores abaixo indexam a tabela de precedências em "expr.c"
 */
typedef	enum
{
	/*
	 *	Minor de Majors sem opções
	 */
	N_NULL	=	-1,	/* Minor de Majors sem opções */

	/*
	 *	Minor de Operadores
	 */
	N_OR,			/* | */
	N_EOR,			/* ^ */
	N_AND,			/* & */
	N_LSHIFT,		/* << */
	N_RSHIFT,		/* >> */
	N_PLUS,			/* + */
	N_MINUS,		/* - */
	N_MUL,			/* * */
	N_DIV,			/* / */
	N_MOD,			/* % */
	N_NOT,			/* ~ */

	N_BYTE,			/* :b ou @b */
	N_WORD,			/* :w ou @w */
	N_LONG,			/* :l ou @l */
	N_QUAD,			/* @q */

	N_EQ,			/* == */
	N_NE,			/* != */
	N_GT,			/* >  */
	N_GE,			/* >= */
	N_LT,			/* <  */
	N_LE,			/* <= */

	N_OROR,			/* || */
	N_ANDAND,		/* && */
	N_NOTNOT		/* ! */

}	MINOR;

/*
 ******	Estrutura de saida do analisador léxico	*********
 */
/*** typedef	struct token	TOKEN; ***/

struct token
{
	MAJOR	l_major;	/* Major do Token */
	MINOR	l_minor;	/* Minor do Token */
	char	*l_begin;	/* Coluna inicial do token na linha */
	char	*l_end;		/* Coluna final do token na linha */
	long	l_line;		/* Número da linha do token */

	long	l_ival;		/* Valor Inteiro */
	double	l_rval;		/* Valor Real */

	char	*l_str;		/* Ponteiro para a cadeia */
	char	*l_name;	/* Ponteiro para o identificador */

	int	l_len;		/* Comprimento da cadeia/identificador */

	char	*l_area;	/* Área alocada */
	int	l_area_sz;	/* Comprimento da área alocada */
};

/*
 ******	Variáveis do analisador léxico **************************
 */
extern	TOKEN	*token,		/* Token Atual e Look-Ahead */
		*latoken;
extern	char	line[];		/* Buffer para a linha */
