.bp
.he 'T_ACCEPT (xti)'TROPIX: Manual de Referência'T_ACCEPT (xti)'
.fo 'Escrito em 30.07.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_accept -"
aceita um pedido de conexão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_accept (int fd, int resfd, const T_CALL *call);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_accept" é utilizada para aceitar um pedido de conexão.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    |			 |	      |		   |
	    | resfd		 |     x      |     /      |
	    |			 |	      |		   |
	    | call->addr.maxlen	 |     /      |     /      |
	    | call->addr.len	 |     x      |     /      |
	    | call->addr.buf	 |    ?(?)    |     /      |
	    |			 |	      |		   |
	    | call->opt.maxlen	 |     /      |     /      |
	    | call->opt.len	 |     x      |     /      |
	    | call->opt.buf	 |    ?(?)    |     /      |
	    |			 |	      |		   |
	    | call->udata.maxlen |     /      |     /      |
	    | call->udata.len	 |     x      |     /      |
	    | call->udata.buf	 |    ?(?)    |     /      |
	    |			 |	      |		   |
	    | call->sequence	 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local no qual chegou o pedido
de conexão, "resfd" identifica o "endpoint" local no qual
a conexão deve ser estabelecida, e "call" contém informações
necessárias pelo sistema de transporte para completar a conexão.

.sp
.(t
O argumento "call" deve apontar para uma estrutura
"T_CALL" que contém os seguintes membros:

.sp
.(l
     NETBUF     addr;
     NETBUF     opt;
     NETBUF     udata;
     int        sequence;
.)l
.)t

.sp
Em "call": "addr" especifica o endereço de protocolo
(endereço INTERNET e "port") do usuário remoto,
"opt" contém informações específicas do protocolo
a serem utilizadas na conexão,
"udata" contém dados opcionais a serem retornados juntamente
com a aceitação da conexão e "sequence" é o valor retornado por
"t_listen", que identifica univocamente a aceitação com uma
indicação de conexão recebida anteriormente.

.sp
Um usuário pode estabelecer a conexão no mesmo "endpoint"
no qual recebeu o pedido de conexão ("fd" == "resfd")
ou em um outro "endpoint" ("fd" != "resfd"). 
Para que o mesmo "endpoint" possa ser utilizado,
o usuário deve ter respondido a todos os pedidos
de conexão nele pendentes (através de "t_accept" ou "t_snddis").
Em caso contrário, "t_accept" irá falhar com [TBADF] em "t_errno".

.sp
Se o usuário especificar um "endpoint" diferente,
este "endpoint" já deve ter um endereço local associado
(função "t_bind") e deve estar no estado T_IDLE (veja "t_getstate"),
antes de chamar "t_accept".

.sp
Para ambos os tipos de "endpoint", "t_accept" irá falhar com [TLOOK] em
"t_errno" se houver indicações de conexão ou desconexão
(eventos T_LISTEN ou T_DISCONNECT) no "endpoint".

.sp
O argumento "opt" permite ao usuário definir opções específicas
do protocolo utilizado na conexão.
O usuário pode escolher não definir opções,
bastando para isto atribuir uma valor NULO ao campo "call->opt.len".
Neste caso, o sistema de tranporte assumirá o funcionamento
padrão.

.bc	/*************************************/
.sp
Nesta implementação (se utilizado) "call->opt.buf" deve apontar para
a estrutura "TCP_OPTIONS" (ver <xti.h>), da qual (nesta versão) apenas
o membro "max_seg_size" é relevante.
.ec	/*************************************/

.sp
O argumento "udata" permite que o usuário envie dados ao usuário
remoto juntamente com a aceitação do pedido de conexão.
No entanto, a quantidade de dados não pode exceder o limite
suportado pelo sistema de tranporte como indicado no  campo
"connect" do argumento "info" da função "t_open" ou "t_getinfo".
Se o campo "len" da estrutura "udata" em "call" tiver um valor NULO,
nenhum dado será enviado ao usuário remoto.

.sp
Todos os campos "maxlen" são irrelevantes.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
O fornecimento do endereço de protocolo do usuário remoto
é opcional, pois ele é determinado através de "call->sequence".
Se for dado, no entanto, e não conferir com o endereço disponível,
"t_accept" irá falhar com [TBADADDR] em "t_errno".

.sp
Esta implementação NÃO suporta a negociação de parâmetros
específicos de protocolo nem a transmissão de dados
durante o estabelecimento da conexão;
portanto, na chamada os valores de "call->udata.len"
e "call->opt.len" devem ser NULOs.

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
t_connect, t_getstate, t_listen, t_open, t_optmgmt,
.br
.wo "        t_rcvconnect"
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
