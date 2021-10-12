.bp
.he 'PCNTL (sys)'TROPIX: Manual de Referência'PCNTL (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "pcntl -"
funções de controle para processos e threads
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/pcntl.h>
#include <sys/syscall.h>

int	pcntl (int cmd, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "pcntl" engloba uma série de funções auxiliares
do sistema operacional TROPIX que controlam a execução de processos e/ou
threads.

.sp
O argumento "cmd" especifica a função desejada,
e tanto o tipo do valor de retorno da chamada, assim como os tipos
dos argumentos adicionais são variáveis, dependendo da função
específica.

.sp
As funções disponíveis são:

.in +3
.ip 1. 4
P_PMUTIMES: preenche numa estrutura "PMUTMS"; os membros "seqtime" com
o valor do tempo de execução sequencial do processo ou thread;
os membros "cseqtime" com o tempo total de todos os processos e/ou
threads que são filhos do processo em questão;
os membros "partime" dá uma estimativa do tempo da
execução deste thread em paralelo com seus filhos se houvesse
processadores disponíveis que permitissem todos os threads serem executados
em paralelo; e os membros "cpartime" dão uma estimativa
do maior tempo gasto por um thread filho se houvesse processadores
disponíveis e todos eles pudessem ser executados em paralelo.
Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/times.h>
		#include <sys/syscall.h>

		int	pcntl (P_PMUTIMES, PMUTIMES *pmutimes);
.)l

.sp
Na estimativa dos tempos de processamento paralelo, esta função leva em
consideração os tempos em que os diversos threads estão se sincronizando
através de eventos e semáforos. Desta forma, três threads, que necessitam
para a sua execução o uso exclusivo de uma região crítica, terão uma estimativa
de tempo de execução em paralelo semelhante à execução sequencial.

.sp
Observar que enquanto todos os tempos do processo começam a ser contados a
partir do início do "fork", os tempos desta chamada são contados a partir do
final do "exec". Desta forma, por exemplo, o tempo sequencial que é obtido
pelo comando "ptime" (que usa esta chamada) é sempre inferior
à soma dos tempos "user" mais "sys" obtidos pelo comando "time", além de
ser, obviamente, independente do tempo de carga do programa.

.sp
A estimativa do tempo paralelo é aproximada. É fácil verificar, entretanto,
que quanto maior a resolução do relógio do computador e quanto menor o
"time-slice" de execução de cada "thread", mais correta é a estimativa
deste tempo. 

.sp
Em caso de sucesso, a função retorna o tempo em segundos desde a
última carga do sistema operacional.

.ip 2. 4
P_GETSTKRGSZ: retorna o valor em bytes do tamanho da região de stack
do processo. Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_GETSTKRGSZ);
.)l

.sp
Em caso de sucesso, a função retorna o tamanho em bytes da região de stack.

.ip 3. 4
P_SETSTKRGSZ:
aumenta o tamanho da região de stack do processo. Se o valor dado,
sempre em bytes, é menor que o atual, o tamanho não é alterado.
Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_SETSTKRGSZ, int size);
.)l

.sp
Em caso de sucesso, a função retorna o tamanho em bytes da região de stack.

.ip 4. 4
P_GETPRI: retorna o valor da prioridade momentânea do processo.
Esses valores no TROPIX estão limitados entre 0 e 1999.
Neste caso, a chamada tem a forma


.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_GETPRI);
.)l

.sp
Em caso de sucesso, a função retorna o valor momentâneo da prioridade do
processo.

.ip 5. 4
P_SETPRI:
altera a prioridade momentânea de um processo.
Esses valores devem estar entre 0 e 999, para processos comuns, e
entre 0 e 1999 para processos de tempo real.
sempre em bytes, é menor que o atual, o tamanho não é alterado.
Esta chamada ao sistema é de uso restrito para superusuários.
Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_SETPRI, int value);
.)l

.sp
Em caso de sucesso, a função retorna o valor momentâneo da
prioridade do processo.

.ip 6. 4
P_SETRTPROC: se "flag" é igual a 1,
transforma um processo ou thread qualquer em processo ou thread de tempo real. 
Se "flag" é igual a -1, 
transforma um processo ou thread qualquer em processo ou thread regular.
Outros valores de "flag" não alteram as características do processo ou
thread.
Esta chamada ao sistema é de uso restrito para superusuários.
Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_SETRTPROC, int flag);
.)l

.sp
Em caso de sucesso, a função retorna zero,
se processo é regular e um se o processo é de tempo real.

.ip 7. 4
P_SETFPREGADDR:
indica ao sistema operacional o endereço de usuário dos registros de
emulação do coprocessador de ponto flutuante para que aquele possa fazer
o carga e o armazenamento dos valores corretos por ocasião das
trocas de contexto.
 Somente é usado pelas rotinas de emulação do coprocessador de ponto
flutuante.
Neste caso, a chamada tem a forma

.sp
.(l
		#include <sys/pcntl.h>
		#include <sys/syscall.h>

		int	pcntl (P_SETFPREGADDR, void *addr);
.)l

.sp
Em caso de sucesso, a função retorna zero,
se o endereço está no espaço de dados do usuário.

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, os respectivos valores de retorno já foram dados.
Em caso contrário, a chamada retorna -1 e indica em "errno" a causa do erro.

.bc	/*************************************/
.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, o valor de retorno da chamada depende de "cmd"
como indica a lista seguinte:
.in 10
.ip P_PMUTIMES: 12
tempo em segundos desde o último boot.
.ip P_GETSTKRGSZ: 12
tamanho em bytes da região de stack.
.ip P_SETSTKRGSZ: 12
tamanho em bytes da região de stack.
.ip P_GETPRI: 12
valor momentâneo da prioridade do processo.
.ip P_SETPRI: 12
valor momentâneo da prioridade do processo.
.ip P_SETRTPROC: 12
zero, se processo comum, um, se processo de tempo real.
.ip P_SETRTPROC: 12
zero, se o endereço está no espaço de dados do usuário.
.ep
.sp
.in 5
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.
.ec	/*************************************/

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada não é padrão, e pode sofrer
extensões/alterações conforme as necessidades.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
ptime, time
.br
.wo "(libc): "
times
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
