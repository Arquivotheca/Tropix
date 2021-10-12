.bp 1
.he 'SLEEP (libc)'TROPIX: Manual de Referência'SLEEP (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
sleep - suspende a execução do processo por um período

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	sleep (int n);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O processo corrente tem a sua execução suspensa por um período 
de "n" segundos.

.sp
O tempo real da suspensão pode
ser inferior ao pedido por duas razões: em primeiro lugar, o mecanismo
de tempo utilizado pelo sistema é baseado em
intervalos de um segundo, e o
pedido em geral não ocorre exatamente no início de um
intervalo; em segundo lugar, um outro sinal (por exemplo, vindo
de um outro processo), pode vir antes do final do período desejado.

.sp
O tempo real da suspensão pode também
ser superior ao pedido, devido ao congestionamento do sistema.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Normalmente, "sleep" retorna zero. No caso de haver um SIGALRM pendente
antes do final do período desejado, "sleep" retorna o número de segundos
que o processo retornou prematuramente.

.in
.sp
.b OBSERVAÇÃO
.in 5
A rotina é implementada utilizando o sinal SIGALRM. O estado anterior
deste sinal é restaurado, após a suspensão do processo. 

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (sys): alarm, pause, signal
.fi


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
