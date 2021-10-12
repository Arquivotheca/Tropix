.bp
.he 'GETUID (sys)'TROPIX: Manual de Referência'GETUID (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Obtem a identificação:
.sp
.wo "getuid  -"
real do usuário do processo
.br
.wo "geteuid -"
efetiva do usuário do processo
.br
.wo "getgid  -"
real do grupo do processo
.br
.wo "getegid -"
efetiva do grupo do processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	getuid (void);
int	geteuid (void);
int	getgid (void);
int	getegid (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getuid" devolve o UID real do processo.

.sp
A chamada ao sistema "geteuid" devolve o UID efetivo do processo.

.sp
A chamada ao sistema "getgid" devolve o GID real do processo.

.sp
A chamada ao sistema "getegid" devolve o GID efetivo do processo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro, setuid
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
