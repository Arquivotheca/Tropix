/*
 ****************************************************************
 *								*
 *			chartb.c				*
 *								*
 *	Tabelas de caracteres do preprocessador			*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 22.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/scan.h"
#include "../h/lex.h"

/*
 ****************************************************************
 *	Abreviaturas						*
 ****************************************************************
 */
#define	I	INV
#define	L	LETTER
#define	D	DIGIT
#define	Q	QUOTE
#define	S	SEPAR
#define	U	USYM
#define	B	BSYM
#define	A	APOST

/*
 ****************************************************************
 *	Tabela ISO de Categoria de  caracteres			*
 ****************************************************************
 */
entry const char	categ[] =
{
/*   00 nul| 01 soh| 02 stx| 03 etx| 04 eot| 05 enq| 06 ack| 07 bel */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   08 bs | 09 ht | 0a nl | 0b vt | 0c ff | 0d cr | 0e so | 0f si  */

	I,	S,	B,	S,	S,	S,	I,	I,

/*   10 dle| 11 dc1| 12 dc2| 13 dc3| 14 dc4| 15 nak| 16 syn| 17 etb */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   18 can| 19 em | 1a sub| 1b esc| 1c fs | 1d gs | 1e rs | 1f us  */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   20 sp | 21  ! | 22  " | 23  # | 24  $ | 25  % | 26  & | 27  '  */

	S,	B,	Q,	B,	L,	U,	B,	A,

/*   28  ( | 29  ) | 2a  * | 2b  + | 2c  , | 2d  - | 2e  . | 2f  /  */

	U,	U,	U,	U,	U,	U,	I,	B,

/*   30  0 | 31  1 | 32  2 | 33  3 | 34  4 | 35  5 | 36  6 | 37  7  */

	D,	D,	D,	D,	D,	D,	D,	D,

/*   38  8 | 39  9 | 3a  : | 3b  ; | 3c  < | 3d  = | 3e  > | 3f  ?  */

	D,	D,	I,	I,	B,	B,	B,	I,

/*   40  @ | 41  A | 42  B | 43  C | 44  D | 45  E | 46  F | 47  G  */

	I,	L,	L,	L,	L,	L,	L,	L,

/*   48  H | 49  I | 4a  J | 4b  K | 4c  L | 4d  M | 4e  N | 4f  O  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   50  P | 51  Q | 52  R | 53  S | 54  T | 55  U | 56  V | 57  W  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   58  X | 59  Y | 5a  Z | 5b  [ | 5c  \ | 5d  ] | 5e  ^ | 5f  _  */

	L,	L,	L,	I,	I,	I,	U,	L,

/*   60  ' | 61  a | 62  b | 63  c | 64  d | 65  e | 66  f | 67  g  */

	I,	L,	L,	L,	L,	L,	L,	L,

/*   68  h | 69  i | 6a  j | 6b  k | 6c  l | 6d  m | 6e  n | 6f  o  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   70  p | 71  q | 72  r | 73  s | 74  t | 75  u | 76  v | 77  w  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   78  x | 79  y | 7a  z | 7b  { | 7c  | | 7d  } | 7e  ~ | 7f del */

	L,	L,	L,	I,	B,	I,	U,	I,

/*   80    | 81    | 82    | 83    | 84    | 85    | 86    | 87     */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   88    | 89    | 8a    | 8b    | 8c    | 8d    | 8e    | 8f     */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   90    | 91    | 92    | 93    | 94    | 95    | 96    | 97     */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   98    | 99    | 9a    | 9b    | 9c    | 9d    | 9e    | 9f     */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   a0 nbs| a1  . | a2  . | a3  . | a4  . | a5  . | a6  . | a7  .  */

	S,	I,	I,	I,	I,	I,	I,	I,

/*   a8  . | a9  . | aa  . | ab << | ac  . | ad  . | ae  . | af  .  */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   b0  . | b1 +- | b2  . | b3  . | b4  . | b5  . | b6  . | b7  .  */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   b8  . | b9  . | ba  . | bb >> | bc 1/4| bd 1/2| be 3/4| bf  .  */

	I,	I,	I,	I,	I,	I,	I,	I,

/*   c0  À | c1  Á | c2  Â | c3  Ã | c4 :A | c5 .A | c6 AE | c7  Ç  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   c8 `E | c9  É | ca  Ê | cb :E | cc `I | cd  Í | ce ^I | cf :I  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   d0  . | d1  Ñ | d2 `O | d3  Ó | d4  Ô | d5  Õ | d6 :O | d7 OE  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   d8 /O | d9 `U | da  Ú | db ^U | dc :U | dd 'Y | de PP | df ss  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   e0  à | e1  á | e2  â | e3  ã | e4 :a | e5 .a | e6 ae | e7  ç  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   e8 `e | e9  é | ea  ê | eb :e | ec `i | ed  í | ee ^i | ef :i  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   f0  . | f1  ñ | f2 `o | f3  ó | f4  ô | f5  õ | f6 :o | f7 oe  */

	L,	L,	L,	L,	L,	L,	L,	L,

/*   f8 /o | f9 `u | fa  ú | fb ^u | fc :u | fd 'y | fe pp | ff :y  */

	L,	L,	L,	L,	L,	L,	L,	L

};

/*
 ****************************************************************
 *	Tabela ISO de índices de caracteres			*
 ****************************************************************
 */
entry const char	cindex[] =
{
/*   00 nul| 01 soh| 02 stx| 03 etx| 04 eot| 05 enq| 06 ack| 07 bel */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   08 bs | 09 ht | 0a nl | 0b vt | 0c ff | 0d cr | 0e so | 0f si  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   10 dle| 11 dc1| 12 dc2| 13 dc3| 14 dc4| 15 nak| 16 syn| 17 etb */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   18 can| 19 em | 1a sub| 1b esc| 1c fs | 1d gs | 1e rs | 1f us  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   20 sp | 21  ! | 22  " | 23  # | 24  $ | 25  % | 26  & | 27  '  */

	0,	1,	0,	8,	0,	0,	2,	0,

/*   28  ( | 29  ) | 2a  * | 2b  + | 2c  , | 2d  - | 2e  . | 2f  /  */

	1,	2,	3,	4,	5,	6,	0,	3,

/*   30  0 | 31  1 | 32  2 | 33  3 | 34  4 | 35  5 | 36  6 | 37  7  */

	0,	1,	2,	3,	4,	5,	6,	7,

/*   38  8 | 39  9 | 3a  : | 3b  ; | 3c  < | 3d  = | 3e  > | 3f  ?  */

	8,	9,	0,	0,	4,	5,	6,	0,

/*   40  @ | 41  A | 42  B | 43  C | 44  D | 45  E | 46  F | 47  G  */

	0,	10,	11,	12,	13,	14,	15,	0,

/*   48  H | 49  I | 4a  J | 4b  K | 4c  L | 4d  M | 4e  N | 4f  O  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   50  P | 51  Q | 52  R | 53  S | 54  T | 55  U | 56  V | 57  W  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   58  X | 59  Y | 5a  Z | 5b  [ | 5c  \ | 5d  ] | 5e  ^ | 5f  _  */

	0,	0,	0,	0,	0,	0,	7,	0,

/*   60  ' | 61  a | 62  b | 63  c | 64  d | 65  e | 66  f | 67  g  */

	0,	10,	11,	12,	13,	14,	15,	0,

/*   68  h | 69  i | 6a  j | 6b  k | 6c  l | 6d  m | 6e  n | 6f  o  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   70  p | 71  q | 72  r | 73  s | 74  t | 75  u | 76  v | 77  w  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   78  x | 79  y | 7a  z | 7b  { | 7c  | | 7d  } | 7e  ~ | 7f del */

	0,	0,	0,	0,	7,	0,	8,	0,

/*   80    | 81    | 82    | 83    | 84    | 85    | 86    | 87     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   88    | 89    | 8a    | 8b    | 8c    | 8d    | 8e    | 8f     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   90    | 91    | 92    | 93    | 94    | 95    | 96    | 97     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   98    | 99    | 9a    | 9b    | 9c    | 9d    | 9e    | 9f     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   a0 nbs| a1  . | a2  . | a3  . | a4  . | a5  . | a6  . | a7  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   a8  . | a9  . | aa  . | ab << | ac  . | ad  . | ae  . | af  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   b0  . | b1 +- | b2  . | b3  . | b4  . | b5  . | b6  . | b7  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   b8  . | b9  . | ba  . | bb >> | bc 1/4| bd 1/2| be 3/4| bf  .  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   c0  À | c1  Á | c2  Â | c3  Ã | c4 :A | c5 .A | c6 AE | c7  Ç  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   c8  È | c9  É | ca  Ê | cb :E | cc  Ì | cd  Í | ce  Î | cf :I  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   d0  . | d1  Ñ | d2  Ò | d3  Ó | d4  Ô | d5  Õ | d6 :O | d7 OE  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   d8 /O | d9  Ù | da  Ú | db  Û | dc :U | dd 'Y | de PP | df ss  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   e0  à | e1  á | e2  â | e3  ã | e4 :a | e5 .a | e6 ae | e7  ç  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   e8  è | e9  é | ea  ê | eb :e | ec  ì | ed  í | ee  î | ef :i  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   f0  . | f1  ñ | f2  ò | f3  ó | f4  ô | f5  õ | f6 :o | f7 oe  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   f8 /o | f9  ù | fa  ú | fb  û | fc :u | fd 'y | fe pp | ff :y  */

	0,	0,	0,	0,	0,	0,	0,	0

};
