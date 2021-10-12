.bp
.he 'FCLOSE (libc)'TROPIX: Manual de Referência'FCLOSE (libc)'
.fo 'Atualizado em 08.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Término do processamento de um arquivo:
.sp
.wo "fclose  -"
fecha um arquivo
.br
.wo "fflush  -"
atualiza o conteúdo do arquivo no disco
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	fclose (FILE *fp);
int	fflush (FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "fclose" atualiza o conteúdo do arquivo "fp" no disco,
e em seguida o fecha.

.sp
A função "fflush" apenas atualiza o conteúdo do arquivo "fp" no disco,
deixando-o aberto.
Se "fp" for um ponteiro NULO, atualiza o conteúdo de todos os arquivos
abertos.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Estas funções retornam 0 no caso normal, e EOF caso
ocorra algum erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "fclose" é automaticamente chamada para todos os
arquivos abertos, quando um programa termina normalmente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
close, exit
.br
.wo "(libc):"
fopen, setbuf, setvbuf
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
