.bp
.he 'MOUNT (cmd)'TROPIX: Manual de Modificações'MOUNT (cmd)'
.fo 'Atualizado em 04.09.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "mount -"
monta um sistema de arquivos
.br

.in
.sp 2
.ip "18.03.87  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "22.07.87  1.0.1" 20
Foi incorporada a proteção de montagem baseada no bite "x" 
do grupo e bite "x" dos outros do dispositivo.

.in
.sp 2
.ip "01.01.88  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "30.11.90  2.3.0" 20
O programa sofreu (ou deliciou-se) com uma revisão global.

.ip "23.06.91  2.3.1" 20
Agora, não é mais alocado "0" bytes para o arquivo "mtab" vazio.

.ip "25.08.91  2.3.2" 20
Foi introduzida a opção "-f".

.ip "??.??.95  3.0.0" 20
Adaptação para o PC.

.ip "08.02.96  3.0.1" 20
???

.ip "30.05.97  3.0.2" 20
Pequena revisão.

.ip "06.12.01  4.2.0" 20
Usando agora a nova fila de SBs.

.ip "06.12.01" 20
Adaptado para montar sistemas de arquivos de diversos tipos.

.ip "30.08.02  4.3.0" 20
Não mais usando o arquivo "/etc/mtab".

.ip "26.09.02" 20
Usando o arquivo "/etc/fstab".

.ip "24.10.02  4.4.0" 20
Acrescentado o sistema de arquivos EXT2.

.ip "02.10.03  4.5.0" 20
Acrescentado o nome do usuário.

.ip "31.07.04  4.6.0" 20
Atualizado para a montagem de arquivos regulares.

.ip "04.09.04" 20
Com a opção "-a" somente tentar montar os ainda não montados.
