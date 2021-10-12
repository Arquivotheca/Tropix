.bp
.he 'WRITE (sys)'TROPIX: Manual de Referência'WRITE (sys)'
.fo 'Atualizado em 24.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "write -"
escreve em um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	write (int fd, const void *area, int count);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "write" tenta escrever "count" bytes
da área apontada por "area",
no arquivo associado ao descritor "fd".
O descritor "fd" é obtido por uma das chamadas "creat", "open",
"dup", "fcntl", ou "pipe".

.sp
Em dispositivos capazes de mover o ponteiro de posição do arquivo,
a escrita é feita a partir da posição dada pelo ponteiro associado a "fd".
Após a escrita, o ponteiro é incrementado do valor de bytes
realmente escritos.

.sp
Em dispositivos incapazes de mover o ponteiro de  posição, a escrita
sempre é feita a partir da posição corrente. O ponteiro de posição
associado neste caso é indefinido.

.sp
Se o indicador "O_APPEND" estiver ligado, o ponteiro de posição
é colocado no final do arquivo antes de cada escrita.

.sp
Se a operação é realizada com sucesso, "write" retorna o número
de bytes realmente escritos e colocados no arquivo; 
normalmente, se este número não é igual a "count",
ocorreu alguma situação anormal.

.sp
Quando se tenta escrever em um arquivo regular no qual se acessaria
uma região trancada por um outro processo (ver "lockf" (sys)):

.sp
.in +5
Se O_NDELAY (veja "open" (sys)) estiver ligado,
a escrita retorna imediatamente
com o erro "EAGAIN".

.sp
Se O_NDELAY estiver desligado, a escrita é bloqueada até
o processo liberar a região.
.ep
.in -5

.sp
Quando se tenta escrever  em um FIFO cheio, o comportamento
da chamada depende do indicador "O_NDELAY":

.sp
.in +5
Se O_NDELAY estiver ligado, a escrita retorna imediatamente
com uma indicação de erro.

.sp
Se O_NDELAY estiver desligado, a escrita é bloqueada até que
alguém leia do FIFO (liberando espaço), ou então que ele seja fechado. 

.in -5
.sp
Temos ainda a possibilidade de usar a chamada ao sistema "write" para
escrever dados em uma rede de comunicações
(veja "t_sndudata" e "t_snd" (xti)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve um inteiro
não negativo, indicando o número de bytes efetivamente escritos.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
As escritas para um arquivo passam em geral através do
mecanismo de gerência de blocos do TROPIX, que armazena a
informação para posterior escrita física no disco.
Isto significa que
uma chamada ao sistema "write" pode retornar com sucesso,
e ocorrer um erro mais tarde, quando a informação for 
efetivamente escrita no disco.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, dup, fcntl, ioctl, intro, lockf, lseek, open,
.br
.wo "        pipe"
.br
.wo "(xti): "
t_snd, t_sndudata
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
