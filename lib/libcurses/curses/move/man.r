.bp
.he 'MOVE (curses)'TROPIX: Manual de Referência'MOVE (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Posição corrente em uma janela:
.sp
.wo "wmove  -"
estabelece posição na janela dada
.br
.wo "move   -"
estabelece posição na janela padrão
.br
.wo "wgetyx -"
informa posição corrente dada a janela

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int wmove (WINDOW *janela, int linha, int coluna);
int move (int linha, int coluna);
void wgetyx (WINDOW *janela, int linha, int coluna);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wmove" estabelece a posição corrente em "janela"
em "linha" e "coluna".
A macro "move" equivale à rotina "wmove" aplicada à janela padrão ("stdwin").

.sp
A rotina "wgetyx" atribui às variáveis apontadas por "linha" e "coluna"
a posição corrente em "janela".

.sp
A contagem das linhas e das colunas começa em zero
e é limitada pelo tamanho da janela.
As primeiras e últimas linhas e colunas não são acessíveis
quando a janela tem contorno (veja "wbox (curses)").

.sp
A criação de uma janela estabelece como posição corrente
as primeiras linha e coluna.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "wmove" e a macro "move" retornam "OK" ou "ERR".

.in
.sp
.b DIAGNÓSTICOS
.in 5
A rotina "wmove" retorna "ERR" quando a posição fornecida
está fora dos limites da janela.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wbox
.br


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
