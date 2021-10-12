.bp
.he 'BSEARCH (libc)'TROPIX: Manual de Referência'BSEARCH (libc)'
.fo 'Atualizado em 09.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "bsearch -"
busca binária
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#define <stdlib.h>

void *bsearch (const void *key, const void *base, size_t n,
	size_t size, int (*compar) (const void *, const void *));
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "bsearch" realiza uma busca binária em um vetor de "n" objetos,
procurando por um membro correspondente ao objeto apontado por "key".
A correspondência é decidida pela função de comparação apontada por "compar".
O início do vetor é apontado por "base", e o tamanho de cada membro
é dado por "size".

.sp
O vetor deve estar ordenado em ordem ascendente, de acordo com a função
de comparação. Esta função é chamada com dois
argumentos que apontam para o objeto "key" e um membro do vetor, nesta
ordem. Ela deve retornar um inteiro negativo, nulo ou positivo se o objeto
"key" é, respectivamente, menor, igual ou maior do que o membro do vetor.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se houver um membro do vetor aceito por "compar", "bsearch" devolve
um ponteiro para este membro.
Em caso contrário, devolve um ponteiro NULO.

.in
.sp
.b OBSERVAÇÃO
.in 5
Se houverem membros repetidos, isto é, se mais de um membro do
vetor for aceito por "compar", é imprevisível qual deles será devolvido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
qsort, qtsort
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
