.bp
.he 'T_LISTEN (xti)'TROPIX: Manual de Referência'T_LISTEN (xti)'
.fo 'Escrito em 27.07.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_listen -"
recebe pedidos de conexão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_listen (int fd, T_CALL *call);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_listen" recebe pedidos de conexão de usuários remotos.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    |			 |	      |		   |
	    | call->addr.maxlen	 |     x      |     /      |
	    | call->addr.len	 |     /      |     x      |
	    | call->addr.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | call->opt.maxlen	 |     x      |     /      |
	    | call->opt.len	 |     /      |     x      |
	    | call->opt.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | call->udata.maxlen |     x      |     /      |
	    | call->udata.len	 |     /      |     x      |
	    | call->udata.buf	 |     x      |    (?)     |
	    |			 |	      |		   |
	    | call->sequence	 |     /      |     x      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local que receberá
os pedidos de conexão, e no retorno o argumento "call" conterá
informações descrevendo o pedido recebido.
O argumento "call" deve apontar para uma estrutura "T_CALL"
que contém os seguintes membros:

.sp
.(l
     NETBUF     addr;
     NETBUF     opt;
     NETBUF     udata;
     int        sequence;
.)l

.sp
No retorno, "addr" contém o endereço ("port" e endereço INTERNET)
do usuário remoto que enviou o pedido,
"opt" contém parâmetros específicos do protocolo associados
à conexão requisitada,
"udata" contém dados recebidos juntamente com o pedido de conexão,
e "sequence" é um número que identifica univocamente o pedido de conexão.
O valor de "sequence" possibilita o usuário a receber vários pedidos de
conexão antes de responder a qualquer um deles.

.sp
Como esta função retorna valores nos campos "addr", "opt" e "udata"
da estrutura "call", o campo "maxlen" de cada um deles deve
ser inicializado antes da chamada à função, para indicar o tamanho
disponível de cada uma das áreas.

.sp
Normalmente, "t_listen" executa no modo síncrono e
espera a chegada de um pedido de conexão antes de retornar.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_listen" executa no modo assíncrono, apenas testando a presença
de um pedido de conexão. 
Se nenhum está presente, retorna -1 e atribui o valor [TNODATA] 
a "t_errno".

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a negociação de parâmetros
específicos de protocolo nem a transmissão de dados
durante o estabelecimento da conexão;
portanto, no retorno "call->udata.len" e "call->opt.len" serão
sempre NULOs.

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
.wo "(sys): "
fcntl
.br
.wo "(xti): "
t_accept, t_alloc, t_bind, t_connect, t_open,
.br
.wo "        t_optmgmt, t_rcvconnect"
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
