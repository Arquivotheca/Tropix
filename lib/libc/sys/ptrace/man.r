.bp
.he 'PTRACE (sys)'TROPIX: Manual de Referência'PTRACE (sys)'
.fo 'Atualizado em 18.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "ptrace -"
controla a execução de um processo filho
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	ptrace (int req, int pid, int addr, int data);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "ptrace" fornece um método pelo qual um processo
pai pode controlar a execução de um processo filho.
Sua utilização principal é na implementação da depuração simbólica
(veja "wsdb" (cmd)).

.sp
O processo filho comporta-se normalmente até receber um sinal
(ver "signal" (sys)), quando entra no estado STOP, e o processo
pai é informado através de uma chamada ao sistema "wait" (sys),
ou assincronamente através do sinal SIGCHLD (ver "sigchild" (sys)). 

.sp
Quando o processo filho está no estado STOP, o seu pai pode
examinar e modificar o processo na memória, utilizando a chamada ao
sistema "ptrace". Além disto, o pai pode causar o filho a terminar
ou continuar a execução, com a possibilidade de ignorar o
sinal que causou a entrada no estado STOP.

.sp
O argumento "req" determina a ação de "ptrace",
e pode ter um dos seguintes valores:
.in 8

.ip 0 7
Esta ação deve ser pedida pelo processo filho, para que a sua execução possa
ser controlado pelo pai.
Ela liga um indicador para que o processo entre no estado STOP
ao receber um sinal.
Por medida de segurança, esta chamada desliga o mecanismo de
"set user ID" e "set group ID", em uma subseqüente chamada ao sistema
"exec" (sys).
Se o processo filho chamar "exec", ele receberá o sinal SIGTRAP,
entrando no estado STOP.

.sp
Os argumentos "pid", "addr" e "data" são ignorados,
e o valor de retorno não é definido.
Se o pai do processo não controlar a execução do filho,
ocorrerão resultados imprevisíveis.

.ep
.in 5
.sp
As ações seguintes podem apenas ser pedidas pelo processo pai.
Em cada uma delas, "pid" é a identificação do processo filho,
que tem de estar no estado STOP para que estas ações possam
ser realizadas.
Todas as leituras/escritas são realizadas em unidades de palavras
(32 bites), em endereços pares.
Todos os endereços são virtuais no espaço de endereçamento do
processo filho, com exceção dos endereços da UPROC
(extensão da tabela de processos, veja <sys/uproc.h>),
que são relativos ao início desta.

.sp
.in 8
.ip "1, 2" 7
Uma palavra contida no endereço "addr" do processo filho
é retornada ao pai. O argumento "data" é ignorado.

.ip 3 7
Uma palavra contida no endereço "addr" da UPROC
do processo filho
é retornada ao pai. O argumento "data" é ignorado.

.ip "4, 5" 7
O valor dado pelo argumento "data" é escrito na
palavra de endereço "addr" do processo filho.
Deve-se utilizar a 
ação "4" para escrever na região do texto do processo
filho e a ação "5" para a região de dados. 
Isto é importante, principalmente para manter a consistência
dos textos de
programas reentrantes (veja "ld" (cmd)).
No caso de sucesso, o valor escrito é retornado.

.ip "6" 7
Através desta ação, podem ser escritas algumas
palavras na UPROC do filho.
O valor a ser escrito é dado pelo argumento "data",
e o endereço por "addr".
Apenas alguns dos registros internos e os
códigos de condição da palavra de estado do processador
podem ser escritos.

.ip "7" 7
O processo filho continua a execução.
Se "addr" for diferente de "1", a execução será continuada
a partir do endereço "addr". Em caso contrário, será continuada
a partir do ponto de interrupção.
Se "data" for igual a ZERO, todos os sinais pendentes são
cancelados antes de continuar a execução.
Se "data" contiver um número válido de sinal,
os sinais pendentes são
cancelados,
e a execução continua como se
o processo filho houvesse recebido o sinal "data".
No caso de sucesso, o valor de "data" é retornado.

.ip "8" 7
Esta ação causa o filho a terminar do mesmo modo que
"exit" (sys).

.ip "9" 7
Esta ação liga o bite de "trace"
da palavra de estado do processador
e em seguida executa os passos da ação "7".
Este bite causa a interrupção após apenas uma
instrução de máquina, possibilitando o
controle passo a passo do filho.

.ep
.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Exceto nos casos mencionados acima, "ptrace" retorna 
o valor de "data" em caso de sucesso,
e "-1" em caso contrário
(indicando em "errno" a causa do erro).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
wsdb
.br
.wo "(sys): "
signal, wait, sigchild, exit
.br

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t
