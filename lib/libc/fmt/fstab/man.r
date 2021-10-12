.bp
.he 'FSTAB (fmt)'TROPIX: Manual de Referência'FSTAB (fmt)'
.fo 'Atualizado em 05.19.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "fstab -"
tabela de informações sobre sistemas de arquivos
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O arquivo "/etc/fstab" contém informações sobre diversos
sistemas de arquivos e é consultado por diversos
programas, entre os quais "mount", "umount", "fsck", "eject" (cmd).

.sp
Cada linha refere-se a um sistema de arquivos, e contém 3 campos
(separados por espaços ou <ht>), como no exemplo:

.sp
.(l
		/dev/zip   /zip   noauto,user,noexec
.)l

.sp
O primeiro campo dá o nome do dispositivo especial estruturado
(de blocos) a ser montado, o segundo campo dá o nome do diretório
no qual o dispositivo deve ser montado, e o terceiro campo dá uma
lista de opções de montagem separadas por vírgulas.

.sp
A ordem das linhas é importante, pois os comandos "mount" e "fsck"
(entre outros) processam as linhas seqüencialmente.

.sp
Para ver a lista de opções de montagem, veja "getmntopt" (libc).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
mount, umount, fsck, eject
.br
.wo "(libc): "
getmntent, getmntopt
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/fstab

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
