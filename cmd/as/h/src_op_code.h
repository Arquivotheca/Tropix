/*
 ****************************************************************
 *								*
 *			src_op_code.h				*
 *								*
 *	Estrutura da tabela de códigos de operadores fonte	*
 *								*
 *	Versão	1.0.0, de 17.09.92				*
 *		3.0.6, de 03.09.94				*
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
 ****** Elemento da tabela de opcodes fonte *********************
 */
#define	MAX_ADDR	12		/* Número máximo de endereços */
#define	MAX_NAME	5		/* Número máximo de nomes */

typedef	struct
{
	char	o_name[MAX_NAME][SIDSZ+1];

	schar	o_type;			/* Tipo (regular, pseudo, ...) */
	schar 	o_group;		/* No. do grupo da instrução */
	schar 	o_flags;		/* Indicadores */
	schar	o_pri;			/* Prioridade */

	OPADDR	o_a[MAX_ADDR];		/* Padrões de endereçamento */

}	SRC_OPCODE;
