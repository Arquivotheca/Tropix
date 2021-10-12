/*
 ****************************************************************
 *								*
 *			op_code.h				*
 *								*
 *	Estrutura da tabela de códigos de operadores		*
 *								*
 *	Versão	1.0.00, de 17.09.92				*
 *		3.0.10, de 13.04.95				*
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


#define	NOFLAG		0	/* Nenhum indicador */

/*
 ******* Indicadores dos OPCODEs ********************************
 */
#define	IQ		0x01	/* Valor do operando imediato já no passo 1 */
#define	NOSKIP		0x02	/* Não pule mesmo dentro de ".if" falso */

/*
 ******* Indicadores dos endereços ******************************
 */
#define	WP		0x01	/* Deve acrescentar 0x66 se 16 bits */
#define	LP		0x02	/* Deve acrescentar 0x66 se 32 bits */
#define	getOPND3	0x04	/* Obtém o terceiro operando */
#define	WAITP		0x08	/* Deve 0x9B (wait para flutuante) */

/*
 ****** Tipos de Instruções *************************************
 */
#define	I_REGULAR	0	/* Instrução Regular */
#define	I_ANOMALOUS	1	/* Instrução Anômala */
#define	I_PSEUDO	2	/* Pseudo-instrução */
#define	I_PREFIX	3	/* Prefixo */
#define	I_FLOAT		4	/* Ponto Flutuante */

/*
 ******* Entrada da tabela de instrucoes ************************
 */
/*** typedef	struct instr	OPCODE; ***/

struct	instr
{
	char	o_name[SIDSZ+1];	/* Nome da instrução */

	schar	o_type;			/* Tipo (regular, pseudo, ...) */
	schar	o_group;		/* Número do grupo da instrução */
	short	o_flags;		/* Indicadores da instrução */

	OPADDR	*o_begin;		/* Primeiro e último endereçamentos */
	OPADDR	*o_end;
	OPCODE	*o_link;		/* Cadeia de colisões */
};

#define	NOOPCODE (OPCODE *)NULL

/*
 ******* Estrutura de um Padrão: opcode + endereçamento *********
 */
/*** typedef	struct opaddr	OPADDR; ***/

struct opaddr
{
	char	o_opcode;	/* Codigo da instrução */
	char	o_opexten;	/* Extensão do opcode */
	char	o_group;	/* Número do grupo do endereçamento */
	char	o_flags;	/* Indicadores do endereçamento */

	ulong	o_opnd1;	/* Os dois operandos */
	ulong	o_opnd2;
};

#define	NOOPADDR (OPADDR *)NULL
