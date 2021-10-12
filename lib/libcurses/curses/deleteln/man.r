.bp
.he 'DELETELN (curses)'TROPIX: Manual de Referência'DELETELN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Remoção de uma linha
.sp
.wo "wdeleteln   -"
na posição corrente na janela dada
.br
.wo "deleteln    -"
na posição corrente na janela padrão
.br
.wo "mvwdeleteln -"
dadas a janela e a posição
.br
.wo "mvdeleteln  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wdeleteln (WINDOW *janela);
void deleteln (void);
void mvwdeleteln (WINDOW *janela, int linha);
void mvdeleteln (int linha);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wdeleteln" remove uma linha em "janela"
na posição corrente.
.sp
A macro "deleteln" remove uma linha na janela padrão ("stdwin").
.sp
As macros "mvwdeleteln" e "mvdeleteln" equivalem respectivamente
à rotina "wdeleteln" e à macro "deleteln" precedidas da rotina "wmove",
que estabelece qual linha será removida.

.sp
A remoção de uma determinada linha de uma janela
causa o deslocamento para cima de todas as linhas abaixo dela
e o surgimento de uma nova linha completamente em branco
na extremidade inferior da janela.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wdelch, winsertln
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
