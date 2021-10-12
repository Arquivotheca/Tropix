.bp
.he 'ALARM (sys)'TROPIX: Manual de Referência'ALARM (sys)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "alarm -"
prepara um alarme do relógio
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

unsigned int	alarm (unsigned int seg);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "alarm" prepara o relógio de alarme do processo
para enviar o sinal "SIGALRM" daqui a "seg" segundos (de tempo real). 

.sp
Apenas um pedido de alarme pode estar ativo em cada instante; cada
pedido (caso ainda não tenha ocorrido o alarme) cancela o anterior.

.sp
Se "seg" é NULO, o pedido de alarme (se houver algum) é cancelado.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema retorna o tempo restante do relógio de alarme
do processo, ou ZERO se não houve uma chamada anterior para "alarm".

.in
.sp
.b OBSERVAÇÃO
.in 5
Se o sinal não foi programado para ser ignorado
ou interceptado, o processo é terminado.
.sp
O tempo real do intervalo entre a chamada ao sistema e o envio do sinal
pode ser inferior ao pedido porque o mecanismo
de tempo utilizado pelo sistema é baseado em
intervalos de um segundo, e o 
pedido em geral não ocorre exatamente no início de um
intervalo.
Por outro lado, ele tambem pode ser superior ao pedido,
devido ao congestionamento
do sistema.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
fork, pause, signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
