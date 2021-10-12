.bp
.he 'FREXP (libc)'TROPIX: Manual de Referência'FREXP (libc)'
.fo 'Escrito em 25.02.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulação de partes de um número de ponto flutuante:
.sp
.wo "frexp -"
obtém mantissa e expoente
.br
.wo "ldexp -"
soma valor ao expoente
.br
.wo "modf  -"
obtém parte inteira e fracionária
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	frexp (double x, int *pe);
double	ldexp (double x, int e);
double	modf (double x, double *pint);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Todos os números de ponto flutuante não nulos podem ser escritos
sem ambiguidade como
.wo "(m * (2 ** n))" 
onde "m" é a fração (mantissa), cujo valor absoluto é maior ou igual
a 1.0 e menor que 2.0, e "n" é o expoente inteiro. A escolha destes
valores segue a representação dos números em ponto flutuante
padronizada pelo IEEE (Institute of Electrical and Electronics
Engineers).
foram escolhidos

.sp
A função "frexp" retorna a mantissa de "x" e armazena
o expoente inteiro de "x" no local apontado pelo ponteiro "pe".

.sp
A função "ldexp" retorna um valor que é o de "x"
com seu expoente somado ao valor de "e". Equivale a uma
multiplicação ou divisão de "x" por uma potência de 2.

.sp
A função "modf" retorna a parte fracionária de "x"
(com o mesmo sinal de "x") e armazena a parte inteira de "x"
(como um "double") no local apontado pelo ponteiro "pint".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se o resultado da função "ldexp" causar "overflow",
ela retorna HUGE_VAL ou -HUGE_VAL conforme o sinal de "x".
Se ele causar "underflow", retorna ZERO.

.in
.sp
.b OBSERVAÇÕES
.in 5
Estas funções foram mantidas no TROPIX simplesmente por
compatibilidade com outros sistemas operacionais. No TROPIX somente são
usadas as funções contidas no título "fgman".
.sp
Verifique que mantissa e expoente seguem a representação de números em
ponto flutuante padronizada pelo IEEE. As funções "frexp" e "modf" de
antigos sistemas operacionais são anteriores a esta padronização e
representam a mantissa entre 0.5 e 1.0. Na improvável necessidade de
se transformar os novos valores nos antigos, basta dividir a mantissa
por 2 e somar 1 ao expoente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, fgman
.br

.sp
.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
