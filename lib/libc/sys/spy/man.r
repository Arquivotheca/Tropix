.bp
.he 'SPY (sys)'TROPIX: Manual de Referência'SPY (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "spy -"
chama a rotina de instrumentação do núcleo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	spy (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "spy"
chama a rotina de instrumentação do núcleo do sistema
operacional. Durante a utilização desta rotina, todo
o processamento normal do sistema operacional fica suspenso.
.sp
Esta chamada só é permitida para superusuários e é usada
somente para a depuração do sistema operacional. 

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve zero.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
