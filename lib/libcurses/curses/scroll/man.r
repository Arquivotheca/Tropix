.bp
.he 'SCROLL (curses)'TROPIX: Manual de Referência'SCROLL (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Rolamento de uma janela:
.sp
.wo "wscroll -"
uma janela dada
.br
.wo "scroll  -"
a janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wscroll (WINDOW *janela, int n);
void scroll (int n);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wscroll" realiza um rolamento de "n" linhas em "janela".
.sp
A rotina "scroll" equivale à rotina "wscroll" aplicada à janela padrão.

.sp
De acordo com o sinal do número "n",
o rolamento será para cima ("n" positivo) ou para baixo.
Em um rolamento para cima (baixo)
a primeira (última) linha na janela é perdida,
as linhas restantes são deslocadas para cima (baixo)
e surge uma nova linha totalmente em branco
na extremidade inferior (superior) da janela.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
addch, addset
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
