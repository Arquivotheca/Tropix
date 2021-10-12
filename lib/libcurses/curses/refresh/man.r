.bp
.he 'REFRESH (curses)'TROPIX: Manual de Referência'REFRESH (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Atualiza uma janela no vídeo:
.sp
.wo "wrefresh -"
uma janela dada
.br
.wo "refresh  -"
a janela padrão ("stdwin")
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wrefresh (WINDOW *janela);
void refresh (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wrefresh" atualiza "janela" no vídeo do terminal.
.sp
A macro "refresh" atualiza a janela padrão ("stdwin").
.sp
O modo de atualização de cada janela é configurável
(veja "wrefset" (curses)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
touchwin, wrefset
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
