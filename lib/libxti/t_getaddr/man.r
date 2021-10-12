.bp
.he 'T_GETADDR (xti)'TROPIX: Manual de Referência'T_GETADDR (xti)'
.fo 'Escrito em 08.01.93'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_getaddr -"
obtém os endereços associados a um "endpoint"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_getaddr (int fd, INADDR *addr);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_getaddr" retorna os endereços associados ao
"endpoint" de transporte especificado pelo descritor de arquivo "fd".
A função permite o usuário a obter os endereços
durante qualquer fase da comunicação, tanto no modo-com-conexão,
no modo-sem-conexão assim como no modo-de-acesso-interno.

.sp
.(l
	    +-------------------+------------+------------+
	    |    Argumentos	| Na chamada | No retorno |
	    +-------------------+------------+------------+
	    | fd		|     x      |     /      |
	    |			|	     |		  |
	    | addr[0].a_addr	|     /      |     x      |
	    | addr[0].a_family	|     /      |     /      |
	    | addr[0].a_port	|     /      |     x      |
	    |			|	     |		  |
	    | addr[1].a_addr	|     /      |     x      |
	    | addr[1].a_family	|     /      |     /      |
	    | addr[1].a_port	|     /      |     x      |
	    |			|	     |		  |
	    | addr[2].a_addr	|     /      |     x      |
	    | addr[2].a_family	|     /      |     /      |
	    | addr[2].a_port	|     /      |     x      |
	    |			|	     |		  |
	    | addr[3].a_addr	|     /      |     x      |
	    | addr[3].a_family	|     /      |     /      |
	    | addr[3].a_port	|     /      |     x      |
	    |			|	     |		  |
	    +-------------------+------------+------------+
.)l

.in
.sp
.b MODO-COM-CONEXÃO
.in 5
No modo-com-conexão, o ponteiro "addr" deve apontar para um vetor
de duas estruturas "INADDR", cujo primeiro elemento é preenchido com
o endereço local e o segundo com o endereço remoto da conexão.
Os endereços são significativos a partir do estado 
T_DATAXFER (isto é, após um "t_accept" ou "t_connect").

.in
.sp
.b MODO-SEM-CONEXÃO
.in 5
No modo-sem-conexão, o ponteiro "addr" deve apontar para um vetor
de quatro estruturas "INADDR", cujos elementos são preenchidos respectivamente
com o endereço local, endereço remoto (obtido no "t_bind"),
endereço remoto (obtido na última emissão de um datagrama),
e o endereço remoto (obtido na última recepção de um datagrama).

.sp
O quarto endereço (endereço remoto obtido na última recepção
de um datagrama) é especialmente útil quando se utiliza a interface
"read" ao invés de "t_rcvudata".

.in
.sp
.b MODO-DE-ACESSO-INTERNO
.in 5
No modo-de-acesso-interno, o ponteiro "addr" deve apontar para um vetor
de quatro estruturas "INADDR", cujos elementos são preenchidos respectivamente
com o endereço local, endereço remoto (obtido no "t_bind"),
endereço remoto (obtido na última emissão de um datagrama),
e o endereço remoto (obtido na última recepção de um datagrama).

.sp
Neste modo, o membro "a_port" representa na realidade o número do protocolo
("a_proto", veja em <xti.h>).

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta função não faz parte da XTI, e portanto seu uso deve ser evitado
se for desejada portabilidade para outras implementações.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função preenche a estrutura e devolve o valor 0.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Nesta versão os campos "a_family" não são utilizados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_intro, t_addr, t_rcvraw, t_rcvudata
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
