.bp
.he 'ADDSTR (curses)'TROPIX: Manual de Referência'ADDSTR (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Entrada de cadeias do teclado:
.sp
.wo "waddstr   -"
na posição corrente na janela dada
.br
.wo "addstr    -"
na posição corrente na janela padrão
.br
.wo "mvwaddstr -"
dadas a janela e a posição
.br
.wo "mvaddstr  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int waddstr (WINDOW *janela, char *str);
int addstr (char *str);
int mvwaddstr (WINDOW *janela, int linha, int coluna, char *str);
int mvaddstr (int linha, int coluna, char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "waddstr" escreve uma cadeia de caracteres em "janela"
a partir da posição corrente.
.sp
A macro "addstr" escreve a cadeia para a janela padrão ("stdwin").
.sp
As macros "mvwaddstr" e "mvaddstr" equivalem respectivamente
à rotina "waddstr" e à macro "addstr" precedidas da rotina "wmove",
que estabelece onde a entrada será realizada.

.sp
O funcionamento da rotina "waddstr" e suas macros associadas
normalmente envolve a execução da rotina "waddch"
para cada caractere na cadeia apontada pelo parâmetro "str".
Na ocorrência de algum erro este procedimento é interrompido.

.sp
.in
.b
VALOR DE RETORNO
.r
.in 5
A rotina "waddstr" e as suas macros associadas
repassam o último valor retornado pela rotina "waddch".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
waddch, waddset, wmove, wprintw
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
