.bp
.he 'XCLOCK (xwin)'TROPIX: Manual de Referência'XCLOCK (xwin)'
.fo 'Escrito em 19.03.03'Versão 4.4.0'Pag. %'

.b NOME
.in 5
xclock - relógio analógico para o X-Window.

.in
.sp
.b SINTAXE
.in 5
xclock [opções]

.in
.sp
.b DESCRIÇÃO
.in 5
O cliente "xclock", além de exibir em uma janela um relógio
analógico, notifica ao usuário a chegada de correspondência
em sua caixa postal.
.sp

A janela "xclock" possui como título a data atual.
.sp

.in 5
As opções disponíveis são as seguintes:
.in 8

.ip "-d <display>" 20
indica o servidor X a contactar. O default é o servidor local.

.ip "-D <desktop>" 20
especifica o número do "desktop" em que a janela "xclock" será
criada.

.ip "-b <cor_de_fundo>" 20
modifica a cor de fundo da janela.
O default é branco, podendo ser alterado em $HOME/.Xdefaults.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-f <cor_de_frente>" 20
modifica a cor do mostrador do relógio (números e ponteiros).
O default é preto, podendo ser alterado em $HOME/.Xdefaults.
Para maiores detalhes sobre especificações de cores, veja "X (xwin)".

.ip "-g <geometria>" 20
especifica a geometria da janela, no formato [<dx>x<dx>][<x><y>].
Para maiores detalhes sobre especificação de geometria, veja "X (xwin)".

.ip "-i" 20
a janela "xclock" inicia na forma "iconificada".

.ip "-u <segundos>" 20
especifica o período de atualização do relógio. O default
é a cada segundo. Se o período especificado não for de 1 segundo,
o ponteiro dos segundos não é mostrado.

.ip "-m <segundos>" 20
especifica o período de consulta à caixa postal do usuário. O default
é a cada minuto.

.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xwin): "
X, fvwm
.br

.in
.sp
.b
ARQUIVOS
.in 5
$HOME/.Xdefaults
.br
/var/mail/$USER

.in
.sp
.b
EXEMPLOS
.r
.ip
xclock -b darkgreen -f white
.in 5
.ip
A janela "xclock" terá fundo verde escuro e mostrador branco.
.in -5

.ip
xclock -g 200x200+30+100 -u 60
.in 5
.ip
A janela "xclock" terá 200x200 pixels.
Se possível, o gerenciador de janelas ("fvwm") posicionará o
canto superior esquerdo da janela nas coordenadas (30, 100),
a partir do canto superior esquerdo do "desktop".
O mostrador é atualizado a cada minuto e o ponteiro dos
segundos não é desenhado.
.in -5
.ep

.in
.sp
.b ESTADO
.in 5
Efetivo.
