.bp 1
.he 'PATMATCH (libc)'TROPIX: Manual de Referência'PATMATCH (libc)'
.fo 'Atualizado em 10.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "patmatch -"
reconhecedor de padrões
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	patmatch (const char *str, const char *pattern);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "patmatch" verifica se a cadeia de caracteres apontada
por "str" satisfaz o padrão apontado por "pattern".

.sp
O padrão "pattern" pode conter caracteres regulares, que
devem coincidir com os caracteres correspondentes de "str", e os
meta-caracteres "?", "*", "[", "]", "!", que devem corresponder
a zero ou mais caracteres de "str", segundo as regras dadas abaixo:

.in 8
.ip c 10 
Corresponde ao caractere "c" (supondo que "c" não é nenhum dos
meta-caracteres). 

.ip ? 10
Corresponde a (um) caractere qualquer.

.ip * 10
Corresponde a zero ou mais caracteres quaisquer.

.ip !c 10
Corresponde a qualquer caractere diferente de "c".

.ip [....] 10
Corresponde a um caractere igual a um dos caracteres entre os colchetes.
Podemos também ter
construções do tipo "x-y"
entre os colchetes,
que representa todos os caracteres de "x" a "y" (inclusive). 

.ip ![....] 10
Corresponde a um caractere que deve ser diferente de todos os caracteres
entre os colchetes. Assim como acima, "x-y" representa todos os
caracteres de "x" a "y".

.ip \c 10
Corresponde ao caractere "c". Esta construção é útil para desfazer
o efeito especial dos meta-caracteres.
.ep

.in 0
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina devolve um valor positivo caso a cadeia tenha sido
aceita. Ela devolve zero se não foi aceita, ou se o padrão contém
um erro de sintaxe.

.in
.sp
.b OBSERVAÇÃO
.in 5
A implementação da rotina é eficiente, só utilizando
recursão para o meta-caractere "*".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br

.in
.sp
.(t
.b EXEMPLOS
.in 5
O padrão "*akefile" reconhece as cadeias "/src/cmd/Makefile" e
"/usr/lib/makefile", mas não a cadeia "makefile+"
.sp
O padrão "?akefile" reconhece as cadeias "Makefile" e
"makefile", mas não a cadeia "/usr/man/makefile"
.sp
O padrão "Make!gile" reconhece as cadeias "Makefile" e
"Makevile", mas não a cadeia "Makegile"
.sp
O padrão "Make[afx-z]ile" reconhece as cadeias "Makeaile", "Makefile",
"Makeyile", mas não a cadeia "Makegile"
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
