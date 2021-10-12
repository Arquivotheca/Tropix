.bp
.he 'DC (cmd)'TROPIX: Manual de Referência'DC (cmd)'
.fo 'Atualizado em 03.11.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "dc -"
Calculador de mesa com aritmética de múmeros inteiros
.br

.in
.sp
.b SINTAXE
.in 5
.(l
dc
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "dc" simula um calculador de mesa (muito simples) com
aritmética de múmeros inteiros.

.sp
Os números podem ser dados
nas bases 10 (decimal), 16 (hexadecimal) ou 8 (octal),
conforme a sintaxe da linguagem "C":
(0x.... para hexadecimal e 0... para octal; os demais são considerados decimais).
Para o caso hexadecimal, somente são aceitas
as letras maiúsculas 'A' a 'F' para representar os dígitos
10 a 15.
Um número pode ser seguido das letras 'K', 'M' ou 'G',
o que fará com que seu valor seja multiplicado por
1024 (KB), 1024^2 (MB) ou 1024^3 (GB), respectivamente.

.sp
Estão disponíveis 26 variáveis para conterem valores intermediários,
cujos nomes são "a" a "z". Estas variáveis são inicializadas com o
valor 0, mas no entanto, "dc" indica quando está sendo usado o valor
de uma variável ainda não atribuída pelo usuário.

.sp
Os operadores disponíveis são: "=", "+", "-", "*", "/", "\%", "&",
"|", "^", ">>", "<<", "~", "(" e ")", conforme a mesma notação da
linguagem "C".
As expressões são formadas usando os operadores
na forma infixa, tal como "a = b + 5".

.sp
Os resultados são dados em decimal e hexadecimal.

.sp
Um <^D> encerra o uso de "dc".

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
fdc
.br

.in
.sp
.b EXEMPLO
.in 7
.(l
	a = 0x34
	b = 234
	c = (a - 4) * (b ^ 1)
.)l

.sp
irá imprimir o valor 11280 (e atribuí-lo à variável "c").

.in
.sp
.b ESTADO
.in 5
Efetivo.
