/*
 ****************************************************************
 *								*
 *			chartb.c				*
 *								*
 *	Tabela de caracteres de conversão para o teclado	*
 *								*
 *	Versão	3.0.0, de 14.12.94				*
 *		3.0.0, de 14.12.94				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	A	0x0100	/* Acento : , ` ' ^ ~ */
#define	M	0x0080	/* Letra maiuscula */
#define	m	0x0040	/* Letra minuscula */
#define	C	0x0020	/* Chave { } ` | ~ */
#define	R	0x0010	/* Paren ( ) ' ! ^ */
#define	S	0x0008	/* Especial ^X */

/*
 ****************************************************************
 *								*
 *	Significados dos vários bits:				*
 *								*
 *	Bite 8: Acento - Um dos caracteres : , ` ' ^ ~, que	*
 *		são usados para acentuação			*
 *								*
 *	Bite 7:	Letra maiúscula					*
 *								*
 *	Bite 6:	Letra minúscula					*
 *								*
 *	Bite 5:	Chave -- Um dos caracteres { } ` | ~, que serão	*
 *		respectivamente convertidos em \( \) \' \! \^	*
 *		na saida em modo XCASE.				*
 *								*
 *	Bite 4:	Paren -- Um dos caracteress ( ) ' ! ^, que	*
 *		serão respectivamente convertidos em { } ` | ~	*
 *		se vierem com um '\' antes, na entrada do modo	*
 *		XCASE.						*
 *								*
 *	Bite 3:	Espec -- Caracteres não visiveis na tela, e sem	*
 *		função própria. Serão transformados em ^X na	*
 *		saída.						*
 *								*
 *	Bites 2-0: -- Código para "delays" e avanco de coluna.	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	A tabela						*
 ****************************************************************
 */
unsigned short	chartb[]	=
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BEL	*/

	S|1,	S|1,	S|1,	S|1,	S|1,	S|1,	S|1,	7,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	2,	4,	3,	S|1,	5,	6,	S|1,	S|1,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	S|1,	S|1,	S|1,	S|1,	S|1,	S|1,	S|1,	S|1,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	S|1,	S|1,	S|1,	1,	S|1,	S|1,	S|1,	S|1,


/*	blank	!	"	#	$	%	&	'	*/

	0,	R,	0,	0,	0,	0,	0,	R|A,


/*	(	)	*	+	,	-	.	/	*/

	R,	R,	0,	0,	A,	0,	0,	0,


/*	0	1	2	3	4	5	6	7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	8	9	:	;	<	=	>	?	*/

	0,	0,	A,	0,	0,	0,	0,	0,


/*	@	A	B	C	D	E	F	G	*/

	0,	M,	M,	M,	M,	M,	M,	M,


/*	H	I	J	K	L	M	N	O	*/

	M,	M,	M,	M,	M,	M,	M,	M,


/*	P	Q	R	S	T	U	V	W	*/

	M,	M,	M,	M,	M,	M,	M,	M,


/*	X	Y	Z	[	\	]	^	_	*/

	M, 	M,	M,	0,	0,	0,	R|A,	0,


/*	`	a	b	c	d	e	f	g	*/

	C|A,	m,	m,	m,	m,	m,	m,	m,


/*	h	i	j	k	l	m	n	o	*/

	m,	m,	m,	m,	m,	m,	m,	m,


/*	p	q	r	s	t	u	v	w	*/

	m,	m,	m,	m,	m,	m,	m,	m,


/*	x	y	z	{	|	}	~	DEL	*/

	m,	m,	m,	C,	C,	C,	C|A,	S|1,


/*	80	81	82	83	84	85	86	87	*/

	1,	1,	1,	1,	1,	1,	1,	1,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	1,	1,	1,	1,	1,	1,	1,	1,


/*	90	91	92	93	94	95	96	97	*/

	1,	1,	1,	1,	1,	1,	1,	1,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	1,	1,	1,	1,	1,	1,	1,	1,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	0,	0,	0,	0,	0,	0,	0,	0,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/
/*	`A	'A	^A	~A	"A	oA	AE	,C	*/

	M,	M,	M,	M,	M,	M,	M,	M,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/
/*	`E	'E	^E	"E	`I	'I	^I	"I	*/

	M,	M,	M,	M,	M,	M,	M,	M,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/
/*	ETH	~N	`O	'O	^O	~O	"O	OE	*/

	M,	M,	M,	M,	M,	M,	M,	M,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/
/*	/O	`U	'U	^U	"U	'Y	THORN	ss	*/	

	M,	M,	M,	M,	M,	M,	M,	m,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/
/*	`a	'a	^a	~a	"a	oa	ae	,c	*/	

	m,	m,	m,	m,	m,	m,	m,	m,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/
/*	`e	'e	^e	"e	`i	'i	^i	"i	*/


	m,	m,	m,	m,	m,	m,	m,	m,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/
/*	eth	~n	`o	'o	^o	~o	"o	oe	*/

	m,	m,	m,	m,	m,	m,	m,	m,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/
/*	/o	`u	'u	^u	"u	'y	thorn	"y	*/	

	m,	m,	m,	m,	m,	m,	m,	m

}	/* end chartb */;
