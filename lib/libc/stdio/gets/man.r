.bp 1
.he 'GETS (libc)'TROPIX: Manual de Referência'GETS (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Lê uma cadeia de um arquivo:
.sp
.wo "gets   -"
lê da entrada padrão
.br
.wo "fngets -"
lê de um arquivo dado omitindo o <nl>
.br
.wo "fgets  -"
lê de um arquivo dado mantendo o <nl>
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

char	*gets (char *s);
char	*fngets (char *s, int n, FILE *fp);
char	*fgets (char *s, int n, FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "gets" lê caracteres da entrada padrão ("stdin") e os armazena
no vetor apontado por "s",
até que seja encontrado um <nl> ou seja detetado o final do arquivo.
O <nl> é substituído por um NULO na cadeia.

.sp
A função "fngets" lê caracteres do arquivo "fp" e os armazena
no vetor apontado por "s", até que (n-1) caracteres sejam lidos,
seja encontrado um <nl> ou seja detetado o final do arquivo.
O <nl> é substituído por um NULO na cadeia.

.sp
A função "fgets" é análoga à função "fngets", com a diferença
de que o <nl> é mantido, seguido de um NULO.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se for detetado o final do arquivo, e nenhum caractere foi lido,
não há transferência de caracteres e um ponteiro NULO é retornado. Caso
ocorra algum erro de leitura, um ponteiro NULO
também é retornado. Em caso contrário, é retornado "s".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
ferror, fopen, fread, getc, scanf, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
