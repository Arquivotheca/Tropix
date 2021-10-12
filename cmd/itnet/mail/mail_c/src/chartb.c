/*
 ****************************************************************
 *								*
 *			chartb.c				*
 *								*
 *	Tabelas de caracteres do preprocessador			*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.0.0, de 09.08.00				*
 *								*
 *	Módulo: MAIL_C						*
 *		Internet					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <time.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Abreviaturas						*
 ****************************************************************
 */
#define	A	ATOM
#define	S	SPACE
#define	K	KEYWORD
#define	V	VALUE

/*
 ****************************************************************
 *	Tabela ISO de Categoria de  caracteres			*
 ****************************************************************
 */
entry const char	categ[] =
{
/*   00 nul| 01 soh| 02 stx| 03 etx| 04 eot| 05 enq| 06 ack| 07 bel */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   08 bs | 09 ht | 0A nl | 0B vt | 0C np | 0D cr | 0E so | 0F si  */

	0,	S,	0,	0,	0,	0,	0,	0,

/*   10 dle| 11 dc1| 12 dc2| 13 dc3| 14 dc4| 15 nak| 16 syn| 17 etb */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   18 can| 19 em | 1A sub| 1B esc| 1C fs | 1D gs | 1E rs | 1F us  */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   20 sp | 21  ! | 22  " | 23  # | 24  $ | 25  % | 26  & | 27  '  */

	S,	A|V,	0,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   28  ( | 29  ) | 2A  * | 2B  + | 2C  , | 2D  - | 2E  . | 2F  /  */

	0,	0,	A|V,	A|V,	0,	A|K|V,	V,	A|V,

/*   30  0 | 31  1 | 32  2 | 33  3 | 34  4 | 35  5 | 36  6 | 37  7  */

	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   38  8 | 39  9 | 3A  : | 3B  ; | 3C  < | 3D  = | 3E  > | 3F  ?  */

	A|V,	A|V,	0,	0,	0,	A|V,	0,	A|V,

/*   40  @ | 41  A | 42  B | 43  C | 44  D | 45  E | 46  F | 47  G  */

	0,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   48  H | 49  I | 4A  J | 4B  K | 4C  L | 4D  M | 4E  N | 4F  O  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   50  P | 51  Q | 52  R | 53  S | 54  T | 55  U | 56  V | 57  W  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   58  X | 59  Y | 5A  Z | 5B  [ | 5C  \ | 5D  ] | 5E  ^ | 5F  _  */

	A|K|V,	A|K|V,	A|K|V,	0,	0,	0,	A|V,	A|K|V,

/*   60  ' | 61  a | 62  b | 63  c | 64  d | 65  e | 66  f | 67  g  */

	A|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   68  h | 69  i | 6A  j | 6B  k | 6C  l | 6D  m | 6E  n | 6F  o  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   70  p | 71  q | 72  r | 73  s | 74  t | 75  u | 76  v | 77  w  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   78  x | 79  y | 7A  z | 7B  { | 7C  | | 7D  } | 7E  ~ | 7F del */

	A|K|V,	A|K|V,	A|K|V,	A|V,	A|V,	A|V,	A|V,	0,

/*   80    | 81    | 82    | 83    | 84    | 85    | 86    | 87     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   88    | 89    | 8A    | 8B    | 8C    | 8D    | 8E    | 8F     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   90    | 91    | 92    | 93    | 94    | 95    | 96    | 97     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   98    | 99    | 9A    | 9B    | 9C    | 9D    | 9E    | 9F     */

	0,	0,	0,	0,	0,	0,	0,	0,

/*   A0 nbs| A1  ¡ | A2  ¢ | A3  £ | A4  ¤ | A5  ¥ | A6  ¦ | A7  §  */

	S,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   A8  ¨ | A9  © | AA  ª | AB  « | AC  ¬ | AD  ­ | AE  ® | AF  ¯  */

	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   B0  ° | B1  ± | B2  ² | B3  ³ | B4  ´ | B5  µ | B6  ¶ | B7  ·  */

	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   B8  ¸ | B9  ¹ | BA  º | BB  » | BC  ¼ | BD  ½ | BE  ¾ | BF  ¿  */

	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,	A|V,

/*   C0  À | C1  Á | C2  Â | C3  Ã | C4  Ä | C5  Å | C6  Æ | C7  Ç  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   C8  È | C9  É | CA  Ê | CB  Ë | CC  Ì | CD  Í | CE  Î | CF  Ï  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   D0  Ð | D1  Ñ | D2  Ò | D3  Ó | D4  Ô | D5  Õ | D6  Ö | D7  ×  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   D8  Ø | D9  Ù | DA  Ú | DB  Û | DC  Ü | DD  Ý | DE  Þ | DF  ß  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   E0  à | E1  á | E2  â | E3  ã | E4  ä | E5  å | E6  æ | E7  ç  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   E8  è | E9  é | EA  ê | EB  ë | EC  ì | ED  í | EE  î | EF  ï  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   F0  ð | F1  ñ | F2  ò | F3  ó | F4  ô | F5  õ | F6  ö | F7  ÷  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,

/*   F8  ø | F9  ù | FA  ú | FB  û | FC  ü | FD  ý | FE  þ | FF  ÿ  */

	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	A|K|V,	0

}	/* end categ */;
