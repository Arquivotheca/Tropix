.bp
.he 'MOUNT (cmd)'TROPIX: Manual de Referência'MOUNT (cmd)'
.fo 'Atualizado em 13.03.06'Versão 4.8.0'Pag. %'

.b NOME
.in 5
Monta e desmonta sistemas de arquivos:
.sp
.wo "mount  -"
monta sistemas de arquivos
.br
.wo "umount -"
desmonta sistemas de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mount [-rw] [-o <opções>] <dispositivo> <diretório>
mount [-rw] [-o <opções>] <dispositivo ou diretório>
mount [-rw] [-o <opções>] -a
mount [-v]

umount [-o <opçÕes>] <dispositivo ou diretório> ...
umount [-o <opçÕes>] -a
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Os comandos "mount" e "umount" alteram a árvore que modela a estrutura
de arquivos do sistema operacional.

.sp
O comando "mount" incorpora uma ou mais subárvores de arquivos
à arvore principal do sistema.
Cada subárvore (indicada como <dispositivo> acima),
pode ser dada como um sistema de arquivos local
contido em um dispositivo (arquivo especial de blocos),
a imagem de um sistema de arquivos contida em um arquivo regular local
ou ainda (no caso de sistemas de arquivos NFS) de
um par da forma "servidor:/diretório", ver "nfs" (fmt).




.bc	/*************************************/
Cada subárvore consiste de um sistema de arquivos
contido em um dispositivo (arquivo especial de blocos)
ou um arquivo regular.

O comando "mount" incorpora uma ou mais subárvores de arquivos
(ou seja, sistemas de arquivos contidos em dispositivos ou imagens
de sistemas de arquivos contidos em arquivos regulares)
à arvore principal do sistema.
.ec	/*************************************/

.sp
O comando "umount" desfaz a ação de "mount", desmontando um ou mais sistemas 
de arquivos da árvore principal.

.sp
O arquivo "/etc/fstab" suplementa a execução de "mount" e "umount",
fornecendo informações que podem estar omitidas de algumas formas de "mount" e "umount",
em particular, as opções de montagem.
Para ver o formato deste arquivo e quais são as opções de montagem,
veja "fstab" (fmt) e "getmntent" (libc).

.sp
Normalmente, somente o superusuário pode montar/desmontar sistemas de arquivos.
No entanto, se uma linha do arquivo "/etc/fstab" contiver a opção "user",
qualquer usuário poderá montar o sistema de arquivos correspondente.
Neste caso, somente o usuário que montou o sistema (ou o superusuário)
poderá desmontá-lo.

.sp
Em sua primeira forma,

.sp
.(l
	mount [-rw] [-o <opções>] <dispositivo> <diretório>
.)l

.sp
o comando "mount" incorpora
a subárvore contida no <dispositivo> à arvore principal do sistema.
O <diretório> passa a ser a raiz desta subárvore.
Se, por acaso, o <diretório> era a raiz de uma subárvore antes da montagem,
esta subárvore fica oculta enquanto o sistema de arquivos estiver montado.
Esta é a única forma em que as opções de montagem definidas em "/etc/fstab"
NÃO são consultadas (as opções usadas são as "default", possivelmente modificadas
pelas opções "-r", "-w" ou "-o").

.sp
Na sua segunda forma,

.sp
.(l
	mount [-rw] [-o <opções>] <dispositivo ou diretório>
.)l

.sp

basta dar o <dispositivo> ou o <diretório>, e a informação
completa é procurada no arquivo  "/etc/fstab".
As opções de montagem usadas são as de "/etc/fstab", possivelmente modificadas
pelas opções "-r", "-w" ou "-o".

.sp
Na sua terceira forma

.sp
.(l
	mount [-rw] [-o <opções>] -a
.)l

.sp

são montados todos os
<dispositivo>s no respectivos <diretório>s dados no arquivo "/etc/fstab",
que possuem a opção de montagem "auto".
As opções de montagem usadas são as de "/etc/fstab", possivelmente modificadas
pelas opções "-r", "-w" ou "-o".

.sp
Na sua quarta forma

.sp
.(l
	mount [-v]
.)l

.sp

o comando "mount" imprime uma tabela contendo os dispositivos mountados, respectivos
diretórios, dados sobre o dispositivo e as opções de montagem em uso (veja a opção "-v", abaixo).

.sp
Se houver clientes remotos com sistemas arquivos NFS montados neste servidor,
isto será indicado ao final da listagem após uma linha em branco.

.sp
Na atual versão do sistema TROPIX, podem ser montados <dispositivo>s
contendo sistemas de arquivos dos tipos TROPIX T1, TROPIX V7,
FAT12, FAT16, FAT32, CDROM (ISO 9660, Rock Ridge, Joliet), LINUX/EXT2,
NTFS (somente para leitura) e NFS (versão 2).
Não é necessário dar o tipo: ele é descoberto automaticamente pelo
código da entrada da tabela de partições (ver "prdisktb" (cmd)) ou pelo
superbloco do sistema de arquivos.

.sp
As opções do comando "mount" são:

.in +3
.ip -o
Fornece uma lista das seguintes opções separadas por vírgula:
"default", "ro", "rw", "user", "nouser", "atime", "noatime", "dev", "nodev", "exec",
"noexec", "suid", "nosuid", "eject", "noeject", "case", "nocase",
"rock", "norock", "joliet", "nojoliet", "lfn", "nolfn",
"hard", "soft", "rsize", "wsize", "timeout" e "retrans".
Para o significado de cada uma destas opções, veja "getmntopt" (libc).

.ip -r
Apenas leitura: equivalente a "-o ro".

.ip -w
Leitura e escrita: equivalente a "-o rw".

.ip -a
Monta todos os dispositivos que possuem a opção de montagem "auto" no
arquivo "/etc/fstab".

.ip -v
Normalmente, "mount" somente imprime as opções de montagem diferentes das "default".
Com a opção "-v" imprime tôdas as opções que se aplicam a vários sistemas de arquivos;
Repetindo a opção ("-vv") imprime tôdas as opções, mesmo as particulares de um único
sistema de arquivos.

.ep
.in -3

.sp 2
O comando "umount" desfaz a ação de "mount", desmontando um ou mais sistemas 
de arquivos da árvore principal, dados através dos <dispositivo ou diretório>s montados.

.sp
As opções do comando "umount" são:

.in +3
.ip -o
Fornece uma lista de seguintes opções separadas por vírgula:
a lista é a mesma da opção "-u", de "mount" (acima), no entanto,
atualmente apenas são consideradas as opções "eject" e "noeject",
para modificar a ação descrita no arquivo "/etc/fstab".

.ip -a
Tenta desmontar todos os dispositivos montados (exceto o dispositivo RAIZ e PIPE).
Isto só será possível para aqueles que não estejam em uso.

.ep
.in -3

.sp
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
prdisktb
.br
.wo "(sys):  "
intro, mount, umount
.br
.wo "(libc): "
getmntent, getmntopt
.br
.wo "(fmt):  "
fstab, nfs
.br

.in
.sp
.b ARQUIVOS
.(l
/etc/fstab
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
