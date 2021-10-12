.bp
.he 'INSCH (curses)'TROPIX: Manual de Referência'INSCH (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Insere um caractere:
.sp
.wo "winsch   -"
na posição corrente na janela dada
.br
.wo "insch    -"
na posição corrente na janela padrão
.br
.wo "mvwinsch -"
dadas a posição e a janela
.br
.wo "mvinsch  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int winsch (WINDOW *janela, chtype ch);
int insch (chtype ch);
int mvwinsch (WINDOW *janela, int linha, int coluna, chtype ch);
int mvinsch (int linha, int coluna, chtype ch);
.)l

.sp

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "winsch" insere o caractere "ch" na posição corrente em "janela".

.sp
A macro "insch" insere o caractere "ch" na janela padrão ("stdwin").

.sp
As macros "mvwinsch" e "mvinsch" equivalem respectivamente
à rotina "winsch" e à macro "insch" precedidas da rotina "wmove",
estabelecendo a posição onde o caractere será escrito.

.sp
A inserção de um caractere desloca o caractere na posição corrente
e os caracteres a sua direita na linha corrente
para a direita de acordo com o espaço ocupado pelo caractere inserido.
O caractere que ocupava a última coluna na linha corrente é perdido.
A posição corrente é avançada de modo
a que a próxima inserção seja realizada imediatamente à direita da atual.
A inserção de uma cadeia é equivalente à inserção de seus caracteres.

.sp
Alguns caracteres têm significado especial na inserção.
O caractere <nl> faz com que os caracteres a direita da posição corrente
sejam movidos para uma nova linha que é inserida
(veja "winsertln" (curses)) imediatamente abaixo
e a posição corrente passe para o início desta nova linha.
O caractere <ht> é expandido em espaços até que seja atingida
a próxima coluna de tabulação na janela.
Os caracteres de controle são inseridos na forma "^X".

.sp
O tipo "chtype" engloba o caractere e seus atributos.
A rotina "winsch" insere o caractere "ch" com seus atributos
e com os atributos correntemente habilitados para "janela"
(veja "wattrset" (curses)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As rotinas e macros retornam as constantes inteiras "OK" (em caso normal)
ou "ERR" (em caso de erro).

.in
.sp
.b DIAGNÓSTICOS
.in 5
A rotina "winsch" retorna "ERR" quando
inserindo na extremidade direita
caracteres que ocupam mais de uma coluna
da janela se as opções "A_WRAP" ou "A_SCROLL"
não estiverem habilitadas no modo de escrita da janela.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
wattrset, winsstr, winsertln, wmove, wprintw

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
