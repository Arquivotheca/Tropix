.bp
.he 'XTERM (xwin)'TROPIX: Manual de Referência'XTERM (xwin)'
.fo 'Escrito em 18.03.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
xterm - emulador de terminal ANSI para o X-Window.

.in
.sp
.b SINTAXE
.in 5
xterm [opções] [-e <comando> [<arg>...]]

.in
.sp
.b DESCRIÇÃO
.in 5
O cliente "xterm" é uma versão simplificada do emulador de terminal "rxvt",
distribuído como contribuição ao X-Window.
Sua função é criar, na tela gráfica, uma janela de texto para que
um comando qualquer do TROPIX possa ser executado.
Normalmente, esse comando é o interpretador "shell" (veja "sh (cmd)").
.sp

.in 5
As opções disponíveis são as seguintes:
.in 8

.ip "-d <display>" 20
indica o servidor X a contactar. O default é o servidor local.

.ip "-D <desktop>" 20
especifica o número do "desktop" em que a janela "xterm" será
criada.

.ip "-b <cor_de_fundo>" 20
modifica a cor de fundo da janela.
O default é branco.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-f <cor_de_frente>" 20
modifica a cor do texto da janela.
O default é preto.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-c <cor_do_cursor>" 20
modifica a cor do cursor.
O default é preto.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-F <fonte>" 20
especifica a fonte a ser utilizada.

.ip "-C" 20
captura as mensagens de erro provenientes do núcleo do sistema.
A janela correspondente terá título "Console (ptyc??)", diferenciando-a
das demais.
Se mais de um comando "xterm" especificar a opção "-C", apenas um deles conseguirá
desviar as mensagens.

.ip "-g <geometria>" 20
especifica a geometria da janela, no formato [<lin>x<col>][<x><y>].
Para maiores detalhes sobre especificação de geometria, veja "X (xwin)".

.ip "-h <diretório>" 20
especifica o diretório inicial.

.ip "-i" 20
a janela "xterm" inicia na forma "iconificada".

.ip "-t <título>" 20
modifica o título da janela. O default é "xterm (ptyc??)",
onde ptyc?? indica o pseudo-terminal utilizado.
Para maiores detalhes sobre especificação de fontes, veja "X (xwin)".

.ip "-s <linhas>" 20
indica o número de linhas que devem ser salvas para posterior
exame através da barra de rolamento. O default é 64 linhas.
Especificar o valor 0 implica a supressão da barra de rolamento.

.ip "-e <comando> [<arg>...]" 20
executa <comando> na janela criada pelo "xterm". O default é
o interpretador de comandos shell ("/bin/sh").
Esta opção deve obrigatoriamente ser a última na chamada,
uma vez que o comando pode receber argumentos.

.ip "-color<n> <cor>" 20
altera o nome da cor básica <n> para <cor>, onde <n> pode variar
de 0 a 15, significando: 0 - preto, 1 - azul, 2 - verde, 3 - ciano,
4 - vermelho, 5 - magenta, 6 - marrom, 7 - branco sujo, 8 - preto,
9 - azul claro, 10 - verde claro, 11 - ciano claro, 12 - vermelho claro,
13 - magenta claro, 14 - marrom claro, 15 - branco.
.ep

.in
.sp
.b
COMPATIBILIDADE
.in 5
Por questão de compatibilidade com o emulador "xterm" original,
são aceitas opções com nomes longos, a saber:
.sp
.nf
          -bg		equivale a	-b
          -background	equivale a	-b
	  -cr		equivale a	-c
	  -desktop	equivale a	-D
          -display	equivale a	-d
          -fg		equivale a	-f
          -foreground	equivale a	-f
          -fn		equivale a	-F
          -font		equivale a	-F
          -geometry	equivale a	-g
          -title	equivale a	-t
.fi
.sp
As opções longas podem ser incluídas no arquivo $HOME/.Xdefaults, de
modo que fiquem implícitas a cada execução do "xterm". Por exemplo,
um arquivo $HOME/.Xdefaults contendo as seguintes linhas
.sp
.nf
          XTerm*background:     black
          XTerm*geometry:       80x44
          XTerm*color4:         orange
.fi
.sp
fará com que toda execução do "xterm" assuma o fundo preto, a geometria
de 44 linhas por 80 colunas e a cor vermelha (4) passe a ser laranja.


.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
sh
.br
.in 5
.wo "(fmt):  "
terminfo
.br
.in 5
.wo "(xwin): "
X, fvwm
.br

.in
.sp
.b
EXEMPLOS
.r
.ip
xterm -b darkgreen -f white -F 7x13bold
.in 5
.ip
A janela "xterm" terá fundo verde escuro, texto branco e usará
a fonte 7x13bold.
.in -5

.ip
xterm -C -g 80x13+30+100
.in 5
.ip
A janela "xterm" terá 80 colunas e 13 linhas.
Se possível, o gerenciador de janelas ("fvwm") posicionará o
canto superior esquerdo da janela nas coordenadas (30, 100),
a partir do canto superior esquerdo do "desktop".
Nesta janela serão impressas as mensagens de erro do núcleo do sistema,
se já não houver outra com essa finalidade.
.in -5

.ip
xterm -g 80x30 -t Coremap -e coremap
.in 5
.ip
A janela "xterm" terá 80 colunas, 30 linhas e título "Coremap (ptyc??)".
O comando a ser executado é "coremap", que mostra um
mapa de alocação de memória (veja "coremap (cmd)").
.in -5
.ep

.in
.sp
.b ESTADO
.in 5
Efetivo.
