.bp
.he 'TEXTMAP (cmd)'TROPIX: Manual de Referência'TEXTMAP (cmd)'
.fo 'Atualizado em 14.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "textmap -"
imprime uma tabela de programas reentrantes em uso
.br

.in
.sp
.b SINTAXE
.in 5
.(l
textmap [-v]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "textmap" imprime uma tabela com informações acêrca
do texto (código executável) dos programas reentrantes
em uso pelo sistema operacional.

.sp
A tabela contém as seguintes colunas:

.in +3
.ip TEXT 8
O último componente do caminho do programa reentrante.

.ip SIZE 8
O tamanho do programa (total) em KB.

.ip M 8
Indica se o programa contém o indicador S_IMETX ligado (veja "chmod (sys)).

.ip T 8
Indica se o programa contém o indicador S_ISVTX ligado (veja "chmod (sys)).

.ip COUNT 8
Dá o número total de processos utilizando o programa
e o número destes programas que estão na memória interna
(isto é, que não estão em SWAP).

.ip CORE 8
Indica (com a letra "c") que o programa está na memória interna.

.ip MADDR 8
Dá o endereço (em KB) do programa na memória (caso a letra "c" esteja presente).

.ip SWAP 8
Indica (com a letra "s") que há uma cópia do programa no SWAP (em disco).

.ip SADDR 8
Dá o endereço (em blocos) da região reservada para o programa no SWAP.

.ep
.in -3

.sp
As opções do comando são:

.in +3
.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Nas versões anteriores do TROPIX (assim como em outros sistemas
operacionais de filosofia UNIX), havia dois tipos de programas
executáveis: "regulares" e "reentrantes".
Atualmente, no entanto, todos os programas do TROPIX são reentrantes. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro, chmod
.br

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in
