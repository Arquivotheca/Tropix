.bp
.he 'SCANW (curses)'TROPIX: Manual de Referência'SCANW (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Leitura formatada do teclado:
.sp
.wo "wscanw -"
para uma janela dada
.br
.wo "scanw  -"
para a janela padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

int wscanw (WINDOW *janela, const char *format, ...);
int scanw (const char *format, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wscanw" lê uma linha do teclado,
que é interpretada de acordo com "formato",
e guarda os valores convertidos nas áreas de memória
dadas pelos argumentos (ponteiros para variáveis).

.sp
A rotina "scanw" corresponde à rotina "wscanw" aplicada à janela padrão
("stdwin").

.sp
A especificação do formato para leitura e conversão
segue as mesmas normas adotadas para a rotina "scanf" da biblioteca
padronizada de entrada e saída ("stdio").

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As rotinas "wscanw" e "scanw" retornam o número de ítens 
para os quais a conversão foi bem sucedida.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):   "
scanf
.br
.wo "(curses): "
wprintw, wgetstr
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
