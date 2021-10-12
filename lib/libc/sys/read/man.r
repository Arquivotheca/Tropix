.bp
.he 'READ (sys)'TROPIX: Manual de Referência'READ (sys)'
.fo 'Atualizado em 18.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "read -"
lê de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	read (int fd, void *area, int count);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "read" tenta ler "count" bytes do arquivo
associado ao descritor "fd" na área apontada por "area".
O descritor "fd" é obtido por uma das chamadas "creat", "open",
"dup", "fcntl", ou "pipe".

.sp
Em dispositivos capazes de mover o ponteiro de posição do arquivo,
a leitura é feita a partir da posição dada pelo ponteiro associado a "fd".
Após a leitura, o ponteiro é incrementado do valor de bytes
realmente lidos.

.sp
Em dispositivos incapazes de mover o ponteiro de  posição, a leitura
sempre é feita a partir da posição corrente. O ponteiro de posição
associado neste caso é indefinido.

.sp
Se a operação é realizada com sucesso, "read" retorna o número
de bytes realmente lidos e colocados na área; este número pode
ser menor do que "count" se o dispositivo é uma linha de comunicação
(veja "ioctl" (sys) e "termio" (fmt)), ou então se o número
de bytes restantes no arquivo é inferior a "count". Se o final
do arquivo foi atingido, é retornado um valor NULO.

.sp

Quando se tenta ler de um arquivo regular no qual se acessaria
uma região trancada por um outro processo (ver "lockf" (sys)):

.sp
.in +5
Se O_NDELAY (veja "open" (sys)) estiver ligado,
a leitura retorna imediatamente
com o erro "EAGAIN".

.sp
Se O_NDELAY estiver desligado, a leitura é bloqueada até
o processo liberar a região.
.ep
.in -5
.sp

Quando se tenta ler  de um FIFO vazio:

.sp
.in +5
Se O_NDELAY estiver ligado, a leitura retorna imediatamente
com um valor NULO.

.sp
Se O_NDELAY estiver desligado, a leitura é bloqueada até que
alguem escreva no FIFO, ou então que ele seja fechado. 
.ep
.in -5
.sp

Quando se tenta ler de uma linha de comunicação sem dados
disponíveis no momento:

.sp
.in +5
Se O_NDELAY estiver ligado, a leitura retorna imediatamente
com um valor NULO.

.sp
Se O_NDELAY estiver desligado, a leitura é bloqueada até que
dados se tornem disponíveis.
.ep
.in -5

.sp
Temos ainda a possibilidade de usar a chamada ao sistema "read" para
ler dados vindos de uma rede de comunicações
(veja "t_rcvudata" e "t_rcv" (xti)).

.bc	/*************************************/
.sp
Temos ainda a possibilidade de ler assincronamente de várias linhas de
comunicações simultaneamente (veja "sigread" (sys)).
.ec	/*************************************/

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve um inteiro
não negativo, indicando o número de bytes efetivamente lidos. 
Um valor NULO indica o final do arquivo, exceto os casos
indicados acima.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, dup, fcntl, ioctl, intro, lockf, open, pipe,
.br
.wo "(fmt): "
termio
.br
.wo "(xti): "
t_rcv, t_rcvudata
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
