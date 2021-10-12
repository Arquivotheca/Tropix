.bp
.he 'T_RCVUDERR (xti)'TROPIX: Manual de Referência'T_RCVUDERR (xti)'
.fo 'Escrito em 09.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvuderr -"
recebe uma indicação de erro de datagrama
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvuderr (int fd, T_UDERROR *uderr);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvuderr" é usada no modo-sem-conexão para receber
informações referentes a um erro ocorrido (anteriormente) durante
o envio de um datagrama.
A função deve ser chamada apenas após a ocorrência do evento
"T_UDERR".
Ela informa o usuário do sistema de transporte que o envio de
um datagrama com um endereço e opções de protocolo específicos
produziu um erro. 

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    |			 |	      |		   |
	    | uderr->addr.maxlen |     x      |     /      |
	    | uderr->addr.len	 |     /      |     x      |
	    | uderr->addr.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | uderr->opt.maxlen	 |     x      |     /      |
	    | uderr->opt.len	 |     /      |     x      |
	    | uderr->opt.buf	 |     x      |    (x)     |
	    |			 |	      |		   |
	    | uderr->error	 |     /      |     x      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local através do qual a indicação
de erro será recebida e "uderr" deve apontar para uma estrutura "T_UDERROR"
que contém os seguintes membros:

.sp
.(l
     NETBUF     addr;
     NETBUF     opt;
     long       error;
.)l

.sp
Os campos "maxlen" de "addr" e "opt" devem ser inicializados
antes da chamada à função para indicar o tamanho máximo da área
de cada um deles.

.sp
No retorno, "addr" especifica o endereço de protocolo
(endereço INTERNET e "port") do datagrama com erro, a estrutura
"opt" identifica opções específicas do protocolo associadas ao datagrama
e "error" especifica um código de erro dependente do protocolo.

.sp
Se o usuário não se importa em identificar o datagrama que causou o
erro, "uderr" pode ser um ponteiro NULO. Neste caso, "t_rcvuderr"
simplesmente irá remover a indicação de erro sem fornecer nenhuma
informação ao usuário.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Nesta implementação, esta função não é suportada; todos os erros
referentes ao envio de um datagrama são reportados imediatamente
no retorno de "t_sndudata" (portanto o evento "T_UDERR" nunca ocorre).

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
t_rcvudata, t_sndudata
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
