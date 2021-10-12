.bp
.he 'EJECT (cmd)'TROPIX: Manual de Referência'EJECT (cmd)'
.fo 'Atualizado em 01.10.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "eject -"
ejeta o meio removível de um dispositivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
eject [-23] [<dispositivo/diretório>]
.)l

.bc	/*************************************/
eject - ejeta o meio removível de um dispositivo

Versão: 4.3.0, de 01.10.00

Sintaxe:
	eject [-23] [<dispositivo/diretório>]

Opções:	-2: O dispositivo será "/dev/zip0"
	-3: O dispositivo será "/dev/zip1"

Obs.:	Se <dev> não for dado, será considerado "/dev/zip"
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "eject" tenta ejetar o meio removível de um dispositivo.
Isto nem sempre é possível, pois nem todos os dispositivos oferecem esta possibilidade.

.sp
Pode ser dado o nome do dispositivo, ou do diretório em que ele é montado usualmente
(através de "/etc/fstab").

.sp
Sem argumento nem opções, ejeta "/dev/zip".

.sp
Se o usuário do programa NÃO for superusuário, verifica se o dispositivo tem a opção
de montagem "user" em "/etc/fstab".

.sp
As opções do comando são:

.in +3
.ip -2
O dispositivo será "/dev/zip0".

.ip -3
O dispositivo será "/dev/zip1".

.ep
.in -3


.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
mount, umount
.br
.wo "(libc): "
getmntent
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
