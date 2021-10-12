.bp
.he 'WAIT (sys)'TROPIX: Manual de Referência'WAIT (sys)'
.fo 'Atualizado em 24.06.97'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "wait -"
espera um processo filho parar ou terminar
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	wait (int *status);

extern void	*_faddr;
extern void	*_fpc;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "wait" suspende o processo pai (que a chamou)
até que um dos seus filhos termine ou pare.
Este segundo caso ocorre quando um filho que chamou "ptrace" (sys)
entrou no estado STOP.
O recebimento de um sinal
causa o retorno prematuro da chamada, e se já havia um
filho terminado ou parado antes da chamada, o retorno é imediato.

.sp
Se o ponteiro "status" for não NULO, dois bytes de informação
são armazenados na parte baixa do inteiro apontado por "status"
que (dependendo da causa do retorno), tem a seguinte interpretação:

.sp
.in 10
.ip 1. 3
EXIT:
.nf
			15	       8 7		0
			+---------------+---------------+
			|   byte mais	|   byte menos	|
			| significativo | significativo	|
			|		|		|
			|     exit	|     0x00	|
			+---------------+---------------+
.fi

.sp
Se o processo filho terminou em virtude de uma chamada "exit",
o byte menos significativo será igual a zero,
e o byte mais significativo conterá
o código de retorno que o processo filho deu como argumento
à chamada "exit" (sys). 

.ip 2. 3
SINAL:

.nf
			15	       8 7		0
			+---------------+---------------+
			|   byte mais	|   byte menos	|
			| significativo | significativo	|
			|		|		|
			|     0x00	|     sinal	|
			+---------------+---------------+
.fi
.sp
Se o processo terminou em virtude de um sinal, o byte mais significativo
será igual a zero, e o byte menos significativo conterá o número do
sinal que causou a terminação. Além disto, se o bite
7 (0x80) estiver ligado, uma imagem do processo na memória
foi copiada para o disco ("core dump"). 
No caso de alguns sinais específicos (por exemplo, "SIGBUS"
e "SIGADR"), as variáveis "_faddr" e "_fpc" conterão o endereço de acesso
e o endereço de execução 
do erro que causou o término do filho.

.ip 3. 3
STOP:

.nf
			15	       8 7		0
			+---------------+---------------+
			|   byte mais	|   byte menos	|
			| significativo | significativo	|
			|		|		|
			|     sinal	|     0x3F	|
			+---------------+---------------+
.fi

.sp
Se o processo entrou no estado STOP, o byte menos significativo
será igual a "0x3F", e o byte mais significativo conterá o número do
sinal que causou o processo a parar.

.ep
.in 5
.sp
Se um processo terminar sem esperar pelos seus processos filhos, o 
novo pai destes processos será o "init", o processo especial de
inicialização (veja "intro" (sys)).

.sp
Se um processo tiver mais de um filho, serão necessários
várias chamadas "wait", uma para cada filho.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve a identificação
do processo filho que terminou.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Um processo que termina, mas o seu processo pai não chama
"wait", permanece em um estado chamado "ZOMBIE", que
ocupa uma entrada da tabela de processos, o que é
indesejável.

.sp
.bc	/*************************************/
Também é possível incluir em uma chamada a "attention" (sys)
.ec	/*************************************/
Também é possível aguardar o término de um processo filho através
da chamada ao sistema "attention" (sys), utilizando o dispositivo
"/dev/grave".
Com isto, podemos aguardar simultaneamente vários eventos, como
a disponibilidade de dados para leitura em algum dispositivo, e o
término de um processo filho.
Após o retorno de "attention" indicando que "há dados disponíveis"
em "/dev/grave", deve-se executar a chamada ao sistema "wait".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, exit, fork, intro, pause, ptrace, signal,
.wo "         attention"
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
