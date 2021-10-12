.bp
.he 'SIGCHILD (sys)'TROPIX: Manual de Referência'SIGCHILD (sys)'
.fo 'Atualizado em 21.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "sigchild -"
prepara um processo filho para enviar o sinal
.wo "           SIGCHLD"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	sigchild (long pid);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Normalmente, um processo pai espera
um processo filho terminar ou entrar no estado STOP (ver "ptrace" (sys)),
através da chamada ao sistema "wait".
Esta espera, no entanto, é síncrona, o que significa que
o processo pai só irá retornar da chamada ao sistema "wait" quando
um de seus filhos terminar ou entrar no estado STOP.

.sp
A chamada ao sistema "sigchild" permite efetuar este protocolo
de maneira assíncrona. Isto é feito do seguinte modo:
após a criação do processo filho, o pai chama "sigchild" dando
como argumento "pid" a identificação do processo filho. 
Quando este filho terminar ou entrar no estado STOP,
ele enviará o sinal
"SIGCHLD" ao pai (ver "signal" (sys)),
o qual, através da
interceptação do sinal, pode efetuar
a chamada "wait", já com a certeza de que um dos filhos terminou ou
está no estado STOP.

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
.b OBSERVAÇÕES
.in 5
Não esquecer de que, se o processo pai não programar o
sinal "SIGCHLD"
para ser interceptado ou ignorado, ele causará o seu término.

.sp
A chamada ao sistema "sigchild" é irreversível,
isto é,
uma vez efetuada a chamada, o filho inexoravelmente enviará o sinal.
Se o sinal se tornar desnecessário,
o pai deve tomar providências para ignorá-lo.

.sp
Se o processo filho entrar no estado STOP mais de uma vez,
o sinal será enviado cada
vez que isto ocorrer
(além, naturalmente, de quando o processo terminar).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
signal, ptrace
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
