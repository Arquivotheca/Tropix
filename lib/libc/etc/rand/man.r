.bp 1
.he 'RAND (libc)'TROPIX: Manual de Referência'RAND (libc)'
.fo 'Atualizado em 19.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Gerador de números aleatórios:
.sp
.wo "srand -"
inicializa o gerador
.br
.wo "rand  -"
gera um número
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

long	rand (void);
void	srand (long seed);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "rand" gera números pseudo-aleatórios entre
0 e 2147483647 (2 ** 31 - 1).

.sp
A função "srand" inicializa o gerador com a semente "seed",
que é utilizado como valor inicial para a geração da seqüência
dos números pseudo-aleatórios.
Se "srand" não for chamada, é utilizado o valor "1" como semente inicial.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A cada chamada, a função "rand" devolve o próximo
número pseudo-aleatório.

.in
.sp
.b OBSERVAÇÃO
.in 5
O método usado para a geração dos números é o
conhecido método congruencial linear onde o multiplicador
é 314159269 e o incremento é 453806245.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
(libc): getrand

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
