/*
 ****************************************************************
 *								*
 *			expr.h					*
 *								*
 *	Definições Acerca de Expressões				*
 *								*
 *	Versão	1.0.0, de 26.02.86				*
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
 ******	Pilha de operações **************************************
 */
#define	ESTACKSZ	20		/* Profundidade Maxima da Stack */

/*** typedef	struct estack	ESTACK; ***/

struct estack
{
	long	e_value;		/* Operando */
	MINOR	e_opr;			/* Operador */
	short	e_type;			/* Tipo do Operando */
	short	e_prec;			/* Precedencia do Operador */
	SYMTB	*e_ref;			/* Operando é identificador */
};

/*
 ******	Estrutura de uma expressão ******************************
 */

/*** typedef	struct exp	EXP; ***/

struct exp
{
	char	*ex_begin;		/* Inicio da Expressão */
	char	*ex_end;		/* Fim da Expressão */
	long	ex_value;		/* Valor da Expressão */
	short	ex_type;		/* Tipo da Expressão */
	short	ex_nopnd;		/* Número de Operandos */
	SYMTB	*ex_ref;		/* Referencia a um Label (para SDI) */
	double	ex_rval;		/* Valor Real */
};

/*
 ******	Máscara sintática  **************************************
 */
#define	E_END		0x01		/* Fim da Expressão */
#define	E_LC		0x02		/* Abre-chave */
#define	E_RC		0x04		/* Fecha-chave */
#define	E_OPND		0x08		/* Operando */
#define	E_OPR		0x10		/* Operador */
#define	E_UNOPR		0x20		/* Operador Unario */
