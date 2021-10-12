/*
 ****************************************************************
 *								*
 *			scan_tb.c				*
 *								*
 *	Diversas Tabelas Relacionadas ao Analisador Léxico	*
 *								*
 *	Versão	1.0.0, de 14.02.86				*
 *		3.0.8, de 26.09.94				*
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

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/lex.h"

/*
 ****************************************************************
 *	Categorias léxicas					*
 ****************************************************************
 */
#define	I	INV
#define	L	LETTER
#define	D	DIGIT
#define	P	APOST
#define	Q	QUOTE
#define	S	SEPAR
#define	U	USYM
#define	B	BSYM

/*
 ****************************************************************
 *	Tabela de Caracteres					*
 ****************************************************************
 */
entry const char categtb[]	=
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	I,	S,	U,	I,	S,	S,	I,	I,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	CAN	EM	SUB	EC	FS	GS	RS	US	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	branco	!	"	#	$	%	&	'	*/

	S,	B,	Q,	U,	U,	U,	B,	P,


/*	(	)	*	+	,	-	.	/	*/

	U,	U,	U,	U,	U,	U,	L,	B,


/*	0	1	2	3	4	5	6	7	*/

	D,	D,	D,	D,	D,	D,	D,	D,


/*	8	9	:	;	<	=	>	?	*/

	D,	D,	B,	U,	B,	B,	B,	I,


/*	@	A	B	C	D	E	F	G	*/

	B,	L,	L,	L,	L,	L,	L,	L,


/*	H	I	J	K	L	M	N	O	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	P	Q	R	S	T	U	V	W	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	X	Y	Z	[	\	]	^	_	*/

	L,	L,	L,	U,	I,	U,	U,	L,


/*	`	a	b	c	d	e	f	g	*/

	I,	L,	L,	L,	L,	L,	L,	L,


/*	h	i	j	k	l	m	n	o	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	p	q	r	s	t	u	v	w	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	x	y	z	{	|	}	~	DEL	*/

	L,	L,	L,	U,	B,	U,	U,	I,


/*	80	81	82	83	84	85	86	87	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	90	91	92	93	94	95	96	97	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	S,	I,	I,	I,	I,	I,	I,	I,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	L,	L,	L,	L,	L,	L,	L,	L

}	/* end categtb */;

/*
 ****************************************************************
 *	Tabela de índice					*
 ****************************************************************
 */
entry const char indextb[]	=
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	CAN	EM	SUB	EC	FS	GS	RS	US	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	branco	!	"	#	$	%	&	'	*/

	0,	7,	Q,	1,	1,	2,	8,	0,


/*	(	)	*	+	,	-	.	/	*/

	3,	4,	5,	6,	7,	8,	0,	0,


/*	0	1	2	3	4	5	6	7	*/

	0,	1,	2,	3,	4,	5,	6,	7,


/*	8	9	:	;	<	=	>	?	*/

	8,	9,	1,	9,	2,	6,	3,	0,


/*	@	A	B	C	D	E	F	G	*/

	4,	10,	11,	12,	13,	14,	15,	0,


/*	H	I	J	K	L	M	N	O	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	P	Q	R	S	T	U	V	W	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	X	Y	Z	[	\	]	^	_	*/

	0,	0,	0,	10,	0,	11,	12,	0,


/*	`	a	b	c	d	e	f	g	*/

	0,	10,	11,	12,	13,	14,	15,	0,


/*	h	i	j	k	l	m	n	o	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	p	q	r	s	t	u	v	w	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	x	y	z	{	|	}	~	DEL	*/

	0,	0,	0,	13,	5,	14,	15,	0,


/*	80	81	82	83	84	85	86	87	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	90	91	92	93	94	95	96	97	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	0,	0,	0,	0,	0,	0,	0,	0

}	/* end indextb */;

/*
 ****************************************************************
 *	Simbolos unários (de um só caractere)			*
 ****************************************************************
 */
entry const UNSYM	unsym[]	=
{
	EOL,	N_NULL,		/*  0: '\0' ('\n') */
	NUMBER,	N_NULL,		/*  1: # */
	OP,	N_MOD,		/*  2: % */
	LP,	N_NULL,		/*  3: ( */
	RP,	N_NULL,		/*  4: ) */
	OP,	N_MUL,		/*  5: * */
	OP,	N_PLUS,		/*  6: + */
	CM,	N_NULL,		/*  7: , */
	OP,	N_MINUS,	/*  8: - */
	SM,	N_NULL,		/*  9: ; */
	LB,	N_NULL,		/* 10: [ */
	RB,	N_NULL,		/* 11: ] */
	OP,	N_EOR,		/* 12: ^ */
	LC,	N_NULL,		/* 13: { */
	RC,	N_NULL,		/* 14: } */
	OP,	N_NOT		/* 15: ~ */

}	/* end unsym */;
