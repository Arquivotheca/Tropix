.bp
.he 'PING (cmd)'TROPIX: Manual de Referência'PING (cmd)'
.fo 'Atualizado em 24.03.98'Versão 3.0.3'Pag. %'

.b NOME
.in 5
.wo "ping -"
teste de acesso a um nó remoto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ping [-spv] [-n <n>] <nó remoto>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ping" envia datagramas de "pedido de eco" (ECHO_REQUEST)
do protocolo ICMP para o <nó remoto>. Dependo do recebimento dos
datagramas de "resposta de eco" (ECHO_RESPONSE) do <nó remoto>, podemos
verificar se ele está disponível, e obter uma noção do tempo 
necessário para a viagem de ida e volta de datagramas.

.sp
Sem opções, "ping" envia apenas um datagrama, imprimindo o tempo de
viagem obtido, ou indicando que não obteve resposta
(após 20 segundos de espera).

.sp
As opções do comando são:

.in +3
.ip -s
Envia 10 datagramas e emite uma estatística (veja a opção "-n").

.ip -p
Envia 10 datagramas e imprime um diagrama de barra (veja a opção "-n").

.ip -v
Verboso.

.ip -n
Envia <n> datagramas ao invés de 10 (máximo de 20 datagramas).

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Ao usar "ping" para a deteção de erros, é recomendável utilizá-lo
inicialmente para o próprio computador, para verificar
se a rêde está operacional.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_intro
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
