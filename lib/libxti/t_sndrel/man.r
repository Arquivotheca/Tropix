.bp
.he 'T_SNDREL (xti)'TROPIX: Manual de Referência'T_SNDREL (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_sndrel -"
inicia uma desconexão ordenada
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_sndrel (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_sndrel" é utilizada para iniciar uma desconexão ordenada e
indica ao usuário remoto que não há mais dados a serem enviados.

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
Depois de chamar "t_sndrel", o usuário não pode mais
enviar dados através da conexão;
no entanto, ele pode continuar a receber dados normalmente,
se uma indicação de desconexão ordenada ainda não foi recebida.

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
.b OBSERVAÇÃO
.in 5
Normalmente, o usuário remoto recebe a indicação
de que não há mais dados a serem recebidos através
de um valor NULO (== EOF) retornado pela função "t_rcv".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_getinfo, t_open, t_rcv, t_rcvrel
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
