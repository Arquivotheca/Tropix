.bp
.he 'SHUTDOWN (cmd)'TROPIX: Manual de Referência'SHUTDOWN (cmd)'
.fo 'Atualizado em 10.09.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "shutdown -"
reinicializa o computador após um certo tempo
.br
.wo "reboot   -"
reinicializa o computador imediatamente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
shutdown [-t <tempo>] [-p] [-v]
reboot
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "shutdown" reinicializa o computador após um certo intervalo de tempo.

.sp
Inicialmente, o comando envia para todos os usuários em sessão, mensagens
a cada 10 segundos, informando que o computador irá "sair do ar" (veja a opção "-p").

.sp
Após um certo tempo (normalmente 30 segundos, veja a opção "-t"), os blocos dos
discos são atualizados, e o computador é reinicializado (através da chamada ao
sistema "boot").

.sp
O comando "reboot" é uma variante para reinicializar o computador imediatamente.

.sp
Estes comandos só podem ser usados pelo superusuário.

.sp
As opções do comando são:

.in +3
.ip -t
Reinicializa o computador após <tempo> segundos ao invés de 30.
O <tempo> dado é arredondado para o próximo múltiplo de 10 segundos.

.ip -p
Reinicializa o computador imediatamente, sem enviar as mensagens nem
atualizar os blocos dos discos.
Este procedimento pode ser necessário em situações "de pânico".

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
.wo "(sys): "
sync, boot
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
