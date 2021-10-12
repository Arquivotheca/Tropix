.bp 1
.he 'QSORT (libc)'TROPIX: Manual de Referência'QSORT (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
qsort - função para ordenação rápida

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void qsort (void *base, size_t nel, size_t size,
		int (*comp) (const void *p1, const void *p2));
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "qsort" ordena o vetor de "nel" elementos 
de "size" bytes cada, apontado por "base".

.sp
O vetor é ordenado em ordem ascendente de acordo com a função
de comparação "comp" (que deve ser fornecida pelo usuário).
Esta função recebe ponteiros para 2 elementos do vetor, "p1" e "p2",
e deve retornar um valor negativo, nulo ou positivo caso "p1"
seja considerado menor do que, igual ou maior do que "p2".

.sp
Se 2 elementos são considerados iguais por "comp", a sua ordem 
no vetor ordenado é indefinida.

.in
.sp
.b OBSERVAÇÕES
.in 5
Esta função foi implementada usando-se o algoritmo 2.21
da referência dada abaixo, com algumas otimizações para se
manter a pilha de tamanho mínimo e não usar recursão para
ordenar apenas dois elementos.

.sp
A função não requer qualquer área adicional de memória pois os
registros são intercambiados usando-se a mesma área de
memória onde os registros estão armazenados.

.sp
A função "qtsort" (libc) realiza uma ordenação utilizando
movimentação de ponteiros, e é mais rápida do que "qsort"
(embora precise de mais memória).

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
qtsort
.br

.in
.sp
.b
REFERÊNCIA
.r
.in 5
Wirth, N., "Algorithms + Data Structures = Programs", Prentice-Hall,
1976.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
