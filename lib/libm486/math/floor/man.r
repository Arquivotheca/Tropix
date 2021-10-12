.bp
.he 'FLOOR (libc)'TROPIX: Manual de Referência'FLOOR (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções matemáticas auxiliares:
.sp
.wo "floor -"
valor ínfimo
.br
.wo "ceil  -"
valor supremo
.br
.wo "fmod  -"
resto da divisão
.br
.wo "fabs  -"
valor absoluto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	floor (double x);
double	ceil (double x);
double	fmod (double x, double y);
double	fabs (double x);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "floor" retorna o maior valor inteiro (formato flutuante)
não maior que o valor de "x".

.sp
A função "ceil" retorna o menor valor inteiro (formato flutuante)
não menor que o valor de "x".

.sp
A função "fmod" retorna o resto da divisão (formato flutuante) de
"x" por "y", ou, então, retorna um valor nulo se "y" é zero ou se
a divisão de "x" por "y" der um resultado maior que HUGE_VAL
(maior valor possível de ser representado).
Nos casos normais, a função retorna um valor "f" que tem o mesmo
sinal de "x" e tal que
.wo "{ x = i * y + f }"
para algum inteiro "i" e tal que o valor absoluto de "f" seja
menor que o valor absoluto de "y".

.sp
A função "fabs" retorna o valor absoluto de "x".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
