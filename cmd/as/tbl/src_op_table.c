/*
 ****************************************************************
 *								*
 *			src_op_table.c				*
 *								*
 *	Tabela de operadores para o INTEL 80386/486/PENTIUM	*
 *								*
 *	Versão	1.0.0, de 17.09.92				*
 *		4.0.0, de 16.11.00				*
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

#include <sys/types.h>

#include <a.out.h>

#include "../h/common.h"
#include "../h/id_sym_tb.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/op_code.h"
#include "../h/src_op_code.h"

/*
 ******	A tabela de operadores para INTEL 80386/486/PENTIUM *****
 */
const SRC_OPCODE src_op_table[] =
{
/*
 *		{ {"nome"},		   TIPO,     GRUPO, IND, PRI,
 *	 OP,   EXT,   GROUPO, FLAGS,	 	OPND1,		OPND2	},
 */

/*
 ******* Instrução MOV (pág 17-109 a 111) ***********************
 */
		{ {"movb"},		I_REGULAR,	0, NOFLAG, 1,
	0xA0,	0,	15,	0,	 	EXPR,		R0,
	0xA2,	0,	16,	0,	 	R0,		EXPR,
	0x88,	0,	17,	0,		GREG8,		RM8,	
	0x8A,	0,	19,	0,		RM8,		GREG8,
	0xB0,	0,	21,	0,		IMMED8U,	GREG8,
	0xC6,	0,	24,	0,		IMMED8U,	RM8	},

		{ {"movw"},		I_REGULAR,	0, NOFLAG, 1,
	0xA1,	0,	15,	WP,	 	EXPR,		R0,
	0xA3,	0,	16,	WP, 		R0,		EXPR,
	0x89,	0,	17,	WP,		GREG16,		RM16,	
	0x8B,	0,	19,	WP,		RM16,		GREG16,
	0xB8,	0,	21,	WP,		IMMED16U,	GREG16,
	0xC7,	0,	24,	WP,		IMMED16U,	RM16,
	0x8C,	0,	17,	0,		SREG,		RM16,
	0x8E,	0,	19,	0,		RM16,		SREG	},

		{ {"movl"},		I_REGULAR,	0, NOFLAG, 1,
	0xA1,	0,	15,	LP, 		EXPR,		R0,
	0xA3,	0,	16,	LP, 		R0,		EXPR,
	0x89,	0,	17,	LP,		GREG32,		RM32,	
	0x8B,	0,	19,	LP,		RM32,		GREG32,
	0xB8,	0,	21,	LP,		IMMED32,	GREG32,
	0xC7,	0,	24,	LP,		IMMED32,	RM32,
	0x20,	0,	18,	0,		CREG,		GREG32,
	0x22,	0,	20,	0,		GREG32,		CREG,
	0x21,	0,	18,	0,		DREG,		GREG32,
	0x23,	0,	20,	0,		GREG32,		DREG,
	0x24,	0,	18,	0,		TREG,		GREG32,
	0x26,	0,	20,	0,		GREG32,		TREG	},

/*
 ******* Instrução MOVSx (pág 17-114) ***************************
 */
		{ {"movsbw"},		I_REGULAR,	0, NOFLAG, 1,
	0xBE,	0,	20,	WP,		RM8,		GREG16	},

		{ {"movsbl"},		I_REGULAR,	0, NOFLAG, 1,
	0xBE,	0,	20,	LP,		RM8,		GREG32	},

		{ {"movswl"},		I_REGULAR,	0, NOFLAG, 1,
	0xBF,	0,	20,	0,		RM16,		GREG32	},

/*
 ******* Instrução MOVZx (pág 17-115) ***************************
 */
		{ {"movubw", "movzbw"},	I_REGULAR,	0, NOFLAG, 1,
	0xB6,	0,	20,	WP,		RM8,		GREG16	},

		{ {"movubl", "movzbl"},	I_REGULAR,	0, NOFLAG, 1,
	0xB6,	0,	20,	LP,		RM8,		GREG32	},

		{ {"movuwl", "movzwl"},	I_REGULAR,	0, NOFLAG, 1,
	0xB7,	0,	20,	0,		RM16,		GREG32	},

/*
 ******* Instrução ADC (pág 17-22) ******************************
 */
		{ {"adcb"},		I_REGULAR,	0, NOFLAG, 0,
	0x14,	0,	22,	0,		IMMED8U,	R0,
	0x80,	2,	25,	0,		IMMED8U,	RM8,
	0x10,	0,	17,	0,		GREG8,		RM8,
	0x12,	0,	19,	0,		RM8,		GREG8	},

		{ {"adcw"},		I_REGULAR,	0, IQ, 0,
	0x83,	2,	25,	WP,		IMMED8,		RM16,
	0x15,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	2,	25,	WP,		IMMED16U,	RM16,
	0x11,	0,	17,	WP,		GREG16,		RM16,
	0x13,	0,	19,	WP,	RM16,		GREG16	},

		{ {"adcl"},		I_REGULAR,	0, IQ, 0,
	0x83,	2,	25,	LP,		IMMED8,		RM32,
	0x15,	0,	22,	LP,		IMMED32,	R0,
	0x81,	2,	25,	LP,		IMMED32,	RM32,
	0x11,	0,	17,	LP,		GREG32,		RM32,
	0x13,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução ADD (pág 17-23) ******************************
 */
		{ {"addb"},		I_REGULAR,	0, NOFLAG, 1,
	0x04,	0,	22,	0,		IMMED8U,	R0,
	0x80,	0,	25,	0,		IMMED8U,	RM8,
	0x00,	0,	17,	0,		GREG8,		RM8,
	0x02,	0,	19,	0,		RM8,		GREG8	},

		{ {"addw"},		I_REGULAR,	0, IQ, 1,
	0x83,	0,	25,	WP,		IMMED8,		RM16,
	0x05,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	0,	25,	WP,		IMMED16U,	RM16,
	0x01,	0,	17,	WP,		GREG16,		RM16,
	0x03,	0,	19,	WP,		RM16,		GREG16	},

		{ {"addl"},		I_REGULAR,	0, IQ, 1,
	0x83,	0,	25,	LP,		IMMED8,		RM32,
	0x05,	0,	22,	LP,		IMMED32,	R0,
	0x81,	0,	25,	LP,		IMMED32,	RM32,
	0x01,	0,	17,	LP,		GREG32,		RM32,
	0x03,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução XADD (pág 25-315) ****************************
 */
		{ {"xaddb"},		I_REGULAR,	0, NOFLAG, 3,
	0xC0,	0,	18,	0,		GREG8,		RM8	},

		{ {"xaddw"},		I_REGULAR,	0, IQ, 3,
	0xC1,	0,	18,	WP,		GREG16,		RM16	},

		{ {"xaddl"},		I_REGULAR,	0, IQ, 3,
	0xC1,	0,	18,	LP,		GREG32,		RM32	},

/*
 ******* Instrução AND (pág 17-24) ******************************
 */
		{ {"andb"},		I_REGULAR,	0, NOFLAG, 1,
	0x24,	0,	22,	0,		IMMED8U,	R0,
	0x80,	4,	25,	0,		IMMED8U,	RM8,
	0x20,	0,	17,	0,		GREG8,		RM8,
	0x22,	0,	19,	0,		RM8,		GREG8	},

		{ {"andw"},		I_REGULAR,	0, IQ, 1,
	0x83,	4,	25,	WP,		IMMED8,		RM16,
	0x25,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	4,	25,	WP,		IMMED16U,	RM16,
	0x21,	0,	17,	WP,		GREG16,		RM16,
	0x23,	0,	19,	WP,		RM16,		GREG16	},

		{ {"andl"},		I_REGULAR,	0, IQ, 1,
	0x83,	4,	25,	LP,		IMMED8,		RM32,
	0x25,	0,	22,	LP,		IMMED32,	R0,
	0x81,	4,	25,	LP,		IMMED32,	RM32,
	0x21,	0,	17,	LP,		GREG32,		RM32,
	0x23,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução CMP (pág 17-50) ******************************
 */
		{ {"cmpb"},		I_REGULAR,	0, NOFLAG, 1,
	0x3C,	0,	22,	0,		IMMED8U,	R0,
	0x80,	7,	25,	0,		IMMED8U,	RM8,
	0x38,	0,	17,	0,		GREG8,		RM8,
	0x3A,	0,	19,	0,		RM8,		GREG8	},

		{ {"cmpw"},		I_REGULAR,	0, IQ, 1,
	0x83,	7,	25,	WP,		IMMED8,		RM16,
	0x3D,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	7,	25,	WP,		IMMED16U,	RM16,
	0x39,	0,	17,	WP,		GREG16,		RM16,
	0x3B,	0,	19,	WP,		RM16,		GREG16	},

		{ {"cmpl"},		I_REGULAR,	0, IQ, 1,
	0x83,	7,	25,	LP,		IMMED8,		RM32,
	0x3D,	0,	22,	LP,		IMMED32,	R0,
	0x81,	7,	25,	LP,		IMMED32,	RM32,
	0x39,	0,	17,	LP,		GREG32,		RM32,
	0x3B,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução OR (pág 17-23) *******************************
 */
		{ {"orb"},		I_REGULAR,	0, NOFLAG, 1,
	0x0C,	0,	22,	0,		IMMED8U,	R0,
	0x80,	1,	25,	0,		IMMED8U,	RM8,
	0x08,	0,	17,	0,		GREG8,		RM8,
	0x0A,	0,	19,	0,		RM8,		GREG8	},

		{ {"orw"},		I_REGULAR,	0, IQ, 1,
	0x83,	1,	25,	WP,		IMMED8,		RM16,
	0x0D,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	1,	25,	WP,		IMMED16U,	RM16,
	0x09,	0,	17,	WP,		GREG16,		RM16,
	0x0B,	0,	19,	WP,		RM16,		GREG16	},

		{ {"orl"},		I_REGULAR,	0, IQ, 1,
	0x83,	1,	25,	LP,		IMMED8,		RM32,
	0x0D,	0,	22,	LP,		IMMED32,	R0,
	0x81,	1,	25,	LP,		IMMED32,	RM32,
	0x09,	0,	17,	LP,		GREG32,		RM32,
	0x0B,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução SBB (pág 17-150) *****************************
 */
		{ {"sbbb"},		I_REGULAR,	0, NOFLAG, 0,
	0x1C,	0,	22,	0,		IMMED8U,	R0,
	0x80,	3,	25,	0,		IMMED8U,	RM8,
	0x18,	0,	17,	0,		GREG8,		RM8,
	0x1A,	0,	19,	0,		RM8,		GREG8	},

		{ {"sbbw"},		I_REGULAR,	0, IQ, 0,
	0x83,	3,	25,	WP,		IMMED8,		RM16,
	0x1D,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	3,	25,	WP,		IMMED16U,	RM16,
	0x19,	0,	17,	WP,		GREG16,		RM16,
	0x1B,	0,	19,	WP,		RM16,		GREG16	},

		{ {"sbbl"},		I_REGULAR,	0, IQ, 0,
	0x83,	3,	25,	LP,		IMMED8,		RM32,
	0x1D,	0,	22,	LP,		IMMED32,	R0,
	0x81,	3,	25,	LP,		IMMED32,	RM32,
	0x19,	0,	17,	LP,		GREG32,		RM32,
	0x1B,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução SUB (pág 17-167) *****************************
 */
		{ {"subb"},		I_REGULAR,	0, NOFLAG, 1,
	0x2C,	0,	22,	0,		IMMED8U,	R0,
	0x80,	5,	25,	0,		IMMED8U,	RM8,
	0x28,	0,	17,	0,		GREG8,		RM8,
	0x2A,	0,	19,	0,		RM8,		GREG8	},

		{ {"subw"},		I_REGULAR,	0, IQ, 1,
	0x83,	5,	25,	WP,		IMMED8,		RM16,
	0x2D,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	5,	25,	WP,		IMMED16U,	RM16,
	0x29,	0,	17,	WP,		GREG16,		RM16,
	0x2B,	0,	19,	WP,		RM16,		GREG16	},

		{ {"subl"},		I_REGULAR,	0, IQ, 1,
	0x83,	5,	25,	LP,		IMMED8,		RM32,
	0x2D,	0,	22,	LP,		IMMED32,	R0,
	0x81,	5,	25,	LP,		IMMED32,	RM32,
	0x29,	0,	17,	LP,		GREG32,		RM32,
	0x2B,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução XOR (pág 17-174) *****************************
 */
		{ {"eorb", "xorb"},	I_REGULAR,	0, NOFLAG, 1,
	0x34,	0,	22,	0,		IMMED8U,	R0,
	0x80,	6,	25,	0,		IMMED8U,	RM8,
	0x30,	0,	17,	0,		GREG8,		RM8,
	0x32,	0,	19,	0,		RM8,		GREG8	},

		{ {"eorw", "xorw"},	I_REGULAR,	0, IQ, 1,
	0x83,	6,	25,	WP,		IMMED8,		RM16,
	0x35,	0,	22,	WP,		IMMED16U,	R0,
	0x81,	6,	25,	WP,		IMMED16U,	RM16,
	0x31,	0,	17,	WP,		GREG16,		RM16,
	0x33,	0,	19,	WP,		RM16,		GREG16	},

		{ {"eorl", "xorl"},	I_REGULAR,	0, IQ, 1,
	0x83,	6,	25,	LP,		IMMED8,		RM32,
	0x35,	0,	22,	LP,		IMMED32,	R0,
	0x81,	6,	25,	LP,		IMMED32,	RM32,
	0x31,	0,	17,	LP,		GREG32,		RM32,
	0x33,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******	Instrução TEST (pág 17-168) *****************************
 */
		{ {"testb"},		I_REGULAR,	0, NOFLAG, 1,
	0xA8,	0,	22,	0,		IMMED8U,	R0,
	0xF6,	0,	25,	0,		IMMED8U,	RM8,
	0x84,	0,	17,	0,		GREG8,		RM8	},

		{ {"testw"},		I_REGULAR,	0, NOFLAG, 1,
	0xA9,	0,	22,	WP,		IMMED16U,	R0,
	0xF7,	0,	25,	WP,		IMMED16U,	RM16,
	0x85,	0,	17,	WP,		GREG16,		RM16	},

		{ {"testl"},		I_REGULAR,	0, NOFLAG, 1,
	0xA9,	0,	22,	LP,		IMMED32,	R0,
	0xF7,	0,	25,	LP,		IMMED32,	RM32,
	0x85,	0,	17,	LP,		GREG32,		RM32	},

/*
 ******* Instrução LEA (pág 17-91) ******************************
 */
		{ {"leaw"},		I_REGULAR,	0, NOFLAG, 0,
	0x8D,	0,	19,	WP,		MEM,		GREG16	},

		{ {"lea", "leal"},	I_REGULAR,	0, NOFLAG, 1,
	0x8D,	0,	19,	LP,		MEM,		GREG32	},

/*
 ******* Outras instruções com sintaxe do grupo 1 ***************
 */
		{ {"arpl"},		I_REGULAR,	0, NOFLAG, 0,
	0x63,	0,	17,	0,		GREG16,		RM16	},

		{ {"boundw"},		I_REGULAR,	0, NOFLAG, 0,
	0x62,	0,	19,	WP,		MEM,		GREG16	},

		{ {"boundl"},		I_REGULAR,	0, NOFLAG, 0,
	0x62,	0,	19,	LP,		MEM,		GREG32	},

/*
 ******* Instrução PUSH (pág 17-131 a 132) **********************
 */
		{ {"pushw"},		I_REGULAR,	0, IQ, 1,
	0xFF,	0x06,	8,	WP,		MEM,		0,
	0x50,	0,	6,	WP,		GREG16,		0,
	0x6A,	0,	5,	WP,		IMMED8,		0,
	0x68,	0,	5,	WP,		IMMED16U,	0,
	0x06,	0x80,	10,	WP,		SREG,		0	},

		{ {"pushl"},		I_REGULAR,	0, IQ, 1,
	0xFF,	0x06,	8,	LP,		MEM,		0,
	0x50,	0,	6,	LP,		GREG32,		0,
	0x6A,	0,	5,	LP,		IMMED8,		0,
	0x68,	0,	5,	LP,		IMMED32,	0,
	0x06,	0x80,	10,	LP,		SREG,		0	},

/*
 ******* Instrução POP (pág 17-125) *****************************
 */
		{ {"popw"},		I_REGULAR,	0, NOFLAG, 1,
	0x8F,	0x00,	8,	WP,		MEM,		0,
	0x58,	0,	6,	WP,		GREG16,		0,
	0x07,	0x81,	11,	WP,		SREG,		0	},

		{ {"popl"},		I_REGULAR,	0, NOFLAG, 1,
	0x8F,	0x00,	8,	LP,		MEM,		0,
	0x58,	0,	6,	LP,		GREG32,		0,
	0x07,	0x81,	11,	LP,		SREG,		0	},

/*
 ******* Desvios curtos (1 byte) (pág 17-80 e 83) ***************
 */
		{ {"bvs"},		I_REGULAR,	0, NOFLAG, 0,
	0x70,	0,	50,	0,		EXPR			},

		{ {"bvc"},		I_REGULAR,	0, NOFLAG, 0,
	0x71,	0,	50,	0,		EXPR			},

		{ {"blo", "bcs"},	I_REGULAR,	0, NOFLAG, 0,
	0x72,	0,	50,	0,		EXPR			},

		{ {"bhs", "bcc"},	I_REGULAR,	0, NOFLAG, 0,
	0x73,	0,	50,	0,		EXPR			},

		{ {"beq"},		I_REGULAR,	0, NOFLAG, 0,
	0x74,	0,	50,	0,		EXPR			},

		{ {"bne"},		I_REGULAR,	0, NOFLAG, 0,
	0x75,	0,	50,	0,		EXPR			},

		{ {"bls"},		I_REGULAR,	0, NOFLAG, 0,
	0x76,	0,	50,	0,		EXPR			},

		{ {"bhi"},		I_REGULAR,	0, NOFLAG, 0,
	0x77,	0,	50,	0,		EXPR			},

		{ {"bmi"},		I_REGULAR,	0, NOFLAG, 0,
	0x78,	0,	50,	0,		EXPR			},

		{ {"bpl"},		I_REGULAR,	0, NOFLAG, 0,
	0x79,	0,	50,	0,		EXPR			},

		{ {"bp", "bpe"},	I_REGULAR,	0, NOFLAG, 0,
	0x7A,	0,	50,	0,		EXPR			},

		{ {"bpo"},		I_REGULAR,	0, NOFLAG, 0,
	0x7B,	0,	50,	0,		EXPR			},

		{ {"blt"},		I_REGULAR,	0, NOFLAG, 0,
	0x7C,	0,	50,	0,		EXPR			},

		{ {"bge"},		I_REGULAR,	0, NOFLAG, 0,
	0x7D,	0,	50,	0,		EXPR			},

		{ {"ble"},		I_REGULAR,	0, NOFLAG, 0,
	0x7E,	0,	50,	0,		EXPR			},

		{ {"bgt"},		I_REGULAR,	0, NOFLAG, 0,
	0x7F,	0,	50,	0,		EXPR			},

		{ {"br1z", "bcxz", "becxz"}, I_REGULAR, 0, NOFLAG, 0,
	0xE3,	0,	50,	0,		EXPR			},

/*
 ******* Desvios médios ou longos (2/4 bytes) (pág 17-80) *******
 */
		{ {"bvsl"},		I_REGULAR,	0, NOFLAG, 0,
	0x80,	0,	52,	0,		EXPR			},

		{ {"bvcl"},		I_REGULAR,	0, NOFLAG, 0,
	0x81,	0,	52,	0,		EXPR			},

		{ {"blol", "bcsl"},	I_REGULAR,	0, NOFLAG, 0,
	0x82,	0,	52,	0,		EXPR			},

		{ {"bhsl", "bccl"},	I_REGULAR,	0, NOFLAG, 0,
	0x83,	0,	52,	0,		EXPR			},

		{ {"beql"},		I_REGULAR,	0, NOFLAG, 0,
	0x84,	0,	52,	0,		EXPR			},

		{ {"bnel"},		I_REGULAR,	0, NOFLAG, 0,
	0x85,	0,	52,	0,		EXPR			},

		{ {"blsl"},		I_REGULAR,	0, NOFLAG, 0,
	0x86,	0,	52,	0,		EXPR			},

		{ {"bhil"},		I_REGULAR,	0, NOFLAG, 0,
	0x87,	0,	52,	0,		EXPR			},

		{ {"bmil"},		I_REGULAR,	0, NOFLAG, 0,
	0x88,	0,	52,	0,		EXPR			},

		{ {"bpll"},		I_REGULAR,	0, NOFLAG, 0,
	0x89,	0,	52,	0,		EXPR			},

		{ {"bpel"},		I_REGULAR,	0, NOFLAG, 0,
	0x8A,	0,	52,	0,		EXPR			},

		{ {"bpol"},		I_REGULAR,	0, NOFLAG, 0,
	0x8B,	0,	52,	0,		EXPR			},

		{ {"bltl"},		I_REGULAR,	0, NOFLAG, 0,
	0x8C,	0,	52,	0,		EXPR			},

		{ {"bgel"},		I_REGULAR,	0, NOFLAG, 0,
	0x8D,	0,	52,	0,		EXPR			},

		{ {"blel"},		I_REGULAR,	0, NOFLAG, 0,
	0x8E,	0,	52,	0,		EXPR			},

		{ {"bgtl"},		I_REGULAR,	0, NOFLAG, 0,
	0x8F,	0,	52,	0,		EXPR			},

/*
 ******* Desvios incondicionais (pág 17-83) *********************
 */
		{ {"bra"},		I_REGULAR,	0, NOFLAG, 0,
	0xEB,	0,	50,	0,		EXPR			},

		{ {"bral"},		I_REGULAR,	0, NOFLAG, 0,
	0xE9,	0,	53,	0,		EXPR,		0,
	0xFF,	4,	8,	0,		RM16 /* == 32 */	},

/*
 **** Desvios incondicionais INTER-SEGMENTOS ********************
 */
		{ {"farjmpw", "jmpiw"},	I_REGULAR,	0, NOFLAG, 0,
	0xEA,	0,	58,	WP,		IMMED16U,	EXPR,
	0xFF,	5,	25,	WP,		IMMED16U,	RM16	},

		{ {"farjmpl", "jmpil"},	I_REGULAR,	0, NOFLAG, 0,
	0xEA,	0,	59,	LP,		IMMED16U,	EXPR,
	0xFF,	5,	25,	LP,		IMMED16U,	RM32	},

/*
 ******* Chamada a função (pág 17-37) ***************************
 */
		{ {"call", "jsr"},	I_REGULAR,	0, NOFLAG, 1,
	0xE8,	0,	53,	0,		EXPR,		0,
	0xFF,	2,	8,	0,		ASTER|RM16 /* == 32 */	},

		{ {"callgw"},		I_REGULAR,	0, NOFLAG, 0,
	0x9A,	0,	58,	WP,		IMMED16U,	EXPR,
	0xFF,	3,	25,	WP,		IMMED16U,	RM16	},

		{ {"callgl"},		I_REGULAR,	0, NOFLAG, 0,
	0x9A,	0,	59,	LP,		IMMED16U,	EXPR,
	0xFF,	3,	25,	LP,		IMMED16U,	RM32	},

/*
 ******* Desvios condicionais SDI (2, 4, 6 bytes) (pág 17-80) ***
 */
		{ {"jvs", "jo"},	I_REGULAR,	0, NOFLAG, 0,
	0x00,	0,	55,	0,		EXPR			},

		{ {"jvc", "jno"},	I_REGULAR,	0, NOFLAG, 0,
	0x01,	0,	55,	0,		EXPR			},

		{ {"jlo", "jcs", "jb", "jc", "jnae"}, I_REGULAR, 0, NOFLAG, 1,
	0x02,	0,	55,	0,		EXPR			},

		{ {"jhs", "jcc", "jnb", "jnc", "jae"}, I_REGULAR, 0, NOFLAG, 1,
	0x03,	0,	55,	0,		EXPR			},

		{ {"jeq", "je", "jz"},	I_REGULAR,	0, NOFLAG, 1,
	0x04,	0,	55,	0,		EXPR			},

		{ {"jne", "jnz"},	I_REGULAR,	0, NOFLAG, 1,
	0x05,	0,	55,	0,		EXPR			},

		{ {"jls", "jna", "jbe"},I_REGULAR,	0, NOFLAG, 1,
	0x06,	0,	55,	0,		EXPR			},

		{ {"jhi", "ja", "jnbe"},I_REGULAR,	0, NOFLAG, 1,
	0x07,	0,	55,	0,		EXPR			},

		{ {"jmi", "js"},	I_REGULAR,	0, NOFLAG, 1,
	0x08,	0,	55,	0,		EXPR			},

		{ {"jpl", "jns"},	I_REGULAR,	0, NOFLAG, 1,
	0x09,	0,	55,	0,		EXPR			},

		{ {"jp", "jpe"},	I_REGULAR,	0, NOFLAG, 0,
	0x0A,	0,	55,	0,		EXPR			},

		{ {"jpo", "jnp"},	I_REGULAR,	0, NOFLAG, 0,
	0x0B,	0,	55,	0,		EXPR			},

		{ {"jlt", "jl", "jnge"}, I_REGULAR,	0, NOFLAG, 1,
	0x0C,	0,	55,	0,		EXPR			},

		{ {"jge", "jnl"},	I_REGULAR,	0, NOFLAG, 1,
	0x0D,	0,	55,	0,		EXPR			},

		{ {"jle", "jng"},	I_REGULAR,	0, NOFLAG, 1,
	0x0E,	0,	55,	0,		EXPR			},

		{ {"jgt", "jg", "jnle"}, I_REGULAR,	0, NOFLAG, 1,
	0x0F,	0,	55,	0,		EXPR			},

/*
 **** Desvios incondicionais SDI (2, 4, 5 bytes) (pág 17-83) ****
 */
		{ {"jmp", "jra"},	I_REGULAR,	0, NOFLAG, 1,
	0x00,	0,	56,	0,		EXPR,		0,
	0xFF,	4,	8,	0,		ASTER|RM32,	0	},

/*
 ****** Instrução RET (pág 17-142) ******************************
 */
		{ {"rts", "ret"},	I_REGULAR,	0, NOFLAG, 1,
	0xC3,	0,	0,	0,		0,		0	},

		{ {"farrts", "lret"},	I_REGULAR,	0, NOFLAG, 0,
	0xCB,	0,	0,	0,		0,		0	},

		{ {"rtd"},		I_REGULAR,	0, NOFLAG, 0,
	0xC2,	0,	5,	0,		IMMED16U,	0	},

		{ {"farrtd", "lrtd"},	I_REGULAR,	0, NOFLAG, 0,
	0xCA,	0,	5,	0,		IMMED16U,	0	},

/*
 ****** Instruções sem operandos, de 1 byte *********************
 */
		{ {"unlk", "leave"},	I_REGULAR,	0, NOFLAG, 1,
	0xC9,	0,	0,	0,		0,		0	},

		{ {"aaa"},		I_REGULAR,	0, NOFLAG, 0,
	0x37,	0,	0,	0,		0,		0	},

		{ {"aas"},		I_REGULAR,	0, NOFLAG, 0,
	0x3F,	0,	0,	0,		0,		0	},

		{ {"extbw", "cbw"},	I_REGULAR,	0, NOFLAG, 0,
	0x98,	0,	0,	WP,		0,		0	},

		{ {"extwl", "cwde"},	I_REGULAR,	0, NOFLAG, 0,
	0x98,	0,	0,	LP,		0,		0	},

		{ {"clc"},		I_REGULAR,	0, NOFLAG, 0,
	0xF8,	0,	0,	0,		0,		0	},

		{ {"up", "cld"},	I_REGULAR,	0, NOFLAG, 0,
	0xFC,	0,	0,	0,		0,		0	},

		{ {"intdis", "disint", "cli"},	I_REGULAR, 0, NOFLAG, 0,
	0xFA,	0,	0,	0,		0,		0	},

		{ {"cmc"},		I_REGULAR,	0, NOFLAG, 0,
	0xF5,	0,	0,	0,		0,		0	},

		{ {"cmpsb"},		I_REGULAR,	0, NOFLAG, 0,
	0xA6,	0,	0,	0,		0,		0	},

		{ {"cmpsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xA7,	0,	0,	WP,		0,		0	},

		{ {"cmpsl", "cmpsd"},	I_REGULAR,	0, NOFLAG, 0,
	0xA7,	0,	0,	LP,		0,		0	},

		{ {"cwd"},		I_REGULAR,	0, NOFLAG, 0,
	0x99,	0,	0,	WP,		0,		0	},

		{ {"extlq", "cdq"},	I_REGULAR,	0, NOFLAG, 1,
	0x99,	0,	0,	LP,		0,		0	},

		{ {"daa"},		I_REGULAR,	0, NOFLAG, 0,
	0x27,	0,	0,	0,		0,		0	},

		{ {"das"},		I_REGULAR,	0, NOFLAG, 0,
	0x2F,	0,	0,	0,		0,		0	},

		{ {"hlt"},		I_REGULAR,	0, NOFLAG, 0,
	0xF4,	0,	0,	0,		0,		0	},

		{ {"insb"},		I_REGULAR,	0, NOFLAG, 0,
	0x6C,	0,	0,	0,		0,		0	},

		{ {"insw"},		I_REGULAR,	0, NOFLAG, 0,
	0x6D,	0,	0,	WP,		0,		0	},

		{ {"insl", "insd"},	I_REGULAR,	0, NOFLAG, 0,
	0x6D,	0,	0,	LP,		0,		0	},

		{ {"iretw"},		I_REGULAR,	0, NOFLAG, 0,
	0xCF,	0,	0,	WP,		0,		0	},

		{ {"rte", "iretl", "iretd"}, I_REGULAR,	0, NOFLAG, 0,
	0xCF,	0,	0,	LP,		0,		0	},

		{ {"lahf"},		I_REGULAR,	0, NOFLAG, 0,
	0x9F,	0,	0,	0,		0,		0	},

		{ {"lock"},		I_REGULAR,	0, NOFLAG, 0,
	0xF0,	0,	0,	0,		0,		0	},

		{ {"lodsb"},		I_REGULAR,	0, NOFLAG, 0,
	0xAC,	0,	0,	0,		0,		0	},

		{ {"lodsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xAD,	0,	0,	WP,		0,		0	},

		{ {"lodsl", "lodsd"},	I_REGULAR,	0, NOFLAG, 0,
	0xAD,	0,	0,	LP,		0,		0	},

		{ {"movsb"},		I_REGULAR,	0, NOFLAG, 0,
	0xA4,	0,	0,	0,		0,		0	},

		{ {"movsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xA5,	0,	0,	WP,		0,		0	},

		{ {"movsl", "movsd"},	I_REGULAR,	0, NOFLAG, 0,
	0xA5,	0,	0,	LP,		0,		0	},

		{ {"nop"},		I_REGULAR,	0, NOFLAG, 0,
	0x90,	0,	0,	0,		0,		0	},

		{ {"outsb"},		I_REGULAR,	0, NOFLAG, 0,
	0x6E,	0,	0,	0,		0,		0	},

		{ {"outsw"},		I_REGULAR,	0, NOFLAG, 0,
	0x6F,	0,	0,	WP,		0,		0	},

		{ {"outsl", "outsd"},	I_REGULAR,	0, NOFLAG, 0,
	0x6F,	0,	0,	LP,		0,		0	},

		{ {"popaw"},		I_REGULAR,	0, NOFLAG, 0,
	0x61,	0,	0,	WP,		0,		0	},

		{ {"popal", "popad"},	I_REGULAR,	0, NOFLAG, 0,
	0x61,	0,	0,	LP,		0,		0	},

		{ {"popfw"},		I_REGULAR,	0, NOFLAG, 0,
	0x9D,	0,	0,	WP,		0,		0	},

		{ {"popfl", "popfd"},	I_REGULAR,	0, NOFLAG, 0,
	0x9D,	0,	0,	LP,		0,		0	},

		{ {"pushaw"},		I_REGULAR,	0, NOFLAG, 0,
	0x60,	0,	0,	WP,		0,		0	},

		{ {"pushal", "pushad"},	I_REGULAR,	0, NOFLAG, 0,
	0x60,	0,	0,	LP,		0,		0	},

		{ {"pushfw"},		I_REGULAR,	0, NOFLAG, 0,
	0x9C,	0,	0,	WP,		0,		0	},

		{ {"pushfl", "pushfd"},	I_REGULAR,	0, NOFLAG, 0,
	0x9C,	0,	0,	LP,		0,		0	},

		{ {"rep", "repeq", "repe", "repz"}, I_REGULAR,	0, NOFLAG, 0,
	0xF3,	0,	0,	0,		0,		0	},

		{ {"repne", "repnz"},	I_REGULAR,	0, NOFLAG, 0,
	0xF2,	0,	0,	0,		0,		0	},

		{ {"sahf"},		I_REGULAR,	0, NOFLAG, 0,
	0x9E,	0,	0,	0,		0,		0	},

		{ {"scasb"},		I_REGULAR,	0, NOFLAG, 0,
	0xAE,	0,	0,	0,		0,		0	},

		{ {"scasw"},		I_REGULAR,	0, NOFLAG, 0,
	0xAF,	0,	0,	WP,		0,		0	},

		{ {"scasl"},		I_REGULAR,	0, NOFLAG, 0,
	0xAF,	0,	0,	LP,		0,		0	},

		{ {"stc"},		I_REGULAR,	0, NOFLAG, 0,
	0xF9,	0,	0,	0,		0,		0	},

		{ {"down", "std"},	I_REGULAR,	0, NOFLAG, 0,
	0xFD,	0,	0,	0,		0,		0	},

		{ {"inten", "enint", "sti"}, I_REGULAR, 0, NOFLAG, 0,
	0xFB,	0,	0,	0,		0,		0	},

		{ {"stosb"},		I_REGULAR,	0, NOFLAG, 0,
	0xAA,	0,	0,	0,		0,		0	},

		{ {"stosw"},		I_REGULAR,	0, NOFLAG, 0,
	0xAB,	0,	0,	WP,		0,		0	},

		{ {"stosl", "stosd"},	I_REGULAR,	0, NOFLAG, 0,
	0xAB,	0,	0,	LP,		0,		0	},

		{ {"wait", "fwait"},	I_REGULAR,	0, NOFLAG, 0,
	0x9B,	0,	0,	0,		0,		0	},

		{ {"xlat", "xlatb"},	I_REGULAR,	0, NOFLAG, 0,
	0xD7,	0,	0,	0,		0,		0	},

/*
 ****** Instruções sem operandos, de 2 bytes ********************
 */
		{ {"aad"},		I_REGULAR,	0, NOFLAG, 0,
	0xD5,	0x0A,	1,	0,		0,		0	},

		{ {"aam"},		I_REGULAR,	0, NOFLAG, 0,
	0xD4,	0x0A,	1,	0,		0,		0	},

		{ {"clts"},		I_REGULAR,	0, NOFLAG, 0,
	0x0F,	0x06,	1,	0,		0,		0	},

		{ {"cpuid"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0xA2,	1,	0,		0,		0	},

		{ {"rdtsc"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0x31,	1,	0,		0,		0	},

		{ {"invd"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0x08,	1,	0,		0,		0	},

		{ {"rdmsr"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0x32,	1,	0,		0,		0	},

		{ {"rsm"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0xAA,	1,	0,		0,		0	},

		{ {"wbinvd"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0x09,	1,	0,		0,		0	},

		{ {"wrmsr"},		I_REGULAR,	0, NOFLAG, 3,
	0x0F,	0x30,	1,	0,		0,		0	},

/*
 ****** Operações de BITs ***************************************
 */
		{ {"bsfw"},		I_REGULAR,	0, NOFLAG, 0,
	0xBC,	0,	20,	WP,		RM16,		GREG16	},

		{ {"bsfl"},		I_REGULAR,	0, NOFLAG, 0,
	0xBC,	0,	20,	LP,		RM32,		GREG32	},

		{ {"bsrw"},		I_REGULAR,	0, NOFLAG, 0,
	0xBD,	0,	20,	WP,		RM16,		GREG16	},

		{ {"bsrl"},		I_REGULAR,	0, NOFLAG, 0,
	0xBD,	0,	20,	LP,		RM32,		GREG32	},

		{ {"btw"},		I_REGULAR,	0, NOFLAG, 0,
	0xA3,	0,	18,	WP,		GREG16,		RM16,
	0xBA,	4,	26,	WP,		IMMED8,		RM16	},

		{ {"btl"},		I_REGULAR,	0, NOFLAG, 0,
	0xA3,	0,	18,	LP,		GREG32,		RM32,
	0xBA,	4,	26,	LP,		IMMED8,		RM32	},

		{ {"btcw"},		I_REGULAR,	0, NOFLAG, 0,
	0xBB,	0,	18,	WP,		GREG16,		RM16,
	0xBA,	7,	26,	WP,		IMMED8,		RM16	},

		{ {"btcl"},		I_REGULAR,	0, NOFLAG, 0,
	0xBB,	0,	18,	LP,		GREG32,		RM32,
	0xBA,	7,	26,	LP,		IMMED8,		RM32	},

		{ {"btrw"},		I_REGULAR,	0, NOFLAG, 0,
	0xB3,	0,	18,	WP,		GREG16,		RM16,
	0xBA,	6,	26,	WP,		IMMED8,		RM16	},

		{ {"btrl"},		I_REGULAR,	0, NOFLAG, 0,
	0xB3,	0,	18,	LP,		GREG32,		RM32,
	0xBA,	6,	26,	LP,		IMMED8,		RM32	},

		{ {"btsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xAB,	0,	18,	WP,		GREG16,		RM16,
	0xBA,	5,	26,	WP,		IMMED8,		RM16	},

		{ {"btsl"},		I_REGULAR,	0, NOFLAG, 0,
	0xAB,	0,	18,	LP,		GREG32,		RM32,
	0xBA,	5,	26,	LP,		IMMED8,		RM32	},

/*
 ******* Instrução DEC (pág 17-56) ******************************
 */
		{ {"decb"},		I_REGULAR,	0, NOFLAG, 1,
	0xFE,	1,	8,	0,		RM8,		0	},

		{ {"decw"},		I_REGULAR,	0, NOFLAG, 1,
	0x48,	0,	6,	WP,		GREG16,		0,
	0xFF,	1,	8,	WP,		RM16,		0	},

		{ {"decl"},		I_REGULAR,	0, NOFLAG, 1,
	0x48,	0,	6,	LP,		GREG32,		0,
	0xFF,	1,	8,	LP,		RM32,		0	},

/*
 ******* Instrução INC (pág 17-66) ******************************
 */
		{ {"incb"},		I_REGULAR,	0, NOFLAG, 1,
	0xFE,	0,	8,	0,		RM8,		0	},

		{ {"incw"},		I_REGULAR,	0, NOFLAG, 1,
	0x40,	0,	6,	WP,		GREG16,		0,
	0xFF,	0,	8,	WP,		RM16,		0	},

		{ {"incl"},		I_REGULAR,	0, NOFLAG, 1,
	0x40,	0,	6,	LP,		GREG32,		0,
	0xFF,	0,	8,	LP,		RM32,		0	},

/*
 ******* Instrução BSWAP (pág 25-41) ****************************
 */
		{ {"bswap", "bswapl"},	I_REGULAR,	0, NOFLAG, 3,
	0xC8,	0,	13,	LP,		GREG32,		0	},

/*
 ******* Instruções de interrupção ******************************
 */
		{ {"int3"},		I_REGULAR,	0, NOFLAG, 0,
	0xCC,	0,	0,	0,		0,		0	},

		{ {"into"},		I_REGULAR,	0, NOFLAG, 0,
	0xCE,	0,	0,	0,		0,		0	},

		{ {"int"},		I_REGULAR,	0, NOFLAG, 0,
	0xCD,	0,	5,	0,		IMMED8U,	0	},

/*
 ******* Instruções de divisão **********************************
 */
		{ {"divub", "divb"},	I_REGULAR,	0, NOFLAG, 1,
	0xF6,	6,	8,	0,		RM8,		0,
	0xF6,	6,	8,	0,		RM8,		R0	},

		{ {"divuw", "divw"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	6,	8,	WP,		RM16,		0,
	0xF7,	6,	8,	WP,		RM16,		R0	},

		{ {"divul", "divl"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	6,	8,	LP,		RM32,		0,
	0xF7,	6,	8,	LP,		RM32,		R0	},

		{ {"divsb", "idivb"},	I_REGULAR,	0, NOFLAG, 1,
	0xF6,	7,	8,	0,		RM8,		0,
	0xF6,	7,	8,	0,		RM8,		R0	},

		{ {"divsw", "idivw"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	7,	8,	WP,		RM16,		0,
	0xF7,	7,	8,	WP,		RM16,		R0	},

		{ {"divsl", "idivl"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	7,	8,	LP,		RM32,		0,
	0xF7,	7,	8,	LP,		RM32,		R0	},

/*
 ****** Instrução ENTER (pág 17-58) *****************************
 */
		{ {"enter"},		I_REGULAR,	0, NOFLAG, 0,
	0xC8,	0,	27,	0,		IMMED16U,	IMMED8	},

/*
 ****** Instrução IN (pág 17-65) ********************************
 */
		{ {"inb"},		I_REGULAR,	0, NOFLAG, 0,
	0xE4,	0,	22,	0,		IMMED8U,	R0,
	0xEC,	0,	28,	0,		GREG16,		R0	},

		{ {"inw"},		I_REGULAR,	0, NOFLAG, 0,
	0xE5,	0,	22,	WP,		IMMED8U,	R0,
	0xED,	0,	28,	WP,		GREG16,		R0	},

		{ {"inl"},		I_REGULAR,	0, NOFLAG, 0,
	0xE5,	0,	22,	LP,		IMMED8U,	R0,
	0xED,	0,	28,	LP,		GREG16,		R0	},

/*
 ****** Instrução INVLPG (pág 25-183) ***************************
 */
		{ {"invlpg"},		I_REGULAR,	0, NOFLAG, 3,
	0x01,	7,	9,	0,		MEM,		0	},

/*
 ****** Instrução IMUL (pág 17-63) ******************************
 */
		{ {"mulsb", "imulb"},	I_REGULAR,	0, NOFLAG, 1,
	0xF6,	5,	8,	0,		RM8,		0,
	0xF6,	5,	8,	0,		RM8,		R0	},

		{ {"mulsw", "imulw"},	I_REGULAR,	0, IQ, 1,
	0xF7,	5,	8,	WP,		RM16,		0,
	0xAF,	0,	20,	WP,		RM16,		GREG16,
	0x6B,	0,	32,	WP|getOPND3,	IMMED8,		RM16,
	0x69,	0,	32,	WP|getOPND3,	IMMED16U,	RM16	},

		{ {"mulsl", "imull"},	I_REGULAR,	0, IQ, 1,
	0xF7,	5,	8,	LP,		RM32,		0,
	0xAF,	0,	20,	LP,		RM32,		GREG32,
	0x6B,	0,	32,	LP|getOPND3,	IMMED8,		RM32,
	0x69,	0,	32,	LP|getOPND3,	IMMED32,	RM32	},

/*
 ****** Instrução MUL (pág 17-116) ******************************
 */
		{ {"mulub", "mulb"},	I_REGULAR,	0, NOFLAG, 1,
	0xF6,	4,	8,	0,		RM8,		R0	},

		{ {"muluw", "mulw"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	4,	8,	WP,		RM16,		R0	},

		{ {"mulul", "mull"},	I_REGULAR,	0, NOFLAG, 1,
	0xF7,	4,	8,	LP,		RM32,		R0	},

/*
 ****** Instrução LAR (pág 17-89) *******************************
 */
		{ {"larw"},		I_REGULAR,	0, NOFLAG, 0,
	0x02,	0,	20,	WP,		RM32,		GREG32	},

		{ {"larl"},		I_REGULAR,	0, NOFLAG, 0,
	0x02,	0,	20,	LP,		RM32,		GREG32	},

/*
 ****** Instrução LGDT/LIDT (pág 17-94) *************************
 */
		{ {"lgdtw"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	2,	9,	WP,		MEM,		0	},

		{ {"lidtw"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	3,	9,	WP,		MEM,		0	},

		{ {"lgdtl"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	2,	9,	LP,		MEM,		0	},

		{ {"lidtl"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	3,	9,	LP,		MEM,		0	},

/*
 ****** Instrução LGS etc... (pág 17-96) ************************
 */
		{ {"ldsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC5,	0,	19,	WP,		MEM,		GREG16	},

		{ {"lssw"},		I_REGULAR,	0, NOFLAG, 0,
	0xB2,	0,	20,	WP,		MEM,		GREG16	},

		{ {"lesw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC4,	0,	19,	WP,		MEM,		GREG16	},

		{ {"lfsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xB4,	0,	20,	WP,		MEM,		GREG16	},

		{ {"lgsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xB5,	0,	20,	WP,		MEM,		GREG16	},

		{ {"ldsl"},		I_REGULAR,	0, NOFLAG, 0,
	0xC5,	0,	19,	LP,		MEM,		GREG32	},

		{ {"lssl"},		I_REGULAR,	0, NOFLAG, 0,
	0xB2,	0,	20,	LP,		MEM,		GREG32	},

		{ {"lesl"},		I_REGULAR,	0, NOFLAG, 0,
	0xC4,	0,	19,	LP,		MEM,		GREG32	},

		{ {"lfsl"},		I_REGULAR,	0, NOFLAG, 0,
	0xB4,	0,	20,	LP,		MEM,		GREG32	},

		{ {"lgsl"},		I_REGULAR,	0, NOFLAG, 0,
	0xB5,	0,	20,	LP,		MEM,		GREG32	},

/*
 ****** Instruções LLDT, LMSW (pág 17-98,99) ********************
 */
		{ {"lldt"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	2,	9,	0,		RM16,		0	},

		{ {"lmsw"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	6,	9,	0,		RM16,		0	},

/*
 ****** Instrução LOOP (pág 17-104) *****************************
 */
		{ {"loop"},		I_REGULAR,	0, NOFLAG, 1,
	0xE2,	0,	50,	0,		EXPR			},

		{ {"loopeq", "loope", "loopz"},I_REGULAR, 0, NOFLAG, 1,
	0xE1,	0,	50,	0,		EXPR			},

		{ {"loopne", "loopnz"},	I_REGULAR,	0, NOFLAG, 1,
	0xE0,	0,	50,	0,		EXPR			},

/*
 ****** Instrução LSL (pág 17-106) ******************************
 */
		{ {"lseglw"},		I_REGULAR,	0, NOFLAG, 0,
	0x03,	0,	20,	WP,		RM16,		GREG16	},

		{ {"lsegll"},		I_REGULAR,	0, NOFLAG, 0,
	0x03,	0,	20,	LP,		RM32,		GREG32	},

/*
 ****** Instrução LTR (pág 17-108) ******************************
 */
		{ {"ltr"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	3,	9,	0,		RM16,		0	},

/*
 ******	Instrução NEG (pág 17-118) ******************************
 */
		{ {"negb"},		I_REGULAR,	0, NOFLAG, 1,
	0xF6,	3,	8,	0,		RM8,		0	},

		{ {"negw"},		I_REGULAR,	0, NOFLAG, 1,
	0xF7,	3,	8,	WP,		RM16,		0	},

		{ {"negl"},		I_REGULAR,	0, NOFLAG, 1,
	0xF7,	3,	8,	LP,		RM32,		0	},

/*
 ******	Instrução NOT (pág 17-120) ******************************
 */
		{ {"notb"},		I_REGULAR,	0, NOFLAG, 1,
	0xF6,	2,	8,	0,		RM8,		0	},

		{ {"notw"},		I_REGULAR,	0, NOFLAG, 1,
	0xF7,	2,	8,	WP,		RM16,		0	},

		{ {"notl"},		I_REGULAR,	0, NOFLAG, 1,
	0xF7,	2,	8,	LP,		RM32,		0	},

/*
 ****** Instrução OUT (pág 17-122) ******************************
 */
		{ {"outb"},		I_REGULAR,	0, NOFLAG, 0,
	0xE6,	0,	23,	0,		R0,		IMMED8U,
	0xEE,	0,	29,	0,		R0,		GREG16	},

		{ {"outw"},		I_REGULAR,	0, NOFLAG, 0,
	0xE7,	0,	23,	WP,		R0,		IMMED8U,
	0xEF,	0,	29,	WP,		R0,		GREG16	},

		{ {"outl"},		I_REGULAR,	0, NOFLAG, 0,
	0xE7,	0,	23,	LP,		R0,		IMMED8U,
	0xEF,	0,	29,	LP,		R0,		GREG16	},

/*
 ****** Instrução ROTATE (pág 17-136) ***************************
 */
		{ {"rclb"},		I_REGULAR,	0, NOFLAG, 0,
	0xC0,	2,	31,	0,		IMMED32,	RM8,
	0xD2,	2,	30,	0,		GREG8,		RM8	},

		{ {"rclw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	2,	31,	WP,		IMMED32,	RM16,
	0xD3,	2,	30,	WP,		GREG8,		RM16	},

		{ {"rcll"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	2,	31,	LP,		IMMED32,	RM32,
	0xD3,	2,	30,	LP,		GREG8,		RM32	},

		{ {"rcrb"},		I_REGULAR,	0, NOFLAG, 0,
	0xC0,	3,	31,	0,		IMMED32,	RM8,
	0xD2,	3,	30,	0,		GREG8,		RM8	},

		{ {"rcrw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	3,	31,	WP,		IMMED32,	RM16,
	0xD3,	3,	30,	WP,		GREG8,		RM16	},

		{ {"rcrl"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	3,	31,	LP,		IMMED32,	RM32,
	0xD3,	3,	30,	LP,		GREG8,		RM32	},

		{ {"rolb"},		I_REGULAR,	0, NOFLAG, 0,
	0xC0,	0,	31,	0,		IMMED32,	RM8,
	0xD2,	0,	30,	0,		GREG8,		RM8	},

		{ {"rolw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	0,	31,	WP,		IMMED32,	RM16,
	0xD3,	0,	30,	WP,		GREG8,		RM16	},

		{ {"roll"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	0,	31,	LP,		IMMED32,	RM32,
	0xD3,	0,	30,	LP,		GREG8,		RM32	},

		{ {"rorb"},		I_REGULAR,	0, NOFLAG, 0,
	0xC0,	1,	31,	0,		IMMED32,	RM8,
	0xD2,	1,	30,	0,		GREG8,		RM8	},

		{ {"rorw"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	1,	31,	WP,		IMMED32,	RM16,
	0xD3,	1,	30,	WP,		GREG8,		RM16	},

		{ {"rorl"},		I_REGULAR,	0, NOFLAG, 0,
	0xC1,	1,	31,	LP,		IMMED32,	RM32,
	0xD3,	1,	30,	LP,		GREG8,		RM32	},

/*
 ****** Instrução SHIFT (pág 17-147) ****************************
 */
		{ {"aslb", "lslb", "salb", "shlb"}, I_REGULAR, 0, NOFLAG, 1,
	0xC0,	4,	31,	0,		IMMED32,	RM8,
	0xD2,	4,	30,	0,		GREG8,		RM8	},

		{ {"aslw", "lslw", "salw", "shlw"}, I_REGULAR, 0, NOFLAG, 1,
	0xC1,	4,	31,	WP,		IMMED32,	RM16,
	0xD3,	4,	30,	WP,		GREG8,		RM16	},

		{ {"asll", "lsll", "sall", "shll"}, I_REGULAR, 0, NOFLAG, 1,
	0xC1,	4,	31,	LP,		IMMED32,	RM32,
	0xD3,	4,	30,	LP,		GREG8,		RM32	},

		{ {"asrb", "sarb"},	I_REGULAR,	0, NOFLAG, 1,
	0xC0,	7,	31,	0,		IMMED32,	RM8,
	0xD2,	7,	30,	0,		GREG8,		RM8	},

		{ {"asrw", "sarw"},	I_REGULAR,	0, NOFLAG, 1,
	0xC1,	7,	31,	WP,		IMMED32,	RM16,
	0xD3,	7,	30,	WP,		GREG8,		RM16	},

		{ {"asrl", "sarl"},	I_REGULAR,	0, NOFLAG, 1,
	0xC1,	7,	31,	LP,		IMMED32,	RM32,
	0xD3,	7,	30,	LP,		GREG8,		RM32	},

		{ {"lsrb", "shrb"},	I_REGULAR,	0, NOFLAG, 1,
	0xC0,	5,	31,	0,		IMMED32,	RM8,
	0xD2,	5,	30,	0,		GREG8,		RM8	},

		{ {"lsrw", "shrw"},	I_REGULAR,	0, NOFLAG, 1,
	0xC1,	5,	31,	WP,		IMMED32,	RM16,
	0xD3,	5,	30,	WP,		GREG8,		RM16	},

		{ {"lsrl", "shrl"},	I_REGULAR,	0, NOFLAG, 1,
	0xC1,	5,	31,	LP,		IMMED32,	RM32,
	0xD3,	5,	30,	LP,		GREG8,		RM32	},

/*
 ****** Instrução SETcc (pág 17-153) ****************************
 */
		{ {"setvs", "seto"},	I_REGULAR,	0, NOFLAG, 1,
	0x90,	0,	9,	0,		RM8			},

		{ {"setvc", "setno"},	I_REGULAR,	0, NOFLAG, 1,
	0x91,	0,	9,	0,		RM8			},

		{ {"setlo", "setcs", "setb", "setc", "setnae"},	I_REGULAR, 0, NOFLAG, 1,
	0x92,	0,	9,	0,		RM8			},

		{ {"seths", "setcc", "setnb", "setnc", "setae"}, I_REGULAR, 0, NOFLAG, 1,
	0x93,	0,	9,	0,		RM8			},

		{ {"seteq", "sete", "setz"},I_REGULAR,	0, NOFLAG, 1,
	0x94,	0,	9,	0,		RM8			},

		{ {"setne", "setnz"},	I_REGULAR,	0, NOFLAG, 1,
	0x95,	0,	9,	0,		RM8			},

		{ {"setls", "setna", "setbe"},	I_REGULAR, 0, NOFLAG, 1,
	0x96,	0,	9,	0,		RM8			},

		{ {"sethi", "seta", "setnbe"},	I_REGULAR, 0, NOFLAG, 1,
	0x97,	0,	9,	0,		RM8			},

		{ {"setmi", "sets"},	I_REGULAR,	0, NOFLAG, 1,
	0x98,	0,	9,	0,		RM8			},

		{ {"setpl", "setns"},	I_REGULAR,	0, NOFLAG, 1,
	0x99,	0,	9,	0,		RM8			},

		{ {"setp", "setpe"},	I_REGULAR,	0, NOFLAG, 1,
	0x9A,	0,	9,	0,		RM8			},

		{ {"setpo", "setnp"},	I_REGULAR,	0, NOFLAG, 1,
	0x9B,	0,	9,	0,		RM8			},

		{ {"setlt", "setl", "setnge"},	I_REGULAR, 0, NOFLAG, 1,
	0x9C,	0,	9,	0,		RM8			},

		{ {"setge", "setnl"},	I_REGULAR,	0, NOFLAG, 1,
	0x9D,	0,	9,	0,		RM8			},

		{ {"setle", "setng"},	I_REGULAR,	0, NOFLAG, 1,
	0x9E,	0,	9,	0,		RM8			},

		{ {"setgt", "setg", "setnle"},	I_REGULAR, 0, NOFLAG, 1,
	0x9F,	0,	9,	0,		RM8			},

/*
 ******* Instruções SGDT/SIDT (pág 17-154) **********************
 */
		{ {"sgdt"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	0,	9,	0,		MEM,		0	},

		{ {"sidt"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	1,	9,	0,		MEM,		0	},

/*
 ******* Instruções SHLD/SHRD (pág 17-155/7) ********************
 */
		{ {"shldw"},		I_REGULAR,	0, NOFLAG, 0,
	0xA4,	0,	34,	WP|getOPND3,	IMMED8,		GREG32,
	0xA5,	0,	33,	WP|getOPND3,	GREG32,		GREG32	},

		{ {"lslq", "shldl"},	I_REGULAR,	0, NOFLAG, 0,
	0xA4,	0,	34,	LP|getOPND3,	IMMED8,		GREG32,
	0xA5,	0,	33,	LP|getOPND3,	GREG32,		GREG32	},

		{ {"shrdw"},		I_REGULAR,	0, NOFLAG, 0,
	0xAC,	0,	34,	WP|getOPND3,	IMMED8,		GREG32,
	0xAD,	0,	33,	WP|getOPND3,	GREG32,		GREG32	},

		{ {"lsrq", "shrdl"},	I_REGULAR,	0, NOFLAG, 0,
	0xAC,	0,	34,	LP|getOPND3,	IMMED8,		GREG32,
	0xAD,	0,	33,	LP|getOPND3,	GREG32,		GREG32	},

/*
 ******* Instruções SLDT/SMSW (pág 17-159/60) *******************
 */
		{ {"sldt"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	0,	9,	0,		RM16,		0	},

		{ {"smsw"},		I_REGULAR,	0, NOFLAG, 0,
	0x01,	4,	9,	0,		RM16,		0	},

/*
 ******* Instruções STRT/VER (pág 17-166/9) *********************
 */
		{ {"str"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	1,	9,	0,		RM16,		0	},

		{ {"verr"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	4,	9,	0,		RM16,		0	},

		{ {"verw"},		I_REGULAR,	0, NOFLAG, 0,
	0x00,	5,	9,	0,		RM16,		0	},

/*
 ******	Instrução XCHG (pág 17-172) *****************************
 */
		{ {"xchgb"},		I_REGULAR,	0, NOFLAG, 0,
	0x86,	0,	17,	0,		GREG8,		RM8,
	0x86,	0,	19,	0,		RM8,		GREG8	},

		{ {"xchgw"},		I_REGULAR,	0, NOFLAG, 0,
	0x90,	0,	6,	WP,		GREG16,		R0,
	0x90,	0,	7,	WP,		R0,		GREG16,
	0x87,	0,	17,	WP,		GREG16,		RM16,
	0x87,	0,	19,	WP,		RM16,		GREG16	},

		{ {"xchgl"},		I_REGULAR,	0, NOFLAG, 0,
	0x90,	0,	6,	LP,		GREG32,		R0,
	0x90,	0,	7,	LP,		R0,		GREG32,
	0x87,	0,	17,	LP,		GREG32,		RM32,
	0x87,	0,	19,	LP,		RM32,		GREG32	},

/*
 ******* Instrução SEG (prefixos de segmentos) ******************
 */
		{ {"seg"},		I_REGULAR,	0, NOFLAG, 0,
	0,	0,	60,	0,		SREG,		0	},

/*
 ******* Instrução sintetizada CLR (XOR, MOV) *******************
 */
		{ {"clrb"},		I_REGULAR,	0, NOFLAG, 2,
	0x30,	0,	61,	0,		GREG8,		0,
	0xC6,	0,	63,	0,		RM8,		0	},

		{ {"clrw"},		I_REGULAR,	0, NOFLAG, 2,
	0x31,	0,	61,	WP,		GREG16,		0,
	0xC7,	0,	63,	WP,		RM16,		0	},

		{ {"clrl"},		I_REGULAR,	0, NOFLAG, 2,
	0x31,	0,	61,	LP,		GREG32,		0,
	0xC7,	0,	63,	LP,		RM32,		0	},

/*
 ******	Instrução sintetizada TST (TEST) ************************
 */
		{ {"tstb"},		I_REGULAR,	0, NOFLAG, 2,
	0x84,	0,	61,	0,		GREG8,		0	},

		{ {"tstw"},		I_REGULAR,	0, NOFLAG, 2,
	0x85,	0,	61,	WP,		GREG16,		0	},

		{ {"tstl"},		I_REGULAR,	0, NOFLAG, 2,
	0x85,	0,	61,	LP,		GREG32,		0	},

/*
 ******	Instrução sintetizada LINK ******************************
 */
		{ {"link"},		I_REGULAR,	0, IQ, 1,
	0,	0,	62,	0,		IMMED32,	0	},

/*
 ******* Instrução FADD de ponto flutuante (pág 26-75) **********
 */
		{ {"faddf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0,	8,	0,		MEM,		0	},

		{ {"faddd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	0,	8,	0,		MEM,		0	},

		{ {"fadd"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xC0,	8,	0,		FREG,		0,
	0xD8,	0xC0,	8,	0,		FREG,		F0,
	0xDC,	0xC0,	12,	0,		F0,		FREG	},

		{ {"faddp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xC0,	8,	0,		FREG,		0,
	0xDE,	0xC0,	12,	0,		F0,		FREG,
	0xDE,	0xC1,	1,	0,		0,		0	},

		{ {"fiaddl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	0,	8,	0,		MEM,		0	},

		{ {"fiaddw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0,	8,	0,		MEM,		0	},

/*
 ******* Instrução FSUB de ponto flutuante (pág 26-137) *********
 */
		{ {"fsubf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	4,	8,	0,		MEM,		0	},

		{ {"fsubd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	4,	8,	0,		MEM,		0	},

		{ {"fsub"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xE0,	8,	0,		FREG,		0,
	0xD8,	0xE0,	8,	0,		FREG,		F0,
	0xDC,	0xE8,	12,	0,		F0,		FREG	},

		{ {"fsubp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xE8,	8,	0,		FREG,		0,
	0xDE,	0xE8,	12,	0,		F0,		FREG,
	0xDE,	0xE9,	1,	0,		0,		0	},

		{ {"fisubl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	4,	8,	0,		MEM,		0	},

		{ {"fisubw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	4,	8,	0,		MEM,		0	},

/*
 ******* Instrução FSUBR de ponto flutuante (pág 26-139) ********
 */
		{ {"fsubrf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	5,	8,	0,		MEM,		0	},

		{ {"fsubrd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	5,	8,	0,		MEM,		0	},

		{ {"fsubr"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xE8,	8,	0,		FREG,		0,
	0xD8,	0xE8,	8,	0,		FREG,		F0,
	0xDC,	0xE0,	12,	0,		F0,		FREG	},

		{ {"fsubrp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xE0,	8,	0,		FREG,		0,
	0xDE,	0xE0,	12,	0,		F0,		FREG,
	0xDE,	0xE1,	1,	0,		0,		0	},

		{ {"fisubrl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	5,	8,	0,		MEM,		0	},

		{ {"fisubrw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	5,	8,	0,		MEM,		0	},

/*
 ******* Instrução FMUL de ponto flutuante (pág 26-109) *********
 */
		{ {"fmulf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	1,	8,	0,		MEM,		0	},

		{ {"fmuld"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	1,	8,	0,		MEM,		0	},

		{ {"fmul"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xC8,	8,	0,		FREG,		0,
	0xD8,	0xC8,	8,	0,		FREG,		F0,
	0xDC,	0xC8,	12,	0,		F0,		FREG	},

		{ {"fmulp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xC8,	8,	0,		FREG,		0,
	0xDE,	0xC8,	12,	0,		F0,		FREG,
	0xDE,	0xC9,	1,	0,		0,		0	},

		{ {"fimull"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	1,	8,	0,		MEM,		0	},

		{ {"fimulw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	1,	8,	0,		MEM,		0	},

/*
 ******* Instrução FDIV de ponto flutuante (pág 26-87) **********
 */
		{ {"fdivf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	6,	8,	0,		MEM,		0	},

		{ {"fdivd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	6,	8,	0,		MEM,		0	},

		{ {"fdiv"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xF0,	8,	0,		FREG,		0,
	0xD8,	0xF0,	8,	0,		FREG,		F0,
	0xDC,	0xF8,	12,	0,		F0,		FREG	},

		{ {"fdivp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xF8,	8,	0,		FREG,		0,
	0xDE,	0xF8,	12,	0,		F0,		FREG,
	0xDE,	0xF9,	1,	0,		0,		0	},

		{ {"fidivl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	6,	8,	0,		MEM,		0	},

		{ {"fidivw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	6,	8,	0,		MEM,		0	},

/*
 ******* Instrução FDIVR de ponto flutuante (pág 26-89) *********
 */
		{ {"fdivrf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	7,	8,	0,		MEM,		0	},

		{ {"fdivrd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	7,	8,	0,		MEM,		0	},

		{ {"fdivr"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xF8,	8,	0,		FREG,		0,
	0xD8,	0xF8,	8,	0,		FREG,		F0,
	0xDC,	0xF0,	12,	0,		F0,		FREG	},

		{ {"fdivrp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xF0,	8,	0,		FREG,		0,
	0xDE,	0xF0,	12,	0,		F0,		FREG,
	0xDE,	0xF1,	1,	0,		0,		0	},

		{ {"fidivrl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	7,	8,	0,		MEM,		0	},

		{ {"fidivrw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	7,	8,	0,		MEM,		0	},

/*
 ******* Instrução FTST de ponto flutuante (pág 26-141) *********
 */
		{ {"ftst"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE4,	1,	0,		0,		0,
	0xD9,	0xE4,	1,	0,		F0,		0	},

/*
 ******* Instrução FSTSW de ponto flutuante (pág 26-135) ********
 */
		{ {"fstsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	7,	8,	WAITP,		MEM,		0,
	0xDF,	0xE0,	1,	WAITP,		R0,		0,
	0xDF,	0xE0,	1,	WAITP,		0,		0	},

		{ {"fnstsw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	7,	8,	0,		MEM,		0,
	0xDF,	0xE0,	1,	0,		R0,		0,
	0xDF,	0xE0,	1,	0,		0,		0	},

/*
 ******* Instrução FLD de ponto flutuante (pág 26-101) **********
 */
		{ {"fldf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0,	8,	0,		MEM,		0	},

		{ {"fldd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0,	8,	0,		MEM,		0	},

		{ {"flde"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	5,	8,	0,		MEM,		0	},

		{ {"fld"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xC0,	8,	0,		FREG,		0	},

/*
 ******* Instrução FST de ponto flutuante (pág 26-130) **********
 */
		{ {"fstf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	2,	8,	0,		MEM,		0	},

		{ {"fstd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	2,	8,	0,		MEM,		0	},

		{ {"fst"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0xD0,	8,	0,		FREG,		0,
	0xDD,	0xD0,	12,	0,		F0,		FREG	},

		{ {"fstpf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	3,	8,	0,		MEM,		0	},

		{ {"fstpd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	3,	8,	0,		MEM,		0	},

		{ {"fstpe"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	7,	8,	0,		MEM,		0	},

		{ {"fstp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0xD8,	8,	0,		FREG,		0,
	0xDD,	0xD8,	12,	0,		F0,		FREG	},

/*
 ******* Instrução FCOM de ponto flutuante (pág 26-82) **********
 */
		{ {"fcomf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	2,	8,	0,		MEM,		0	},

		{ {"fcomd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	2,	8,	0,		MEM,		0	},

		{ {"fcom"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xD0,	12,	0,		F0,		FREG,
	0xD8,	0xD0,	8,	0,		FREG,		0,
	0xD8,	0xD1,	1,	0,		0,		0	},

		{ {"fcompf"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	3,	8,	0,		MEM,		0	},

		{ {"fcompd"},		I_REGULAR,	0, NOFLAG, 0,
	0xDC,	3,	8,	0,		MEM,		0	},

		{ {"fcomp"},		I_REGULAR,	0, NOFLAG, 0,
	0xD8,	0xD8,	12,	0,		F0,		FREG,
	0xD8,	0xD8,	8,	0,		FREG,		0,
	0xD8,	0xD9,	1,	0,		0,		0	},

		{ {"fcompp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	0xD8,	12,	0,		F0,		F1,
	0xDE,	0xD8,	8,	0,		F1,		0,
	0xDE,	0xD9,	1,	0,		0,		0	},

/*
 ******* Instrução FILD de ponto flutuante (pág 26-94) **********
 */
		{ {"fildw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	0,	8,	0,		MEM,		0	},

		{ {"fildl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	0,	8,	0,		MEM,		0	},

		{ {"fildq"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	5,	8,	0,		MEM,		0	},

/*
 ******* Instrução FABS de ponto flutuante (pág 26-74) **********
 */
		{ {"fabs"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE1,	1,	0,		F0,		0,
	0xD9,	0xE1,	1,	0,		0,		0	},

/*
 ******* Instrução FCHS de ponto flutuante (pág 26-80) **********
 */
		{ {"fchs"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE0,	1,	0,		F0,		0,
	0xD9,	0xE0,	1,	0,		0,		0	},

/*
 ******* Instrução FINIT de ponto flutuante (pág 26-97) *********
 */
		{ {"finit"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	0xE3,	1,	0,		0,		0	},

/*
 ******* Instrução FSTCW de ponto flutuante (pág 26-132) ********
 */
		{ {"fstcw"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	7,	8,	0,		MEM,		0	},

/*
 ******* Instrução FLDCW de ponto flutuante (pág 26-132) ********
 */
		{ {"fldcw"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	5,	8,	0,		MEM,		0	},

/*
 ******* Instrução FSAVE de ponto flutuante (pág 26-122) ********
 */
		{ {"fsave"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	6,	8,	0,		MEM,		0	},

/*
 ******* Instrução FRSTOR de ponto flutuante (pág 26-120) *******
 */
		{ {"frstor"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	4,	8,	0,		MEM,		0	},

/*
 ******* Instrução FRNDINT de ponto flutuante (pág 26-119) ******
 */
		{ {"frndint"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFC,	1,	0,		F0,		0,
	0xD9,	0xFC,	1,	0,		0,		0	},

/*
 ******* Instrução FIST de ponto flutuante (pág 26-99) **********
 */
		{ {"fistw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	2,	8,	0,		MEM,		0	},

		{ {"fistl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	2,	8,	0,		MEM,		0	},

		{ {"fistpw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	3,	8,	0,		MEM,		0	},

		{ {"fistpl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	3,	8,	0,		MEM,		0	},

		{ {"fistpq"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	7,	8,	0,		MEM,		0	},

/*
 ******* Instruções FLD1, ... de ponto flutuante (pág 26-103) ***
 */
		{ {"fld1"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE8,	1,	0,		0,		0	},

		{ {"fldl2t"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE9,	1,	0,		0,		0	},

		{ {"fldl2e"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xEA,	1,	0,		0,		0	},

		{ {"fldpi"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xEB,	1,	0,		0,		0	},

		{ {"fldlg2"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xEC,	1,	0,		0,		0	},

		{ {"fldln2"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xED,	1,	0,		0,		0	},

		{ {"fldz"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xEE,	1,	0,		0,		0	},

/*
 ******* Instrução FSCALE de ponto flutuante (pág 26-124) *******
 */
		{ {"fscale"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFD,	1,	0,		0,		0	},

/*
 ******* Instrução F2XM1 de ponto flutuante (pág 26-72) *********
 */
		{ {"f2xm1"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF0,	1,	0,		0,		0	},

/*
 ******* Instrução FBLD de ponto flutuante (pág 26-77) **********
 */
		{ {"fbld"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	4,	8,	0,		MEM,		0	},

/*
 ******* Instrução FBSTP de ponto flutuante (pág 26-79) *********
 */
		{ {"fbstp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDF,	6,	8,	0,		MEM,		0	},

/*
 ******* Instrução FCLEX de ponto flutuante (pág 26-81) *********
 */
		{ {"fclex"},		I_REGULAR,	0, NOFLAG, 0,
	0xDB,	0xE2,	1,	0,		0,		0	},

/*
 ******* Instrução FCOS de ponto flutuante (pág 26-84) **********
 */
		{ {"fcos"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFF,	1,	0,		F0,		0,
	0xD9,	0xFF,	1,	0,		0,		0	},

/*
 ******* Instrução FDECSTP de ponto flutuante (pág 26-86) *******
 */
		{ {"fdecstp"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF6,	1,	0,		0,		0	},

/*
 ******* Instrução FFREE de ponto flutuante (pág 26-91) *********
 */
		{ {"ffree"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0xC0,	8,	0,		FREG,		0	},

/*
 ******* Instrução FICOM de ponto flutuante (pág 26-92) *********
 */
		{ {"ficomw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	2,	8,	0,		MEM,		0	},

		{ {"ficoml"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	2,	8,	0,		MEM,		0	},

		{ {"ficompw"},		I_REGULAR,	0, NOFLAG, 0,
	0xDE,	3,	8,	0,		MEM,		0	},

		{ {"ficompl"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	3,	8,	0,		MEM,		0	},

/*
 ******* Instrução FINCSTP de ponto flutuante (pág 26-96) *******
 */
		{ {"fincstp"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF7,	1,	0,		0,		0	},

/*
 ******* Instrução FLDENV de ponto flutuante (pág 26-107) *******
 */
		{ {"fldenv"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	4,	8,	0,		MEM,		0	},

/*
 ******* Instrução FNOP de ponto flutuante (pág 26-111) *********
 */
		{ {"fnop"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xD0,	1,	0,		0,		0	},

/*
 ******* Instrução FPATAN de ponto flutuante (pág 26-112) ********
 */
		{ {"fpatan"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF3,	1,	0,		0,		0	},

/*
 ******* Instrução FPREM de ponto flutuante (pág 26-113,115) *****
 */
		{ {"fprem"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF8,	1,	0,		0,		0	},

		{ {"fprem1"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF5,	1,	0,		0,		0	},

/*
 ******* Instrução FPTAN de ponto flutuante (pág 26-117) *********
 */
		{ {"fptan"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF2,	1,	0,		0,		0	},

/*
 ******* Instrução FSIN de ponto flutuante (pág 26-125) **********
 */
		{ {"fsin"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFE,	1,	0,		0,		0	},

/*
 ******* Instrução FSINCOS de ponto flutuante (pág 26-127) *******
 */
		{ {"fsincos"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFB,	1,	0,		0,		0	},

/*
 ******* Instrução FSQRT de ponto flutuante (pág 26-129) *********
 */
		{ {"fsqrt"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xFA,	1,	0,		0,		0	},

/*
 ******* Instrução FSTENV de ponto flutuante (pág 26-133) *******
 */
		{ {"fstenv"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	6,	8,	0,		MEM,		0	},

/*
 ******* Instrução FUCOM de ponto flutuante (pág 26-143) ********
 */
		{ {"fucom"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0xE0,	8,	0,		FREG,		0,
	0xDD,	0xE1,	1,	0,		0,		0	},

		{ {"fucomp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDD,	0xE8,	8,	0,		FREG,		0,
	0xDD,	0xE9,	1,	0,		0,		0	},

		{ {"fucompp"},		I_REGULAR,	0, NOFLAG, 0,
	0xDA,	0xE9,	1,	0,		F1,		0,
	0xDA,	0xE9,	1,	0,		0,		0	},

/*
 ******* Instrução FXAM de ponto flutuante (pág 26-146) **********
 */
		{ {"fxam"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xE5,	1,	0,		0,		0	},

/*
 ******* Instrução FXCH de ponto flutuante (pág 26-148) **********
 */
		{ {"fxch"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xC8,	8,	0,		FREG,		0,
	0xD9,	0xC9,	1,	0,		0,		0	},

/*
 ******* Instrução FXTRACT de ponto flutuante (pág 26-150) *******
 */
		{ {"fxtract"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF4,	1,	0,		0,		0	},

/*
 ******* Instrução FYL2X de ponto flutuante (pág 26-152) *********
 */
		{ {"fyl2x"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF1,	1,	0,		0,		0	},

/*
 ******* Instrução FYL2XP1 de ponto flutuante (pág 26-154) *******
 */
		{ {"fyl2xp1"},		I_REGULAR,	0, NOFLAG, 0,
	0xD9,	0xF9,	1,	0,		0,		0	},

/*
 ****** Pseudo-Instruções ***************************************
 */
		{ {".text"},		I_PSEUDO,	6,  NOFLAG, 1	},
		{ {".data"},		I_PSEUDO,	7,  NOFLAG, 1	},
		{ {".const"},		I_PSEUDO,	28, NOFLAG, 1	},
		{ {".global", ".globl"}, I_PSEUDO,	10, NOFLAG, 2	},
		{ {".common", ".comm"},	I_PSEUDO,	9,  NOFLAG, 2	},
		{ {".byte"},		I_PSEUDO,	3,  NOFLAG, 2	},
		{ {".long"},		I_PSEUDO,	4,  NOFLAG, 2	},
		{ {".word"},		I_PSEUDO,	5,  NOFLAG, 2	},
		{ {".align"},		I_PSEUDO,	18, NOFLAG, 2	},
		{ {".even"},		I_PSEUDO,	8,  NOFLAG, 1	},
		{ {".dsb"},		I_PSEUDO,	14, NOFLAG, 2	},
		{ {".dsw"},		I_PSEUDO,	15, NOFLAG, 2	},
		{ {".dsl"},		I_PSEUDO,	16, NOFLAG, 2	},
		{ {".iso"},		I_PSEUDO,	24, NOFLAG, 0	},
		{ {".isoz"},		I_PSEUDO,	25, NOFLAG, 2	},
		{ {".ascii"},		I_PSEUDO,	12, NOFLAG, 0	},
		{ {".asciz"},		I_PSEUDO,	13, NOFLAG, 1	},
		{ {".bss"},		I_PSEUDO,	2,  NOFLAG, 1	},
#if (0)	/*******************************************************/
		{ {".extd"},		I_PSEUDO,	21, NOFLAG, 2	},
#endif	/*******************************************************/
		{ {".double"},		I_PSEUDO,	22, NOFLAG, 2	},
		{ {".float"},		I_PSEUDO,	23, NOFLAG, 2	},
		{ {".page"},		I_PSEUDO,	17, NOFLAG, 0	},
		{ {".org"},		I_PSEUDO,	19, NOFLAG, 0	},
		{ {".time"},		I_PSEUDO,	20, NOFLAG, 0	},
		{ {".line"},		I_PSEUDO,	26, NOFLAG, 0	},
		{ {".syment"},		I_PSEUDO,	27, NOFLAG, 0	},
		{ {".asm"},		I_PSEUDO,	29, NOFLAG, 0	},
		{ {".seg16"},		I_PSEUDO,	30, NOFLAG, 0	},
		{ {".seg32"},		I_PSEUDO,	31, NOFLAG, 0	},

		{ {".if"},		I_PSEUDO,	35, NOSKIP, 0	},
		{ {".ifdef"},		I_PSEUDO,	37, NOSKIP, 0	},
		{ {".ifndef"},		I_PSEUDO,	38, NOSKIP, 0	},
		{ {".elif"},		I_PSEUDO,	39, NOSKIP, 0	},
		{ {".eldef"},		I_PSEUDO,	41, NOSKIP, 0	},
		{ {".elndef"},		I_PSEUDO,	42, NOSKIP, 0	},
		{ {".else"},		I_PSEUDO,	43, NOSKIP, 0	},
		{ {".endif"},		I_PSEUDO,	44, NOSKIP, 0	},

		{ {".warn"},		I_PSEUDO,	50, NOFLAG, 0	},
		{ {".error"},		I_PSEUDO,	51, NOFLAG, 0	},

/*
 ****** Sentinela ***********************************************
 */
	{ {""} }
};
