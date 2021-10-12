.bp
.he 'KILL (sys)'TROPIX: Manual de Referência'KILL (sys)'
.fo 'Atualizado em 16.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "kill -"
envia um sinal para um processo ou grupo de processos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <signal.h>

int	kill (int pid, int sig);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "kill" envia um sinal para um processo ou
grupo de processos. O processo ou grupo de processos é especificado
por "pid". O sinal enviado é especificado por "sig",
e é um elemento da lista
dada em <signal.h>, ou zero (veja a observação).

.sp
No texto abaixo, chamaremos o processo que envia o sinal
(isto é, que chamou "kill") de "transmissor",
e o processo que recebe o sinal de "receptor".

.sp
O UID real ou efetivo do transmissor
deve ser igual ao UID real ou efetivo do
receptor, a não ser que o UID efetivo
do transmissor é o do superusuário.

.sp
Os processos cujas identificações vão de "0" a "initpid" são processos
especiais; os demais são "processos regulares"
(veja "intro" (sys)).

.sp
Dependendo dos valores de "pid" e outros atributos,
o sinal é enviado para diversos
receptores:

.in +3
.ip 1. 3
.wo "PROCESSO INDIVIDUAL:"
se "pid" for maior do que zero, o sinal "sig" será enviado ao
processo de identificação "pid". Neste caso, "pid" pode ser igual
a "initpid".

.ip 2. 3
.wo "GRUPO DADO:"
se "pid" é negativo, mas não -1, "sig" será enviado a todos
processos que pertencem ao grupo de processos cuja identificação 
é o valor absoluto de "pid".

.ip 3. 3
.wo "GRUPO IMPLÍCITO:"
se "pid" é 0, o sinal será enviado a todos processos regulares
que são membros do grupo de processos a que pertence o transmissor
(ou seja, cuja identificação do grupo de processos é igual
à identificação do grupo de processos do transmissor).

.ip 4. 3
.wo "DONO IMPLÍCITO:"
se "pid" é -1 e o UID efetivo do transmissor NÃO
é o superusuário, "sig" será enviado a todos os processos
regulares cujo UID REAL é igual ao UID EFETIVO do transmissor.

.ip 5. 3
.wo "UNIVERSAL:"
se "pid" é -1 e o UID efetivo do transmissor
é o superusuário, "sig" será enviado a todos os processos regulares.

.ep
.in -3

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
Se "sig" for zero (o sinal NULO),
a consistencia de erros é realizada, mas nenhum sinal é enviado.
Isto pode ser utilizado para verificar a validade de "pid".
.sp
Processos podem enviar sinais para eles próprios.

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
getpid, intro, setpgrp, signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
