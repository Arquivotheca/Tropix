.bp
.he 'DELCH (curses)'TROPIX: Manual de Referência'DELCH (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Remove um caractere
.sp
.wo "wdelch   -"
na posição corrente na janela dada
.br
.wo "delch    -"
na posição corrente na janela padrão
.br
.wo "mvwdelch -"
dadas a posição e a janela
.br
.wo "mvdelch  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wdelch (WINDOW *janela);
void delch (void);
void mvwdelch (WINDOW *janela, int linha, int coluna);
void mvdelch (int linha, int coluna);
.)l


.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wdelch" remove o caractere na posição corrente em "janela".

.sp
A macro "delch" remove um caractere na janela padrão ("stdwin").

.sp
As macros "mvwdelch" e "mvdelch" equivalem respectivamente
à rotina "wdelch" e à macro "delch" precedidas da rotina "wmove",
estabelecendo qual caractere será removido.

.sp
Quando um caractere é removido todos os caracteres à direita
e na mesma linha são deslocados para a esquerda de uma coluna
e é escrito um espaço na última coluna.
A posição corrente permanece inalterada.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
wdeleteln, wmove
.ep

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
