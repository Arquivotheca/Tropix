.bp
.he 'ERASE (curses)'TROPIX: Manual de Referência'ERASE (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Preenche totalmente uma janela com espaços:
.sp
.wo "werase -"
uma janela dada
.br
.wo "erase  -"
a janela padrão ("stdwin")
.br
.wo "wclear -"
uma janela dada com a opção "R_CLEAR"
.br
.wo "clear  -"
a janela padrão com a opção "R_CLEAR"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void werase (WINDOW *janela);
void erase (void);
void wclear (WINDOW *janela);
void clear (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "werase" preenche "janela" com espaços.

.sp
A macro "erase" equivale à rotina "werase" aplicada à janela padrão ("stdwin").

.sp
As macros "wclear" e "clear" são respectivamente equivalentes
à rotina "werase" e à macro "erase" com a opção de atualização "R_CLEAR"
(veja "wrefset" (curses)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wclrtoeol, wrefset
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
