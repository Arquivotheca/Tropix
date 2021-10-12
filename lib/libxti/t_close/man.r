.bp
.he 'T_CLOSE (xti)'TROPIX: Manual de Referência'T_CLOSE (xti)'
.fo 'Escrito em 08.01.93'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_close -"
fecha um "endpoint" de transporte
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_close (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_close" informa o sistema de transporte que o usuário
terminou a utilização do "endpoint" de transporte especificado por "fd".

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
A função "t_close" deve ser chamada no estado T_UNBND (veja "t_getstate").
No entanto, isto não é verificado, de tal modo que ela pode ser chamada
em qualquer estado.

.sp
Em todos os casos, são liberados todos os recursos associados ao "endpoint".

.sp
No modo-com-conexão, t_close" será abortiva para o usuário remoto, no caso
em que seja verificado (pelo estado do "endpoint") que o usuário
remoto não está esperando o término da conexão.

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
t_getstate, t_open, t_unbind
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
