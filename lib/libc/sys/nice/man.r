.bp
.he 'NICE (sys)'TROPIX: Manual de Referência'NICE (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.br
.in 5
.wo "nice -"
modifica a prioridade de um processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	nice (int incr);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "nice" modifica a prioridade do processo para a
obtenção de um processador.
O valor dado "incr" é acrescentado ao valor do "nice" atual
do processo.
Valores positivos de "incr" diminuem a prioridade do processo,
e valores negativos a aumentam. 
.sp
A prioridade máxima é obtida com o valor de "nice" de 0 e a
prioridade mínima é obtida com o valor de 40.
O valor normal é de 20.
Caso a chamada resulte em valores abaixo ou acima deste limite,
o valor utilizado é o do limite correspondente.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o novo valor de "nice".
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Somente superusuários podem usar "incr" negativo.
.sp
A definição não intuitiva dada acima, em que aumentando o valor
de "nice" diminue-se a prioridade e vice-versa, corresponde
à visão do usuário da chamada ao sistema; internamente,
o TROPIX utiliza valores de -20 a +20, com valores maiores
correspondendo a prioridades maiores. São estes últimos
valores que são obtidos com o comando "ps" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ps, nice
.br
.wo "(sys): "
exec
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
