.bp
.he 'XTSET (xwin)'TROPIX: Manual de Referência'XTSET (xwin)'
.fo 'Escrito em 19.03.03'Versão 4.4.0'Pag. %'

.b NOME
.in 5
xtset - modifica características de uma janela XTERM

.in
.sp
.b SINTAXE
.in 5
.in 5
.(l
xtset [-b <cor_de_fundo>] <-f <cor_de_frente>] [-F <fonte>]
      [-g <ncol>x<nlin>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "xtset" permite ao usuário modificar algumas características
da janela "xterm" onde ele é invocado.
.sp

.in 5
As opções disponíveis são as seguintes:
.in 8

.ip "-b <cor_de_fundo>" 20
modifica a cor de fundo da janela "xterm".
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-f <cor_de_frente>" 20
modifica a cor do texto da janela "xterm" e também a cor do cursor.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-F <fonte>" 20
modifica a fonte a ser utilizada.

.ip "-g <ncol>x<nlin>" 20
modifica o tamanho da janela, que passará a ter <ncol> colunas e <nlin> linhas.
.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xwin): "
xterm
.br

.in
.sp
.b
EXEMPLOS
.r
.ip
xtset -b darkgreen -f white -F 7x13bold -g 90x30
.in 5
.ip
A janela "xterm" terá fundo verde escuro, texto branco, 30
linhas, 90 colunas e usará a fonte 7x13bold.
.in -5
.ep

.in
.sp
.b ESTADO
.in 5
Efetivo.
