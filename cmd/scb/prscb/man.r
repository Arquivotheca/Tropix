.bp
.he 'PRSCB (cmd)'TROPIX: Manual de Referência'PRSCB (cmd)'
.fo 'Atualizado em 04.12.01'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "prscb -"
imprime o bloco de controle do núcleo do TROPIX
.br

.in
.sp
.b SINTAXE
.in 5
.(l
prscb [-v]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O núcleo do sistema operacional TROPIX contém um bloco de controle
(SCB) com diversos parâmetros,
cujo objetivo é definir a configuração do sistema.
Alterando um (ou mais) destes parâmetros podemos modificar vários
aspectos do sistema operacional,
e deste modo atender a diversos desejos e necessidades.
Os parâmetros do SCB são descritos em "scb" (fmt).

.sp
O comando "prscb" imprime o bloco de controle do núcleo do TROPIX.
Os valores impressos são dos parâmetros em uso efetivo no momento.

.sp
Para alterar os valores dos parâmetros, veja "edscb" (cmd).

.sp
As opções do comando são:

.in +3
.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
edscb
.br
.wo "(fmt): "
scb
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
