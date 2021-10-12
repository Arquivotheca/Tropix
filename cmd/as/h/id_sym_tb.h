/*
 ****************************************************************
 *								*
 *			id_sym_tb.h				*
 *								*
 *	Estrutura da tabela de símbolos				*
 *								*
 *	Versão	1.0.0, de 28.01.86				*
 *		3.0.6, de 04.09.94				*
 *								*
 *	Módulo: AS-68000/10/20/30				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****** Definições gerais **************************************
 */
#define	NOSYMTB		(SYMTB *)NULL

#define	LABSZ		30

/*
 ****** Elemento da tabela de símbolos **************************
 */
/*** typedef	struct symbol	SYMTB; ***/

struct symbol
{
	SYMTB		*s_next;	/* Próxima definição */
	SYMTB		*s_def;		/* Próxima definição */
	short		s_line;		/* Linha em que foi definido */
	short		s_index;	/* Número do Simbolo */
	double		s_rval;		/* Valor Real */
	SYMTB		*s_link;	/* Lista de colisão */
	SYM		s;		/* Símbolo */
};

/*
 ****** Tipo da busca *******************************************
 */

/*** typedef enum sdef	SDEF; ***/

enum sdef { S_DEFINE, S_SEARCH };

/*
 ****** Tipos De Simbolos (usados somente pelo assembler) *******
 */
#define	CONST	0x05		/* Pseudo-seção ".const" */
#define	ERR	0x0A		/* Erro em Expressão */
#define	REAL	0x0B		/* Expressão tem valor Real */

/*
 ****** Vetor auxiliar para alinhamento dos rótulos *************
 */
extern SYMTB	*labtb[], **labp;
