/*
 ****************************************************************
 *								*
 *			chartb.c				*
 *								*
 *	Tabela de categorias dos caracteres			*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.2.3, de 09.01.00				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include "../h/categ.h"

/*
 ****************************************************************
 *	Abreviações para as Categorias Léxicas			*
 ****************************************************************
 */
#define	I	C_INV
#define	H	C_COM
#define	S	C_SEP
#define	L	C_LET
#define	D	C_DIG
#define	V	C_VAL
#define	Q	C_QUO
#define	M	C_MAC
#define	C	C_CL
#define	E	C_EQ
#define	B	C_BL
#define	Y	C_SM
#define	N	C_EOL
#define	T	C_EOT

#define	X	C_MCALL
#define	Z	C_META

/*
 ****************************************************************
 *	Tabela de Categorias					*
 ****************************************************************
 */
entry const uchar chartb[]	=
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	T,	I,	I,	I,	I,	I,	I,	I,


/*	BS	HT	NL	VT	FF	CR	SO	SI	*/

	I,	S,	Z|N,	S,	S,	I,	I,	I,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	CAN	EM	SUB	EC	FS	GS	RS	US	*/

	I,	I,	I,	I,	I,	I,	I,	I,


/*	branco	!	"	#	$	%	&	'	*/

	S,	V,	Z|Q,	H,	Z|X|M,	V,	Z|V,	Z|Q,


/*	(	)	*	+	,	-	.	/	*/

	Z|V,	Z|V,	Z|X|V,	V,	V,	V,	V,	V,


/*	0	1	2	3	4	5	6	7	*/

	X|D,	X|D,	X|D,	X|D,	X|D,	X|D,	X|D,	X|D,


/*	8	9	:	;	<	=	>	?	*/

	X|D,	X|D,	Z|C,	Z|Y,	Z|X|V,	Z|E,	Z|V,	Z|X|V,


/*	@	A	B	C	D	E	F	G	*/

	Z|X|V,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	H	I	J	K	L	M	N	O	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	P	Q	R	S	T	U	V	W	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	X	Y	Z	[	\	]	^	_	*/

	X|L,	X|L,	X|L,	Z|V,	Z|B,	Z|V,	Z|V,	X|L,


/*	`	a	b	c	d	e	f	g	*/

	Z|V,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	h	i	j	k	l	m	n	o	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	p	q	r	s	t	u	v	w	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	x	y	z	{	|	}	~	DEL	*/

	X|L,	X|L,	X|L,	Z|V,	Z|V,	Z|V,	Z|V,	I,


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


/*	À	Á	Â	Ã	Ä	C5	C6	Ç	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	È	É	Ê	Ë	Ì	Í	Î	Ï	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	D0	~N	Ò	Ó	Ô	Ö	D6	D7	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	D8	Ù	Ú	Û	Ü	'Y	DE	DF	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	à	á	â	ã	ä	E5	E6	ç	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	è	é	ê	ë	ì	í	î	ï	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	F0	~n	ò	ó	ô	õ	ö	F7	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,


/*	F8	ù	ú	û	ü	'y	FE	:y	*/

	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L,	X|L

};	/* end categtb */
