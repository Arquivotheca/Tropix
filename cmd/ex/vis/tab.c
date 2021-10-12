/*
 ****************************************************************
 *								*
 *			tab.c					*
 *								*
 *	Tabelas de caracteres para o modo visual		*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/global.h"
#include "../h/obj.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Inicialização da tabela de interpretação		*
 ****************************************************************
 */
entry const char	int_tab[] =
{
/*	NUL	ctl-A	ctl-B	ctl-C	ctl-D	ctl-E	ctl-F	ctl-G	*/
	LN|FN,	LN|FN,	LC|FW,	LN|FN,	LC|FW,	LC|FW,	LC|FW,	LC|FX,

/*	BS	TAB	LF	ctl-K	FF	CR	ctl-N	ctl-O	*/
	LJ|FM,	LJ|FM,	LJ|FM,	LN|FN,	LC|FW,	LJ|FM,	LJ|FM,	LN|FN,

/*	ctl-P	ctl-Q	ctl-R	ctl-S	ctl-T	ctl-U	ctl-V	ctl-W	*/
	LJ|FM,	LN|FN,	LC|FW,	LN|FN,	LN|FN,	LC|FW,	LN|FN,	LN|FN,

/*	ctl-X	ctl-Y	ctl-Z	ESC	FS	GS	RS	US	*/
	LN|FN,	LC|FW,	LC|FX,	LE|FN,	LN|FN,	LN|FN,	LN|FN,	LN|FN,

/*	branco	!	"	#	$	%	&	'	*/
	LJ|FM,	LC|FX,	LB|FN,	LN|FN,	LJ|FM,	LJ|FM,	LJ|FM,	LK|FM,

/*	(	)	*	+	,	-	.	/	*/
	LJ|FM,	LJ|FM,	LN|FN,	LJ|FM,	LJ|FM,	LJ|FM,	LC|FX,	LS|FM,

/*	0	1	2	3	4	5	6	7	*/
	LJ|FM,	LD|FN,	LD|FN,	LD|FN,	LD|FN,	LD|FN,	LD|FN,	LD|FN,

/*	8	9	:	;	<	=	>	?	*/
	LD|FN,	LD|FN,	LC|FX,	LJ|FM,	LP|FS,	LN|FN,	LP|FS,	LS|FM,

/*	@	A	B	C	D	E	F	G	*/
	LN|FN,	LY|FN,	LJ|FM,	LY|FN,	LY|FN,	LJ|FM,	LF|FM,	LC|FW,

/*	H	I	J	K	L	M	N	O	*/
	LJ|FM,	LY|FN,	LC|FJ,	LN|FN,	LJ|FM,	LJ|FM,	LJ|FM,	LY|FN,

/*	P	Q	R	S	T	U	V	W	*/
	LY|FN,	LC|FX,	LC|FX,	LY|FN,	LF|FM,	LC|FX,	LN|FN,	LJ|FM,

/*	X	Y	Z	[	\	]	^	_	*/
	LY|FN,	LY|FN,	LN|FN,	LJ|FM,	LN|FN,	LJ|FM,	LJ|FM,	LN|FN,

/*	`	a	b	c	d	e	f	g	*/
	LK|FM,	LY|FN,	LJ|FM,	LP|FC,	LP|FD,	LJ|FM,	LF|FM,	LC|FW,

/*	h	i	j	k	l	m	n	o	*/
	LJ|FM,	LC|FI,	LJ|FM,	LJ|FM,	LJ|FM,	LM|FK,	LJ|FM,	LY|FN,

/*	p	q	r	s	t	u	v	w	*/
	LC|FP,	LN|FN,	LC|FR,	LY|FN,	LF|FM,	LC|FU,	LN|FN,	LJ|FM,

/*	x	y	z	{	|	}	~	DEL	*/
	LY|FN,	LP|FY,	LN|FN,	LJ|FM,	LJ|FM,	LJ|FM,	LN|FN,	LJ|FM

};	/* end int_tab */

/*
 ****************************************************************
 *	Tabela com Majors e Minors para os Objetos		*
 ****************************************************************
 */
entry const char	obj_tab[] =
{
/*	NUL	ctl-A	ctl-B	ctl-C	ctl-D	ctl-E	ctl-F	ctl-G	*/
	0,	0,	0,	0,	0,	0,	0,	0,

/*	BS	TAB	LF	ctl-K	FF	CR	ctl-N	ctl-O	*/
	ML|1,	ML|4,	MC|2,	0,	0,	MH|0,	MC|2,	0,

/*	ctl-P	ctl-Q	ctl-R	ctl-S	ctl-T	ctl-U	ctl-V	ctl-W	*/
	MC|0,	0,	0,	0,	0,	0,	0,	0,

/*	ctl-X	ctl-Y	ctl-Z	ESC	FS	GS	RS	US	*/
	0,	0,	0,	0,	0,	0,	0,	0,

/*	branco	!	"	#	$	%	&	'	*/
	ML|2,	0,	0,	0,	ML|3,	MB|0,	MS|4,	MK|1,

/*	(	)	*	+	,	-	.	/	*/
	ME|0,	ME|1,	0,	MH|0,	MF|5,	MH|2,	0,	MS|1,

/*	0	1	2	3	4	5	6	7	*/
	ML|0,	0,	0,	0,	0,	0,	0,	0,

/*	8	9	:	;	<	=	>	?	*/
	0,	0,	0,	MF|4,	MH|0,	0,	MH|0,	MS|0,

/*	@	A	B	C	D	E	F	G	*/
	0,	0,	MW|5,	0,	0,	MW|4,	MF|2,	0,

/*	H	I	J	K	L	M	N	O	*/
	MP|0,	0,	0,	0,	MP|2,	MP|1,	MS|3,	0,

/*	P	Q	R	S	T	U	V	W	*/
	0,	0,	0,	0,	MF|3,	0,	0,	MW|3,

/*	X	Y	Z	[	\	]	^	_	*/
	0,	0,	0,	MO|0,	0,	MO|1,	MH|1,	0,

/*	`	a	b	c	d	e	f	g	*/
	MK|0,	0,	MW|2,	MH|0,	MH|0,	MW|1,	MF|0,	0,

/*	h	i	j	k	l	m	n	o	*/
	ML|1,	0,	MC|2,	MC|0,	ML|2,	0,	MS|2,	0,

/*	p	q	r	s	t	u	v	w	*/
	0,	0,	0,	0,	MF|1,	0,	0,	MW|0,

/*	x	y	z	{	|	}	~	DEL	*/
	0,	MH|0,	0,	MG|0,	MC|1,	MG|1,	0,	ML|1

};	/* end obj_tab */
