.bp
.he 'T_RCVCONNECT (xti)'TROPIX: Manual de Referência'T_RCVCONNECT (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvconnect -"
recebe a aceitação de um pedido de conexão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvconnect (int fd, T_CALL *call);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvconnect" permite um usuário a determinar o estado
de um pedido de conexão enviado anteriormente, e é usado em conjunto
com "t_connect" para estabelecer uma conexão no modo assíncrono.
A conexão estará completada após um retorno com sucesso desta função.

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
O argumento "fd" indica o "endpoint" local no qual
se estabelecerá a conexão, e no retorno o argumento "call" conterá
informações sobre a conexão estabelecida.
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
Em "call", "addr" retorna o endereço ("port" e endereço INTERNET)
associado ao "endpoint" remoto (através do qual foi aceita a conexão),
"opt" contém informações específicas do protocolo associadas
à conexão, "udata" contém dados opcionais recebidos juntamente com
a aceitação do pedido de conexão e "sequence" não tem significado para
esta função.

.sp
Como esta função retorna valores nos campos "addr", "opt" e "udata"
da estrutura "call", o campo "maxlen" de cada um deles deve
ser inicializado antes da chamada à função, para indicar o tamanho
disponível de cada uma das áreas. No entanto, "call" pode ser um
ponteiro NULO, caso em que nenhuma informação é dada ao usuário no
retorno de "t_rcvconnect".

.bc	/*************************************/
.sp
Nesta implementação (se utilizado) "call->opt.buf" deve apontar para
a estrutura "TCP_OPTIONS" (ver <xti.h>), da qual (nesta versão) apenas
o membro "max_seg_size" é relevante.
.ec	/*************************************/

.sp
Normalmente, "t_rcvconnect" executa no modo síncrono e
espera a chegada da aceitação do pedido de conexão antes de retornar.
No retorno, os campos "addr", "opt" e "udata" refletem dados
associados com a conexão.

.sp
Se o indicador O_NONBLOCK foi ligado (através de "t_open" ou "fcntl" (sys)),
"t_rcvconnect" executa no modo assíncrono, apenas testando a presença
de uma aceitação de pedido de conexão. 
Se nenhuma está presente, retorna -1 e atribui o valor [TNODATA] 
a "t_errno". Neste caso, "t_rcvconnect" tem de ser chamada novamente
para completar o estabelecimento da conexão, e obter a informação
fornecida em "call".

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a negociação de parâmetros
específicos de protocolo nem a transmissão de dados
durante o estabelecimento da conexão;
portanto, no retorno os valores de "call->udata.len"
e "call->opt.len" serão sempre NULOs.

.sp
O endereço retornado em "call->addr" é o endereço fornecido pelo
usuário remoto na confirmação da conexão, que será o endereço efetivamente
utilizado na conexão. Pode ser diferente do endereço dado em "sndcall->addr"
na chamada de "t_connect", no caso de servidores que mantêm endereços
(constantes) conhecidos para cada serviço prestado, e novos endereços
vão sendo criados para as conexões com os diversos clientes. 

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
t_accept, t_alloc, t_bind, t_connect, t_listen, t_open
.br
.wo "        t_optmgmt"
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
