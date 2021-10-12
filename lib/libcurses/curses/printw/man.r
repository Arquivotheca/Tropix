.bp
.he 'PRINTW (curses)'TROPIX: Manual de Referência'PRINTW (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Converte e formata cadeias:
.sp
.wo "wprintw -"
em uma janela dada
.br
.wo "printw  -"
na janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int wprintw (WINDOW *janela, const char *format, ...);
int printw (const char *format, ...)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wprintw" converte, formata e escreve seus argumentos em "janela",
a partir da posição corrente e
sob o controle de "formato".
.sp
A rotina "printw" corresponde à rotina "wprintw" aplicada à janela padrão
("stdwin").

.sp
A especificação do formato para escrita
segue as mesmas normas adotadas para a rotina "printf" da biblioteca
padronizada de entrada e saída ("stdio").

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As rotinas "printw" e "wprintw" repassam o valor retornado
pela rotina "waddch".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):  "
printf
.br
.wo "(curses):"
waddstr, wscanw
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
