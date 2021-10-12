.bp
.he 'FDC (cmd)'TROPIX: Manual de Referência'FDC (cmd)'
.fo 'Atualizado em 25.03.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "fdc -"
Calculador de mesa com aritmética de múmeros flutuantes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
fdc
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "fdc" simula um calculador de mesa (muito simples) com
aritmética de múmeros flutuantes.

.sp
Os números devem ser dados
conforme a sintaxe da linguagem "C":
veja "strtod" (libc).

.sp
Estão disponíveis 26 variáveis para conterem valores intermediários,
cujos nomes são "a" a "z". Estas variáveis são inicializadas com o
valor 0.0, mas no entanto, "fdc" indica quando está sendo usado o valor
de uma variável ainda não atribuída pelo usuário.

.sp
Os operadores disponíveis são: "=", "+", "-", "*" e "/".
As expressões são formadas usando os operadores
na forma infixa, tal como "a = b + 5.0".

.sp
Um <^D> encerra o uso de "fdc".

.bc	/*************************************/
.sp
As opções do comando são:

.in +3
.ip -v
Verboso: (nesta versão sem função).

.ep
.in -3
.ec	/*************************************/

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
dc
.br
.wo "(libc):"
strtod
.br

.in
.sp
.b EXEMPLO
.in 7
.(l
	a = 4.0e+12
	b = 23
	c = (a - 1.0e+12) * (b / 100)
.)l

.sp
irá imprimir o valor 6.9e+11 (e atribuí-lo à variável "c").

.in
.sp
.b ESTADO
.in 5
Efetivo.
