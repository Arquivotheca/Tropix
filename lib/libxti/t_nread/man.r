.bp
.he 'T_NREAD (xti)'TROPIX: Manual de Referência'T_NREAD (xti)'
.fo 'Escrito em 09.07.98'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_nread -"
informa o número de bytes disponíveis para recepção
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_nread (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_nread" fornece o número de bytes disponíveis na fila de
entrada do "endpoint" de transporte "fd", prontos para a recepção
(através de "t_rcv", "t_rcvudata" ou "t_rcvraw").

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
A função "t_nread" pode ser usada tanto no modo-com-conexão,
no modo-sem-conexão como no modo-de-acesso-interno;
ela é normalmente usada no modo síncrono para
evitar que o processo fique bloqueado em uma recepção.

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
Em caso de sucesso a função devolve o número de bytes disponíveis
(ou ZERO, caso a fila de entrada esteja vazia).
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.sp
No modo-com-conexão, se a fila de entrada estiver vazia,
mas tiver sido recebida uma indicação de "final de arquivo",
a função retorna -1 com [TLOOK] em "t_errno" e o evento "T_ORDREL"
(veja "t_look" (xti)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_rcv, t_rcvudata, t_rcvraw, t_look
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
