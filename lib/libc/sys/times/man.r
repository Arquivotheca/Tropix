.bp
.he 'TIMES (sys)'TROPIX: Manual de Referência'TIMES (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Obtem os tempos do processo e seus filhos:
.sp
.wo "times   -"
em "ticks" do relógio
.br
.wo "mutimes -"
em segundos e microsegundos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/times.h>
#include <sys/syscall.h>

time_t	times (TMS *tp);
time_t	mutimes (MUTMS *mp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "times" preenche a estrutura "TMS" apontada por "tp"
com informações de tempo de CPU utilizados pelo processo e seus filhos.
A estrutura contém os seguintes membros:
.sp
.nf
	time_t	tms_utime;	/* Tempo de usuário */
	time_t	tms_stime;	/* Tempo de sistema */
	time_t	tms_cutime;	/* Tempo de usuário dos filhos */ 
	time_t	tms_cstime;	/* Tempo de sistema dos filhos */
.fi

.sp
Esta informação inclui o processo e todos os seus filhos para
os quais ele executou um "wait". Todos os tempos são dados em
"ticks" de relógio, que é um parâmetro dependente do computador e
que pode ser obtido através do membro "y_hz" da estrutura "SCB"
(veja "getscb" (sys)).

.sp
A chamada ao sistema "mutimes" é uma extensão da chamada "times",
fornecendo os tempos já em uma forma independente do computador.
Ela preenche a estrutura "MUTMS" apontada por "tp"
com informações de tempo de CPU utilizados pelo processo e seus filhos.
A estrutura contém os seguintes membros:

.sp
.nf
	time_t	mu_utime;	/* Tempo de usuário */
	time_t	mu_uutime;
	time_t	mu_stime;	/* Tempo de sistema */
	time_t	mu_sutime;
	time_t	mu_cutime;	/* Tempo de usuário dos filhos */
	time_t	mu_cuutime;
	time_t	mu_cstime;	/* Tempo de sistema dos filhos */
	time_t	mu_csutime;
.fi

.sp
Esta informação inclui o processo e todos os seus filhos para
os quais ele executou um "wait".
Cada par fornece os tempos em segundos (na primeira variável)
e microsegundos (na segunda).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, as chamadas ao sistema devolvem o número 
de segundos decorridos desde a última carga do sistema operacional.
Em caso contrário, devolvem -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
A chamada "mutimes" devolve microsegundos, porém isto não
implica em que o relógio interno do sistema tenha
esta resolução.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, fork, time, wait
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
