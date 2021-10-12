.bp
.he 'T_CONNECT (xti)'TROPIX: Manual de Referência'T_CONNECT (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_connect -"
estabelece uma conexão com um usuário remoto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_connect (int fd, const T_CALL *sndcall, T_CALL *rcvcall);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_connect" estabelece uma conexão com um outro usuário
através do envio de um pedido de conexão e da confirmação da aceitação
deste pedido.

.sp
.(l
	+-----------------------+------------+------------+
	|   Argumentos		| Na chamada | No retorno |
	+-----------------------+------------+------------+
	| fd			|     x      |     /	  |
	|			|	     |	   	  |
	| sndcall->addr.maxlen	|     /      |     /	  |
	| sndcall->addr.len	|     x      |     /	  |
	| sndcall->addr.buf	|    x(x)    |     /	  |
	|			|	     |	   	  |
	| sndcall->opt.maxlen	|     /      |     /	  |
	| sndcall->opt.len	|     x      |	   /	  |
	| sndcall->opt.buf	|    ?(?)    |	   /	  |
	|			|	     |		  |
	| sndcall->udata.maxlen	|     /      |	   /	  |
	| sndcall->udata.len	|     x      |	   /	  |
	| sndcall->udata.buf	|    ?(?)    |	   /	  |
	|			|	     |		  |
	| sndcall->sequence	|     /	     |	   /	  |
	|			|	     |		  |
	| rcvcall->addr.maxlen	|     x      |	   /	  |
	| rcvcall->addr.len	|     /      |	   x	  |
	| rcvcall->addr.buf	|     x      |	  (x)	  |
	|			|	     |		  |
	| rcvcall->opt.maxlen	|     x      |	   /	  |
	| rcvcall->opt.len	|     /      |	   x	  |
	| rcvcall->opt.buf	|     x      |	  (x)	  |
	|			|	     |		  |
	| rcvcall->udata.maxlen	|     x      |	   /	  |
	| rcvcall->udata.len	|     /      |	   x	  |
	| rcvcall->udata.buf	|     x      |	  (?)	  |
	|			|	     |		  |
	| rcvcall->sequence	|     /	     |	   /	  |
	+-----------------------+------------+------------+
.)l

.sp
Esta função só pode ser chamada no estado T_IDLE.
O argumento "fd" indica o "endpoint" local no qual
se estabelecerá a conexão.

.sp
.(t
Os argumentos "sndcall" e "rcvcall" devem apontar para estruturas
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
O argumento "sndcall" contém informações necessárias pelo sistema
de transporte para o estabelecimento da conexão e o argumento "rcvcall"
fornece informações sobre a conexão estabelecida.

.sp
Em "sndcall": "addr" especifica o endereço de protocolo
(endereço INTERNET e "port") do usuário remoto,
"opt" contém informações específicas do protocolo
a serem utilizadas na conexão,
"udata" contém dados opcionais a serem enviados juntamente
com o pedido de conexão e "sequence" não tem significado para esta função.

.sp
No retorno, em "rcvcall": "addr" indica o endereço do "endpoint"
remoto (através do qual foi aceita a conexão),
"opt" contém informações específicas do protocolo associadas
à conexão, "udata" contém dados opcionais recebidos juntamente com
a aceitação do pedido de conexão e "sequence" não tem significado para
esta função.

.sp
O argumento "opt" permite o usuário a definir opções específicas
do protocolo utilizado na conexão.
O usuário pode escolher não negociar opções,
bastando para isto atribuir uma valor NULO ao campo "sndcall->opt.len".
Neste caso, o sistema de tranporte assumirá o funcionamento
padrão.

.bc	/*************************************/
.sp
Nesta implementação (se utilizado) "sndcall->opt.buf" deve apontar para
a estrutura "TCP_OPTIONS" (ver <xti.h>), da qual (nesta versão) apenas
o membro "max_seg_size" é relevante.
Os campos "maxlen" e "buf" de "rcvcall->addr" e "rcvcall->opt"
devem estar inicializados antes da chamada.
.ec	/*************************************/

.sp
O argumento "udata" permite que o usuário envie dados ao usuário
remoto e receba dados deste usuário durante o estabelecimento
da conexão.
No entanto, a quantidade de dados não pode exceder o limite
suportado pelo sistema de tranporte como indicado no  campo
"connect" do argumento "info" da função "t_open" ou "t_getinfo".
Se o campo "len" da estrutura "udata" em "sndcall" tiver um valor NULO,
nenhum dado será enviado ao usuário remoto.

.sp
No retorno, os campos "addr", "opt" e "udata" de "rcvcall"
conterão valores associados à conexão estabelecida.
Deste modo, o campo "maxlen" de cada argumento deve ser inicializado,
antes da chamada a esta função, para indicar o tamanho máximo da área
de cada um. No entanto, "rcvcall" pode ser um ponteiro NULO,
caso em que nenhuma informação será retornada ao usuário.

.sp
Normalmente, "t_connect" executa no modo síncrono,
e após ter enviado o pedido de conexão,
espera pela resposta do usuário remoto antes de retornar.
Um retorno com sucesso, indica que a conexão
requisitada foi estabelecida.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_connect" executa no modo assíncrono.
Neste caso, "t_connect" não espera a resposta do usuário remoto
após o envio do pedido de conexão, retornando -1 imediatamente
com o valor [TNODATA]  em "t_errno".
No modo assíncrono, a função "t_connect" apenas inicia o processo
de estabelecimento da conexão através do envio de um pedido de conexão;
a função "t_rcvconnect" deve ser utilizada posteriormente
para completar o estabelecimento da conexão.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a negociação de parâmetros
específicos de protocolo nem a transmissão de dados
durante o estabelecimento da conexão;
portanto, na chamada os valores de "sndcall->udata.len"
e "sndcall->opt.len" devem ser NULOs.

.sp
O endereço retornado em "rcvcall->addr" é o endereço fornecido pelo
usuário remoto na confirmação da conexão, que será o endereço efetivamente
utilizado na conexão. Pode ser diferente do endereço dado na chamada
em "sndcall->addr", no caso de servidores que mantêm endereços
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
t_accept, t_alloc, t_getinfo, t_listen, t_open,
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
