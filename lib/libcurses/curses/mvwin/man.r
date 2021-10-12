.bp
.he 'MVWIN (curses)'TROPIX: Manual de Referência'MVWIN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
.wo "mvwin -"
movimento da janela
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void mvwin (WINDOW *janela, int linha, int coluna);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "mvwin" altera a posição de "janela" no vídeo do terminal.

.in
.sp
.b OBSERVAÇÃO
.in 5
A rotina "mvwin" automaticamente atualiza todas as janelas
no terminal.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
wmove, touchwin
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
