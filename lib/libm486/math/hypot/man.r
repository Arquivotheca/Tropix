.bp
.he 'HYPOT (libc)'TROPIX: Manual de Referência'HYPOT (libc)'
.fo 'Escrito em 11.03.96'Versão 2.3'Pag. %'

.b NOME
.in 5
.sp
.wo "hypot -"
distância Euclideana
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	hypot (double x, double y);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "hypot" retorna o valor
.wo "{ sqrt ((x * x) + (y * y)) }"
tomando cuidado para não causar "overflow" desnecessário
durante o cálculo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Quando o valor absoluto do resultado é maior que
HUGE_VAL (maior número possível de ser representado)
a função retorna o valor HUGE_VAL.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, sqrt
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
