.bp
.he 'SINH (libc)'TROPIX: Manual de Referência'SINH (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções hiperbólicas:
.sp
.wo "sinh -"
seno hiperbólico
.br
.wo "cosh -"
cosseno hiperbólico
.br
.wo "tanh -"
tangente hiperbólica
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	sinh (double x);
double	cosh (double x);
double	tanh (double x);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "sinh", "cosh" e "tanh" retornam, respectivamente,
o seno, cosseno e a tangente hiperbólicos de "x".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Quando o valor absoluto do resultado correto é maior que
HUGE_VAL (maior número possível de ser representado)
as funções "sinh" e "cosh" retornam HUGE_VAL,
exceto a função "sinh", que retorna -HUGE_VAL se "x" é negativo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, exp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
