.bp 1
.he 'STRHASH (libc)'TROPIX: Manual de Referência'STRHASH (libc)'
.fo 'Atualizado em 09.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "strhash -"
calcula o índice de uma cadeia em uma tabela "hash"
.br

.in
.sp
.b SINTAXE
.(l
#include <string.h>

int	strhash (const char *str, int len, int size);
.)l

.sp
.b DESCRIÇÃO
.in 5
A função "strhash" calcula a entrada correspondente 
à cadeia "str" de "len" caracteres, em uma tabela
"hash" de tamanho "size", devolvendo um número inteiro no intervalo
.wo '[0 .. size-1].'

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
sid, lid, mem
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
