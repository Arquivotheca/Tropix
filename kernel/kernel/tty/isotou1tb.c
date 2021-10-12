/*
 ****************************************************************
 *								*
 *			isotou1tb.c				*
 *								*
 *	Tabela para conversão do código ISO para USER1		*
 *	(Inicializada como ISO para ABICOMP)			*
 *								*
 *	Versão	3.0.0, de 15.12.94				*
 *		3.0.0, de 15.12.94				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	A Tabela						*
 ****************************************************************
 */
const char	isotou1tb[] =
{

/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	0x00,  	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	0x08,	0x09,	0x0A,	0x0B,	0x0C,	0x0D,	0x0E,	0x0F,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	0x18,	0x19,	0x1A,	0x1B,	0x1C,	0x1D,	0x1E,	0x1F,


/*	blank	!	"	#	$	%	&	'	*/

	' ',	'!',	'"',	'#',	'$',	'%',	'&',	'\'',


/*	(	)	*	+	,	-	.	/	*/

	'(',	')',	'*',	'+',	',',	'-',	'.',	'/',


/*	0	1	2	3	4	5	6	7	*/

	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',


/*	8	9	:	;	<	=	>	?	*/

	'8',	'9',	':',	';',	'<',	'=',	'>',	'?',


/*	@	A	B	C	D	E	F	G	*/

	'@',	'A',	'B',	'C',	'D',	'E',	'F',	'G',


/*	H	I	J	K	L	M	N	O	*/

	'H',	'I',	'J',	'K',	'L',	'M',	'N',	'O',


/*	P	Q	R	S	T	U	V	W	*/

	'P',	'Q',	'R',	'S',	'T',	'U',	'V',	'W',


/*	X	Y	Z	[	\	]	^	_	*/

	'X',	'Y',	'Z',	'[',	'\\',	']',	'^',	'_',


/*	`	a	b	c	d	e	f	g	*/

	'`',	'a',	'b',	'c',	'd',	'e',	'f',	'g',


/*	h	i	j	k	l	m	n	o	*/

	'h',	'i',	'j',	'k',	'l',	'm',	'n',	'o',


/*	p	q	r	s	t	u	v	w	*/

	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',


/*	x	y	z	{	|	}	~	(del)	*/

	'x',	'y',	'z',	'{',	'|',	'}',	'~',	0x7F,


/*	80	81	82	83	84	85	86	87	*/

	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	0x88,	0x89,	0x8A,	0x8B,	0x8C,	0x8D,	0x8E,	0x8F,


/*	90	91	92	93	94	95	96	97	*/

	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	0x98,	0x99,	0x9A,	0x9B,	0x9C,	0x9D,	0x9E,	0x9F,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	' ',	'.',	'.',	0xBC,	'.',	'.',	'|',	0xBE,

/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	0xBB,	'c',	0xDC,	'<',	'~',	'-',	'r',	'_',


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	0xDC,	0xDF,	'2',	'3',	'\'',	'u',	'P',	'.',


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	',',	'1',	0xDD,	'>',	'.',	'.',	'.',	0xDE,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	0xA1,	0xA2,	0xA3,	0xA4,	0xA5,	'A',	0xA5,	0xA6,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	0xA7,	0xA8,	0xA9,	0xAA,	0xAB,	0xAC,	0xAD,	0xAE,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	'E',	0xAF,	0xB0,	0xB1,	0xB2,	0xB3,	0xB4,	0xB5,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	'O',	0xB6,	0xB7,	0xB8,	0xB9  ,	'Y',	'T',	0xDE,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	0xC1,	0xC2,	0xC3,	0xC4,	0xC5,	'a',	0xC5,	0xC6,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	0xC7,	0xC8,	0xC9,	0xCA,	0xCB,	0xCC,	0xCD,	0xCE,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	'e',	0xCF,	0xD0,	0xD1,	0xD2,	0xD3,	0xD4,	0xD5,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	'o',	0xD6,	0xD7,	0xD8,	0xD9,	'y',	't',	0xDA

}	/* end isotou1tb */;
