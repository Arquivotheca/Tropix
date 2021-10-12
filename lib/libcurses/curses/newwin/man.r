.bp
.he 'NEWWIN (curses)'TROPIX: Manual de Referência'NEWWIN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
.wo "newwin -"
cria uma nova janela
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

WINDOW *newwin (int linhas, int colunas, int linhabase, int colunabase);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "newwin" cria uma nova janela com tamanho dado
pelos parâmetros "linhas" e "colunas"
e com o canto superior esquerdo localizado
no vídeo de acordo com os parâmetros "linhabase" e "colunabase".

.sp
A contagem para as linhas e colunas do vídeo se inicia em zero
e segue de cima para baixo, no caso das linhas,
e da esquerda para a direita, no caso das colunas.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "newwin" normalmente retorna um ponteiro para uma estrutura "WINDOW"
que referenciará a nova janela em todas as suas operações.
Em caso de erro, a rotina "newwin" retorna o ponteiro nulo "NOWINDOW".


.in
.sp
.b DIAGNÓSTICOS
.in 5
A criação de um nova janela não é possível quando já estão excedidos
os limites de memória determinados
pela implementação da biblioteca "curses".


.in
.sp
.b OBSERVAÇÃO
.in 5
A existência de uma janela não implica em sua presença no vídeo;
para torná-la visível é necessário utilizar a rotina "touchwin".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
delwin, touchwin, wbox
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
