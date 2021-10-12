.bp
.he 'EXP (libc)'TROPIX: Manual de Referência'EXP (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Função exponencial:
.sp
.wo "exp   -"
exponencial na base "e"
.br
.wo "exp2  -"
exponencial na base 2
.br
.wo "exp10 -"
exponencial na base 10
.br
.wo "expb  -"
exponencial na base dada como argumento
.br
.wo "pow   -"
exponencial na base dada como argumento
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double exp (double x);
double exp2 (double x);
double exp10 (double x);
double expb (double base, double x);
double pow (double base, double x);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "exp", "exp2" e "exp10" retornam, respectivamente,
o valor de "e" (2.718...), 2.0 e 10.0 elevados à potência dada
por "x".

.sp
As funções "expb" e "pow" retornam o valor de "base"
elevado à potência dada por "x".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Quando o valor absoluto do resultado correto
é maior que HUGE_VAL (maior valor possível
de ser representado), as funções retornam o valor HUGE_VAL.

.sp
Quando o valor do resultado correto é menor que o menor valor possível de
ser representado com números normalizados ("underflow"), o resultado
obtido é um número não normalizado, embora haja perda de precisão.
No caso extremo, quando há total perda de precisão,
as funções retornam o valor ZERO.

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
Intel Corporation, "Intel486 Microprocessor Family Programmer's
Reference Manual", 1993.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, log
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
