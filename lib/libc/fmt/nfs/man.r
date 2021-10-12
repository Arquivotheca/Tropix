.bp
.he 'NFS (fmt)'TROPIX: Manual de Referência'NFS (fmt)'
.fo 'Atualizado em 05.03.06'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "nfs -"
o sistema de arquivos NFS
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O Sistema de Arquivos NFS (Network File System) permite o acesso a arquivos
de computadores remotos do mesmo modo do acesso a arquivos locais. Este acesso
a arquivos remotos é (quase) completamente transparente para o cliente,
e funciona através de uma variedade de arquitetura de servidores e computadores.

.sp
Um computador cliente pode montar um diretório (isto é, um sistema de arquivos)
de um computador remoto (servidor) em um
diretório local do mesmo modo do que monta um dispositivo físico (um sistema de
arquivos local). A sintaxe é quase a mesma, porém contém também o nome do computador
remoto. Assim, por exemplo, para montar o diretório remoto "/src" do computador "beta"
no diretório "/beta/src" do computador (local) "alpha", deve-se usar um comando da forma

.sp
.nf
		mount beta:/src /beta/src
.fi

.sp
À primeira vista, a semelhança entre "beta:/src" e "/beta/src" pode parecer confusa,
mas "/beta/src" é o modo mais claro de nos referimos ao diretório "/src" do computador "beta".

.sp
O comando "mount" irá tentar conectar o servidor NFS do computador "beta". Este por
sua vez, irá checar se o computador "alpha" possui a permissão para acessar e montar
o diretório "/src", e se este for o caso, irá retornar o sucesso da montagem.

.sp
Uma vez montado, os arquivos remotos podem ser acessados através do diretório (local)
"/beta/src". Como o protocolo NFS utiliza os UIDs e GIDs numéricos locais para verificar as permissões
de acesso aos arquivos remotos, é interessante que estes números sejam os mesmos no cliente e
servidor. Se este não for o caso, é possível dar indicações sobre o mapeamento dos
UIDs e GIDs no arquivo "/etc/exports" (fmt).

.sp
Há várias versões do protocolo NFS: 2, 3 e 4. Atualmente o TROPIX possui apenas a versão 2
implementada; isto no entanto, não é um problema grave, pois a maioria dos servidores
aceitam esta versão, tais como o FreeBSD e LINUX.
Além disto, o TROPIX permite a transferência de apenas 1 KB de dados em cada datagrama NFS, enquanto
que os servidores em geral preferem transferir 4 KB ou 8 KB. Mais adiante veremos como
configurar o FreeBSD e LINUX para usar datagramas de apenas 1 KB de dados.

.sp
O comando "mount" lista tanto os sistemas de arquivos NFS remotos montados por este
cliente, como os sistemas de arquivos NFS locais montados por clientes remotos
veja "mount" (cmd).

.in
.sp
.b "CONFIGURANDO O NFS"
.in 5
Para utilizar o NFS, são necessários 3 passos:

.in +3
.ip 1. 3
Certificar-se de que o cliente/servidor está acessível. Isto não faz parte do protocolo NFS,
e deve ser consultado em "internet" (cmd) e "itnetdev" (fmt).

.ip 2. 3
Preparar uma entrada em "etc/fstab" (no cliente), como o exemplo abaixo:

.sp
.nf
		beta:/src    /beta/src    noauto,nofsck
.fi

.sp
Com esta entrada podemos usar apenas

.sp
.nf
		mount /beta/src
.fi

.sp
ao invés da linha mais acima. Se for desejado, pode ser usada a opção "auto" para
que a montagem seja realizada automaticamente durante a inicialização.

.sp
Esta entrada não é obrigatória, mas
auxilia a execução do comando "mount", pois não será então necessário dar as diversas
opções de montagem.
Veja os detalhes em "fstab" (fmt) e "getmntopt" (libc).


.ip 3. 3
Preparar uma entrada em "etc/exports" (no servidor), como o exemplo abaixo:

.sp
.nf
		/src alpha.nce.ufrj.br(rw,async,no_root_squash)
.fi

.sp
Esta entrada é necessária.
Veja os detalhes em "exports" (fmt) e "exportfs" (cmd).

.sp
Repare que através do comando "exportfs" é possível mudar dinamicamente
os diretórios que podem ser montados remotamente.

.ep
.in -3


.in
.sp
.b "CONFIGURANDO O LINUX"
.in 5
Para um cliente LINUX acessar o servidor NFS do TROPIX, é necessário
lembrar que o TROPIX permite a transferência de apenas 1 KB de dados por datagrama.
Devemos pois usar um comando "mount" da forma:

.sp
.nf
	mount -o rsize=1024,wsize=1024 beta:/src /beta/src
.fi

.sp
ou melhor ainda, preparar uma entrada no arquivo "/etc/fstab" da forma:

.sp
.nf
	beta:/src /beta/src nfs noauto,rsize=1024,wsize=1024 0 0
.fi

.sp
Não é necessário especificar a versão 2 do NFS, pois esta é a versão
"default" do LINUX. Para maiores detalhes, é claro, consulte a documentação
do LINUX.

.in
.sp
.b "CONFIGURANDO O FreeBSD"
.in 5
Assim como no caso do LINUX,  é necessário lembrar que o TROPIX permite
a transferência de apenas 1 KB de dados por datagrama.
Devemos pois usar um comando "mount" da forma:

.sp
.nf
	mount_nfs -2 -r 1024 -w 1024 beta:/src /beta/src
.fi

.sp
Com a opção "-2" especificamos a versão 2 do NFS, pois a versão
"default" do FreeBSD é a 3.

.sp
No arquivo "/etc/exports", repare que o equivalente ao parametro "anonuid"
é "maproot". Assim, uma linha exemplo deste arquivo seria:

.sp
.nf
	/src -maproot=root alpha
.fi

.sp
Para maiores detalhes, é claro, consulte a documentação do FreeBSD.


.in
.sp
.b
.(t
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
internet, mount, exportfs
.br
.wo "(fmt): "
itnetdev, fstab, exports
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
 /etc/fstab
 /etc/exports

.in
.sp
.b ESTADO
.in 5
Efetivo.

.bc	/*************************************/

	LINUX

/dev/hda9        swap             swap        defaults         0   0
/dev/hda10       /                ext2        defaults         1   1
/dev/hda11       /home            ext2        defaults         1   2
/dev/hda1        /dos_c           vfat        defaults         1   0
/dev/hda5        /dos_d           vfat        defaults         1   0
/dev/hda6        /dos_e           vfat        defaults         1   0
/dev/hda7        /dos_f           vfat        defaults         1   0
/dev/hda8        /dos_g           vfat        defaults         1   0
/dev/cdrom       /mnt/cdrom       iso9660     noauto,owner,ro  0   0
/dev/fd0         /fd0             auto        noauto,owner     0   0
piano:/src	/piano/src	nfs	noauto,soft,rw,rsize=1024,wsize=1024	0	0
piano:/tmp	/piano/tmp	nfs	noauto,soft,rw,rsize=1024,wsize=1024	0	0
piano:/home/pedrosal /piano/home/pedrosal nfs noauto,soft,rw,rsize=1024,wsize=1024 0 0
piano:/home	/piano/home	nfs noauto,soft,rw,rsize=1024,wsize=1024 0 0
piano:/alia	/piano/alia	nfs noauto,soft,rw,rsize=1024,wsize=1024 0 0
grave:/src	/grave/src	nfs	noauto,soft,rw,rsize=1024,wsize=1024	0	0
grave:/tmp	/grave/tmp	nfs	noauto,soft,rw,rsize=1024,wsize=1024	0	0
grave:/home/pedrosal /grave/home/pedrosal nfs noauto,soft,rw,rsize=1024,wsize=1024 0 0
devpts           /dev/pts         devpts      gid=5,mode=620   0   0
proc             /proc            proc        defaults         0   0


# See exports(5) for a description.
# This file contains a list of all directories exported to other computers.
# It is used by rpc.nfsd and rpc.mountd.
#
/home/pedrosal		piano(rw,async,no_root_squash) grave(rw,async)




	FreeBSD


# Device		Mountpoint	FStype	Options		Dump	Pass#
/dev/ad0s3b		none		swap	sw		0	0
/dev/ad0s3a		/		ufs	rw		1	1
/dev/ad0s3e		/tmp		ufs	rw		2	2
/dev/ad0s3f		/usr		ufs	rw		2	2
/dev/ad0s3d		/var		ufs	rw		2	2
/dev/cd0		/cdrom		cd9660	ro,noauto	0	0
/dev/cd1		/cdrom1		cd9660	ro,noauto	0	0


#The following examples export /usr to 3 machines named after ducks,
#/usr/src and /usr/ports read-only to machines named after trouble makers
#/home and all directories under it to machines named after dead rock stars
#and, /a to a network of privileged machines allowed to write on it as root.
#/usr                   huey louie dewie
#/usr/src /usr/obj -ro  calvin hobbes
#/home   -alldirs       janice jimmy frank
#/a      -maproot=0  -network 10.0.1.0 -mask 255.255.248.0
#
# You should replace these lines with your actual exported filesystems.
# Note that BSD's export syntax is 'host-centric' vs. Sun's 'FS-centric' one.

/root    -maproot=root  piano.nce.ufrj.br
/usr/src -maproot=root  piano.nce.ufrj.br


mount_nfs -2 -r 1024 -w 1024 piano:/src /mnt






.ec	/*************************************/
