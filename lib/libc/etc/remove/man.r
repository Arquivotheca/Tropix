.bp
.he 'REMOVE (libc)'TROPIX: Manual de Referência'REMOVE (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "remove -"
remove uma entrada de um diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	remove (const char *filenm)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "remove" remove um ponteiro para um arquivo,
ou seja, remove a entrada do diretório cujo nome é dado por "filenm".

.sp
Quando todos os ponteiros para um arquivo foram removidos,
e nenhum processo está com o arquivo aberto, o espaço ocupado
por ele é liberado e o arquivo deixa de existir.

.sp
Se um ou mais processos estiverem com o arquivo aberto
quando o último ponteiro for removido, a sua remoção
é adiada até que todos os processos fechem o arquivo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a função devolve zero.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
rm
.br
.wo "(sys): "
close, unlink, open
.br
.wo "(libc):"
fclose, fopen
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
