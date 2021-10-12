.bp
.he 'T_RCVRAW (xti)'TROPIX: Manual de Referência'T_RCVRAW (xti)'
.fo 'Escrito em 17.08.00'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvraw -"
recebe um datagrama no modo-de-acesso-interno
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvraw (int fd, T_UNITDATA *data, int *flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvraw" é usada para receber um datagrama de um
nó da rede no modo-de-acesso-interno.

.sp
O modo-de-acesso-interno interage com o nível mais básico do
sistema de transporte, permitindo o envio/recepção de datagramas com
protocolos definidos pelo usuário.
O objetivo deste modo é o de possibilitar o uso de protocolos
não convencionais e/ou experimentais. Somente o superusuário pode
usar este modo.

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
recebido e o argumento "flags" aponta para uma variável onde serão
retornados indicadores opcionais.
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
No retorno, o membro "addr" especifica o endereço de protocolo
(endereço INTERNET) do nó da rede que enviou o datagrama
e o número do protocolo do datagrama (em "a_proto"),
"opt" identifica opções específicas do protocolo associadas ao datagrama
e "udata" contém os dados recebidos.

.sp
Normalmente, "t_rcvraw" opera no modo síncrono e espera a chegada
de dados, caso não haja dados disponíveis no momento da chamada.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_rcvraw" executa no modo assíncrono, e se não houver dados disponíveis
retorna imediatamente com -1 e atribui o valor [TNODATA]  a "t_errno".

.sp
A área definida no campo "udata" de "data" deve ser suficiente para
conter o datagrama recebido. Em caso contrário, ocorrerá o erro [TBUFOVFLW].

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta função não faz parte da XTI, e portanto seu uso deve ser evitado
se for desejada portabilidade para outras implementações.

.sp
No retorno, o membro "addr" pode também fornecer o endereço IP
de destino do datagrama (isto é, do próprio computador).
Isto é útil no caso de roteadores, no qual o computador possui vários
endereços (um para cada interface).
Para obter este endereço, basta fornecer "data->addr.maxlen" com um tamanho
suficiente para 2 estruturas INADDR; ele será fornecido no segundo elemento do
vetor.

.sp
Os dados recebidos em "data->udata" representam um datagrama
no protocolo definido pelo usuário (sobre o qual o sistema de transporte
nada conhece nem interpreta). É de inteira responsabilidade do
usuário gerenciar todos os campos deste datagrama,
inclusive o "checksum" (se houver).

.sp
Nesta versão, ainda não há opções nem indicadores opcionais disponíveis;
portanto, no retorno "data->opt.len" e "flags" serão sempre NULOs.

.sp
Se forem dados o endereço remoto e/ou o número de protocolo em "t_bind" (xti),
estes valores são usados para "filtrar" os datagramas
de entrada: se "a_addr" for não NULO, serão aceitos apenas os datagramas
cujos endereços remotos forem iguais ao dado;
se "a_proto" for não NULO, serão aceitos apenas os datagramas de entrada,
cujos números de protocolo forem iguais ao dado.

.sp
Através da função "t_getaddr" (xti) é possível obter 
todos os endereços/números de protocolos associados ao "endpoint". 

.sp
Um modo de não ficar bloqueado em uma chamada a "t_rcvraw",
disponível nesta implementação
(no modo síncrono) é a utilização da função "t_nread",
que fornece o número de bytes de dados disponíveis.

.sp
A chamada ao sistema "attention" (sys) pode ser usada para aguardar
a disponibilidade de dados para "t_rcvraw".

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
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
fcntl, attention
.br
.wo "(xti): "
t_alloc, t_open, t_bind, t_nread, t_getaddr
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
