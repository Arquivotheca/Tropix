.bp
.he 'ED (cmd)'TROPIX: Manual de Modificações'ED (cmd)'
.fo 'Atualizado em 26.04.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
ed - editor de textos orientado por linhas
.br

.in
.sp 2
.ip "01.04.87  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "10.08.87  1.0.1" 20
Devido a um erro no compilador (na fase de otimização de código),
a remoção completa do arquivo (por exemplo através do comando "1,$d")
causava um "BUS ERROR".

.sp
O erro do compilador ocorre na inicialização de variáveis longas (4 bytes)
com o valor zero, quando realizada após a inicialização de uma variável menor
(de 1 ou 2 bytes) também com valor nulo.
Esta situação ocorria no módulo "src/list.c" na função "txt_remove";
o problema foi solucionado alterando-se o tipo da variável "new_Root" de "char"
para "int".

.in
.sp 2
.ip "01.01.88  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "26.04.97  3.0.0" 20
O programa sofreu (ou deliciou-se com) uma revisão global.
