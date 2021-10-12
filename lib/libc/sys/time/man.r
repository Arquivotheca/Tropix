.bp
.he 'TIME (sys)'TROPIX: Manual de Referência'TIME (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Obtem a data e tempo:
.sp
.wo "time   -"
apenas em segundos
.br
.wo "mutime -"
em segundos e microsegundos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

time_t	time (time_t *tp);

#include <sys/types.h>
#include <sys/times.h>

int	mutime (MUTM *mp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "time" retorna o número de segundos decorridos
desde 00:00:00 do dia 1. de janeiro de 1970, horário UT
(Tempo Universal, antes chamado de GMT).

.sp
Se "tp" for um ponteiro não NULO, este valor será também armazenado
na posição apontada por ele.

.sp
A chamada ao sistema "mutime" 
preenche a estrutura "MUTM" apontada por "mp" cujos membros são:

.sp
.nf
	time_t	mu_time;	/* Tempo (segundos) */
	time_t	mu_utime;	/* Tempo (micro-segundos) */
.fi

.sp
Esta chamada é uma extensão da chamada ao sistema "time",
e armazena em "mu_time" o mesmo tempo descrito acima,
e em "mu_utime" o número de microsegundos adicionais transcorridos.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema "time" retorna o tempo
(como descrito acima), e a chamada "mutime" retorna zero.
Em caso contrário, as chamadas retornam -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
A chamada "mutime" retorna o valor do tempo em microsegundos, porém isto não
implica em que o relógio interno do computador tenha
esta resolução.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
stime
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
