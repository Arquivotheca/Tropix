.bp
.he 'GETCH (curses)'TROPIX: Manual de Referência'GETCH (curses)'
.fo 'Atualizado em 28.09.89'Versão 2.3'Pag. %'

.b NOME
.in 5
Entrada de um caractere do teclado
.sp
.wo "wgetch   -"
na posição corrente na janela dada
.br
.wo "getch    -"
na posição corrente na janela padrão
.br
.wo "mvwgetch -"
dadas a janela e a posiçao
.br
.wo "mvgetch  -"
dada a posição na janela padrão

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int wgetch (WINDOW *janela);
int getch (void);
int mvwgetch (WINDOW *janela, int linha, int coluna);
int mvgetch (int linha, int coluna);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wgetch" faz a entrada de um caractere do teclado para "janela",
na posição corrente.

.sp
A macro "getch" faz a entrada de um caractere para a janela padrão ("stdwin").

.sp
As macros "mvwgetch" e "mvgetch" equivalem respectivamente
à rotina "wgetch" e à macro "getch" precedidas da rotina "wmove",
que estabelece para onde a entrada será realizada.

.sp
Normalmente a entrada de texto do teclado é realizada no seguinte modo:
.in 8
.ip (a)
com eco, utilizando a rotina "waddch" na janela indicada,
.ip (b)
editada por linha, 
.ip (c)
com geração de caracteres acentuados se o teclado não for capaz de gerá-los,
.ip (d)
convertendo o caractere <cr> no caractere <nl> e
.ip (e)
sem reconhecimento de teclas especiais.
.lp
.in 5
Como a entrada de texto é editada por linha,
somente após o término de uma linha os seus caracteres
se tornam disponíveis ao programa.
O modo de entrada de texto de cada janela pode ser reconfigurado
(veja "wgetset" (curses)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "wgetch" e as suas macros associadas
normalmente retornam um caractere lido do teclado;
em alguns casos especiais elas podem retornar
constantes predefinidas no arquivo para inclusão "<curses.h>".
.sp
Quando a edição da entrada está habilitada
a linha pode ser terminada pelos caracteres
<nl>, "eol" ou "eof" (veja "termio" (fmt));
neste caso chamadas sucessivas à rotina "wgetch"
retornam cada um dos caracteres da linha
e a seguir, de acordo com o caractere de terminação,
respectivamente <nl> ou as constantes "EOL" e "EOT".
.sp
De acordo com a configuração do modo de entrada,
a constante "ERR" pode ser retornada
imediatamente após a recepção de um sinal
ou para indicar que no momento da chamada à rotina "wgetch"
não havia caracteres disponíveis
(veja "wgetset" (curses)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):   "
stty
.br
.wo "(libc):  "
strerror, perror
.br
.wo "(curses):"
waddch, wgetset, wgetstr, wmove, wscanw
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
