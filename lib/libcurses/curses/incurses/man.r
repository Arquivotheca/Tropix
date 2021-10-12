.bp
.he 'INCURSES (curses)'TROPIX: Manual de Referência'INCURSES (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Entrada e saída de "curses":

.sp
.wo "incurses  -"
entrada de "curses"
.br
.wo "outcurses -"
saída de "curses"

.sp
.in
.b SINTAXE
.in 5
.(l
#include <curses.h>

int incurses (const char *pgname);
void outcurses (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "incurses" é usada para iniciar a operação de "curses"
em um terminal ou reiniciá-la após uma suspensão temporária.

.sp
A rotina "outcurses" é usada para terminar a operação de "curses"
ou para suspendê-la temporariamente.

.sp
O parâmetro "pgname" deve apontar uma cadeia de caracteres
contendo o nome do programa.
Este nome é usado em mensagens explicativas em caso de erro
na iniciação de "curses".

.sp
O início da operação de "curses" envolve
a obtenção da descrição do terminal em uso,
a criação de estruturas de dados correspondentes ao terminal
e a alteração do modo de comunicação da linha do terminal.

.sp
O tipo do terminal é identificado por um nome passado
para "curses" através do ambiente (variável "TERM").

.sp
As estruturas de dados correspondentes ao terminal incluem
uma janela padrão no tamanho do vídeo
e o seu número de linhas e colunas,
respectivamente acessadas pelo programador pelas variáveis externas
"stdwin", "LINES" e "COLS".
Além disso, são também atribuidos os valores corretos
às variáveis correspondentes
aos caracteres semigráficos (veja "waddch" (curses)).

.sp
Durante o funcionamento de "curses" a comunicação com o terminal
não utiliza os serviços de eco, de edição da entrada
e de processamento da saída
da interface de terminais do kernel.

.sp
A suspensão ou término da operação de "curses"
retorna o modo de comunicação original do programa,
rola o vídeo de uma linha
e posiciona o cursor na primeira coluna da última linha.

.sp
Após a iniciação ou cada reiniciação de "curses"
a primeira chamada à rotina "wrefresh"
limpa o vídeo do terminal.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "incurses" retorna uma dentre as constantes inteiras "OK" e "ERR",
indicando respectivamente sucesso ou insucesso na operação.

.in
.sp
.b DIAGNÓSTICOS
.in 5
Veja esta seção em "newterm" (curses).

.in
.sp
.b OBSERVAÇÕES
.in 5
Programas que se preparam para interceptar os sinais "SIGQUIT" e "SIGINT"
devem fazê-lo antes de chamar a rotina "incurses" pela primeira vez.

.sp
A biblioteca "curses" pode manipular mais de um terminal
(veja "newterm" (curses)).

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
newterm, outcurses, setterm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo

.in
