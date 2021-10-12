.bp
.he 'XCOREMAP (cmd)'TROPIX: Manual de Referência'XCOREMAP (cmd)'
.fo 'Atualizado em 14.04.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "xcoremap -"
desenha um mapa de alocação da memória
.br

.in
.sp
.b SINTAXE
.in 5
.(l
xcoremap [-d <display>] [-g <geometria>] [-f <frente>]
					[-b <fundo>] [<período>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando gráfico "xcoremap" desenha um mapa de alocação da memória interna
do computador.

.sp
O mapa é atualizado cada <período> segundos (1 segundo se o <período>
não for dado).

.sp
O mapa contém uma coluna à esquerda com o endereço da memória (em KB)
correspondente ao primeiro quadrado do mapa na linha.
Abaixo do mapa, temos uma linha informando
o número de bytes correspondentes a cada
quadrado do mapa, a memória total disponível para programas do
usuário, a parte usada e a percentagem.

.sp
Os significados dos quadrados são:

.in +3
.ip Cinza: 10
Regiões livres da memória.

.ip Vermelho: 10
O texto do núcleo do TROPIX.

.ip Laranja: 10
As tabelas alocadas pelo núcleo do TROPIX.

.ip Azul: 10
Programas alocados na memória. Normalmente, os textos
ficam no final da memória e a região de dados e a pilha no começo.

.ip Verde: 10
Memória alocada para a simulação dos discos
em memória (dispositivos "/dev/ramd0" e "/dev/ramd1").

.ep
.in -3

.sp
As opções do comando são:

.in +3
.ip "-d <display>" 15
Conecta ao servidor X <display>, ao invés do sevidor local.

.ip "-g <geometria>" 15
Define o tamanho da janela e a posição, no formato "[<dx>x<dy>][+<x>+<y>]";
<dx> e <dy> são os tamanhos e <x> e <y> as posições (em pixels).

.ip "-f <frente>" 15
Define a cor de frente, no padrão RGB ou usando os nomes simbólicos do
arquivo "/usr/xwin/conf/rgb.txt".

.ip "-b <fundo>" 15
Define a cor de fundo.

.ep
.in -3

.sp
Para encerrar o programa, pode-se teclar "q" com o cursor em sua janela.

.in
.sp
.b ESTADO
.in 5
Efetivo.
