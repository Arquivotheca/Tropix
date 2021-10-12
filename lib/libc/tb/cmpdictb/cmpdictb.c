/*
 ****************************************************************
 *								*
 *			cmpdictb.c				*
 *								*
 *	Tabela para comparação de caracteres onde todas		*	
 *	os caracteres que não são letras e numeros		*
 *	são tratados como brancos (dicionario)			*
 *								*
 *	Versão	1.0.0, de 01.10.86				*
 *		3.0.0, de 11.08.95				*
 *								*
 *	Módulo: cmpdictb					*
 *		libc/tb						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <string.h>

/*
 ****************************************************************
 *	A Tabela						*
 ****************************************************************
 */
entry const char cmpdictb[] =
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	blank	!	"	#	$	%	&	'	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	(	)	*	+	,	-	.	/	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	0	1	2	3	4	5	6	7	*/

	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',


/*	8	9	:	;	<	=	>	?	*/

	'8',	'9',	' ',	' ',	' ',	' ',	' ',	' ',


/*	@	A	B	C	D	E	F	G	*/

	' ',	'a',	'b',	'c',	'd',	'e',	'f',	'g',


/*	H	I	J	K	L	M	N	O	*/

	'h',	'i',	'j',	'k',	'l',	'm',	'n',	'o',


/*	P	Q	R	S	T	U	V	W	*/

	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',


/*	X	Y	Z	[	\	]	^	_	*/

	'x',	'y',	'z',	' ',	' ',	' ',	' ',	' ',


/*	`	a	b	c	d	e	f	g	*/

	' ',	'a',	'b',	'c',	'd',	'e',	'f',	'g',


/*	h	i	j	k	l	m	n	o	*/

	'h',	'i',	'j',	'k',	'l',	'm',	'n',	'o',


/*	p	q	r	s	t	u	v	w	*/

	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',


/*	x	y	z	{	|	}	~	(del)	*/

	'x',	'y',	'z',	' ',	' ',	' ',	' ',	' ',


/*	80	81	82	83	84	85	86	87	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	90	91	92	93	94	95	96	97	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	'a',	'a',	'a',	'a',	'a',	'a',	'a',	'c',


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	'e',	'e',	'e',	'e',	'i',	'i',	'i',	'i',


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	'e',	'n',	'o',	'o',	'o',	'o',	'o',	'o',


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	'o',	'u',	'u',	'u',	'u',	'y',	't',	's',


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	'a',	'a',	'a',	'a',	'a',	'a',	'a',	'c',


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	'e',	'e',	'e',	'e',	'i',	'i',	'i',	'i',


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	'e',	'n',	'o',	'o',	'o',	'o',	'o',	'o',


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	'o',	'u',	'u',	'u',	'u',	'y',	't',	'y'

}	/* end cmpdictb */;
