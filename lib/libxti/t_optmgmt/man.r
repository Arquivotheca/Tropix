.bp
.he 'T_OPTMGMT (xti)'TROPIX: Manual de Referência'T_OPTMGMT (xti)'
.fo 'Atualizado em 24.06.98'Versão 3.0.2'Pag. %'

.b NOME
.in 5
.wo "t_optmgmt -"
gerencia opções de um "endpoint" de transporte
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_optmgmt (int fd, const T_OPTMGMT *req, T_OPTMGMT *ret);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_optmgmt" permite que o usuário recupere, verifique
ou negocie opções específicas do protocolo utilizado.

.sp
.(l
	    +-------------------+------------+------------+
	    |   Argumentos	| Na chamada | No retorno |
	    +-------------------+------------+------------+
	    | fd		|     x      |     /      |
	    |			|	     |		  |
	    | req->opt.maxlen	|     /      |     /      |
	    | req->opt.len	|     x      |     /      |
	    | req->opt.buf	|    x (x)   |     /      |
	    | req->flags	|     x      |     /      |
	    |			|	     |		  |
	    | ret->opt.maxlen	|     x      |     /      |
	    | ret->opt.len	|     /	     |     x      |
	    | ret->opt.buf	|     x      |    (x)     |
	    | ret->flags	|     /      |     x      |
	    +-------------------+------------+------------+
.)l

.sp
O argumento "fd" identifica o  "endpoint" de transporte,
que já deve ter um endereço associado (através de "t_bind").

.sp
Os argumentos "req" e "ret" devem apontar para estruturas "T_OPTMGMT",
que contêm os seguintes membros:

.sp
.(l
     NETBUF     opt;
     long       flags;
.)l

.sp
O membro "opt" da estrutura "T_OPTMGMT" especifica as opções
de protocolo e o membro "flags" é usado para especificar a
acão a tomar com estas opções.

.sp
As opções são representadas por uma estrutura "NETBUF" de forma
similar ao endereço utilizado pela função "t_bind".
O argumento "req" é utilizado para enviar as opções ao sistema de
transporte e requisitar uma ação específica sobre estas opções.
O campo "len" especifica o número de bytes das opções,
"buf" aponta para a área com as opções e "maxlen" não tem significado
neste argumento.

.sp
O sistema de transporte pode retornar valores de opções e "flags"
para o usuário através da estrutura "ret".
Em "ret", "maxlen" especifica o tamanho da área de opções e "buf"
aponta para esta área. Após o retorno, "len" contém o número de
bytes de opções retornados.

.sp
O campo "flags" do argumento "req" deve especificar uma das ações abaixo:

.in +3
.ip T_NEGOTIATE 13
Esta ação possibilita o usuário a negociar os valores das opções
especificados em "req" com o sistema de transporte.
O sistema irá avaliar as opções requisitadas,
negociar seus valores e retornar os valores negociados em "ret".

.ip T_CHECK 13
Esta ação possibilita o usuário a verificar se as opções
especificadas em "req" são suportadas pelo sistema de transporte.
No retorno, o campo "flags" de "ret" conterá "T_SUCCESS" ou
"T_FAILURE" para indicar se as opções são suportadas.
Estes "flags" só tem significado para a ação "T_CHECK".

.ip T_DEFAULT	13
Esta ação permite que o usuário seja informado acerca das opções
"default". O sistema de transporte armazena estes valores no
campo "opt" de "ret".
Em "req", o campo "len" de "opt" deve ser zero e o campo "buf"
pode conter um ponteiro NULO.

.in -3
.ep

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Nesta implementação, a função "t_optmgmt" pode ser usada apenas para 
o protocolo TCP (isto é, no modo-com-conexão), e "req->opt.buf" e
"ret->opt.buf" devem apontar para estruturas "TCP_OPTIONS"
(ver <xti.h>), das quais (nesta versão) apenas os membros
"max_seg_size", "max_wait", "max_silence" e "window_size" são relevantes.

.sp
O membro "max_seg_size" dá o tamanho máximo do segmento permitido;
o membro "max_wait" é o tempo máximo permitido para o estabelecimento
de uma conexão (após o qual a tentativa de conexão é abortada
(não vale para o estado "T_INCON" (LISTEN)));
o membro "max_silence" é o tempo máximo permitido para uma conexão
ficar sem receber/enviar segmentos
(após o qual a conexão é abortada);
o membro "window_size" dá o tamanho da janela de recepção. Este valor
é também usado para o tamanho da área de entrada/saída para o
armazenamento do conteúdo dos segmentos.

.sp
No momento da chamada a "t_optmgmt", o estado associado ao
"endpoint" deve ser "T_IDLE" (ver "t_getstate" (xti)).

.sp
No caso da ação "T_CHECK", se "ret->opt.maxlen" indicar que "ret->opt.buf"
contém espaço suficiente para uma estrutura "TCP_OPTIONS", serão
retornados os valores das opções em uso no momento.

.sp
Repare que, para alterar apenas um (ou dois) dos membros, é necessário obter
inicialmente os valores em uso dos outros membros (através de uma
ação "T_DEFAULT" ou "T_CHECK").

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
t_accept, t_alloc, t_connect, t_getinfo, t_listen,
.br
.wo '        t_open, t_rcvconnect, t_bind, t_getstate'
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
