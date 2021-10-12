.bp 1
.he 'ISO (fmt)'TROPIX: Manual de Referência'ISO (fmt)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Tabelas de códigos de cararacteres:
.sp
.wo "iso   -"
código de caracteres padrão ISO (ou CBII, ou BRASCII)
.br
.wo "ascii -"
código de caracteres padrão ASCII
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O código de padrão ISO (International Standard Organization)
que é igual ao código CBII (Código Brasileiro para o 
Intercâmbio de Informações) é o código padrão do TROPIX.
Este é um código de 8 bites.

.sp
O código ASCII (American Standard Code for Information 
Interchange) é um código de 7 bites. 

.sp
Os 128 caracteres possíveis de serem representados no código
ASCII coincidem com os 128 primeiros caracteres da representação do
código ISO, que permite 256 combinações. Os códigos são
representados pela tabela abaixo onde aparecem o valor do caracter
em hexadecimal seguido do seu nome.

.sp
.nf
  | 00 nul| 01 soh| 02 stx| 03 etx| 04 eot| 05 enq| 06 ack| 07 bel|
  | 08 bs | 09 ht | 0A nl | 0B vt | 0C np | 0D cr | 0E so | 0F si |
  | 10 dle| 11 dc1| 12 dc2| 13 dc3| 14 dc4| 15 nak| 16 syn| 17 etb|
  | 18 can| 19 em | 1A sub| 1B esc| 1C fs | 1D gs | 1E rs | 1F us |
  | 20 sp | 21  ! | 22  " | 23  # | 24  $ | 25  \% | 26  & | 27  ' |
  | 28  ( | 29  ) | 2A  * | 2B  + | 2C  , | 2D  - | 2E  . | 2F  / |
  | 30  0 | 31  1 | 32  2 | 33  3 | 34  4 | 35  5 | 36  6 | 37  7 |
  | 38  8 | 39  9 | 3A  : | 3B  ; | 3C  < | 3D  = | 3E  > | 3F  ? |
  | 40  @ | 41  A | 42  B | 43  C | 44  D | 45  E | 46  F | 47  G |
  | 48  H | 49  I | 4A  J | 4B  K | 4C  L | 4D  M | 4E  N | 4F  O |
  | 50  P | 51  Q | 52  R | 53  S | 54  T | 55  U | 56  V | 57  W |
  | 58  X | 59  Y | 5A  Z | 5B  [ | 5C  \ | 5D  ] | 5E  ^ | 5F  _ |
  | 60  ` | 61  a | 62  b | 63  c | 64  d | 65  e | 66  f | 67  g |
  | 68  h | 69  i | 6A  j | 6B  k | 6C  l | 6D  m | 6E  n | 6F  o |
  | 70  p | 71  q | 72  r | 73  s | 74  t | 75  u | 76  v | 77  w |
  | 78  x | 79  y | 7A  z | 7B  { | 7C  | | 7D  } | 7E  ~ | 7F del|

  | 80    | 81    | 82    | 83    | 84    | 85    | 86    | 87    |
  | 88    | 89    | 8A    | 8B    | 8C    | 8D    | 8E    | 8F    |
  | 90    | 91    | 92    | 93    | 94    | 95    | 96    | 97    |
  | 98    | 99    | 9A    | 9B    | 9C    | 9D    | 9E    | 9F    |
  | A0 nbs| A1  ¡ | A2  ¢ | A3  £ | A4  ¤ | A5  ¥ | A6  ¦ | A7  § |
  | A8  ¨ | A9  © | AA  ª | AB  « | AC  ¬ | AD  ­ | AE  ® | AF  ¯ |
  | B0  ° | B1  ± | B2  ² | B3  ³ | B4  ´ | B5  µ | B6  ¶ | B7  · |
  | B8  ¸ | B9  ¹ | BA  º | BB  » | BC  ¼ | BD  ½ | BE  ¾ | BF  ¿ |
  | C0  À | C1  Á | C2  Â | C3  Ã | C4  Ä | C5  Å | C6  Æ | C7  Ç |
  | C8  È | C9  É | CA  Ê | CB  Ë | CC  Ì | CD  Í | CE  Î | CF  Ï |
  | D0  Ð | D1  Ñ | D2  Ò | D3  Ó | D4  Ô | D5  Õ | D6  Ö | D7  × |
  | D8  Ø | D9  Ù | DA  Ú | DB  Û | DC  Ü | DD  Ý | DE  Þ | DF  ß |
  | E0  à | E1  á | E2  â | E3  ã | E4  ä | E5  å | E6  æ | E7  ç |
  | E8  è | E9  é | EA  ê | EB  ë | EC  ì | ED  í | EE  î | EF  ï |
  | F0  ð | F1  ñ | F2  ò | F3  ó | F4  ô | F5  õ | F6  ö | F7  ÷ |
  | F8  ø | F9  ù | FA  ú | FB  û | FC  ü | FD  ý | FE  þ | FF  ÿ |

.fi
.in
.sp
.b OBSERVAÇÃO
.in 5
Como o código ISO é o padrão para o TROPIX, o
oitavo bite dos caracteres nunca é desprezado.
Para garantir a
compatibilidade de caracteres em código ASCII  com o 
código ISO, o oitavo bite do
código ASCII deve ser mantido em zero.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sort
.br
.wo "(libc):"
strcmp, memcmp, cmptb
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
