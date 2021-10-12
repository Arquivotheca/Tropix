/*
 ****************************************************************
 *								*
 *			xd.c					*
 *								*
 *	Gera um "dump" em hexadecimal de um arquivo		*
 *								*
 *	Versão	1.0.0, de 14.05.85				*
 *		4.2.0, de 02.04.02				*
 *								*
 *	Módulo: xd						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	LINESZ	16

entry char	*pgversion =  "Versão:	4.2.0, de 02.04.02";

entry int	aflag;		/* Começo da coluna de endereços  */

entry int	eflag;		/* Ebcdic */

entry int	vflag;		/* Verbose */

extern const char etoa[];

/*
 ******	Protótipos de funções ***********************************
 */
long		getint (const char *);
void		help (void);

/*
 ****************************************************************
 *	Programa de gerar um "dump" hexadecimal			*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt, n, i;
	register const char	*cp;
	long			start = 0, offset = 0, addr = 0;
	char			dump[LINESZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "es:a:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':
			eflag++;
			break;

		    case 's':
			start = getint (optarg);
			start -= start % LINESZ;
			break;

		    case 'a':
			addr = getint (optarg);
			addr -= addr % LINESZ;
			aflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fprintf (stderr, "\n");
			help ();
		}

	}

	argv += optind;
	argc -= optind;

	/*
	 *	Pequena consistencia
	 */
	if (argc > 1)
	{
		fprintf (stderr, "Pode ser dado apenas um arquivo\n\n");
		help ();
	}

	if (argv[0])
	{
		if (freopen (argv[0], "r", stdin) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}
	}
	else
	{
		argv[0] = "stdin";
	}

	/*
	 *	Ajeita o offset
	 */
	if (aflag)
		offset = addr - start;

	/*
	 *	Posiciona no local correto do arquivo
	 */
	if (start != 0 && fseek (stdin, start, 0) == -1)
	{
		fprintf
		(	stderr,
			"%s: Erro no posicionamento no byte %d do arquivo \"%s\" (%s)\n",
			pgname, start, argv[0], strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Inicia o dump
	 */
	while ((n = fread (dump, 1, sizeof (dump), stdin)) > 0) 
	{
		printf ("%P:   ", start + offset);

		for (i = 0; i < LINESZ; i++)
		{
			if ((i & 1) == 0)
				printf (" ");

			if (i == LINESZ/2)
				printf (" ");

			if (i < n)
				printf ("%02X", dump[i]);
			else
				printf ("  ");
		}

		printf ("     ");

		/*
		 *	Escreve caracteres em ascii
		 */
		for
		(	cp = dump, i = 0;
			i < LINESZ && i < n;
			cp++, i++
		)
		{
			register int	c;

			c = eflag ? etoa[*cp] : *cp;
			if (c < 0x20)
				c = '.';
			putchar (c);
		}

		putc ('\n', stdout);

		/*
		 *	Se o endereço é multiplo de 128, pula uma linha
		 */
		start += n;

		if (((start + offset) & 127) == 0)
			putc ('\n', stdout);

	}	/* end while */

	/*
	 *	Verifica se foi EOF ou erro
	 */
	if (ferror (stdin))
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura do arquivo \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Converte um número dado					*
 ****************************************************************
 */
long
getint (const char *str)
{
	register long 	n;
	const char	*ptr;

	n = strtol (str, &ptr, 0);

	switch (*ptr)
	{
	    case 'w':
	    case 'W':
		n *= 2;
		break;

	    case 'l':
	    case 'L':
		n *= 4;
		break;

	    case 'c':
	    case 'C':
		n *= 2 * 1024;
		break;

	    case 'k':
	    case 'K':
		n *= 1024;
		break;

	    case 'b':
	    case 'B':
		n *= 512;
		break;

	    case '\0':
		break;
		
	    default:
		fprintf
		(	stderr,
			"%s: Sufixo de número inválido: '%c'\n",
			pgname, *ptr
		);
		exit (1);

	}	/* end switch (*ptr) */

#if (0)	/*************************************/
	if (n < 0)
	{
		fprintf (stderr, "%s: Número inválido: \"%s\"\n", pgname, str);
		exit (1);
	}
#endif	/*************************************/

	return (n);

}	/* end getint */

/*
 ****************************************************************
 *	Conversão de EBCDIC para ASCII				*
 ****************************************************************
 */
entry const char	etoa[]	=
{
/*	NUL	SOH	STX	ETX	9C	HT	86	(del)	*/

	0,	0x01,	0x02,	0x03,	0x9C,	0x09,	0x86,	0x7F,


/*	97	8D	8E	VT	FF	CR	SO	SI	*/

	0x97,	0x8D,	0x8E,	0x0B,	0x0C,	0x0D,	0x0E,	0x0F,


/*	DLE	DC1	DC2	DC3	9D	85	BS	87	*/

	0x10,	0x11,	0x12,	0x13,	0x9D,	0x85,	0x08,	0x87,


/*	CAN	EM	92	8F	FS	GS	RS	US	*/

	0x18,	0x19,	0x92,	0x8F,	0x1C,	0x1D,	0x1E,	0x1F,


/*	80	81	82	83	84	LF	ETB	ESC	*/

	0x80,	0x81,	0x82,	0x83,	0x84,	0x0A,	0x17,	0x1B,


/*	88	89	8A	8B	8C	ENQ	ACK	BE	*/

	0x88,	0x89,	0x8A,	0x8B,	0x8C,	0x05,	0x06,	0x07,


/*	90	91	SYN	93	94	95	96	EOT	*/

	0x90,	0x91,	0x16,	0x93,	0x94,	0x95,	0x96,	0x04,


/*	98	99	9A	9B	DC4	NAK	9E	SUB	*/

	0x98,	0x99,	0x9A,	0x9B,	0x14,	0x15,	0x9E,	0x1A,


/*	blank	A0	A1	A2	A3	A4	A5	A6	*/

	0x20,	0xA0,	0xA1,	0xA2,	0xA3,	0xA4,	0xA5,	0xA6,


/*	A7	A8	[	.	<	(	+	!	*/

	0xA7,	0xA8,	0x5B,	0x2E,	0x3C,	0x28,	0x2B,	0x21,


/*	&	A9	AA	AB	AC	AD	AE	AF	*/

	0x26,	0xA9,	0xAA,	0xAB,	0xAC,	0xAD,	0xAE,	0xAF,


/*	B0	B1	]	$	*	)	;	^	*/

	0xB0,	0xB1,	0x5D,	0x24,	0x2A,	0x29,	0x3B,	0x5E,


/*	-	/	B2	B3	B4	B5	B6	B7	*/

	0x2D,	0x2F,	0xB2,	0xB3,	0xB4,	0xB5,	0xB6,	0xB7,


/*	B8	B9	|	,	%	_	>	?	*/

	0xB8,	0xB9,	0x7C,	0x2C,	0x25,	0x5F,	0x3E,	0x3F,


/*	BA	BB	BC	BD	BE	BF	C0	C1	*/

	0xBA,	0xBB,	0xBC,	0xBD,	0xBE,	0xBF,	0xC0,	0xC1,


/*	C2	`	:	#	@	'	=	"	*/

	0xC2,	0x60,	0x3A,	0x23,	0x40,	0x27,	0x3D,	0x22,


/*	C3	a	b	c	d	e	f	g	*/

	0xC3,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,


/*	h	i	C4	C5	C6	C7	C8	C9	*/

	0x68,	0x69,	0xC4,	0xC5,	0xC6,	0xC7,	0xC8,	0xC9,


/*	CA	;	k	l	m	n	o	p	*/

	0xCA,	0x6A,	0x6B,	0x6C,	0x6D,	0x6E,	0x6F,	0x70,


/*	q	r	CB	CC	CD	CE	CF	D0	*/

	0x71,	0x72,	0xCB,	0xCC,	0xCD,	0xCE,	0xCF,	0xD0,


/*	D1	~	s	t	u	v	w	x	*/

	0xD1,	0x7E,	0x73,	0x74,	0x75,	0x76,	0x77,	0x78,


/*	y	z	D2	D3	D4	D5	D6	D7	*/

	0x79,	0x7A,	0xD2,	0xD3,	0xD4,	0xD5,	0xD6,	0xD7,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	0xD8,	0xD9,	0xDA,	0xDB,	0xDC,	0xDD,	0xDE,	0xDF,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	0xE0,	0xE1,	0xE2,	0xE3,	0xE4,	0xE5,	0xE6,	0xE7,


/*	{	A	B	C	D	E	F	G	*/

	0x7B,	0x41,	0x42,	0x43,	0x44,	0x45,	0x46,	0x47,


/*	H	I	E8	E9	EA	EB	EC	ED	*/

	0x48,	0x49,	0xE8,	0xE9,	0xEA,	0xEB,	0xEC,	0xED,


/*	}	J	K	L	M	N	O	P	*/

	0x7D,	0x4A,	0x4B,	0x4C,	0x4D,	0x4E,	0x4F,	0x50,


/*	Q	R	EE	EF	F0	F1	F2	F3	*/

	0x51,	0x52,	0xEE,	0xEF,	0xF0,	0xF1,	0xF2,	0xF3,


/*	\	9F	S	T	U	V	W	X	*/

	0x5C,	0x9F,	0x53,	0x54,	0x55,	0x56,	0x57,	0x58,


/*	Y	Z	F4	F5	F6	F7	F8	F9	*/

	0x59,	0x5A,	0xF4,	0xF5,	0xF6,	0xF7,	0xF8,	0xF9,


/*	0	1	2	3	4	5	6	7	*/

	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,


/*	8	9	FA	FB	FC	FD	FE	FF	*/

	0x38,	0x39,	0xFA,	0xFB,	0xFC,	0xFD,	0xFE,	0xFF

}	/* end etoa */;

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf (stderr, "%s - \"dump\" em hexadecimal e ISO\n", pgname);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf
	(	stderr,
		"\t%s [-e] [-s <start>] [-a <ender>] [<arquivo>]\n\n",
		pgname
	);
	fprintf (stderr, "Opções:\t-e: Converta EBCDIC em ISO (ASCII)\n");
	fprintf
	(	stderr,
		"\t-s: comece o \"dump\" a partir do endereço <start>\n"
	);
	fprintf
	(	stderr,
		"\t-a: comece a coluna de endereços a partir de <ender>\n"
	);
	fprintf
	(	stderr,
		"\nObs.:\tSe <arquivo> não for dado, "
		"será feito o \"dump\" da entrada padrão\n"
	);
	exit (2);

}	/* end help */
