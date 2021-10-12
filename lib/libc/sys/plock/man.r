.bp
.he 'PLOCK (sys)'TROPIX: Manual de Referência'PLOCK (sys)'
.fo 'Atualizado em 18.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "plock -"
tranca/libera um processo na memória interna
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	plock (int flag);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "plock" permite a uma processo ficar trancado
na memória interna. Esta chamada é permitida apenas a processos
cujo UID efetivo é de superusuário.

.sp
Processos trancados na memória interna ficam imunes ao "swapping",
exceto se não houver espaço adjacente ao processo na memória
durante um aumento de tamanho do processo.

.sp
Se "flag" for não-NULO, o processo é trancado; em caso contrário
é destrancado.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve ZERO.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Processos trancados na memória interna interferem com a compactação
da memória interna e podem causar "deadlock".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, exit, fork
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
