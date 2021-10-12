.bp
.he 'TOUCHWIN (curses)'TROPIX: Manual de Referência'TOUCHWIN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Altera a visibilidade de uma janela:
.sp
.wo "touchwin   -"
para máxima prioridade
.br
.wo "untouchwin -"
para mínima prioridade

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void touchwin (WINDOW *janela);
void untouchwin (WINDOW *janela);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As rotinas "touchwin" e "untouchwin" põem "janela"
no início ou no fim, respectivamente,  da fila de visibilidade das janelas em um terminal.

.sp
A ordem de uma janela na fila de visibilidade determina
em cada posição do vídeo
correspondente a mais de uma janela
qual destas deve ser atualizada.
Assim a imagem final no vídeo sugere a superposição
das janelas mais prioritárias sobre as de menor prioridade.

.sp
É necessário executar a rotina "touchwin"
pelo menos uma vez para cada janela criada,
de modo que ela possa ser efetivamente atualizada no vídeo.
O início das atividades de "curses" (veja "incurses" (curses))
já coloca a janela padrão ("stdwin") no início da fila de visibilidade.

.in
.sp
.b OBSERVAÇÃO
.in 5
As rotinas "touchwin" e "untouchwin" automaticamente
atualizam o vídeo do terminal.
A rotina "touchwin" atualiza a janela referente ao seu argumento
e a rotina "untouchwin" atualiza todas as janelas visíveis no terminal.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
incurses, mvwin
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
