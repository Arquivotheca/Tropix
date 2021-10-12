.bp
.he 'MALLOC (libc)'TROPIX: Manual de Modificações'MALLOC (libc)'
.fo 'Atualizado em 15.09.92'Versão 3.0.0'Pag. %'

.b NOME
.in 5
Alocação dinâmica de memória:
.sp
.wo "malloc  -"
alocação de memória
.br
.wo "free    -"
liberação de memória
.br
.wo "realloc -"
realocação de memória
.br
.wo "calloc  -"
alocação de área zerada
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "17.07.87  1.0.1" 20
O seguinte refinamento foi acrescentado à estratégia para alocação de blocos
em "malloc.c":
se um bloco adquirido do Sistema é contíguo à Arena,
então ele é aglutinado aos últimos blocos livres da Arena.
.sp
Esta modificação resulta em uma menor fragmentação da Arena.

.in
.sp 2
.ip "17.07.87  2.1.0" 20
VERSÃO CORRENTE
.sp

.ip "15.08.89  2.2.1" 20
Eliminados erros relativos à iniciação da "malloc",
quando chamada pela primeira vez em um programa.

.ip "15.12.91  2.3.0" 20
As funções sofreram (ou deliciaram-se com) uma revisão global.

.ip "15.09.92  3.0.0" 20
Foi reimplantado o "smalloc".
