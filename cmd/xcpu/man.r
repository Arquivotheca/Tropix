.bp
.he 'XCPU (cmd)'TROPIX: Manual de Referência'XCPU (cmd)'
.fo 'Atualizado em 21.10.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "xcpu -"
desenha o gráfico de uso da UCP
.br

.in
.sp
.b SINTAXE
.in 5
.(l
xcpu [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário gráfico "xcpu" desenha um mapa do uso da Unidade Central de Processamento.

.sp
Cada linha vertical corresponde a um segundo; a sua altura indica o uso da UCP, de
0 a 100 \%. O eixo horizontal corresponde ao tempo, com uma escala em minutos.

.sp
As opções do comando são:

.in +3
.ip "-d <display>" 15
Conecta ao servidor X <display>, ao invés do sevidor local.

.ip "-D <desktop>" 15
Inicia no <desktop> dado.

.ip "-g <geometria>" 15
Define o tamanho da janela e a posição, no formato "[<dx>x<dy>][+<x>+<y>]";
<dx> e <dy> são os tamanhos e <x> e <y> as posições (em pixels).

.ip "-f <frente>" 15
Define a cor de frente, no padrão RGB ou usando os nomes simbólicos do
arquivo "/usr/xwin/conf/rgb.txt".

.ip "-b <fundo>" 15
Define a cor de fundo.

.ip "-v" 15
Verboso.

.ep
.in -3

.sp
Para encerrar o programa, pode-se teclar "q" com o cursor em sua janela.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
