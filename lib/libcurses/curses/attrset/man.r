.bp
.he 'ATTRSET (curses)'TROPIX: Manual de Referência'ATTRSET (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
controla os atributos de escrita em uma janela:
.sp
.wo "wattrset -"
estabelece atributos
.br
.wo "wattron  -"
habilita alguns atributos
.br
.wo "wattroff -"
desabilita alguns atributos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wattrset (WINDOW *janela, int atributos);
void wattron (WINDOW *janela, int atributos);
void wattroff (WINDOW *janela, int atributos);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As macros "wattrset", "wattron" e "wattroff"
estabelecem os atributos com os quais os caracteres são escritos em uma janela.

.sp
A macro "wattrset" estabelece os atributos correntes para uma janela,
enquanto que as macros "wattron" e "wattroff"
habilitam e desabilitam, respectivamente,  alguns dos atributos.

.sp
Os atributos correspondem às seguintes constantes simbólicas:

.in +3
.ip "A_NORMAL:" 14
Sem atributos.
Esta constante não deve ser combinada e só é válida para a macro "wattrset".

.ip "A_STANDOUT:" 14
Caracteres bem destacados.

.ip "A_REVERSE:" 14
Vídeo reverso.

.ip "A_BOLD:" 14
Forte.

.ip "A_DIM:" 14
Suave.

.ip "A_BLINK:" 14
Piscante.

.ip "A_UNDERLINE:" 14
Sublinhado.

.ip "A_ALTCHARSET:" 14
Conjunto alternativo de caracteres semigráficos.

.in -3
.ep
.sp
Estes atributos podem ser combinados atráves do operador "|"
("ou" bit a bit).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
waddch, winsch, waddset, wgetset, wrefset
.br

.in
.sp
.b EXEMPLOS
.in 5
Após a chamada a seguir não serão acrescentados atributos aos caracteres
escritos em "janela".
.in 10
.sp
wattrset (janela, A_NORMAL);

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
