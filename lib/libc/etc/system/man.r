.bp 1
.he 'SYSTEM (libc)'TROPIX: Manual de Referência'SYSTEM (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "system  -"
executa um comando
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	system (const char *comando);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "system" entrega a cadeia "comando" para o interpretador
de comandos "sh" como se a linha tivesse sido dada no terminal.
A função espera até que o comando e o "sh" terminem, e devolve
o codigo de retorno do "sh".

.sp
Uma cadeia NULA para "comando" é considerada uma consulta
sobre a existência de um interpretador de comandos.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A função "system" executa as chamadas ao sistema "fork" e "exec" 
para executar o comando. Se uma delas falhar, "system" devolve
um valor negativo e um código de erro em "errno". 

.sp
Para o caso de uma cadeia NULA, a função retorna um valor não
NULO se existe um interpretador de comandos.
No nosso caso temos o interpretador de comandos "sh", e o valor
de retorno é 1.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(sys): "
exec, fork
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
Kernighan, B. W.,
"UNIX I/O PROGRAMMING",
pg. 17.

.in
.sp
.b ARQUIVOS
.in 5
/bin/sh

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
