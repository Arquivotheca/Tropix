.bp
.he 'ASIN (libc)'TROPIX: Manual de Referência'ASIN (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções trigonométricas inversas:
.sp
.wo "asin  -"
arco-seno
.br
.wo "acos  -"
arco-cosseno
.br
.wo "atan  -"
arco-tangente
.br
.wo "atan2 -"
arco-tangente dados os catetos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	asin (double x);
double	acos (double x);
double	atan (double x);
double	atan2 (double y, double x);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "asin" e "acos" retornam, respectivamente,
em radianos, o arco-seno e o arco-cosseno do argumento "x".
O valor de "x" deve estar no intervalo [-1, +1].

.sp
A função "atan" retorna em radianos o arco-tangente do argumento "x".

.sp
A função "atan2" retorna o arco-tangente da razão "y/x"
usando os sinais de "y" e "x" para determinar o quadrante.

.in
.sp
.b
VALORES DE RETORNO
.r
.in 5
As funções "asin" e "atan" retornam valores no intervalo [-PI/2, PI/2].

.sp
A função "acos" retorna valores no intervalo [0, PI].

.sp
A função "atan2" retorna valores no intervalo [-PI, PI].

.sp
As funções "asin" e "acos" retornam um valor NULO quando
o valor de "x" está fora do intervalo [-1, +1].
A função "atan2" também retona um valor NULO quando ambos argumentos
são NULOs.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, sin
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
Intel Corporation, "Intel486 Microprocessor Family Programmer's
Reference Manual", 1993.

.sp
.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
