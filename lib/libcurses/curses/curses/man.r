.bp
.he 'CURSES (curses)'TROPIX: Manual de Referência'CURSES (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
curses - biblioteca desenvolvimento de aplicativos interativos

.sp
.in
.b SINTAXE
.in 5
.(l
#include <curses.h>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A biblioteca "curses" contém rotinas e definições de macros
que facilitam a construção de interfaces programa-usuário
através da criação e manipulação de janelas em terminais de vídeo.
Os programas que usam a "curses" independem dos tipos de terminais
em que são executados.

.sp
Janelas são estruturas bidimensionais de texto
que podem ser total ou parcialmente expostas no vídeo do terminal.
A forma de apresentação das janelas e o texto nelas contido
são definidos pelo programador através da execução de rotinas e macros,
enquanto que as ações necessárias para atualizar o vídeo do terminal
são realizadas pela "curses".

.sp
Cada rotina ou macro disponível na biblioteca "curses"
realiza um dos seguintes tipos de função:
.in 10
.sp
 (a) ativação da "curses",
 (b) manipulação de janelas,
 (c) entrada e saída nas janelas e
 (d) configuração da entrada e saída.
.in 5

.sp
As rotinas de ativação da "curses"
são usadas para iniciar e terminar a sua operação,
assim como para suspendê-la temporariamente
quando for desejado executar outros programas.
A biblioteca "curses" pode operar com um ou vários terminais.

.sp
A manipulação de janelas
corresponde a operações que atingem a janela como um todo.
Dentre estas operações encontram-se
a criação  e remoção de janelas
e a alteração de características das janelas existentes,
tais como tamanho, localização
e prioridade de visibilidade.

.sp
As operações de entrada e saída
são usadas pelo programador para definir o conteúdo das janelas.
Estas operações sempre são realizadas
em uma posição no interior da janela, denominada posição corrente,
que pode ser modificada usando-se a rotina "wmove",
e são baseadas
nas rotinas "wgetch" (leitura de um caractere do teclado),
"waddch" (escrita de um caractere),
"winsch" (inserção de um caractere) e
"wdelch" (remoção de um caractere).
Estas rotinas avançam a posição corrente
correspondendo aos caracteres lidos, escritos ou inseridos.
As janelas podem ainda ser demarcadas no vídeo
por um contorno (veja "wbox" (curses)).

.sp
O comportamento das operações de entrada e saída é configurável
independentemente para cada janela de modo a atender necessidades específicas
de cada janela.
As opções de configuração são agrupadas de acordo suas funcionalidades em:
.in 10
.sp
 (a) modo de entrada de texto do teclado,
 (b) modo de escrita nas janelas,
 (c) modo de atualização do vídeo e
 (d) conjunto de atributos correntes.
.in 5
.sp
Todas as janelas são criadas com a configuração
que normalmente é a utilizada com maior freqüência
(veja "newwin" (curses)).

.sp
Uma janela é identificada por um ponteiro para uma estrutura do tipo "WINDOW".
O ponteiro correspondente a cada janela é obtido na sua criação
e a identifica nas operações seguintes.
A iniciação da biblioteca "curses" (veja "incurses" (curses))
normalmente já cria uma janela padrão (denominada  "stdwin")
que é associada ao terminal correntemente em uso e ocupa todo o seu vídeo.
Um ponteiro nulo ("NOWINDOW") identifica uma janela inexistente.

.sp
As variáveis externas "COLS" e "LINES", declaradas no arquivo "<curses.h>"
contêm respectivamente o número de colunas e de linhas no vídeo do terminal.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As rotinas e macros da biblioteca retornam em sua maioria
um dentre os valores inteiros "OK" e "ERR" definidos no arquivo "<curses.h>",
indicando respectivamente uma operação bem sucedida ou a ocorrência de um erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta biblioteca, apesar da semelhança na forma de utilização,
não é compatível com a biblioteca "curses" criada originalmente
na Universidade da California em Berkeley
nem com a versão da biblioteca "curses" disponível no UNIX System V da AT&T.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):   "
tic, tac
.br
.wo "(curses):"
incurses, newwin, wgetch, waddch, winsch, wdelch,
.wo "         "
wmove, wrefresh, touchwin, wbox
.br
.wo "(fmt):   "
terminfo
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
