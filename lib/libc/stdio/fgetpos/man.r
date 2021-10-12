.bp
.he 'FGETPOS (libc)'TROPIX: Manual de Referência'FGETPOS (libc)'
.fo 'Atualizado em 08.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulação do ponteiro da posição corrente de arquivos: 
.sp
.wo "fgetpos -"
obtém o ponteiro
.br
.wo "fsetpos -"
restaura o ponteiro
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	fgetpos (FILE *fp, fpos_t *pos);
int	fsetpos (FILE *fp, const fpos_t *pos);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "fgetpos" armazena o valor do ponteiro da posição corrente
do arquivo "fp" no objeto apontado por "pos".

.sp
A função "fsetpos" posiciona o valor do ponteiro da posição corrente
do arquivo "fp" de acordo com o objeto apontado por "pos".
Este objeto deve conter o valor obtido através de uma chamada anterior
de "fgetpos" para o mesmo arquivo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Ambas funções devolvem ZERO em caso de sucesso.
Em caso contrário, devolvem -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Lembre-se que as funções "fgetpos" e "fsetpos" não podem ser usadas em
arquivos associados a terminais, nem a "pipes".

.sp
As funções "fgetpos" e "fsetpos" desfazem os efeitos de "ungetc".

.sp
Após um "fsetpos" em um arquivo de atualização, a próxima operação pode ser
de leitura ou escrita.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
ftell, fseek, rewind
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
