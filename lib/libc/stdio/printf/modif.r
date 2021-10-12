.bp
.he 'PRINTF (libc)'TROPIX: Manual de Modificações'PRINTF (libc)'
.fo 'Atualizado em 01.11.02'Versão 4.4.0'Pag. %'

.b NOME
.in 5
Formata a saída:
.sp
.wo "printf  -"
escreve na saída padrão
.br
.wo "fprintf -"
escreve no arquivo dado
.br
.wo "sprintf -"
coloca em um vetor dado
.br

.in
.sp 2
.ip "01.12.87  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "03.04.89  2.1.0" 20
Foi extendida a rotina "doprnt" para que processe também o formato 'P'
(ponteiros). 

.ip "30.11.91  2.3.1" 20
Foi corrigido o problema do formato "g" que imprimia em formato "e"
ao invés de "f" quando havia zeros no número mesmo que a precisão fosse
grande (com precisao = 6 o número 100 era impresso incorretamente 1e+002).

.ip "22.07.95  3.0.0" 20
A função deliciou-se com uma revisão total.

.ip "01.11.02  4.4.0" 20
Acrescentadas as conversões "quad".
