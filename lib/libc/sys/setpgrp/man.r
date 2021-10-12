.bp
.he 'SETPGRP (sys)'TROPIX: Manual de Referência'SETPGRP (sys)'
.fo 'Atualizado em 20.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "setpgrp -"
cria um novo grupo de processos 
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	setpgrp (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "setpgrp" cria um novo grupo de processos
cujo líder será o processo que a chamou (ver "intro" (sys)).

.sp
Os outros membros do grupo serão os filhos deste processo.

.sp
A identificação do grupo a que pertence o processo
tem importância para a chamada ao sistema "kill" (sys).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada devolve a identificação do novo grupo
de processos (que é igual ao PID do próprio processo).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, fork, getpid, intro, kill, signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
