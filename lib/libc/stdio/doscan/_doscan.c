/*
 ****************************************************************
 *								*
 *			_doscan.c				*
 *								*
 *	Formata a entrada de dados				*
 *								*
 *	Versão	1.0.0, de 16.03.86				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: scanf						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 ******	Definições globais **************************************
 */
#define EVER		;;
#define elif		else if

#define STOP		0x01
#define SPACE		0x02
#define ALGAR		0x04
#define HEXA		0x08

#define NOLIMIT		1000000

/*
 ******	Definição de tipos **************************************
 */
typedef enum { NIL, SHORT, REGULAR, LONG, EXTRA }	SIZE;

typedef struct { char eof[4], tb[256]; }		TABLE;

#define NOTABLE	(TABLE *)NULL

/*
 ******	Variáveis globais do arquivo ****************************
 */
static int		nscan;		/* No. de caracteres lidos */
static int		nitems;		/* No. de itens processados */
static TABLE		*scantb;	/* Tabela de caracteres */
extern const TABLE	const_scantb;	/* Protótipo da tabela de caracteres */

/*
 ******	Protótipos de funções ***********************************
 */
int		int_conv (void *, int, int, int, int, FILE *);
int		float_conv (void *, int, int, FILE *);
int		num_conv (void *, int, int, int, FILE *);
int		str_conv (char *, int, int, FILE *);
const char	*getscanset (const char *);

extern void	_error (char *);

/*
 ****************************************************************
 *	Formata a entrada de dados				*
 ****************************************************************
 */
int
_doscan (register FILE *fp, register const char *fmt, void **argp)
{
	register int	fc, c, field, size, r;
	register void	*ptr;

	/*
	 *	Inicialização
	 */
	if (scantb == NOTABLE)
	{
		if ((scantb = malloc (sizeof (TABLE))) == NOTABLE)
			return (-1);

		memmove (scantb, &const_scantb, sizeof (TABLE));
	}

	nscan = 0; nitems = 0;

	/*
	 *	Percorre a cadeia do formato
	 */
	for (EVER) switch (fc = *fmt++)
	{
		/*
		 ******	Final da cadeia de formato **********************
		 */
	    case '\0':
		return (nitems);

		/*
		 ****** Espaços	*****************************************
		 */
	    case ' ':
	    case '\t':
	    case '\n':
	    case '\v':
	    case '\f':
	    case '\r':
		while (nscan++, scantb->tb[c = getc (fp)] & SPACE)
			/* vazio */;

		if (c == EOF)
			return (nitems ? nitems : EOF);

		ungetc (c, fp); nscan--;
		break;

		/*
		 ****** Início de conversão *****************************
		 */
	    case '%':
		if ((fc = *fmt++) == '%')
			goto case_regular;

		/*
		 *	Processa o '*'
		 */
		if (fc == '*')
			{ ptr = NOVOID; fc = *fmt++; }
		else
			{ ptr = *argp++; }

		/*
		 *	Processa o comprimento do campo
		 */
		for (field = 0; scantb->tb[fc] & ALGAR; fc = *fmt++)
			field = field * 10 + fc - '0';

		if (field == 0)
			field = NOLIMIT;

		/*
		 *	Processa o tamanho da variável
		 */
		size = REGULAR;

		if   (fc == 'h')
			{ size = SHORT;	fc = *fmt++; }
		elif (fc == 'l')
			{ size = LONG;	fc = *fmt++; }
		elif (fc == 'L')
			{ size = EXTRA; fc = *fmt++; }

		if (ptr == NOVOID)
			size = NIL;

		/*
		 *	Processa a conversão (field > 0)
		 */
		switch (fc)
		{
		    case 'd':
		    case 'u':
			r = int_conv (ptr, fc, 10, field, size, fp);
			break;

		    case 'i':
			r = int_conv (ptr, fc, 0, field, size, fp);
			break;

		    case 'o':
			r = int_conv (ptr, fc, 8, field, size, fp);
			break;

		    case 'p':
		    case 'P':
		    case 'x':
		    case 'X':
			r = int_conv (ptr, fc, 16, field, size, fp);
			break;

		    case 'e':
		    case 'E':
		    case 'f':
		    case 'g':
		    case 'G':
			r = float_conv (ptr, field, size, fp);
			break;

		    case '[':
			fmt = getscanset (fmt);
		    case 'c':
		    case 's':
			r = str_conv (ptr, fc, field, fp);
			break;

		    case 'n':
			if   (size == SHORT)
				*(short int *)ptr = nscan;
			elif (size != NIL)
				*(int *)ptr = nscan;

			r = 1;
			break;

		    default:	/* Conversão inválida (inclui '\0') */
			return (EOF);

		}	/* end switch (conversão) */

		/*
		 *	Analisa a performance das funções de conversão
		 */
		if (fc != 'n' && ptr != NOVOID && r > 0)
			nitems++;

		if (r < 0)
			return (nitems ? nitems : EOF);

		if (r == 0)
			return (nitems);

		break;

		/*
		 ****** Caractere regular *******************************
		 */
	    default:
	    case_regular:
		c = getc (fp); nscan++;

		if (c != fc)
		{
			if (c == EOF)
				return (nitems ? nitems : EOF);

			ungetc (c, fp); /*** nscan--; ***/
			return (nitems);
		}

		break;

	}	/* end for (EVER) switch */

}	/* end _doscan */

/*
 ****************************************************************
 *	Conversão inteira					*
 ****************************************************************
 */
static int
int_conv (void *ptr, int conv, int base, int field, int size, register FILE *fp)
{
	register unsigned long	val = 0;
	register int		c;
	int			minus = 0, pause = 0, old_field;

	/*
	 *	Retorna:
	 *		-1 leu EOF
	 *		 0 Matching error
	 *		+1 Conversão correta (com ou sem atribuição)
	 */

	/*
	 *	Pula os brancos iniciais
	 */
	if (field-- <= 0)
		return (0);

	while (nscan++, scantb->tb[c = getc (fp)] & SPACE)
		/* vazio */;

	/*
	 *	Analisa o sinal
	 */
	switch (c)
	{
	    case '-':
		minus++;
	    case '+':
		if (field-- <= 0)
			return (0);

		c = getc (fp); nscan++;
	}

	/*
	 *	Analisa os prefixos "0" e "0x"
	 */
	if (base == 0 || conv == 'p')
	{
		if (c == '0')
		{
			if (field-- <= 0)
				return (0);

			c = getc (fp); nscan++;

			if (c == 'x' || c == 'X')
			{
				base = 16;

				if (conv == 'p')
					pause++;

				if (field-- <= 0)
					return (0);

				c = getc (fp); nscan++;
			}
			else
			{
				field++; ungetc (c, fp); nscan--; c = '0';

				if (conv != 'p')
					base = 8;
			}
		}
		else
		{
			if (conv != 'p')
				base = 10;
		}
	}
	
	/*
	 *	Processa a cadeia de dígitos
	 */
	for (old_field = field; field >= 0; c = getc (fp), nscan++, field--)
	{
		if   (scantb->tb[c] & ALGAR)
		{
			if   (base == 8)
				{ val <<= 3; if (c > '7') break; }
			elif (base == 10)
				val *= 10;
			elif (base == 16)
				val <<= 4;
			else /* base == ? */
				_error ("_doscan: base\n");

			val += c - '0';
		}
		elif (base == 16 && (scantb->tb[c] & HEXA))
		{
			val <<= 4;

			if (c >= 'A' && c <='F')
				val += c - 'A' + 10;
			else
				val += c - 'a' + 10;
		}
		elif (conv == 'p' && c == ' ')
		{
			if (pause++)
				break;
		}
		else	/* O caractere não pertence a um inteiro */
		{
			break;
		}

	}	/* end for (pelos dígitos) */

	/*
	 *	Ajustes finais
	 */
	ungetc (c, fp); nscan--;

	if (field == old_field)
	{
		if (c == EOF)
			return (-1);
		else
			return (0);
	}

	/*
	 *	Armazena o número
	 */
	if (minus)
		val = -val;

	switch (size)
	{
	    case NIL:
		break;

	    case SHORT:
		*(short *)ptr = val;
		break;

	    case REGULAR:
		*(int *)ptr = val;
		break;

	    case LONG:
	    case EXTRA:
		*(long *)ptr = val;
		break;

	    default:
		_error ("_doscan: size\n");
	}

	return (1);

}	/* end int_conv */

/*
 ****************************************************************
 *	Conversão flutuante					*
 ****************************************************************
 */
static int
float_conv (void *ptr, int field, int size, register FILE *fp)
{
	register long double	val;
	register int		c;
	char			dot_seen = 0, E_seen = 0, sign_seen = 0;
	int			mantissa_seen = 0, exponent_seen = 0;
	char			area[128];
	register char		*cp = area;
	const char		*str;

	/*
	 *	Retorna:
	 *		-1 leu EOF
	 *		 0 Matching error
	 *		+1 Conversão correta
	 */

	/*
	 *	Pula os brancos iniciais
	 */
	if (field-- <= 0)
		return (0);

	while (nscan++, scantb->tb[c = getc (fp)] & SPACE)
		/* vazio */;

	/*
	 *	Analisa o sinal
	 */
	switch (c)
	{
	    case '-':
	    case '+':
		*cp++ = c;

		if (field-- <= 0)
			return (0);

		c = getc (fp); nscan++;
	}

	/*
	 *	Processa a cadeia de dígitos
	 */
	for (/* vazio */; field >= 0; *cp++ = c, c = getc (fp), nscan++, field--)
	{
		if   (c == '.')
		{
			if (dot_seen++ || E_seen || sign_seen)
				break;
		}
		elif (c == 'e' || c == 'E')
		{
			if (E_seen++ || sign_seen || !mantissa_seen)
				break;
		}
		elif (c == '+' || c == '-')
		{
			if (sign_seen++ || !E_seen)
				break;
		}
		elif (scantb->tb[c] & ALGAR)
		{
			if (E_seen)
				exponent_seen++;
			else
				mantissa_seen++;
		}
		else
		{
			break;
		}

	}	/* end for (pelos dígitos) */

	/*
	 *	Ajustes finais
	 */
	ungetc (c, fp); nscan--;

	if (!mantissa_seen || E_seen && !exponent_seen)
	{
	    bad:
		if (c == EOF)
			return (-1);
		else
			return (0);
	}

	/*
	 *	Converte o número para binário
	 */
	*cp = '\0'; val = strtod (area, &str); 

	if (cp != str)
		goto bad;

	/*
	 *	Armazena o número
	 */
	switch (size)
	{
	    case NIL:
		break;

	    case SHORT:
	    case REGULAR:
		*(float *)ptr = val;
		break;

	    case LONG:
		*(double *)ptr = val;
		break;

	    case EXTRA:
		*(long double *)ptr = val;
		break;

	    default:
		_error ("_doscan: size\n");
	}

	return (1);

}	/* end float_conv */

/*
 ****************************************************************
 *	Conversão de cadeias					*
 ****************************************************************
 */
static int
str_conv (register char *str, int conv, int field, register FILE *fp)
{
	register int	c, stop;
	char		*old_str = str;

	/*
	 *	Processa 'c', 's' e '['
	 */
	if (conv == 'c' && field == NOLIMIT)
		field = 1;

	/*
	 *	Inicialmente, pula os caracteres a ignorar (stop == ignore)
	 */
	if (conv == 's')
		stop = SPACE;
	else
		stop = 0;

	if (field-- <= 0)
		return (0);

	while (nscan++, scantb->tb[c = getc (fp)] & stop)
		/* vazio */;

	/*
	 *	Processa os caracteres a reconhecer
	 */
	if   (conv == 'c')
		stop = 0;
	elif (conv == '[')
		stop = STOP;
	else /* conv == 's' */
		stop = SPACE;

	for (/* vazio */; field >= 0; c = getc (fp), nscan++, field--)
	{
		if (c == EOF || (scantb->tb[c] & stop))
			break;

		if (str != NOSTR)
			*str++ = c;
		else
			old_str++;	/* Conta o No. que teria sido */ 
	}

	/*
	 *	Conclusões
	 */
	ungetc (c, fp); nscan--;

	if (str != NOSTR)
	{
		if (str != old_str)
		{
			if (conv != 'c')
				*str = '\0';

			return (1);
		}
	}
	elif (old_str != NOSTR)
	{
		return (1);
	}

	if (c == EOF)
		return (-1);
	else
		return (0);

}	/* end str_conv */

/*
 ****************************************************************
 *	Prepara o "scanset"					*
 ****************************************************************
 */
static const char *
getscanset (register const char *fmt)
{
	register int	c, e;

	/*
	 *	É dado o conjunto de caracteres a não aceitar
	 */
	if (*fmt == '^')
	{
		fmt++;

		for (c = 0; c < 256; c++)
			scantb->tb[c] &= ~STOP;

		if   (*fmt == ']')
		{
			scantb->tb[']'] |= STOP; fmt++;

			if (*fmt == '-')
				{ scantb->tb['-'] |= STOP; fmt++; }
		}
		elif (*fmt == '-')
		{
			scantb->tb['-'] |= STOP; fmt++;
		}

		while ((c = *fmt++) != ']' && c != '\0')
		{
			scantb->tb[c] |= STOP;

			if (*fmt != '-')
				continue;

			fmt++;

			if ((e = *fmt++) == '\0')
				{ c = e; break; }

			for (/* vazio */; c <= e; c++)
				scantb->tb[c] |= STOP;
		}
	}
	else	/*	É dado o conjunto de caracteres a aceitar */
	{
		for (c = 0; c < 256; c++)
			scantb->tb[c] |= STOP;

		if   (*fmt == ']')
		{
			scantb->tb[']'] &= ~STOP; fmt++;

			if (*fmt == '-')
				{ scantb->tb['-'] &= ~STOP; fmt++; }
		}
		elif (*fmt == '-')
		{
			scantb->tb['-'] &= ~STOP; fmt++;
		}

		while ((c = *fmt++) != ']' && c != '\0')
		{
			scantb->tb[c] &= ~STOP;

			if (*fmt != '-')
				continue;

			fmt++;

			if ((e = *fmt++) == '\0')
				{ c = e; break; }

			for (/* vazio */; c <= e; c++)
				scantb->tb[c] &= ~STOP;
		}
	}

	if (c == '\0')
		fmt--;

	return (fmt);

}	/* end getscanset */

/*
 ****************************************************************
 *	Tabela							*
 ****************************************************************
 */
#define P	STOP
#define S	SPACE
#define A	ALGAR
#define X	HEXA

static const TABLE	const_scantb =
{
  /* -4    | -3    | -2    | -1 eof */

	P,	P,	P,	P,

  /* 00 nul| 01 soh| 02 stx| 03 etx| 04 eot| 05 enq| 06 ack| 07 bel */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 08 bs | 09 ht | 0a nl | 0b vt | 0c ff | 0d cr | 0e so | 0f si  */

	0,	S,	S,	S,	S,	S,	0,	0,

  /* 10 dle| 11 dc1| 12 dc2| 13 dc3| 14 dc4| 15 nak| 16 syn| 17 etb */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 18 can| 19 em | 1a sub| 1b esc| 1c fs | 1d gs | 1e rs | 1f us  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 20 sp | 21  ! | 22  " | 23  # | 24  $ | 25  1 | 26  & | 27  '  */

	S,	0,	0,	0,	0,	0,	0,	0,

  /* 28  ( | 29  ) | 2a  * | 2b  + | 2c  , | 2d  - | 2e  . | 2f  /  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 30  0 | 31  1 | 32  2 | 33  3 | 34  4 | 35  5 | 36  6 | 37  7  */

	A,	A,	A,	A,	A,	A,	A,	A,

  /* 38  8 | 39  9 | 3a  : | 3b  ; | 3c  < | 3d  = | 3e  > | 3f  ?  */

	A,	A,	0,	0,	0,	0,	0,	0,

  /* 40  @ | 41  A | 42  B | 43  C | 44  D | 45  E | 46  F | 47  G  */

	0,	X,	X,	X,	X,	X,	X,	0,

  /* 48  H | 49  I | 4a  J | 4b  K | 4c  L | 4d  M | 4e  N | 4f  O  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 50  P | 51  Q | 52  R | 53  S | 54  T | 55  U | 56  V | 57  W  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 58  X | 59  Y | 5a  Z | 5b  [ | 5c  \ | 5d  ] | 5e  ^ | 5f  _  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 60  ' | 61  a | 62  b | 63  c | 64  d | 65  e | 66  f | 67  g  */

	0,	X,	X,	X,	X,	X,	X,	0,

  /* 68  h | 69  i | 6a  j | 6b  k | 6c  l | 6d  m | 6e  n | 6f  o  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 70  p | 71  q | 72  r | 73  s | 74  t | 75  u | 76  v | 77  w  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 78  x | 79  y | 7a  z | 7b  { | 7c  | | 7d  } | 7e  ~ | 7f del */

	0,	0,	0,	0,	0,	0,	0,	0,



  /* 80    | 81    | 82    | 83    | 84    | 85    | 86    | 87     */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 88    | 89    | 8a    | 8b    | 8c    | 8d    | 8e    | 8f     */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 90    | 91    | 92    | 93    | 94    | 95    | 96    | 97     */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* 98    | 99    | 9a    | 9b    | 9c    | 9d    | 9e    | 9f     */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* a0 nbs| a1  . | a2  . | a3  . | a4  . | a5  . | a6  . | a7  .  */

	S,	0,	0,	0,	0,	0,	0,	0,

  /* a8  . | a9  . | aa  . | ab << | ac  . | ad  . | ae  . | af  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* b0  . | b1 +- | b2  . | b3  . | b4  . | b5  . | b6  . | b7  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* b8  . | b9  . | ba  . | bb >> | bc 1/4| bd 1/2| be 3/4| bf  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* c0  À | c1  Á | c2  Â | c3  Ã | c4 :A | c5 .A | c6 AE | c7  Ç  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* c8  È | c9  É | ca  Ê | cb :E | cc  Ì | cd  Í | ce  Î | cf :I  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* d0  . | d1  Ñ | d2  Ò | d3  Ó | d4  Ô | d5  Õ | d6 :O | d7 OE  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* d8 /O | d9  Ù | da  Ú | db  Û | dc :U | dd 'Y | de PP | df ss  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* e0  à | e1  á | e2  â | e3  ã | e4 :a | e5 .a | e6 ae | e7  ç  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* e8  è | e9  é | ea  ê | eb :e | ec  ì | ed  í | ee  î | ef :i  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* f0  . | f1  ñ | f2  ò | f3  ó | f4  ô | f5  õ | f6 :o | f7 oe  */

	0,	0,	0,	0,	0,	0,	0,	0,

  /* f8 /o | f9  ù | fa  ú | fb  û | fc :u | fd 'y | fe pp | ff :y  */

	0,	0,	0,	0,	0,	0,	0,	0

};	/* end const_scantb */
