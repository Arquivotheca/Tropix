.bp
.he 'T_SNDUDATA (xti)'TROPIX: Manual de Referência'T_SNDUDATA (xti)'
.fo 'Escrito em 17.08.00'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "t_sndudata -"
envia um datagrama
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_sndudata (int fd, const T_UNITDATA *data);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_sndudata" é usada para enviar um datagrama para um outro
usuário no modo-sem-conexão.

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
O membro "addr" especifica o endereço de protocolo
(endereço INTERNET e "port") do usuário destinatário,
"opt" identifica opções específicas do protocolo associadas ao pedido
e "udata" contém os dados a enviar.
O usuário pode preferir não dar opções, atribuindo um valor NULO
a "opt.len"; neste caso o sistema de transporte usará opções
"default".

.sp
Normalmente, "t_sndudata" opera no modo síncrono e pode esperar caso
haja retrições de controle de fluxo.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_sndudata" executa no modo assíncrono, e se houver
retrições de controle de fluxo
retorna imediatamente com -1 e atribui o valor [TFLOW]  a "t_errno".

.sp
A quantidade de dados especificados em "udata" não pode exceder
o valor do campo "tsdu" da estrutura "info"
retornada por "t_open" ou "t_getinfo".
Se "t_sndudata" é chamada antes que o usuário destinatário
tenha ativado o seu "endpoint" (veja "t_bind"),
o datagrama pode ser descartado.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a associação de opções
específicas de protocolo com o datagrama;
portanto, na chamada "data->opt.len" deve ser NULO.

.sp
É também possível especificar o endereço IP
de fonte do datagrama (isto é, do próprio computador).
Isto é útil no caso de roteadores, no qual o computador possui vários
endereços (um para cada interface).
Para dar este endereço, basta fornecer "data->addr.len" com um tamanho
suficiente para 2 estruturas INADDR; ele deverá ser dado no segundo elemento do
vetor.

.sp
Além disto, a função "t_sndudata" só opera no modo
síncrono, isto é, ela sempre irá esperar caso haja retrições de
controle de fluxo.

.sp
No protocolo UDP, o "checksum" é opcional. Nesta versão do sistema,
no entanto, NÃO é possível enviar um datagrama sem "checksum".

.sp
Se o endereço de protocolo não for dado ("data->addr.len" NULO),
será utilizado o endereço dado no "t_bind" (xti).

.sp
Nesta implementação, também é possível a utilização da chamada ao
sistema "write" (sys) para enviar um datagrama.
A chamada

.sp
.(l
	  write (fd, buf, nbytes);
.)l

.sp
é equivalente a

.sp
.(l
	  data->addr.len  = 0;

	  data->udata.len = nbytes;
	  data->udata.buf = buf;

	  t_sndudata (fd, data);
.)l

.sp
onde o endereço remoto utilizado será o endereço dado no "t_bind" (xti).

.sp
O envio de um datagrama vazio (isto é, com "data->udata.len" NULO)
pode ser feito com "t_sndudata", mas não com "write".

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
fcntl, write
.br
.wo "(xti): "
t_alloc, t_open, t_bind, t_rcvuderr, t_rcvudata
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
