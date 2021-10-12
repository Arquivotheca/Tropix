.bp
.he 'GETSET (curses)'TROPIX: Manual de Referência'GETSET (curses)'
.fo 'Atualizado em 29.09.89'Versão 2.3'Pag. %'

.b NOME
.in 5
Controla o modo de entrada de texto do teclado:
.sp
.wo "wgetset -"
estabelece o modo
.br
.wo "wgeton  -"
habilita algumas opções do modo
.br
.wo "wgetoff -"
desabilita algumas opções do modo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void wgetset (WINDOW *janela, int modo);
void wgeton (WINDOW *janela, int opções);
void wgetoff (WINDOW *janela, int opções);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As macros "wgetset", "wgeton" e "wgetoff"
controlam o modo de entrada de texto do teclado
de cada janela.

.sp
A macro "wgetset" estabelece o modo de entrada,
enquanto que as macros "wgeton" e "wgetoff"
habilitam e desabilitam, respectivamente,  algumas de suas opções.

.sp
A entrada de texto de "curses" reproduz os serviços realizados
pela interface de terminais do TROPIX
e oferece outras facilidades.
O modo de entrada dos caracteres teclados
é estabelecido habilitando ou desabilitando suas opções
de acordo com os seguintes indicadores:

.in +3
.ip "G_ECHO:" 11
Eco. Os caracteres teclados são escritos em uma janela
e, conseqüentemente, são ecoados no vídeo do terminal.

.ip "G_INSERT:" 11
Inserção.
Quando a entrada de caracteres é ecoada, os caracteres podem ser inseridos
ou podem substituir os caracteres já existentes na janela.
Este indicador habilita a inserção.
Com a opção de inserção
os caracteres lidos são ecoados através da rotina "winsch",
enquanto que sem esta opção através da rotina "waddch".
Além disso, na entrada de texto com inserção
a posição onde o próximo caractere será inserido
é indicada por um espaço extra apontado pelo cursor.

.ip "G_EDIT:" 11
Edição por linha.
A entrada de caracteres pelo teclado normalmente é editada linha a linha.
Isto significa que os caracteres em uma linha
só se tornam disponíveis após o término desta (com um <nl> ou outro terminador)
e que, enquanto isto não ocorre,
o usuário pode acrescentar ou remover caracteres em qualquer posição da linha
(veja a seção "INTERAÇÃO COM O USUÁRIO").
Quando a entrada não é editada
os caracteres são recebidos à medida em que são teclados.
A edição da entrada sempre é realizada com eco.

.ip "G_TOISO:" 11
Geração de caracteres acentuados.
Se o teclado do terminal não for capaz de gerar caracteres acentuados
e a entrada for editada,
determinados pares de caracteres são convertidos
nos caracteres acentuados correspondentes.
Os pares definidos são "," seguido de "c" ou "C" (significando "ç" ou "Ç"),
":" seguido de vogal (significando a vogal com trema),
"=" seguido de "a" ou "o"  (significando ª ou º)
e os acentos "'", "`", "^" e "~" seguidos de vogal
(significando a vogal com o acento).
Para evitar a conversão de um par de caracteres,
basta teclar "\" entre eles (por ex: "'\a" para gerar "'a").
Usando este indicador o programa pode determinar se haverá
a conversão de pares de caracteres em caracteres acentuados
independentemente do teclado poder gerá-los ou não.

.ip "G_TONL:" 11
Conversão do caractere <cr> no caractere <nl>.

.ip "G_KEYPAD:" 11
Reconhecimento de teclas especiais.
Se esta opção está habilitada
as seqüências de caracteres correspondentes às teclas especiais
são convertidas em constantes simbólicas.
Este mecanismo elimina a necessidade
de realização de um processamento dependente 
do tipo do terminal em que o programa está sendo executado.
O reconhecimento de teclas especiais abrange na versão corrente
as seguintes constantes simbólicas:
"KEY_HOME",
"KEY_DOWN",
"KEY_UP",
"KEY_LEFT",
"KEY_RIGHT" e
"KEY_F0" a "KEY_F9".
Estas constantes estão definidas no arquivo para inclusão "<curses.h>".

.ip "G_NODELAY:" 11
Entrada de texto não bloqueada.
O comportamento da rotina "wgetch" quando não há caracteres teclados
depende desta opção;
normalmente ela fica bloqueada esperando o teclado,
com esta opção ela retorna imediatamente a constante inteira "ERR".

.ip "G_SIGRET:" 11
Retorno imediato quando a leitura é interrompida por algum sinal.
Normalmente a recepção de um sinal não afeta a leitura
de caracteres do teclado; com esta opção a rotina "wgetch"
retorna o valor "ERR" imediatamente após a recepção do sinal.

.in -3
.ep
.in 5
.sp
Estes indicadores podem ser combinados através do operador "|"
("ou" bit a bit).

.sp
Em sua configuração inicial a entrada de texto de todas as janelas
é ecoada, editada por linha e converte <cr> em <nl>.

.sp
.in 0
.b
INTERAÇÃO COM O USUÁRIO
.r
.in 5
Nesta seção estão descritos todos os caracteres
que têm significado especial para a entrada de teclado da "curses".
Alguns destes caracteres são usados para edição por linha
e os outros para gerar sinais para o programa.
O nome que acompanha a explicação de cada caractere
é o nome através do qual ele pode ser alterado pelo comando "stty" (cmd).

.sp
O modo de entrada editada proporciona ao usuário algumas facilidades
para corrigir erros de digitação em uma linha ainda não terminada.
A edição é comandada pelos caracteres de edição (descritos abaixo);
os outros caracteres são simplesmente inseridos ou escritos
na posição apontada pelo cursor.

.sp
Os caracteres de edição não são ecoados,
mas seus efeitos são percebidos instantaneamente pelo usuário
através do movimento do cursor
e/ou da alteração da linha no vídeo do terminal.
Um caractere de ediçâo perde seu significado
quando precedido do caractere <^V>.
Os caracteres de edição são apresentados a seguir:

.in +3
.ip "\"erase\":" 10
Este caractere remove o caractere imediatamente à esquerda do cursor.

.ip "\"aerase\":" 10
Este caractere é uma alternativa ao caractere "erase".

.ip "\"word\":" 10
Este caractere remove a palavra imediatamente à esquerda do cursor.
Uma palavra é definida como uma sequência de caracteres
que não contenha <sp> nem <ht>.

.ip "\"kill\":" 10
Este caractere remove toda a linha teclada até o momento.

.ip "\"retype\":" 10
Este caractere reescreve a janela corrente no terminal.
Ele é util quando processos executando em "background"
enviam mensagens ao vídeo que interferem na apresentação da linha em edição.

.ip "\"eol\":" 10
Caractere alternativo para indicar fim de linha.
Este caractere não é ecoado.
Retorna a constante predefinida "EOL".

.ip "\"eof\":" 10
Mais um
caractere alternativo para indicar fim de linha.
Este caractere, no entanto, é ecoado.
Retorna a constante predefinida "EOT".

.ip "\"switch\":" 10
Este caractere causa uma chamada a uma rotina
previamente definida pelo programador.
Ao final da execução desta rotina, a edição prossegue normalmente.
O caractere "switch" pode ser usado para indicar que o usuário
deseja interagir com outra janela.

.ep
.in -3

.sp
Os caracteres a seguir geram sinais.
A cada tipo de sinal está associado uma ação padrão sobre o programa,
entretanto, através da chamada ao sistema "signal" (sys),
um sinal pode ser ignorado ou interceptado.

.in +3
.ip "\"intr\":" 10
Este caractere gera um sinal de interrupção
("SIGINT").
Normalmente, o programa termina  ao
receber  este  sinal.
                    
.ip "\"quit\":" 10
Este  caractere  gera  um   sinal   de   "quit"
("SIGQUIT").
Normalmente,  o programa termina com a
gravação  da  imagem  do  processo   no   disco
("core-dump")  ao  receber este sinal.

.in -3
.ep
.in
.sp
.b OBSERVAÇÕES
.in 5
Por sua própria natureza,
os sinais são considerados eventos ou elementos para interação
com o programa como um todo.
Consequentemente os sinais são sempre gerados
quando seus caracteres correspondentes são teclados
e o comportamento do programa ao recebê-los
(padrão, ignoramento ou interceptação)
não pode ser definido de forma independente para cada janela.

.sp
A preparação para a recepção dos sinais (via "signal" (sys))
deve ser feita antes do início das atividades de "curses"
(veja "incurses" (curses)).

.sp
O controle  de "start/stop" da saída
é realizado automaticamente pelo núcleo do sistema.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):   "
stty
.br
.wo "(sys):   "
ioctl
.br
.wo "(curses):"
waddch, wgetch, winsch, waddset, wattrset, wrefset
.br
.wo "(fmt):   "
termio
.br

.in
.sp
.b EXEMPLOS
.in 5
A chamada a seguir reestabelece a configuração inicial de entrada
na janela padrão:
.sp
.in 10
wgetset (stdwin, G_ECHO|G_TONL|G_EDIT);


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
