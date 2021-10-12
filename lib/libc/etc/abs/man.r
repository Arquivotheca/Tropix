.bp
.he 'ABS (libc)'TROPIX: Manual de Referência'ABS (libc)'
.fo 'Atualizado em 08.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Valor absoluto:
.sp
.wo "abs  -"
de um inteiro
.br
.wo "labs -"
de um inteiro longo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	abs (int j);
long	labs (long j);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "abs" calcula o valor absoluto de um inteiro "j".

.sp
A função "labs" é análoga à "abs", exceto que o argumento e o valor de
retorno tem o tipo "long".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções devolvem o valor absoluto do argumento.

.in
.sp
.b OBSERVAÇÃO
.in 5
Ambas funções obtém o valor correto exceto para o número mais negativo
(-2147483648) cujo valor absoluto não tem representação.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fabs
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
