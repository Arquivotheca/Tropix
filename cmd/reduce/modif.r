.bp
.he 'REDUCE (cmd)'TROPIX: Manual de Modificações'REDUCE (cmd)'
.fo 'Atualizado em 14.01.91'Versão 2.3.4'Pag. %'

.b NOME
.in 5
Comprime/expande um arquivo segundo o algoritmo LZW:
.sp
.wo "reduce -"
comprime ou expande, de acordo com as opções
.br
.wo "rcat   -"
apenas expande
.br

.in
.sp 2
.ip "15.08.90  2.3.0" 20
VERSÃO CORRENTE
.sp

.ip "28.09.90  2.3.1" 20
Foi resolvido o problema da emissão de "max_code" justamente quando
o tamanho do código mudava.

.ip "21.11.90  2.3.2" 20
Novamente, foi resolvido o problema da emissão de "max_code"
justamente quando o tamanho do código mudava, o que não foi o caso
na versão 2.3.1.

.ip "12.12.90  2.3.3" 20
Novamente, mais uma vêz foi resolvido o problema da emissão de "max_code"
justamente quando o tamanho do código mudava, o que também não foi o caso
na versão 2.3.2.
Alterados todos os módulos.

.sp
A área de códigos foi passada de "short" para "int".
Lembrei de alterar a compactação/descompactação de 16 bites em "fillcode" e
"flushcode".

.ip "14.01.91  2.3.4" 20
Foi acrescentada o teste "bit_sz < MAX_BIT_SZ" para certificar-se de que o
limite não é passado.
