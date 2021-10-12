/*
 ****************************************************************
 *								*
 *			categtb.c				*
 *								*
 *	Tabela de Categorias Léxicas				*
 *								*
 *	Versão	3.2.0, de 21.06.99				*
 *		4.5.0, de 12.11.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>

#include	"../h/categ.h"

/*
 ****************************************************************
 *	Tabela de Categorias					*
 ****************************************************************
 */
const uchar	categtb[]	=
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	U|1,	I,	I,	I,	I,	I,	I,	I,


/*	BS	HT	NL	VT	FF	CR	SO	SI	*/

	I,	S|0,	S|1,	I,	I,	S|1,	I,	I,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	branco	!	"	#	$	%	&	'	*/

	S,	O|M|6,	Q|3,	U|0,	Q|1,	O,	B|0,	Q|A|2,


/*	(	)	*	+	,	-	.	/	*/

	U|2,	U|3,	O|M|1,	O,	O|A,	O,	O,	O,


/*	0	1	2	3	4	5	6	7	*/

	D,	D,	D,	D,	D,	D,	D,	D,


/*	8	9	:	;	<	=	>	?	*/

	D,	D,	O|A,	B|1,	B|2,	O,	B|3,	O|M|2,


/*	@	A	B	C	D	E	F	G	*/

	O,	L,	L,	L,	L,	L,	L,	L,


/*	H	I	J	K	L	M	N	O	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	P	Q	R	S	T	U	V	W	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	X	Y	Z	[	\	]	^	_	*/

	L,	L,	L,	O|M|3,	Q|0,	O|M|4,	O|M|A|5,L,


/*	`	a	b	c	d	e	f	g	*/

	Q|A|4,	L,	L,	L,	L,	L,	L,	L,


/*	h	i	j	k	l	m	n	o	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	p	q	r	s	t	u	v	w	*/

	L,	L,	L,	L,	L,	L,	L,	L,


/*	x	y	z	{	|	}	~	DEL	*/

	L,	L,	L,	U|4,	B|4,	U|5,	Q|A|5,	I,


/*	80	81	82	83	84	85	86	87	*/

	O,	O,	O,	O,	O,	O,	O,	O,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	O,	O,	O,	O,	O,	O,	O,	O,


/*	90	91	92	93	94	95	96	97	*/

	O,	O,	O,	O,	O,	O,	O,	O,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	O,	O,	O,	O,	O,	O,	O,	O,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	S,	O,	O,	O,	O,	O,	O,	O,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	O|A,	O,	O,	O,	O,	O,	O,	O,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	O,	O,	O,	O,	O,	O,	O,	O,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	O,	O,	O,	O,	O,	O,	O,	O,


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
