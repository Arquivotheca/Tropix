.bp
.he 'UMOUNT (cmd)'TROPIX: Manual de Modificações'UMOUNT (cmd)'
.fo 'Atualizado em 31.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "umount -"
desmonta sistemas de arquivos
.br

.in
.sp 2
.ip "18.3.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "22.7.87  1.0.1" 20
Foi incorporada a proteção de montagem baseada no bite "x" 
do grupo e bite "x" dos outros do dispositivo.

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

.ip "03.03.95  3.0.0" 20
Adaptado para o PC.
Foi introduzida a opção "-f".

.ip "30.05.97  3.0.1" 20
Agora, para desmontar todos foi criada a opção "-a".

.ip "03.04.98  3.0.2" 20
Com a opção "-a", desmonta os dispositivos na ordem contrária
à que foram montados.

.ip "23.01.02  4.2.0" 20
Pequena revisão (agora usando "error") e retirado o "sync".

.ip "30.08.02  4.3.0" 20
Agora NÃO mais usando "/etc/mtab".

.ip "02.10.02" 20
Desmontando de trás para a frente.

.ip "02.10.02" 20
Atualizado para a desmontagem de arquivos regulares.
