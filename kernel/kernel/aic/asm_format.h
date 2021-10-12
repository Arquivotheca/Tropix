/*
 ****************************************************************
 *								*
 *			asm.2940.c				*
 *								*
 *	Montador para a linguagem simbólica da placa 2940	*
 *								*
 *	Versão	4.0.0, de 08.03.01				*
 *		4.0.0, de 08.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

struct ins_format1
{
	ulong	immediate	: 8,
			source		: 9,
			destination	: 9,
			ret		: 1,
			opcode		: 4,
			parity		: 1;
};

struct ins_format2
{
	ulong	shift_control	: 8,
			source		: 9,
			destination	: 9,
			ret		: 1,
			opcode		: 4,
			parity		: 1;
};

struct ins_format3
{
	ulong	immediate	: 8,
			source		: 9,
			address		: 10,
			opcode		: 4,
			parity		: 1;
};

union ins_formats
{
		struct ins_format1 format1;
		struct ins_format2 format2;
		struct ins_format3 format3;
		uchar		   bytes[4];
		ulong		   integer;
};

struct instruction
{
	union				ins_formats format;
	unsigned			srcline;
	struct symbol			*patch_label;
#if (0)	/*******************************************************/
	STAILQ_ENTRY (instruction)	links;
#endif	/*******************************************************/
	struct { struct instruction	*stqe_next; }	links;
};

#define	AIC_OP_OR	0x0
#define	AIC_OP_AND	0x1
#define AIC_OP_XOR	0x2
#define	AIC_OP_ADD	0x3
#define	AIC_OP_ADC	0x4
#define	AIC_OP_ROL	0x5
#define	AIC_OP_BMOV	0x6

#define	AIC_OP_JMP	0x8
#define AIC_OP_JC	0x9
#define AIC_OP_JNC	0xa
#define AIC_OP_CALL	0xb
#define	AIC_OP_JNE	0xc
#define	AIC_OP_JNZ	0xd
#define	AIC_OP_JE	0xe
#define	AIC_OP_JZ	0xf

/* Pseudo Ops */
#define	AIC_OP_SHL	0x10
#define	AIC_OP_SHR	0x20
#define	AIC_OP_ROR	0x30
