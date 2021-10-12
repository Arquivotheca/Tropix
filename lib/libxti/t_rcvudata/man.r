.bp
.he 'T_RCVUDATA (xti)'TROPIX: Manual de Referência'T_RCVUDATA (xti)'
.fo 'Escrito em 17.08.00'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvudata -"
recebe um datagrama
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvudata (int fd, T_UNITDATA *data, int *flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvudata" é usada para receber um datagrama de um outro
usuário no modo-sem-conexão.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    |			 |	      |		   |
	    | data->addr.maxlen	 |     x      |     /      |
	    | data->addr.len	 |     /      |     x      |
	    | data->addr.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | data->opt.maxlen	 |     x      |     /      |
	    | data->opt.len	 |     /      |     x      |
	    | data->opt.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | data->udata.maxlen |     x      |     /      |
	    | data->udata.len	 |     /      |     x      |
	    | data->udata.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | flags		 |     x      |    (x)     |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local através do qual o datagrama
será recebido, "data" contém informações associadas ao datagrama
recebido. O argumento "flags" aponta para uma variável onde serão
retornados indicadores opcionais, como descritos abaixo. Se "flags"
for um ponteiro NULO, não serão retornados os indicadores.

.sp
O argumento "data" deve apontar para uma estrutura "T_UNITDATA"
que contém os seguintes membros:

.sp
.(l
     NETBUF     addr;
     NETBUF     opt;
     NETBUF     udata;
.)l

.sp
Os campos "maxlen" de "addr", "opt" e "udata" devem ser inicializados
antes da chamada à função para indicar o tamanho máximo da área
de cada um deles.

.sp
No retorno, "addr" especifica o endereço de protocolo
(endereço INTERNET e "port") do usuário que enviou o datagrama,
"opt" identifica opções específicas do protocolo associadas ao datagrama
e "udata" contém os dados recebidos.

.sp
Normalmente, "t_rcvudata" opera no modo síncrono e espera a chegada
de dados, caso não haja dados disponíveis no momento da chamada.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_rcvudata" executa no modo assíncrono, e se não houver dados disponíveis
retorna imediatamente com -1 e atribui o valor [TNODATA]  a "t_errno".

.sp
Se a área definida no campo "udata" de "data" não é suficiente para
conter o datagrama recebido, a área será preenchida e o indicador
"T_MORE" será ligado em "flags" no retorno.
Neste caso, "t_rcvudata" deve ser chamada novamente para obter
os dados restantes.

.bc	/*************************************/
Nestas chamadas subsequentes, "t_rcvudata" retornará ZERO no
tamanho do endereço e das opções até que o datagrama seja obtido
inteiramente.
.ec	/*************************************/

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a associação de opções
específicas de protocolo com o datagrama;
portanto, no retorno "data->opt.len" será sempre NULO.

.sp
No retorno, o membro "addr" pode também fornecer o endereço IP
de destino do datagrama (isto é, do próprio computador).
Isto é útil no caso de roteadores, no qual o computador possui vários
endereços (um para cada interface).
Para obter este endereço, basta fornecer "data->addr.maxlen" com um tamanho
suficiente para 2 estruturas INADDR; ele será fornecido no segundo elemento do
vetor.

.sp
No protocolo UDP, o "checksum" é opcional. Nesta versão do sistema,
no entanto, NÃO é possível enviar um datagrama sem "checksum".
Por outro lado, é possível receber datagramas sem "checksum",
oriundos de outros sistemas operacionais.
Se o datagrama recebido NÃO contiver "checksum", será retornado
o indicador "T_NO_CHECKSUM" ligado em "flags".

.sp
Um modo de não ficar bloqueado em uma chamada a "t_rcvudata",
disponível nesta implementação
(no modo síncrono) é a utilização da função "t_nread",
que fornece o número de bytes de dados disponíveis.

.sp
A chamada ao sistema "attention" (sys) pode ser usada para aguardar
a disponibilidade de dados para "t_rcvudata".

.sp
Nesta implementação, também é possível a utilização da chamada ao
sistema "read" (sys) para receber um datagrama.
A chamada

.sp
.(l
	  read (fd, buf, nbytes);
.)l

.sp
é equivalente a

.sp
.(l
	  data->addr.maxlen  = 0;

	  data->udata.maxlen = nbytes;
	  data->udata.buf    = buf;

	  t_rcvudata (fd, data, (int *)NULL);
.)l

.sp
onde o endereço remoto contido no datagrama não é fornecido.
Isto no entanto, pode ser obtido através de "t_getaddr" (xti).

.bc	/*************************************/
.sp
O tamanho do endereço é retornado corretamente em todas as chamadas.
.ec	/*************************************/

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
fcntl, read, attention
.br
.wo "(xti): "
t_alloc, t_open, t_rcvuderr, t_sndudata, t_nread,
.br
.wo "        t_getaddr"
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
