/*
 ****************************************************************
 *								*
 *			strtol.c				*
 *								*
 *	Extrai valores numericos inteiros de uma cadeia		*
 *								*
 *	Versão	1.0.0, de 16.09.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: libc/etc					*
 *		Biblioteca Basica				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ******	Definições globais **************************************
 */
#define	NIL	(-1)
#define	EVER	;;

/*
 ****************************************************************
 *	Tabela para a conversão de caracteres			*
 ****************************************************************
 */
const char	_strtol_base_tb[] =
{

/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	BS	HT	LF	VT	FF	CR	SO	SI	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	CAN	EM	SUB	EC	FS	GS	RS	US	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	branco	!	"	#	$	%	&	'	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	(	)	*	+	,	-	.	/	*/
	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	0	1	2	3	4	5	6	7	*/
	0,	1,	2,	3,	4,	5,	6,	7,

/*	8	9	:	;	<	=	>	?	*/
	8,	9,	NIL,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	@	A	B	C	D	E	F	G	*/
	NIL,	10,	11,	12,	13,	14,	15,	16,

/*	H	I	J	K	L	M	N	O	*/
	17,	18,	19,	20,	21,	22,	23,	24,

/*	P	Q	R	S	T	U	V	W	*/
	25,	26,	27,	28,	29,	30,	31,	32,

/*	X	Y	Z	[	\	]	^	_	*/
	33,	34,	35,	NIL,	NIL,	NIL,	NIL,	NIL,

/*	`	a	b	c	d	e	f	g	*/
	NIL,	10,	11,	12,	13,	14,	15,	16,

/*	h	i	j	k	l	m	n	o	*/
	17,	18,	19,	20,	21,	22,	23,	24,

/*	p	q	r	s	t	u	v	w	*/
	25,	26,	27,	28,	29,	30,	31,	32,

/*	x	y	z	{	|	}	~	DEL	*/
	33,	34,	35,	NIL,	NIL,	NIL,	NIL,	NIL

};

/*
 ****************************************************************
 *	Função de conversão					*
 ****************************************************************
 */
long
strtol (const char *str, const char **ptr, int base)
{
	register const char	*cp = str;
	register int		c, sign = 0;
	register long		n = 0, val;

	/*
	 *	Ignora Brancos ou TABs iniciais
	 */
	while (*cp == ' ' || *cp == '\t')
		cp++;

	/*
	 *	Analisa o sinal
	 */
	if ((c = *cp) == '+' || c == '-')
		{ sign = (c == '-'); cp++; }

	/*
	 *	Se não foi especificada uma base, descobre de acordo
	 *	com o inicio da cadeia
	 */
	if (base < 2 || base > 36)
	{
		if (*cp == '0')
		{
			if ((c = *++cp) == 'x' || c == 'X')
				{ base = 16; cp++; }
			else
				base = 8;
		}
		else
		{
			base = 10;
		}
	}
	else
	{
		/*
		 *	Ignora os zeros iniciais
		 */
		while (*cp == '0')
			cp++;

		if (base == 16 && ((c = *cp) == 'x' || c == 'X'))
			cp++;
	}

	/*
	 *	Realiza a conversão
	 */
	for (EVER)
	{
		if ((val = _strtol_base_tb[*cp]) == NIL || val >= base)
			break;

		n = n * base + val; cp++;
	}

	if (ptr != (const char **)NULL)
		*ptr = cp;

	return (sign ? -n : n);

}	/* end strtol */
