.bp
.he 'FFBS (libc)'TROPIX: Manual de Referência'FFBS (libc)'
.fo 'Atualizado em 26.12.99'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "ffbs -"
determina o índice do primeiro bit ligado
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	ffbs (unsigned long value);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "ffbs" retorna o índice do primeiro bit ligado de "value".

.sp
Os bits são numerados a partir de 0, do menos para o mais significativo.
Se não houver nenhum bit ligado (isto é, "value" for NULO), a função
retorna -1.

.sp
Repare que, se "value" só contiver um bit ligado, a função retorna o logarítmo
de "value" na base 2.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
