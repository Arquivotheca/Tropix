.bp
.he 'SETPPID (libc)'TROPIX: Manual de Referência'SETPPID (libc)'
.fo 'Atualizado em 20.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "setppid -"
altera o pai de um processo

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	setppid (long ppid);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "setppid" é usada para modificar a paternalidade de
um processo. O novo pai do processo (que executou a chamada ao sistema)
passa a ser "ppid".

.sp
No momento, somente superusuários podem utilizar esta chamada ao sistema,
e o único valor de "ppid" aceito é 0 (zero), simbolizando o
processo "init". 

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
.b OBSERVAÇÃO
.in 5
No momento, a única função de "setppid" é a de tornar o processo um
filho de "init", para que o antigo processo pai não precise dar um
"wait" (sys).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro, getpid, getppid, wait
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
