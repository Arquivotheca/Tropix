/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
 *								*
 *	Versão	1.0.0, de 23.08.88				*
 *		4.4.0, de 23.12.02				*
 *								*
 *	Módulo: POP3_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xti.h>

#include "../h/xpop3.h"

/*
 ******	Definições globais **************************************
 */
const char	base_64_tb[];

/*
 ****************************************************************
 *	Processa o MIME 2					*
 ****************************************************************
 */
void
do_mime_2 (const char *in_area, char *out_area)
{
	const char	*rp = in_area;
	char		*cp = out_area;
	int		c;
	char		questions = 0, q_field = 0;

	/*
	 *	Verifica se a cadeia dada é NULA
	 */
	if (in_area == NOSTR)
		{ out_area[0] = '\0'; return; }

	/*
	 *	Processa o MIME 2
	 */
	for (EVER)
	{
		if   ((c = *rp++) == '\0')
		{
			break;
		}
		elif (c == '=')
		{
			if   (questions == 0)
			{
				if (*rp == '?')
					{ questions++; rp++; }
				else
					*cp++ = c;
			}
			elif (q_field)
			{
				char	d, high = 0, low = 0;

				d = rp[0];

				if   (d >= '0' && d <= '9')
					high = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					high = d - 'A' + 10 + 1;

				d = rp[1];

				if   (d >= '0' && d <= '9')
					low = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					low = d - 'A' + 10 + 1;

				if (high && low)
				{
					*cp++ = (high - 1) << 4 | (low - 1);
					rp += 2;
				}
			}
		}
		elif (c == '?')
		{
			if (questions == 0)
				{ *cp++ = c; continue; }

			if   (++questions == 2)
			{
				if   (*rp == 'Q' || *rp == 'q')
				{
					q_field++; rp++;
				}
				elif (*rp == 'B' || *rp == 'b')
				{
					char		t;
					int		total_bits = 0;
					long		word = 0;

					rp++;	/* Pula o "B" */

					while ((c = *rp++) != 0)
					{
						if   (c == '=')
						{
							continue;
						}
						elif (c == '?')
						{
							if (++questions == 3)
								continue;

							questions--; rp--;

							break;
						}

						if ((t = base_64_tb[c]) == 0xFF)
							break;

						word <<= 6; word |= t;

						if ((total_bits += 6) >= 24)
						{
							*cp++ = (word >> 16);
							*cp++ = (word >> 8);
							*cp++ =  word;

							total_bits -= 24;
							word = 0;
						}
					}

					/* Emite os bytes residuais */

					if   (total_bits >= 18)
					{
						*cp++ = (word >> 10);
						*cp++ = (word >> 2);
					}
					elif (total_bits >= 12)
					{
						*cp++ = (word >> 4);
					}
				}
			}
		   /***	elif (questions == 3)	/* "?" após "Q" ou "B" ***/
		   /***	{	***/
		
		   /***	}	***/
			elif (questions >= 4)
			{
				if (*rp == '=')
				{
					rp++;
					questions = q_field = 0;
				}
				else
				{
					questions--;
					*cp++ = c;
				}
			}
		}
		elif (questions == 1)
		{
			/* Pula o "ISO-8859-1" */
		}
		elif (c == '_' && q_field)
		{
			*cp++ = ' ';
		}
		else	/* Caractere sem transformação */
		{
			*cp++ = c;
		}

	}	/* end for (EVER) */

	*cp = '\0';

}	/* end do_mime_2 */

/*
 ****** Tabela do alfabeto Base 64 ******************************
 */
const char	base_64_tb[256] =
{

/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	blank	!	"	#	$	%	&	'	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	(	)	*	+	,	-	.	/	*/

	0xFF,  	0xFF,	0xFF,	62,	0xFF,	0xFF,	0xFF,	63,


/*	0	1	2	3	4	5	6	7	*/

	52,  	53,	54,	55,	56,	57,	58,	59,


/*	8	9	:	;	<	=	>	?	*/

	60,  	61,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	@	A	B	C	D	E	F	G	*/

	0xFF,  	0,	1,	2,	3,	4,	5,	6,


/*	H	I	J	K	L	M	N	O	*/

	7,  	8,	9,	10,	11,	12,	13,	14,


/*	P	Q	R	S	T	U	V	W	*/

	15,  	16,	17,	18,	19,	20,	21,	22,


/*	X	Y	Z	[	\	]	^	_	*/

	23,  	24,	25,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	`	a	b	c	d	e	f	g	*/

	0xFF,  	26,	27,	28,	29,	30,	31,	32,


/*	h	i	j	k	l	m	n	o	*/

	33,  	34,	35,	36,	37,	38,	39,	40,


/*	p	q	r	s	t	u	v	w	*/

	41,  	42,	43,	44,	45,	46,	47,	48,


/*	x	y	z	{	|	}	~	(del)	*/

	49,  	50,	51,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	80	81	82	83	84	85	86	87	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	90	91	92	93	94	95	96	97	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	0xFF,  	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF

}	/* end base_64_tb[] */;
