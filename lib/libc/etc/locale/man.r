.bp
.he 'LOCALE (libc)'TROPIX: Manual de Referência'LOCALE (libc)'
.fo 'Atualizado em 03.02.98'Versão 3.0.0'Pag. %'

.b NOME
.in 5
Localização do comportamento de algumas funções da "libc":
.sp
.wo "setlocale  -"
controla a localização
.br
.wo "localeconv -"
fornece informações para uma formatação numérica
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <locale.h>

const char *setlocale (int category, const char *locale);
const LCONV *localeconv (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções gerenciam a localização do comportamento de algumas
funções da "libc" em relação a conversões de campos numéricos, comparação
de cadeias de caracteres e conversões de tempos e datas.

.sp
A função "setlocale" seleciona o comportamento das funções apropriadas
do programa, de acôrdo com os argumentos "category" e "locale".
Esta função pode ser usada para consultar e/ou alterar 
a localização do comportamento de tôdas as funções
(que permitem localização) ou somente de algumas delas.

.sp
O argumento "category" pode assumir os seguintes valores:

.in +3
.ip "LC_ALL:" 13
Modifica simultaneamente o comportamente de tôdas as funções
(que permitem localização).
Uma chamada com este valor é equivalente a chamar a função
"setlocale", uma vez com cada um dos valores abaixo.

.ip "LC_COLLATE:" 13
Modifica o comportamente das funções "strcoll" e "strxfrm"
(ainda NÃO implementado).

.ip "LC_CTYPE:" 13
Modifica o comportamente das funções de manuseamento de caracteres
(ainda NÃO implementado).

.ip "LC_MONETARY:" 13
Modifica a informação de formatação monetária retornada pela função
"localeconv"
(ainda NÃO implementado).

.ip "LC_NUMERIC:" 13
Modifica o comportamente das conversões numéricas das funções de entrada/saída
formatada, assim como a informação de formatação não-monetária retornada pela função
"localeconv"
(ainda NÃO implementado).

.ip "LC_TIME:" 13
Modifica o comportamente da função "strftime".

.ep
.in -3

.sp
No momento, somente estão definidos dois valores para "locale", que são
"P" (para português) e "E" (para inglês).
Os valores "" e "C" são equivalentes a "P".
Também pode ser dada uma cadeia NULA para "locale"
(veja o VALOR de RETORNO, abaixo).

.sp
Todos os programas sempre começam como se tivesse sido realizada a chamada

.sp
.nf
		setlocale (LC_ALL, "P");
.fi

.sp
A função "localeconv" retorna um ponteiro para uma estrutura de tipo "lconv"
contendo diversos valores úteis para a formatação de quantidades numéricas
(monetárias ou não), de acôrdo com a localização corrente.
Consulte o arquivo de inclusão "locale.h" para ver os membros da estrutura.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se o argumento "locale" for um ponteiro não NULO, e
a cadeia apontada contiver um valor aceitável, a função "setlocale" altera a
localização e retorna um ponteiro para a nova cadeia associada
à categoria "category" especificada.
Se o valor NÃO for aceitável, "setlocale" retorna um ponteiro NULO,
e a localização não é alterada.

.sp
Se o argumento "locale" for um ponteiro NULO, a função retorna
um ponteiro para a cadeia associada à categoria "category" especificada
(sem efetuar nenhuma alteração).

.in
.sp
.b OBSERVAÇÃO
.in 5
Como já foi indicado acima, na presente versão do sistema, apenas
a função "strftime" (libc) está sendo afetada pelo "locale". 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
strftime
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/locale.h

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
