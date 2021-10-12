.bp
.he 'COREMAP (cmd)'TROPIX: Manual de Referência'COREMAP (cmd)'
.fo 'Atualizado em 23.05.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "coremap -"
imprime um mapa de alocação da memória
.br

.in
.sp
.b SINTAXE
.in 5
.(l
coremap [<período>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "coremap" imprime um mapa de alocação da memória interna
do computador.
O mapa é atualizado cada <período> segundos (1 segundo se o <período>
não for dado).

.sp
O mapa contém uma coluna à esquerda com o endereço da memória (em KB)
correspondente ao primeiro caractere do mapa na linha.
Abaixo do mapa, temos uma linha informando
o número de bytes correspondentes a cada
caractere do mapa, a memória total disponível para programas do
usuário, a parte usada e a percentagem.

.sp
Os significados dos caracteres são:

.in +3
.ip '"."'
Parte livre da memória.

.ip '"+"'
No início da memória: parte usado pelo núcleo do TROPIX, incluindo
as diversas tabelas.
No final da memória: parte alocada para a simulação dos discos
em memória (dispositivos "/dev/ramd0" e "/dev/ramd1").

.ip '"*"'
Partes usados pelos diversos programas.

.ep
.in -3

.sp
Para encerrar o programa, tecle o caractere "intr" (normalmente
<^C>, veja "stty" (cmd)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
stty
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
