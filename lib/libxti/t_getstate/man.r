.bp
.he 'T_GETSTATE (xti)'TROPIX: Manual de Referência'T_GETSTATE (xti)'
.fo 'Escrito em 14.05.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_getstate -"
obtém o estado atual de um "endpoint"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_getstate (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_getstate" retorna o estado corrente do sistema de transporte
associado ao "endpoint" especificado por "fd".

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

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
.wo "(xti): "
t_open
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
