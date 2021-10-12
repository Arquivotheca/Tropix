.bp
.he 'T_RCVREL (xti)'TROPIX: Manual de Referência'T_RCVREL (xti)'
.fo 'Escrito em 15.05.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_rcvrel -"
recebe uma indicação de desconexão ordenada
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcvrel (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcvrel" é usada para notificar a recepção de uma
desconexão ordenada.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "fd" indica o "endpoint" local onde existe a conexão.

.sp
Depois da recepção de uma desconexão ordenada, o usuário não deve tentar
receber mais dados pois poderá ficar bloqueado para sempre.
No entanto, o usuário pode continuar a enviar dados através
da conexão se a função "t_sndrel" ainda não foi chamada.

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
t_getinfo, t_open, t_sndrel
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
