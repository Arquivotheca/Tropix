.bp
.he 'STIME (sys)'TROPIX: Manual de Referência'STIME (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "stime -"
inicializa o tempo do sistema
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	stime (const time_t *tp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "stime" inicializa o relógio do sistema.

.sp
O tempo dado no conteúdo de "tp" deve conter o número de segundos
decorridos desde 00:00:00 do dia 1. de janeiro de 1970,
horário UT (Tempo Universal, antes chamado de GMT).
.sp
Somente superusuários podem alterar o relógio do sistema.

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
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
time
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
