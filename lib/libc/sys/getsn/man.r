.bp
.he 'GETSN (sys)'TROPIX: Manual de Referência'GETSN (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
getsn - obtém o número de série

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

long	getsn (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getsn" devolve o número de série do
computador.

.sp
O número de série do computador deve coincidir com o
número de série do sistema operacional. Caso isto não
ocorra, a chamada ao sistema "getsn" devolve -1.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
uname
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
