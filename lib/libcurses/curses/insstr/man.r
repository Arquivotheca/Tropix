.bp
.he 'INSSTR (curses)'TROPIX: Manual de Referência'INSSTR (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Inserção de uma cadeia:
.sp
.wo "winsstr   -"
na posição corrente da janela dada
.br
.wo "insstr    -"
na posição corrente da janela padrão
.br
.wo "mvwinsstr -"
dadas a janela e a posição
.br
.wo "mvinsstr  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int winsstr (WINDOW *janela, char *str);
int insstr (char *str);
int mvwinsstr (WINDOW *janela, int linha, int coluna, char *str);
int mvinsstr (int linha, int coluna, char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "winsstr" escreve uma cadeia de caracteres em "janela"
a partir da posição corrente.
.sp
A macro "insstr" escreve a cadeia para a janela padrão ("stdwin").

.sp
As macros "mvwinsstr" e "mvinsstr" equivalem respectivamente
à rotina "winsstr" e à macro "insstr" precedidas da rotina "wmove",
que estabelece onde a entrada será realizada.

.sp
O funcionamento da rotina "winsstr" e suas macros associadas
envolve a execução da rotina "winsch"
para cada caractere na cadeia apontada pelo parâmetro "str".
.sp
.in
.b
VALOR DE RETORNO
.r
.in 5
A rotina "winsstr" e as suas macros associadas
repassam o valor retornado pela rotina "winsch"
quando chamada para o último caractere em "str".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wattrset, winsch, wmove
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
