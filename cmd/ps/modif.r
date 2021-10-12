.bp
.he 'PS (cmd)'TROPIX: Manual de Modificações'PS (cmd)'
.fo 'Atualizado em 10.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "ps -"
informa sobre o estado dos processos
.br

.in
.sp 2
.ip "01.12.87  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "07.11.88  2.1.0" 20
Foi consertado o problema do usuário não superusuário
do programa "ps" poder listar e até cancelar (!) programas
de outros usuários.

.sp
A malha pela tabela de processos é agora terminada pelo indicador
"lastproc", não necessitando de percorrer a tabela toda.

.ip "09.04.90  2.3.0" 20
Foi feita uma revisão geral. Ajustado o tamanho dá área de leitura
da UPROC do SWAP.

.ip "23.02.91  2.3.1" 20
Foi incorporado na tabela o nome dos "pseudo-terminais".

.ip "06.04.91  2.3.2" 20
Foi feita uma função de leitura de disco "raw" com "count" e "offset"
genéricos.

.ip "14.10.91  2.3.3" 20
Foi introduzida a opção "-lll" para imprimir uma lista de tamanhos.

.sp
A tabela de terminais agora está ordenada.

.sp
A opção "-k" envia agora SIGTERM, e  "-kk" SIGKILL.

.ip "20.10.91  2.3.4" 20
Foi generalizado o "ps" para funcionar também com o TROPIX.

.ip "20.05.96  3.0.0" 20
Foi introduzida a opção "-llll" para imprimir informação sobre SIGALARM.

.ip "27.06.97  3.0.1" 20
Pequena revisão. Atualizado o "ttyc00" para "t00".

.ip "05.07.98  3.0.2" 20
Desmembrado o tempo em tempo de usuário e tempo do sistema.

.ip "16.07.98  3.1.0" 20
Consertado a malha sem fim com nomes de terminais inexistentes.

.ip "24.07.98  3.1.1" 20
Foi introduzida a opção "-lllll" para imprimir informação sobre a STACK do KERNEL.

.ip "09.08.01  4.0.0" 20
Usando "readdir".

.ip "10.08.04  4.6.0" 20
Adaptado para "uproc" apenas.
