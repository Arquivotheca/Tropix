.bp
.he 'ADDCH (curses)'TROPIX: Manual de Referência'ADDCH (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Escreve um caractere:
.sp
.wo "waddch   -"
na posição corrente na janela dada
.br
.wo "addch    -"
na posição corrente na janela padrão
.br
.wo "mvwaddch -"
dadas a posição e a janela
.br
.wo "mvaddch  -"
dada a posição na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int waddch (WINDOW *janela, chtype ch);
int addch (chtype ch);
int mvwaddch (WINDOW *janela, int linha, int coluna, chtype ch);
int mvaddch (int linha, int coluna, chtype ch);
.)l

.sp
.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "waddch" escreve o caractere "ch" na posição corrente em "janela".
.sp
A macro "addch" escreve o caractere "ch" na janela padrão ("stdwin").
.sp
As macros "mvwaddch" e "mvaddch" equivalem respectivamente
à rotina "waddch" e à macro "addch" precedidas da rotina "wmove",
estabelecendo a posição onde o caractere será escrito.

.sp
O tipo "chtype" engloba o caractere e seus atributos.
A rotina "waddch" escreve o caractere "ch" com seus atributos
e com os atributos correntemente habilitados para "janela"
(veja "wattrset" (curses)).

.sp
Normalmente os caracteres de controle têm significado especial
para a rotina "waddch".
O caractere <nl> equivale a uma seqüência de espaços
suficiente para preencher o restante da linha corrente.
O caractere <ht> é expandido em espaços
até que seja atingida a próxima coluna de tabulação na janela.
Os caracteres <bs> e <cr> retrocedem a posição corrente
respectivamente para a coluna anterior
e para a primeira coluna na linha corrente.
Os demais caracteres de controle são escritos na forma "^X".
É possível evitar a interpretação especial dos caracteres de controle
escrevendo-os diretamente na janela (veja "waddset" (curses)).

.sp
A rotina "waddch"
avança a posição corrente de acordo
com o espaço ocupado pelo caractere escrito.
O avanço da posição corrente é análogo ao avanço do cursor
na maioria dos terminais.
Dentro de uma linha a posição corrente avança
incrementando-se a coluna corrente,
ao final de uma linha passando-se para o início da linha seguinte
e ao final da janela rolando-a.
Este comportamento pode ser alterado
(veja "waddset" (curses)).

.sp
Um conjunto de caracteres semigráficos está disponível
para escrita nas janelas.
Nos terminais que não suportam este caracteres,
eles serão atualizados com caracteres comuns,
tais como '-', '+', '|', etc...
Cada caractere semigráfico pode ser referenciado
por uma variável externa associada,
cujos nomes são prefixadas pela seqüência "ACS_".
A relação completa dos caracteres semigráficos
se encontra no arquivo para inclusão "<curses.h>".

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
As seguintes situações causam erro:
chamada das rotinas e macros com parâmetros inválidos
(por ex: janela nula ou posição fora da janela)
e a impossibilidade de escrever o caractere
devido à posição corrente ter ultrapassado o final da linha corrente
ou o final da janela (veja "waddset" (curses)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
waddset, waddstr, wattrset, winsch, wmove, wprintw,
.wo "          "
wscroll
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
