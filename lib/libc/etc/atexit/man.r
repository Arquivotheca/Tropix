.bp
.he 'ATEXIT (libc)'TROPIX: Manual de Referência'ATEXIT (libc)'
.fo 'Atualizado em 08.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "atexit -"
registra funções para que sejam executadas no "exit"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	atexit (void (*func) (void));
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "atexit" registra a função apontada por "func", para que ela
seja chamada (sem argumentos) ao término normal do programa.

.sp
Ao término normal do programa (seja pela execução de "exit" (sys),
ou o retorno da função "main"), todas as funções registradas através
de "atexit" serão chamadas, na ordem inversa dos registros.
Cada função é executada tantas vezes quantas foi registrada. 

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função devolve ZERO se conseguiu registrar a função, e -1
no caso contrário.

.in
.sp
.b OBSERVAÇÕES
.in 5
No momento é possível registrar um máximo de 32 funções.

.sp
Repare que em caso de término ANORMAL do programa,
as funções NÃO são chamadas (exceto no caso de "abort" (libc)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys):  "
exit
.br
.wo "(libc): "
abort
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
