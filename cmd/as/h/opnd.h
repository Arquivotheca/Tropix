/*
 ****************************************************************
 *								*
 *			opnd.h					*
 *								*
 *	Definições relativas aos operandos			*
 *								*
 *	Versão	3.0.6, de 03.09.94				*
 *		4.4.0, de 17.04.03				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Tipos de Operandos **************************************
 */
#define	INVAL	0x0000000	/* Inválido */

#define	RA	0x0000001	/* Registro  r0 */
#define	R3	0x0000002	/* Registros r1, r2, r3 */
#define	R4	0x0000004	/* Registros sp, fp, r4, r5 */
#define	HA	0x0000008	/* Registro  h0 */
#define	H3	0x0000010	/* Registros h1, h2, h3 */

#define	R0	RA		/* Registro  r0 */
#define	H0	HA		/* Registro  h0 */

#define	GREG8	(RA|R3|HA|H3)	/* Registros de uso geral (8 bits) */
#define	GREG16	(RA|R3|R4)	/* Registros de uso geral (16 bits) */
#define	GREG32	(RA|R3|R4)	/* Registros de uso geral (32 bits) */
#define	GREG	(RA|R3|R4|HA|H3) /* Registros de uso geral (8/16/32 bits) */

#define	F0	0x0000020	/* f0 */
#define	F1	0x0000040	/* f1 */
#define	F7	0x0000080	/* Registros de pto. flutuante f2 a f7 */
#define	FREG	(F7|F1|F0)	/* Registros de pto. flutuante f0 a f7 */

#define	CREG	0x0000100	/* cr0, cr2, cr3 */
#define	DREG	0x0000200	/* dr0-3, dr6, dr7 */
#define	TREG	0x0000400	/* tr6, tr7 */
#define	SREG	0x0000800	/* cs, ds, ss, es, fs, gs */

#if (0)	/*************************************/
#define	F0	0x0001000	/* f0 */
#define	F7	0x0002000	/* Registros de pto. flutuante f1 a f7 */
#define	FREG	(F7|F0)		/* Registros de pto. flutuante f0 a f7 */
#endif	/*************************************/

#define	IMM_8	0x0004000	/* Imediato: # -128 <= expr <= +127 */
#define	IMM_8U	0x0008000	/* Imediato: # +128 <= expr <= +255 */
#define	IMM_16	0x0010000	/* Imediato: # -32768 <= expr <= +32767 */
#define	IMM_16U	0x0020000	/* Imediato: # +32768 <= expr <= +65536 */
#define	IMM_32	0x0040000	/* Imediato: # -4 Gb <= expr <= +4 Gb */

#define	IMMED8	  IMM_8			/* Imediato: 8 bits com sinal */
#define	IMMED8U	 (IMMED8|IMM_8U)	/* Imediato: 8 bits sem sinal */
#define	IMMED16	 (IMMED8U|IMM_16)	/* Imediato: 16 bits com sinal */
#define	IMMED16U (IMMED16|IMM_16U)	/* Imediato: 16 bits sem sinal */
#define	IMMED32	 (IMMED16U|IMM_32)	/* Imediato: 32 bits com/sem sinal */
#define	IMMED	  IMMED32		/* Imediato: qualquer um */

#define	FIMMED	0x0080000	/* Imediato real */

#define	EXPR	0x0100000	/* expr */

#define	INDIR	0x0200000	/* (base) */
#define	BDISP	0x0400000	/* expr(base) */
#define	INDEX	0x0800000	/* expr(base, index[@scale]) */
#define	NOBASE	0x1000000	/* expr(*,    index[@scale]) */
#define	ASTER	0x2000000	/* *expr */

/*
 ******	Operandos em Memória ************************************
 */
#define	MEM	(EXPR|INDIR|BDISP|INDEX|NOBASE)

#define	RM8	(GREG8|MEM)
#define	RM16	(GREG16|MEM)
#define	RM32	(GREG32|MEM)
#define	RM	(GREG|MEM)

/*
 ******	Estrutura de um Operando ********************************
 */
#define	NOOPND	(OPND *)NULL

/*** typedef	struct opnd	OPND; ***/

struct opnd
{
	char	*op_begin;		/* Inicio do Operando na Linha */
	char	*op_end;		/* Fim do Operando na Linha */
	long	op_type;		/* Tipo do Operando (ver acima) */

	short	op_reg,			/* Registro base */
		op_index,		/* Registro indice */
		op_scale;		/* Fator de escalonamento do indice */

	ushort	op_exsize;		/* Tamanho de expressão */
	EXP	op_expr;		/* Expressão */

};

#define	OMITTED	0xFFFF

/*
 ****** Macros para identificação dos operandos *****************
 */
#define ISAREG(sp)	((sp)->s.s_type == REG)

/*
 ****** Macros para obtenção e validação de operandos ***********
 */
#define	TOKTEST(t,m)	if (token->l_major != t)		\
			{					\
				msg (ERRO, SHORTMARK, m);	\
				return (0);			\
			}

#define	GETOPND(v,e) 	gettoken ();					\
			if ((v = getoperand (opnderr = &(e))) == INVAL)	\
				return (0);

#define	GETOPND2(e,v)	gettoken ();			\
			if (getoperand (e) == INVAL)	\
				return (v);

#define	FGETOPND(v,e)	gettoken ();	v = getoperand (&(e));

#define	FFGETOPND(e)	gettoken ();	getoperand (&(e));

/*
 ****** Protótipos e variáveis externas *************************
 */
extern int	reg_value_vec[];	/* Valores associados aos registros */
