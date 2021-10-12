.bp
.he 'T_BIND (xti)'TROPIX: Manual de Referência'T_BIND (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_bind -"
associa endereço(s) de protocolo a um "endpoint"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_bind (int fd, const T_BIND *req, T_BIND *ret);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_bind" associa endereço(s) de protocolo ao "endpoint"
especificado por "fd", ativando este "endpoint".

.sp
.(l
	    +-------------------+------------+------------+
	    |   Argumentos	| Na chamada | No retorno |
	    +-------------------+------------+------------+
	    | fd		|     x      |     /      |
	    |			|	     |		  |
	    | req->addr.maxlen	|     /      |     /      |
	    | req->addr.len	|   x >= 0   |     /      |
	    | req->addr.buf	|    x (x)   |     /      |
	    | req->qlen		|   x >= 0   |     /      |
	    |			|	     |		  |
	    | ret->addr.maxlen	|     x      |     /      |
	    | ret->addr.len	|     /	     |     x      |
	    | ret->addr.buf	|     x      |    (x)     |
	    | ret->qlen		|     /      |   x >= 0   |
	    +-------------------+------------+------------+
.)l

.sp
Após uma chamada com sucesso a "t_bind":
no modo-com-conexão, o sistema de transporte pode começar a enfileirar
pedidos de conexão recebidos para este "endpoint",
assim como utilizar este "endpoint" para enviar pedidos de conexão;
nos modo-sem-conexão e modo-de-acesso-interno o usuário pode
enviar e receber datagramas através deste "endpoint".

.sp
Os argumentos "req" e "ret" devem apontar para estruturas "T_BIND",
que contêm os seguintes membros:

.sp
.(l
     NETBUF     addr;
     unsigned   qlen;
.)l

.sp
O membro "addr" da estrutura "T_BIND" especifica o(s) endereço(s)
a ser(em) associado ao "endpoint", e o membro "qlen" é usado para indicar
o número máximo permitido de pedidos de conexão pendentes.

.sp
O argumento "req" é usado para requisitar que determinado(s)
endereço(s) sejam associado(s) ao "endpoint".
O parâmetro "len" especifica o número de bytes do(s) endereço(s)
e "buf" aponta para a área que contém o(s) endereço(s)
(um vetor de  estruturas "INADDR", veja <xti.h>).
O parâmetro "maxlen" não tem significado para o argumento "req".

.sp
No retorno, "ret" contém o(s) endereço(s) efetivamente associado(s)
ao "endpoint" (o endereço local pode ser diferente do requisitado em "req").
Em "ret", o usuário especifica "maxlen",
que é o tamanho máximo da área que irá conter o(s) endereço(s),
e "buf", que aponta para esta área.
No retorno, "len" especifica o número de bytes do(s) endereço(s)
colocado(s) em "buf". Se "maxlen" não for suficiente para
conter o(s) endereço(s), ocorrerá um erro.

.sp
Se o endereço local requisitado não está disponível,
ou se nenhum endereço local foi especificado em "req"
(o campo "len" de "addr" em "req" é zero),
o sistema de transporte irá gerar automaticamente
um endereço, que será retornado no campo "addr" de "ret".
O usuário pode comparar os endereços em "req" e "ret" para determinar
se o sistema de transporte utilizou o endereço pedido, ou se gerou
um outro endereço.

.sp
O parâmetro "req" pode ser um ponteiro NULO, caso o usuário
não deseje especificar nenhum endereço.
Neste caso, o valor de "qlen" é considerado zero e o
sistema de transporte fornecerá um endereço local para o "endpoint".
Da mesma forma, "ret" pode ser um ponteiro NULO,
se o usuário não se interessa pelo endereço local efetivamente
associado ao "endpoint" nem pelo valor negociado de "qlen".
É possível inclusive chamar a função "t_bind" com ambos os ponteiros NULOs.
Neste caso, o sistema de transporte gera um endereço local para associar
ao "endpoint" e não retorna nenhuma informação para o usuário.

.sp
O membro "qlen" tem significado apenas no modo-com-conexão.
Ele indica o número máximo de pedidos de conexão pendentes que o
sistema de transporte deve enfileirar para o "endpoint".
Um pedido de conexão pendente é aquele pedido de conexão
que já foi notificado ao usuário, mas que ainda não foi aceito
ou rejeitado.
O valor de "qlen" só deve ser maior que zero
no caso de "endpoints" que serão usados para receber pedidos de conexão
(servidores).
O valor de  "qlen" será negociado pelo sistema de transporte,
podendo ser alterado caso o valor indicado pelo usuário exceda
o limite estabelecido pelo sistema.
No retorno, o membro "qlen" de "ret" conterá o valor negociado.

.sp
A função permite que mais de um "endpoint" de transporte esteja
associado ao mesmo endereço local de protocolo, mas não é possível
associar mais de um endereço de protocolo a um "endpoint"
de transporte. Se um usuário associar mais de um "endpoint" de
transporte ao mesmo endereço de protocolo, somente um dos 
"endpoints" pode ser usado para receber pedidos de conexão.
Em outras palavras, somente um "t_bind" para um determinado
endereço de protocolo pode especificar um valor de "qlen" maior
do que zero. Deste modo, o sistema de transporte pode determinar
qual "endpoint" deve receber um pedido de conexão.

.sp
Se um usuário
tentar associar o mesmo endereço local de protocolo a um segundo "endpoint"
com o valor de "qlen" maior do que zero, o sistema de transporte
irá obter um outro endereço para ser associado àquele "endpoint",
ou (se a geração automática de endereços não for suportada), irá
atribuir [TADDRBUSY] a "t_errno" e retornar -1. Quando um usuário
aceita uma conexão em um "endpoint" de transporte que está sendo
usado como "endpoint" de recebimento de pedidos de conexão, o endereço de
protocolo estará ocupado durante esta conexão, até que seja chamada uma
das funções "t_unbind" ou "t_close".

.sp
Nenhum outro "endpoint" de
transporte pode estar recebendo pedidos de conexão no mesmo
endereço local de protocolo enquanto aquele "endpoint" inicial de recebimento
de pedidos de conexão está ativo. Isto irá impedir que mais de um
"endpoint" de transporte  associados ao mesmo endereço de protocolo 
recebam pedidos de conexão.

.bc	/*************************************/
a: "endpoint" de transporte 
b: endereço de protocolo 
c: o sistema de transporte
d: "endpoint"
e: recebimento de pedidos de conexão
.ec	/*************************************/

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação suporta a geração automática de endereços locais
(modo-com-conexão e modo-sem-conexão apenas).
Esta geração só é utilizada para a definição de "ports" locais,
e somente se o endereço local não for dado ou for dado um valor NULO
para o "port".

.sp
No modo-com-conexão:
O membro "addr" refere-se a duas estruturas "INADDR", do qual
a primeira estrutura contém o endereço local e a segunda
contém o endereço remoto. De acordo com o valor de "req->addr.len"
podem ser dadas as duas estruturas (2 * sizeof (INADDR)),
apenas uma, a do endereço local (sizeof (INADDR))
ou nenhuma (0).
No endereço local, apenas o membro "a_port" é relevante,
definindo o "port" local.
Definir o endereço remoto somente é necessário para servidores
que irão realizar a conexão no mesmo "port" local utilizado para
receber os pedidos de conexão (mas outro "endpoint").

.sp
No modo-sem-conexão: 
.bc	/*************************************/
O membro "addr" refere-se a apenas uma estrutura "INADDR", da qual
apenas o membro "a_port" é relevante, definindo o "port" local.
.ec	/*************************************/
O membro "addr" refere-se a duas estrutura "INADDR", do qual
a primeira estrutura contém o endereço local e a segunda
contém o endereço remoto. De acordo com o valor de "req->addr.len"
podem ser dadas as duas estruturas (2 * sizeof (INADDR)),
apenas uma, a do endereço local (sizeof (INADDR))
ou nenhuma (0).
Do endereço local apenas o membro "a_port" é relevante,
definindo o "port" local.
Através do endereço remoto, podemos definir o endereço destino
de datagramas enviadas através de um "write" ou de um "t_sndudata" (xti)
no qual o endereço não é dado.

.sp
No modo-de-acesso-interno:
O membro "addr" refere-se a apenas uma estrutura "INADDR", da qual
o membro "a_addr" contém o endereço remoto e o membro "a_proto"
contém o número do protocolo.
Estes dados são utilizados do seguinte modo: em um "t_sndraw" (xti):
estes valores são usados caso o membro "addr" não seja dado; em um
"t_rcvraw" (xti): estes valores são usados para "filtrar" os datagramas
de entrada: se "a_addr" for não NULO, serão aceitos apenas os datagramas
cujos endereços remotos forem iguais ao dado;
se "a_proto" for não NULO, serão aceitos apenas os datagramas de entrada,
cujos números de protocolo forem iguais ao dado.

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
t_intro, t_open, t_close, t_optmgmt, t_unbind,
.br
.wo "        t_rcvraw, t_sndraw, t_sndudata"
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
