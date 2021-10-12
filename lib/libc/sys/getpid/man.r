.bp
.he 'GETPID (sys)'TROPIX: Manual de Referência'GETPID (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Obtem a identificação do:
.sp
.wo "getpid  -"
processo
.br
.wo "getpgrp -"
grupo de processos
.br
.wo "getppid -"
pai do processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

long	getpid (void);
long	getpgrp (void);
long	getppid (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getpid" devolve a identificação do processo.

.sp
A chamada ao sistema "getpgrp" devolve a identificação do
grupo de processos do qual este processo é membro.

.sp
A chamada ao sistema "getppid" devolve a identificação do 
pai do processo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, fork, intro, setpgrp, signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
