.bp
.he 'SYSMAP (cmd)'TROPIX: Manual de Referência'SYSMAP (cmd)'
.fo 'Atualizado em 23.01.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "sysmap -"
desenha um mapa de uso de alguns recursos do sistema
.br

.in
.sp
.b SINTAXE
.in 5
.(l
sysmap [-v] [-d <display>] [-D <desktop>] [-g <geometria>]
					[-f <frente>] [-b <fundo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário gráfico "sysmap" desenha um mapa do uso de alguns recursos do sistema.
Para cada recurso é desenhada uma barra representando a percentagem de seu uso.

.sp
Os recursos são:

.in +3
.ip "1. CPU:" 11
A utilização da CPU durante o último segundo.

.ip "2. HD/SD:" 11
A utilização dos discos rígidos durante o último segundo.

.ip "3. CPU:" 11
A utilização da memória.

.ip "4. ETHER:" 11
A utilização do dispositivo de "ethernet" ("/dev/ed?" e "/dev/fed?") durante o último segundo.

.ip "5. PPPIN:" 11
A utilização da dispositivo de entrada de pacotes PPP ("/dev/ppp???") durante o último segundo.

.ip "6. PPPOUT:" 11
A utilização da dispositivo de saída de pacotes PPP ("/dev/ppp???") durante o último segundo.

.ip "7. DIRTY:" 11
Porcentual de áreas do "cache" de dispositivos estruturados
ainda NÃO atualizados.

.ip "8. HIT:" 11
Porcentual de acerto do "cache" de dispositivos estruturados durante o último minuto.

.ip "9. INODE:" 11
Porcentual de uso da tabela de INODEs (nós-índice) de arquivos.

.ep
.in -3

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
