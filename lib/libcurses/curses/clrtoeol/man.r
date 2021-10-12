.bp
.he 'CLRTOEOL (curses)'TROPIX: Manual de Referência'CLRTOEOL (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Escreve espaços
.sp
.wo "wclrtoeol -"
até o fim da linha dada a janela
.br
.wo "wclrtobot -"
até o fim da janela dada a janela
.br
.wo "clrtoeol  -"
até o fim da linha na janela padrão
.br
.wo "clrtobot  -"
até o fim da janela na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wclrtoeol (WINDOW *janela);
void wclrtobot (WINDOW *janela);
void clrtoeol (void);
void clrtobot (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wclrtoeol" escreve espaços
da posição corrente até o fim da linha corrente em "janela".

.sp
A rotina "wclrtobot" escreve espaços
da posição corrente até o fim (canto inferior direito) de "janela".

.sp
As macros "clrtoeol" e "clrtobot" são equivalentes
às rotinas "wclrtoeol" e "wclrtobot" aplicadas à janela padrão ("stdwin").


.in
.sp
.b OBSERVAÇÃO
.in 5
A posição corrente permanece inalterada após a chamada às rotinas
"wclrtoeol" ou "wclrtobot".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
wmove
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
