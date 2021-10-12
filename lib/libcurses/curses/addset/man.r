.bp
.he 'ADDSET (curses)'TROPIX: Manual de Referência'ADDSET (curses)'
.fo 'Atualizado em 29.01.89'Versão 2.3'Pag. %'

.b NOME
.in 5
Controla o modo de escrita em uma janela:
.sp
.wo "waddset -"
estabelece o modo
.br
.wo "waddon  -"
habilita algumas opções do modo
.br
.wo "waddoff -"
desabilita algumas opções do modo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void waddset (WINDOW *janela, int modo);
void waddon (WINDOW *janela, int opções);
void waddoff (WINDOW *janela, int opções);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As macros "waddset", "waddon" e "waddoff"
controlam o modo de escrita de cada janela.

.sp
A macro "waddset" estabelece o modo de escrita,
enquanto que as macros "waddon" e "waddoff"
habilitam e desabilitam, respectivamente, algumas de suas opções.

.sp
O modo de escrita
é estabelecido habilitando ou desabilitando suas opções
de acordo com os seguintes indicadores:

.in 10
.ip "A_WRAP:" 11
Permite a passagem para a primeira coluna da próxima linha
após a última coluna da linha corrente.

.ip "A_SCROLL:" 11
Rolamento da Janela.
Quando esta opção está habilitada 
a escrita após o canto inferior direito da janela
causará um rolamento na janela.

.ip "A_PAGE:" 11
Paginação.
Esta opção tem o objetivo de evitar que o usuário perca informação
quando uma seqüência muito longa de linhas é escrita em uma janela
antes que a rotina "wrefresh" seja chamada.
Quando esta opção está habilitada,
logo antes da janela sofrer um rolamento
que oculte alguma linha ainda não mostrada ao usuário,
a janela é atualizada
e espera-se o usuário apertar alguma tecla
antes que a escrita na janela prossiga;
se a tecla for o caractere <esc> a opção torna-se inoperante
até a próxima atualização da janela.

.ip "A_REAL:" 11
Os caracteres de controle são escritos diretamente na janela,
ou seja, sem serem transformados para a forma "^X" ou expandidos
em espaços (para os caracteres de tabulação).
.ep
.in 5
.sp
Estes indicadores podem ser combinados através do operador "|"
("ou" bit a bit).

.sp
O modo de escrita inicial de cada janela
é o estabelecido pela execução da macro "waddset"
com os indicadores "A_WRAP" e "A_SCROLL".
.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
waddch, winsch, wattrset, wgetset, wrefset
.br


.in
.sp
.b EXEMPLOS
.in 5
A chamada a seguir reestabelece a configuração inicial de escrita
na janela padrão:
.in 10
.sp
waddset (stdwin, A_WRAP|A_SCROLL);

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
