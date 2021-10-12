.bp 1
.he 'STRTOD (libc)'TROPIX: Manual de Referência'STRTOD (libc)'
.fo 'Escrito em 10.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Conversão de cadeias em números de ponto flutuante:
.sp
.wo "strtod  -"
com indicador para o final do número
.br
.wo "atof    -"
sem indicador
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

double		strtod (const char *str, const char **ptr);
double		atof (const char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "strtod" tenta interpretar a cadeia de caracteres apontada
por "str" como um número de ponto flutuante, e retorna o seu valor
convertido para "double".

.bc	/*************************************/
A função "strtod" retorna como um "double" o valor flutuante
representado pela cadeia de caracteres apontada por "str".
.ec	/*************************************/

A cadeia é processada até o primeiro caracter inconsistente
com a sintaxe de números flutuantes (veja abaixo). Caracteres brancos e
tabulações (<ht>) antes do início do número são ignorados.

.sp
Se "ptr" for um ponteiro não NULO, um ponteiro para o primeiro caracter
não reconhecido é armazenado na posição apontada por "ptr". 
Se nenhum número de ponto flutuante foi reconhecido,  a
função  devolve  "0.0",  e a posição apontada por "ptr" contém
"str".

.sp
A função "atof" é equivalente à "strtod", com a exceção de que
não  devolve  um  ponteiro  para  o  primeiro   caracter   não
reconhecido.

.sp
A sintaxe de números flutuantes aceita consiste de um sinal
("+" ou "-", opcional), uma mantissa e uma parte de expoente (opcional).
A mantissa pode ter a forma "ddd", "ddd.ddd", "ddd." ou ".ddd",
onde "d" são dígitos decimais.
A parte de expoente consiste de um caractere "e" ou "E" seguido de
um sinal ("+" ou "-", opcional) e de um expoente decimal.

.in
.sp
.b OBSERVAÇÕES
.in 5
Em  caso  de  "overflow",  as  funções  devolvem  +HUGE_VAL ou
-HUGE_VAL.
.sp
Em caso de "underflow", as funções devolvem "0.0".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
scanf, strtol
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
