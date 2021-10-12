.bp
.he 'SIGNAL (sys)'TROPIX: Manual de Referência'SIGNAL (sys)'
.fo 'Atualizado em 21.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "signal -"
gerência de sinais
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <signal.h>

void (*signal) (int sig, void (*func) (int, void *, void *))
					(int, void *, void *);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "signal" permite o processo a escolher
uma de três possibilidades de tratar a ocorrência de um sinal.
O argumento "sig" especifica o sinal e o argumento "func"
especifica a escolha desejada.

.sp
O sinal pode ser um dos seguintes, exceto SIGKILL (que sempre
cancela o processo):
.br

.nf

	SIGHUP	1	/* hangup */
	SIGINT	2	/* interrupção */
	SIGQUIT	3	/* quit */
	SIGILL	4	/* Instrução inválida */
	SIGTRAP	5	/* Trace ou breakpoint */
	SIGIOT	6	/* I/O Trap */
	SIGEMT	7	/* Emulation Trap */
	SIGFPE	8	/* Exceção de ponto flutuante */
	SIGKILL	9	/* kill, morte inevitável */
	SIGBUS	10	/* Bus error */
	SIGSEGV	11	/* Violação de segmentação */
	SIGSYS	12	/* Erro nos arg. de uma chamada ao sis. */
	SIGPIPE	13	/* Escrita em "pipe" sem leitor */
	SIGALRM	14	/* Alarme de relógio */
	SIGTERM	15	/* Sinal normal do comando "kill" */
	SIGADR	16	/* Erro de endereçamento */
	SIGDVZ	17	/* Divisão por zero */
	SIGCHK	18	/* Instrução "check" */
	SIGTRPV	19	/* Instrução "trapv" */
	SIGVIOL	20	/* Violação de privilegio */
	SIGCHLD 21	/* Término de um Filho */
	SIGABRT	22	/* Chamada à rotina ABORT */
	SIGUSR1	23	/* Sinal Reservado para o usuário (1) */
	SIGUSR2	24	/* Sinal Reservado para o usuário (2) */
.fi

.sp
O argumento "func" pode receber um de três valores: SIG_DFL, SIG_IGN,
ou o endereço de uma função.
Os valores de SIG_DFL e SIG_IGN estão definidos em <signal.h>,
e são macros que não podem ser confundidas com nenhuma função
declarada pelo usuário.

.sp
As ações prescritas pelos valores de "func" são as seguintes:
.in 8

.ip SIG_DFL 9
Termina o processo.
No caso da ocorrência do sinal, o processo é terminado,
com todas as consequências descritas em "exit" (sys).
Para alguns destes sinais, uma imagem do processo na memória
pode ser gravada no disco ("core dump"), para exame posterior.

.ip SIG_IGN 9
Ignora o sinal.
O sinal "sig" é ignorado (exceto o sinal SIGKILL, que não pode ser ignorado).

.ip função 9
Intercepta o sinal.
No caso da ocorrência do sinal, o processo executa a função
interceptora cujo endereço é "func".
Esta função deve ter a forma

.sp
.in +5
	void	func (int signo, void *pc,  void *fa);
.in -5

.sp
O argumento "signo" é o número do sinal ocorrido;
o argumento "pc" contém
o contador do programa no momento da ocorrência do sinal;
o argumento "fa" contém o endereço de memória que foi tentado
acessar, gerando uma exceção e um sinal associado
(este último argumento só é definido para alguns sinais,
tais como SIGBUS, SIGADR e SIGSEGV). 

.bc	/*************************************/
.sp
No caso particular do sinal SIGREAD, a função interceptora
é chamada com a forma:

.sp
.in +5
	func (signo, fd);
.in -5

.sp
onde "fd" é o descritor do arquivo com os dados disponíveis
(veja "sigread" (sys)).
.ec	/*************************************/

.sp
Antes de iniciar a função, o valor de "func" do sinal ocorrido
é restaurado para SIG_DFL, exceto para os sinais SIGILL e
SIGTRAP.

.sp
Após a execução da função interceptadora, o processo
retorna para o ponto que estava executando quando foi interrompido
pelo sinal. No caso de alguns sinais, tais como SIGBUS, SIGSEGV
e outros, o ponto de retorno pode ser a própria instrução que
originou o sinal, podendo acarretar malhas sem fim.  

.sp
Quando um sinal que deve ser interceptado ocorre durante um "read" (sys),
"write" (sys), "open" (sys) ou "ioctl" (sys) em um dispositivo lento
(tal como uma linha de comunicações, mas não um arquivo em disco),
ou durante um "pause" (sys) ou "wait" (sys), a função interceptadora
é executada, e a chamada ao sistema interrompida pode retornar
erro -1, com o código de erro "EINTR".

.sp
Resultados imprevisíveis ocorrerão se for dado um endereço
inválido como sendo de uma função interceptadora.

.sp
O sinal SIGKILL não pode ser interceptado.
.ep

.sp
.in 5
Uma chamada a "signal" cancela um sinal pendente, exceto se ele
for o sinal SIGKILL.

.ep
.in 0
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o valor
antigo de "func" para este sinal.
Em caso contrário, devolve SIG_ERR e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
kill
.br
.wo "(sys): "
intro, kill, pause, wait
.br
.wo "(libc):"
setjmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
