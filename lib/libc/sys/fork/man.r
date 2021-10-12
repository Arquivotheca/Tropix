.bp
.he 'FORK (sys)'TROPIX: Manual de Referência'FORK (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "fork -"
cria um processo (sem compartilhar a região "data")
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	fork (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "fork" cria um novo processo.
Este novo processo (processo "filho") compartilha a região "text" com o
processo original (processo "pai"), e recebe cópias das regiões
"data" e "stack" (também do processo pai).

.sp
Para ver como criar um processo filho compartilhando também a
região "data", veja "thread" (sys).

.sp
O processo filho herda do processo pai os seguintes atributos:

.sp
.in 10
Ambiente.

.sp
Indicadores de "feche na execução" dos descritores dos
arquivos abertos (veja "exec" (sys)).

.sp
Estados dos sinais (veja "signal" (sys)).

.sp
UID/GID real e efetivo.

.sp
Valor de "nice" (veja "nice" (sys)).

.sp
Identificação do grupo de terminais e de processos (veja "intro" (sys)).

.sp
Diretório corrente e raiz.

.sp
Máscara de criação de arquivos (veja "umask" (sys)).

.sp
.in 5
O processo filho difere do processo pai nos seguintes itens:

.sp
.in 10
O processo filho tem uma
identificação única (o seu PID, um inteiro de 1 a
2147483647).

.sp
Identicação do pai do processo (os dois processos não
tem o mesmo pai).

.sp
O processo filho tem sua cópia própria dos descritores dos
arquivos abertos. No entanto, estes descritores têm os ponteiros
de posição em comum com os descritores respectivos do
processo pai.

.sp
Trancas em regiões de arquivos não são herdadas (ver "lockf" (sys)).

.sp
Os tempos do processo filho são zerados.

.sp
O alarme do relógio do processo filho é desligado.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema retorna zero para o processo filho
e retorna o PID do filho para o pai.
Em caso contrário, retorna -1 para o processo pai,
nenhum filho é criado, e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que a identificação de um processo no
TROPIX é um inteiro longo,
e pode ser maior do que 30000.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec, intro, thread, nice, signal, times, umask, wait
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
