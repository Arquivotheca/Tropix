.bp
.he 'T_SNDRAW (xti)'TROPIX: Manual de Referência'T_SNDRAW (xti)'
.fo 'Escrito em 17.08.00'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "t_sndraw -"
envia um datagrama no modo-de-acesso-interno
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_sndraw (int fd, const T_UNITDATA *data);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_sndraw" é usada para enviar um datagrama para um
nó da rede no modo-de-acesso-interno.

.sp
O modo-de-acesso-interno interage com o nível mais básico do
sistema de transporte, permitindo o envio/recepção de datagramas com
protocolos definidos pelo usuário.
O objetivo deste modo é o de possibilitar o uso de protocolos
não convencionais e/ou experimentais. Somente o superusuário
pode usar este modo.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    |			 |	      |		   |
	    | data->addr.maxlen	 |     /      |     /      |
	    | data->addr.len	 |     x      |     /      |
	    | data->addr.buf	 |    x(x)    |     /      |
	    |			 |	      |		   |
	    | data->opt.maxlen	 |     /      |     /      |
	    | data->opt.len	 |     x      |     /      |
	    | data->opt.buf	 |    ?(?)    |     /      |
	    |			 |	      |		   |
	    | data->udata.maxlen |     /      |     /      |
	    | data->udata.len	 |     x      |     /      |
	    | data->udata.buf	 |    x(x)    |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local através do qual o datagrama
será enviado e "data" contém informações associadas ao datagrama
que será enviado.
O argumento "data" deve apontar para uma estrutura "T_UNITDATA"
que contém os seguintes membros:

.sp
.(l
     NETBUF     addr;
     NETBUF     opt;
     NETBUF     udata;
.)l

.sp
O membro "addr" especifica o endereço de protocolo (endereço INTERNET)
do nó destinatário da rede e o número do protocolo (em "a_proto"),
"opt" identifica opções específicas do protocolo associadas ao pedido
e "udata" contém os dados a enviar.
O usuário pode preferir não dar opções, atribuindo um valor NULO
a "opt.len"; neste caso o sistema de transporte usará opções
"default".

.sp
Normalmente, "t_sndraw" opera no modo síncrono e pode esperar caso
haja retrições de controle de fluxo.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_sndraw" executa no modo assíncrono, e se houver
retrições de controle de fluxo
retorna imediatamente com -1 e atribui o valor [TFLOW]  a "t_errno".

.sp
A quantidade de dados especificados em "udata" não pode exceder
o valor do campo "tsdu" da estrutura "info"
retornada por "t_open" ou "t_getinfo".

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Os dados fornecidos em "data->udata" representam um datagrama
no protocolo definido pelo usuário (sobre o qual o sistema de transporte
nada conhece nem interpreta). É de inteira responsabilidade do
usuário gerenciar todos os campos deste datagrama,
inclusive o "checksum" (se houver).

.sp
É também possível especificar o endereço IP
de fonte do datagrama (isto é, do próprio computador).
Isto é útil no caso de roteadores, no qual o computador possui vários
endereços (um para cada interface).
Para dar este endereço, basta fornecer "data->addr.len" com um tamanho
suficiente para 2 estruturas INADDR; ele deverá ser dado no segundo elemento do
vetor.

.sp
Nesta versão, ainda não há opções disponíveis;
portanto, na chamada "data->opt.len" deve ser NULO.

.sp
Além disto, a função "t_sndraw" só opera no modo
síncrono, isto é, ela sempre irá esperar caso haja retrições de
controle de fluxo.

.sp
Se o endereço/número do protocolo não for dado no "t_sndraw"
(isto é, se "data->addr.len" for NULO), serão
usados os valores dados no "t_bind" (xti).

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
t_open, t_bind
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
