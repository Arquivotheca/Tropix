.bp
.he 'GETSTR (curses)'TROPIX: Manual de Referência'GETSTR (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Entrada de cadeias do teclado:
.sp
.wo "wgetstr   -"
na posição corrente na janela dada
.br
.wo "getstr    -"
na posição corrente na janela padrão
.br
.wo "mvwgetstr -"
dadas a janela e a posição
.br
.wo "mvgetstr  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int wgetstr (WINDOW *janela, char *str);
int getstr (char *str);
int mvwgetstr (WINDOW *janela, int linha, int coluna, char *str);
int mvgetstr (int linha, int coluna, char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wgetstr" faz a entrada de uma cadeia de caracteres
do teclado para "janela",
a partir da posição corrente, até que seja encontrado um <nl>.

.sp
A macro "getstr" faz a entrada de uma cadeia para a janela padrão ("stdwin").

.sp
As macros "mvwgetstr" e "mvgetstr" equivalem respectivamente
à rotina "wgetstr" e à macro "getstr" precedidas da rotina "wmove",
que estabelece onde a entrada será realizada.

.sp
O funcionamento da rotina "wgetstr" e suas macros associadas
envolve uma sequência de execuções da rotina "wgetch"
até que um caractere <nl> seja encontrado,
seja sinalizado um fim de arquivo
ou ocorra um erro.

.sp
.in
.b
VALOR DE RETORNO
.r
.in 5
A rotina "wgetstr" e as suas macros associadas
repassam o último valor retornado pela rotina "wgetch".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wgetch, wgetset, wmove
.br


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
