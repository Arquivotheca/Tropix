.bp
.he 'REFSET (curses)'TROPIX: Manual de Referência'REFSET (curses)'
.fo 'Atualizado em 11.04.89'Versão 2.3'Pag. %'

.b NOME
.in 5
Controla o modo de atualização do vídeo:
.sp
.wo "wrefset -"
estabelece o modo
.br
.wo "wrefon  -"
habilita algumas opções do modo
.br
.wo "wrefoff -"
desabilita algumas opções do modo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wrefset (WINDOW *janela, int modo);
void wrefon (WINDOW *janela, int opções);
void wrefoff (WINDOW *janela, int opções);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As macros "wrefset", "wrefon" e "wrefoff"
controlam o modo de atualização do vídeo para cada janela.

.sp
A macro "wrefset" estabelece o modo de atualização do vídeo,
enquanto que as macros "wrefon" e "wrefoff"
respectivamente habilitam e desabilitam algumas de suas opções.

.sp
O modo de atualização do vídeo
é estabelecido habilitando ou desabilitando suas opções
de acordo com os seguintes indicadores:

.in +3
.ip "R_CLEAR:" 11
Limpar o vídeo antes de atualizá-lo.
Esta opção é desabilitada após cada chamada à rotina "wrefresh",
ou seja ela só é válida para a próxima chamada à "wrefresh".

.ip "R_CURSOR:" 11
Após a atualização de "janela",
posicionar o cursor na sua posição corrente.

.ip "R_INPUT:" 11
Atualizar a janela antes de cada entrada via teclado.
Para as janelas em que as operações de escrita são
seguidas de operações de entrada,
esta opção
elimina a necessidade do programador explicitamente
chamar a rotina "wrefresh" para atualizar a janela.

.ip "R_INVIS:" 11
Manter o cursor invisível durante a atualização.

.(t
.ip "R_WATCH:" 11
Observação do teclado.
Este indicador habilita
a interrupção da atualização em curso do vídeo do terminal
se um caractere for teclado.
A interrupção da atualização é útil em aplicações
nas quais a entrada de cada caractere
pode determinar uma realimentação visual
que signifique uma grande alteração no vídeo,
se for considerado que quando o usuário
tecla rapidamente uma sequência de caracteres,
ele não necessita realmente da realimentação visual
consequente a cada caractere, mas apenas ao último.
Neste caso, a interrupção da atualização
evita um processo longo de atualização do vídeo
para uma série de estados intermediários,
melhorando o tempo de resposta ao usuário.
É garantido que a atualização do vídeo
abrangerá pelo menos
a atualização da linha corrente
e o movimento final do cursor para a posição corrente
(no caso das janelas em que a permanência do cursor
sobre a posição corrente após a atualização é obrigatória).
.)t

.ip "R_FORCE:" 11
Garantir o posicionamento do cursor antes da atualização.

.ip "R_HOLD:" 11
Enviar efetivamente os caracteres correspondentes à próxima atualização
apenas quando houver entrada do teclado.
Esta opção é útil para ocasiões
em que várias janelas são atualizadas em seqüência
e deseja-se, com o objetivo de tornar o programa mais eficiente,
atualizá-las todas num único fluxo de caracteres para o terminal.

.in -3
.ep
.in 5
.sp
Estes indicadores podem ser combinados através do operador "|"
("ou" bit a bit).

.sp
O modo inicial de atualização do vídeo de todas as janelas
inclui as opções correspondentes ao indicador "R_CURSOR".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses):"
waddset, wattrset, wgetset, wrefresh
.br

.in
.sp
.b EXEMPLOS
.in 5
A chamada a seguir reestabelece a configuração inicial de atualização
na janela padrão:
.in 10
.sp
wrefset (stdwin, R_CURSOR);

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
