.bp
.he 'FREAD (libc)'TROPIX: Manual de Referência'FREAD (libc)'
.fo 'Atualizado em 09.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Leitura/escrita sem conversão:
.sp
.wo "fread  -"
leitura de registros
.br
.wo "fwrite -"
escrita de registros
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

size_t fread (void *ptr, size_t size, size_t nregs, FILE *fp);
size_t fwrite (const void *ptr, size_t size,
					size_t nregs, FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "fread" lê "nregs" registros do arquivo "fp"
para a área de memória apontada por "ptr".
Cada registro consiste de uma seqüência de "size" bytes
(NÃO necessariamente terminado com um caractere NULO).
A função não completa a leitura
se for detetado o final do arquivo (EOF),
ou ocorrer um erro.

.sp
A função "fwrite" escreve  "nregs" registros apontados por 
"ptr" no arquivo  "fp".
A função não completa a escrita se for detetado um erro.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Ambas as funções retornam o número de itens lidos ou escritos
com sucesso.
Caso  "size" ou "nregs"
não for positivo, não será lido/escrito nenhum registro,
e será retornado zero.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
read, write
.br
.wo "(libc):"
fopen, getc, gets, printf, putc, puts, scanf, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
