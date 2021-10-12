.bp
.he 'PAUSE (sys)'TROPIX: Manual de Referência'PAUSE (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "pause -"
suspende o processo até a ocorrência de um sinal
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	pause (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "pause" suspende a execução do processo
até que ele receba um sinal. O sinal tem que ser um
que atualmente não esteja programado para ser ignorado.

.sp
Se o sinal causar a terminação do processo, "pause" não irá
retornar.

.sp
Se o sinal foi programado para ser interceptado por uma função,
e esta função retornar, o processo irá continuar após
o "pause", com um valor de retorno de -1.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se a chamada retornar,
ela devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
alarm, kill, signal, wait
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
