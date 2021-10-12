.bp
.he 'SETUID (sys)'TROPIX: Manual de Referência'SETUID (sys)'
.fo 'Atualizado em 21.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Modifica as identificações do usuário/grupo do processo:
.sp
.wo "setuid  -"
modifica os UIDs real e efetivo
.br
.wo "seteuid -"
modifica o  UID efetivo
.br
.wo "setruid -"
modifica o  UID real
.br
.wo "setgid  -"
modifica os GIDs real e efetivo
.br
.wo "setegid -"
modifica o  GID efetivo
.br
.wo "setrgid -"
modifica o  GID real
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	setuid (int uid);
int	setruid (int ruid);
int	seteuid (int euid);
int	setgid (int gid);
int	setrgid (int rgid);
int	setegid (int egid);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas chamadas alteram o
identificador real do usuário (UID real),
identificador efetivo do usuário (UID efetivo),
identificador real do grupo (GID real) e
identificador efetivo do grupo (GID efetivo)
do processo corrente.

.sp
A chamada ao sistema "setuid" atribui o valor "uid" para
ambos o UID real e o UID efetivo.

.sp
A chamada ao sistema "setruid" atribui o valor "ruid" para o UID real.

.sp
A chamada ao sistema "seteuid" atribui o valor "euid" para o UID efetivo.

.sp
A chamada ao sistema "setgid" atribui o valor "gid" para
ambos o GID real e o GID efetivo.

.sp
A chamada ao sistema "setrgid" atribui o valor "rgid" para o GID real.

.sp
A chamada ao sistema "setegid" atribui o valor "egid" para o GID efetivo.

.sp
Estas chamadas só são permitidas para o superusuário ou se o argumento
já é igual a um dos identificadores real ou efetivo correspondente.

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
getuid, intro
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
