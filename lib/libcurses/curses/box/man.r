.bp
.he 'BOX (curses)'TROPIX: Manual de Referência'BOX (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Contorno e nome para uma janela
.sp
.wo "wbox    -"
desenha o contorno
.br
.wo "wclrbox -"
apaga o contorno

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wbox (WINDOW *janela, char *nome, chtype vert, chtype hor);
void wclrbox (WINDOW *janela);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "wbox" desenha um contorno em "janela"
escrevendo o caractere "vert" nas suas colunas
primeira e última
e o caractere "hor" nas suas linhas
primeira e última.
A cadeia de caracteres apontada pelo parâmetro "nome"
é escrita no canto superior esquerdo,
ou seja, a partir da quarta coluna da primeira linha.

.sp
A rotina "wclrbox" apaga o contorno escrevendo caracteres <sp>
em seu lugar.

.sp
Quando os caracteres "vert" e "hor" são nulos
a rotina "wbox" escolhe os caracteres para o contorno.
Esta forma é a mais aconselhável, uma vez que, se possível,
serão usados caracteres semigráficos,
que proporcionam uma sensação visual mais agradável.

.sp
Quando o parâmetro "nome" for nulo, a primeira linha é totalmente
preenchida com o contorno.

.in
.sp
.b OBSERVAÇÃO
.in 5
Nesta implementação da biblioteca "curses",
todas as rotinas que alteram o conteúdo de janelas
automaticamente reconhecem e respeitam o  contorno das janelas que os possuem.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
newwin, touchwin
.br

.in
.sp
.b EXEMPLOS
.in 5
Contorno em "janela" com o nome "Menu",
formado por caracteres escolhidos pela rotina "wbox":
.in 10
.sp
wbox (janela, "Menu", 0, 0);

.in 5
.sp 2
Contorno em "janela" contínuo (não tem nome)
formado pelos caracteres '!' na vertical e '-' na horizontal:
.in 10
.sp
wbox (janela, NULL, '!', '-');


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
