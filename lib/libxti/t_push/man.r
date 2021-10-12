.bp
.he 'T_PUSH (xti)'TROPIX: Manual de Referência'T_PUSH (xti)'
.fo 'Escrito em 04.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_push -"
transmite um segmento incompleto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_push (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_push" transmite efetivamente um segmento que não tenha
sido completado no último "t_snd" relativo ao "endpoint" de
transporte "fd".

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
Por questões de eficiência, no modo-com-conexão, os segmentos só são
transmitidos quando completam o tamanho dado pelo membro "max_seg_size"
da estrutura "TCP_OPTIONS" (veja "t_optmgmt" (xti)).

.sp
Em certas situações, no entanto, é necessário transmitir o segmento
mesmo estando incompleto. Para isto, podemos utilizar o indicador
"T_PUSH" (veja "t_snd" (xti)) ou então chamar a função "t_push" após o
"t_snd".

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
Em caso de sucesso a função devolve 0.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_snd, t_optmgmt
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
