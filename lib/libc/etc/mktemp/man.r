.bp 1
.he 'MKTEMP (libc)'TROPIX: Manual de Referência'MKTEMP (libc)'
.fo 'Atualizado em 22.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
mktemp - gera um nome para um arquivo temporário

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

char	*mktemp (const char *pattern);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O objetivo de "mktemp" é gerar uma cadeia que possa ser usada
como o nome de um arquivo temporário. 

.sp
O argumento "pattern" da função deve ser o ponteiro para uma cadeia
de caracteres terminada por seis caracteres "X", na forma "......XXXXXX".
A função "mktemp" copia a cadeia e substitui os seis caracteres "X"
por uma configuração de caracteres tal que o nome gerado não coincida
com o de um arquivo já existente.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "mktemp" devolve um ponteiro para uma cadeia que pode ser usada
como o nome de um arquivo.

.in
.sp
.b DIAGNÓSTICOS
.in 5
Se o padrão dado não for válido, ou "mktemp" não conseguir um nome
que corresponda a um arquivo inexistente, devolve um ponteiro NULO.

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "mktemp" copia o padrão dado para uma área estática,
de tal modo que o padrão dado PODE estar
situado na região "text" de um programa reentrante,
ou então em uma
área automática, que irá ser liberada quando a função
que chamou "mktemp" retornar. 

.sp
Repare que a cadeia de caracteres dada (apontada por "pattern")
NÃO é modificada.

.sp
A rotina "mktemp" usa o número do processo na identificação
do arquivo temporário.

.sp
Em um mesmo processo, "mktemp" só consegue gerar 26 nomes
distintos a partir de um mesmo padrão.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys):  "
getpid
.br
.wo "(libc): "
tmpfile
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
