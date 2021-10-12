.bp
.he 'RAISE (libc)'TROPIX: Manual de Referência'RAISE (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "raise -"
envia um sinal para o próprio processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <signal.h>

int	raise (int sig);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "raise" envia o sinal "sig" para o próprio programa em execução.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função retorna zero em caso de sucesso.
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
kill
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
