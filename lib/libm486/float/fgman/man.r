.bp
.he 'FGMAN (libc)'TROPIX: Manual de Referência'FGMAN (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulação de partes de um número de ponto flutuante:
.sp
.wo "fgman    -"
obtém o valor da mantissa
.br
.wo "fgexp    -"
obtém o valor do expoente
.br
.wo "faexp    -"
soma um valor ao expoente
.br
.wo "fgint    -"
obtém a parte inteira
.br
.wo "fgfra    -"
obtém a parte fracionária
.br
.wo "fhuge    -"
retorna o maior número possível antes de infinito
.br
.wo "HUGE_VAL -"
o maior número possível antes de infinito
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

double	fgman (double x);
int	fgexp (double x);
double	faexp (int e, double x);
double	fgint (double x);
double	fgfra (double x);
double	fhuge (void);

extern double	HUGE_VAL;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Todos os números de ponto flutuante não nulos podem ser escritos
sem ambiguidade como
.wo "(m * (2 ** n))" 
onde "m" é a mantissa (significando), cujo valor absoluto é maior ou igual
a 1.0 e menor que 2.0, e o expoente "n" é um número inteiro. A escolha
destes valores segue a representação de números em ponto flutuante
padronizada pelo IEEE (Institute of Electrical and Electronics
Engineers).

.sp
A função "fgman" retorna a mantissa de "x".

.sp
A função "fgexp" retorna o expoente de "x".

.sp
A função "faexp" retorna um valor que é o do argumento "x"
a cujo expoente é somado o valor de "e".
Isto equivale a uma multiplicação ou divisão de "x" por uma potência de 2.

.sp
A função "fgint" retorna a parte inteira de "x".

.sp
A função "fgfra" retorna a parte fracionária de "x".

.sp
A função "fhuge" retorna HUGE_VAL (veja abaixo),
o maior número possível antes de infinito.

.sp
A variável externa HUGE_VAL contém o maior número possível
antes de infinito. Esta valor é aproximadamente "1.79769...E+308".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se os argumentos da função "faexp" causarem "overflow",
ela retorna HUGE_VAL ou -HUGE_VAL, dependendo do sinal de "x".
Se eles causarem "underflow", ela retorna ZERO.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fexcep, frexp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
