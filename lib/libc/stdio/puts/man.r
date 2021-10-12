.bp 1
.he 'PUTS (libc)'TROPIX: Manual de Referência'PUTS (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Escreve uma cadeia em um arquivo:
.sp
.wo "puts   -"
escreve na saída padrão ("stdout")
.br
.wo "fnputs -"
escreve em um arquivo dado inserindo um <nl>
.br
.wo "fputs  -"
escreve em um arquivo dado sem inserir um <nl>
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	puts (const char *str);
int	fnputs (const char *str, FILE *fp);
int	fputs (const char *str, FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "puts" escreve a cadeia apontada por "str",
seguida de um <nl> na saída padrão ("stdout").

.sp
A função "fnputs" escreve a cadeia apontada por "str",
seguida de um <nl> no arquivo definido por "fp".

.sp
A função "fputs" é análoga à função "fnputs", mas não escreve o <nl>.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Todas estas funções retornam ZERO em caso normal e EOF em caso de erro.
Este segundo caso irá ocorrer no caso de se tentar escrever em um
arquivo que não está aberto para a escrita.

.in
.sp
.b OBSERVAÇÃO
.in 5
Nenhuma das funções acima escreve o caractere NULO
(que termina a cadeia) no arquivo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
ferror, fopen, fread, printf, putc, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
