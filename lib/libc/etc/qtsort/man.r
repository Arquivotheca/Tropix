.bp 1
.he 'QTSORT (libc)'TROPIX: Manual de Referência'QTSORT (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
qsort - função para ordenação rápida de ponteiros

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void qtsort (void *base[], size_t nel,
		int (*comp) (const void *p1, const void *p2));
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "qtsort" ordena "nel" registros
dados através de um vetor de apontadores cujo endereço é "base".
Cada elemento do vetor aponta para um dos registros a serem ordenados. 

.sp
O vetor é ordenado em ordem ascendente de acordo com a função
de comparação "comp" (que deve ser fornecida pelo usuário).
Esta função recebe ponteiros para 2 registros, "p1" e "p2",
e deve retornar um valor negativo, nulo ou positivo caso "p1"
seja considerado menor do que, igual ou maior do que "p2".

.sp
Se 2 registros são considerados iguais por "comp", a sua ordem 
no vetor de ponteiros ordenado é indefinida.

.sp
Esta função movimenta apenas os apontadores, ao invés dos registros.
Desta forma, a ordenação é mais rápida e os registros
podem ter um tamanho variável.

.in
.sp
.b OBSERVAÇÕES
.in 5
Esta função foi implementada usando-se o algoritmo 2.21
da referência dada abaixo, com algumas otimizações para se
manter a pilha de tamanho mínimo e não usar recursão para
ordenar apenas dois elementos.

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
