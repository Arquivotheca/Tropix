.bp
.he 'T_SYNC (xti)'TROPIX: Manual de Referência'T_SYNC (xti)'
.fo 'Escrito em 09.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_sync -"
sincroniza a biblioteca do sistema de transporte
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_sync (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_sync" sincroniza as estruturas de dados da biblioteca
de transporte com as informações do sistema de transporte.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local que está utilizando
as estruturas de dados.

.sp
A função "t_sync" pode converter um descritor de arquivos não
inicializado (obtido através de "open", "dup" ou como resultado
de "fork"/"exec") em um "endpoint" de transporte inicializado.
Esta função também permite dois processos cooperantes a sincronizar
as suas interações com o sistema de transporte.

.sp
É importante lembrar que o sistema de transporte trata todos os
usuários de um "endpoint" de transporte como um usuário apenas.
Se vários processos estão usando o mesmo "endpoint", eles devem
coordenar suas atividades para que o estado do "endpoint" não
seja violado. A função "t_sync" retorna o estado corrente do
"endpoint" para o usuário, possibilitando-o a verificar este
estado antes de tomar outras ações. Esta coordenação só é
válida entre processos cooperantes; é possível que um processo
ou um evento mude o estado do "endpoint" após "t_sync" ter sido
chamada.

.sp
Se o "endpoint" de transporte está trocando de estado no momento em que
a "t_sync" foi chamada, a função irá falhar.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Nesta implementação, esta função não é necessária, e uma chamada
a "t_sync" não tem efeito.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de erro, a função devolve -1 e indica em "t_errno" a causa do erro.
Em caso de sucesso devolve um dos estados correntes da tabela abaixo:

.sp
.(t
.(l
	+---------------+-------------------------------+
	|    Estado	|	   Descrição		|
	+---------------+-------------------------------+
	| T_UNINIT	| Não inicializado - estado	|
	|		| inicial e final da interface	|
	+---------------+-------------------------------+
	| T_UNBND	| Inicializado, mas sem endereço|
	+---------------+-------------------------------+
	| T_IDLE	| Inicializado e com endereço,	|
	|		| mas sem conexão		|
	+---------------+-------------------------------+
	| T_OUTCON	| Aguardando resposta de um	|
	|		| pedido de conexão (cliente)	|
	+---------------+-------------------------------+
	| T_INCON	| Aguardando um	pedido de	|
	|		| conexão (servidor)		|
	+---------------+-------------------------------+
	| T_DATAXFER	| Transferindo dados		|
	+---------------+-------------------------------+
	| T_OUTREL	| Aguardando resposta de um	|
	|		| pedido de desconexão ordenada	|
	+---------------+-------------------------------+
	| T_INREL	| Aguardando um	pedido de	|
	|		| desconexão ordenada		|
	+---------------+-------------------------------+
.)l
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
dup, exec, fork, open
.br
.wo "(xti): "
t_intro
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
