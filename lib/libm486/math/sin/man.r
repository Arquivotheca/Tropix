.bp
.he 'SIN (libc)'TROPIX: Manual de Referência'SIN (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções trigonométricas diretas:
.sp
.wo "sin -"
seno
.br
.wo "cos -"
cosseno
.br
.wo "tan -"
tangente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	sin (double x);
double	cos (double x);
double	tan (double x);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "sin", "cos" e "tan" retornam, respectivamente,
o seno, cosseno e a tangente do argumento "x" dado em
radianos.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções "sin" de "cos" devolvem valores no intervalo
.wo '[-1, +1].'

.in
.sp
.b OBSERVAÇÃO
.in 5
O valor calculado pelas funções "sin" e "cos" perde precisão à medida que
o valor absoluto do argumento "x" cresce. Nos casos de valores de "x"
muito grandes, o resultado pode ser incorreto. Veja "Referências" abaixo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, asin
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
Intel Corporation, "Intel486 Microprocessor Family Programmer's
Reference Manual", 1993.
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
