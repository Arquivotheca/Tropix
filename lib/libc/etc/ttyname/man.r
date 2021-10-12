.bp 1
.he 'TTYNAME (libc)'TROPIX: Manual de Referência'TTYNAME (libc)'
.fo 'Atualizado em 27.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Informações sobre o terminal:
.sp
.wo "ttyname -"
obtém o nome do terminal
.br
.wo "isatty  -"
verifica se o arquivo é um terminal
.br

.in
.sp
.b SINTAXE
.(l
#include <stdlib.h>

char	*ttyname (int fd);
int	isatty (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "ttyname" devolve um ponteiro para uma cadeia contendo
o nome do terminal associado ao descritor "fd".

.sp
A função "isatty" retorna um número não NULO se o arquivo
associado ao descritor "fd" for um terminal, e zero no caso contrário.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A função "ttyname" devolve  um ponteiro NULO,
se ao descritor não estiver associado um terminal.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "ttyname" devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
a função ser chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
(cmd): tty

.in
.sp
.b ARQUIVOS
.in 5
/dev/*

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
