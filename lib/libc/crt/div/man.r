.bp
.he 'DIV (libc)'TROPIX: Manual de Referência'DIV (libc)'
.fo 'Atualizado em 27.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Calcula o quociente e o resto:
.sp
.wo "div  -"
da divisão de inteiros
.br
.wo "ldiv -"
da divisão de longos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

div_t  div  (int numer,  int denom);
ldiv_t ldiv (long numer, long denom);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "div" calcula o quociente e o resto da divisão do numerador
"numer" pelo denominador "denom".
Se a divisão é inexata, o sinal do quociente resultante é o do quociente
algébrico, e o módulo do quociente resultante é o maior inteiro menor
do que o módulo do quociente algébrico.
Se o resultado não puder ser representado, o resultado é indefinido;
em caso contrário, "quot * denom + rem == numer". 

.sp
A função "ldiv" é análoga à "div", exceto que os argumentos e os valores de
retorno têm o tipo "long int".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções devolvem estruturas do tipo "div_t" e "ldiv_t" (respectivamente),
contendo o quociente e o resto nos membros "quot" e "rem".

.in
.sp
.b OBSERVAÇÕES
.in 5
Como normalmente as instruções das CPUs para a divisão fornecem
também o resto, a utilização da função "div" (ou "ldiv") é em geral mais
rápida do que o cálculo separado do quociente e do resto.

.sp
Como no TROPIX os inteiros são sempre longos (32 bites), as duas estruturas
e as duas funções são equivalentes entre si.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
