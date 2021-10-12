.bp
.he 'T_SNDDIS (xti)'TROPIX: Manual de Referência'T_SNDDIS (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_snddis -"
envia uma indicação de desconexão abortiva
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_snddis (int fd, const T_CALL *call);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_snddis" é usada para iniciar uma desconexão abortiva
em uma conexão já estabelecida ou rejeitar um pedido de conexão.

.sp
.(l
	+-----------------------+------------+------------+
	|   Argumentos		| Na chamada | No retorno |
	+-----------------------+------------+------------+
	| fd			|     x      |     /	  |
	|			|	     |	   	  |
	| call->addr.maxlen	|     /      |     /	  |
	| call->addr.len	|     /      |     /	  |
	| call->addr.buf	|     /      |     /	  |
	|			|	     |	   	  |
	| call->opt.maxlen	|     /      |     /	  |
	| call->opt.len		|     /      |     /	  |
	| call->opt.buf		|     /      |     /	  |
	|			|	     |	   	  |
	| call->udata.maxlen	|     /      |     /	  |
	| call->udata.len	|     x      |     /	  |
	| call->udata.buf	|    ?(?)    |     /	  |
	|			|	     |	   	  |
	| call->sequence	|     ?      |     /	  |
	|			|	     |	   	  |
	+-----------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local da conexão
e "call" especifica informações associadas à desconexão abortiva.
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
Os valores em "call" possuem significados diferentes, dependendo
do contexto no qual a função é chamada.

.sp
Para rejeitar um pedido de conexão, "call" deve ser um ponteiro
não NULO e conter um número válido de "sequence" para identificar
univocamente o pedido a ser rejeitado.
O campo "sequence" só tem significado se o "endpoint"
se encontra no estado "T_INCON".
Os campos "addr" e "opt" de "call" são ignorados.

.sp
Em todos os outros casos, "call" somente precisa ser utilizado
se forem enviados dados juntamente com a indicação de desconexão.
Os campos "addr", "opt" e "sequence" de "call" são ignorados.
Se o usuário não deseja enviar dados, "call" pode ser um ponteiro NULO.

.sp
A estrutura "udata" especifica os dados a serem enviados ao usuário remoto.
A quantidade de dados não deve exceder o valor retornado no
campo "discon" do argumento "info", obtido através de "t_open" ou "t_getinfo".
Se o campo "len" de "udata" é zero, nenhum dado será
enviado ao usuário remoto.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação NÃO suporta a transmissão de dados
juntamente com uma indicação de desconexão abortiva;
portanto, na chamada "call->udata.len" deve ser NULO.

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
t_connect, t_getinfo, t_listen, t_open
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
