.bp
.he 'INSERTLN (curses)'TROPIX: Manual de Referência'INSERTLN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Inserção de uma linha:
.sp
.wo "winsertln   -"
na posição corrente da janela dada
.br
.wo "insertln    -"
na posição corrente da janela padrão
.br
.wo "mvwinsertln -"
dadas a janela e a posição
.br
.wo "mvinsertln  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void winsertln (WINDOW *janela, char *str);
void insertln (char *str);
void mvwinsertln (WINDOW *janela, int linha, char *str);
void mvinsertln (int linha, char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "winsertln" insere a linha "str" em "janela"
na posição corrente.
.sp
A macro "insertln" insere uma linha na janela padrão ("stdwin").

.sp
As macros "mvwinsertln" e "mvinsertln" equivalem respectivamente
à rotina "winsertln" e à macro "insertln" precedidas da rotina "wmove",
que estabelece qual linha será removida.

.sp
A inserção de uma nova linha em uma janela
causa o deslocamento das linhas abaixo dela para baixo,
desaparecendo a linha antes existente na extremidade inferior da janela.
A nova linha é inserida completamente em branco.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
winsch, wdeleteln
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
