.bp
.he 'T_RCVDIS (xti)'TROPIX: Manual de Referência'T_RCVDIS (xti)'
.fo 'Escrito em 03.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvdis -"
recebe uma indicação de desconexão abortiva
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvdis (int fd, T_DISCON *discon);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvdis" é usada para identificar a causa de uma desconexão
abortiva e obter dados do usuário enviados com a desconexão.

.sp
.(l
	+-----------------------+------------+------------+
	|   Argumentos		| Na chamada | No retorno |
	+-----------------------+------------+------------+
	| fd			|     x      |     /	  |
	|			|	     |	   	  |
	| discon->udata.maxlen	|     x      |     /	  |
	| discon->udata.len	|     /      |     x	  |
	| discon->udata.buf	|     x      |    (?)	  |
	|			|	     |	   	  |
	| discon->reason	|     /      |     x	  |
	|			|	     |	   	  |
	| discon->sequence	|     /      |     ?	  |
	|			|	     |	   	  |
	+-----------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local onde existe (ou existia)
a conexão e o argumento "discon" deve apontar para uma estrutura
"T_DISCON" que contém os seguintes membros:

.sp
.(l
     NETBUF     udata;
     int        reason;
     int        sequence;
.)l

.sp
Após o retorno, o campo "reason" especifica a razão pela qual
a conexão foi desfeita (através de um código dependente do protocolo),
"udata" contém dados recebidos juntamente com a indicação da desconexão
abortiva e "sequence" pode identificar um pedido de conexão pendente com o
qual a desconexão está associada.

.sp
O campo "sequence" só tem significado quando "t_rcvdis" é chamado
por um usuário passivo (servidor) que já chamou "t_listen" uma ou mais
vezes e está processando os pedidos de conexão obtidos.
Neste caso, "sequence" identifica qual dos pedidos de conexão pendentes
é associado à desconexão.

.sp
Se um usuário não se interessa pelos dados associados à desconexão
nem pelos valores de "reason" e "sequence", "discon" pode ser
um ponteiro NULO. Neste caso, os dados associados ao pedido de
desconexão serão descartados.
No entanto, se um "endpoint" possuir
mais de um pedido de conexão pendente (via "t_listen") e receber um pedido
de desconexão para um deles, o usuário não será capaz de identificar
a qual dos pedidos a indicação de desconexão está associada.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a transmissão de dados
durante uma desconexão abortiva;
portanto, no retorno "discon->udata.len" será sempre NULO.

.sp
Além disto, no retorno os campos e "discon->reason" e "discon->sequence"
serão também sempre NULOs.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve o valor 0.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_alloc, t_connect, t_listen, t_open, t_snddis
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
