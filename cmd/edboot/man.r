.bp
.he 'EDBOOT (cmd)'TROPIX: Manual de Referência'EDBOOT (cmd)'
.fo 'Atualizado em 29.08.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "edboot -"
gerencia "boot0", "boot1" e "boot2"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
edboot  -0 [-wfv] [-i <índice>] [<dispositivo>]
edboot  -1 [-wfv] [<dispositivo>]
edboot [-2] [-v] [-d <dev>] [-p <pgm>] [-V <vídeo>] [<arquivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O processo de carga do sistema operacional TROPIX é composto de
4 etapas: BIOS (em ROM), "boot0", "boot1" e "boot2".

.sp
A BIOS (após diversos testes e inicializações), le o primeiro
bloco de um disco (em geral "/dev/hda" ou "/dev/sda") para a memória, e passa o
controle para o programa contido neste bloco.

.sp
Este primeiro bloco contém o início de "boot0", que por sua vez
le os seus blocos restantes do disco.
O "boot0" imprime uma tabela com as partições ativas dos diversos
discos encontrados no sistema, e pede o índice da partição desejado
para prosseguir o procedimento de carga.

.sp
É impresso também o
índice "default" (valor por omissão), que será utilizado se
for teclado apenas <nl> ou se for decorrido um certo tempo
(da ordem de dezenas de segundos) sem que seja teclado algo.

.sp
O "boot0" permite carregar diversos sistemas operacionais,
e já foi testado com MS-DOS, Win3.1, Win95, Win98, WinNT, Win2000, WinXP, Linux
e FreeBSD (além, naturalmente, do TROPIX).

.sp
Dependendo do índice escohido, iremos carregar o TROPIX ou um
outro sistema operacional.
O programa "edboot" permite imprimir o valor
do índice "default" atual ou alterá-lo (veja a opção "-i" abaixo).

.sp
Se for escolhido a continuação da carga do TROPIX, o "boot0" carrega
o "boot1", que aguarda o nome do "boot2" (normalmente "boot").

.sp
A última etapa, o "boot2", permite escolher o nome do programa que
contém o núcleo do TROPIX (normalmente "tropix")
e também o dispositivo no qual ele deve ser procurado
(normalmente o mesmo dispositivo de onde foi lido "boot1").
.bc	/*************************************/
(normalmente a primeira
partição de tipo TROPIX encontrado durante a análise das várias partições
dos diversos discos).
.ec	/*************************************/
Tanto o nome do programa como o dispositivo podem ser consultados e/ou
modificações através de "edboot" (veja as opções "-d" e "-p" abaixo).

.sp
Normalmente "edboot" procura o "boot0" em "/dev/hda" e "/dev/sda",
o "boot1" em "/dev/root" e o "boot2" em "/boot".
Os argumentos <dispositivo>/<arquivo> permitem alterar isto.

.sp
Sem opções, o programa "edboot" imprime o nome do
programa (núcleo do TROPIX) e dispositivo contidos em "/boot".

.sp
Nesta versão, os diversos programas de "boot" tentam fornecer para a
etapa seguinte qual foi o dispositivo do qual foram lidos.
O objetivo consiste em escolher um dispositivo durante a segunda etapa
("boot0"), e este mesmo dispositivo ser usado para a leitura de "boot1",
"boot2", o núcleo do TROPIX e finalmente ser usado como sistema de arquivos
RAIZ do sistema operacional.

.sp
As opções do comando são:

.bc	/*************************************/
edboot - gerencia "boot0", "boot1" e "boot2"

Versão:	4.9.0, de 29.08.06

Sintaxe:
	edboot  -0 [-wfv] [-i <índice>] [<dispositivo>]
	edboot  -1 [-wfv] [<dispositivo>]
	edboot [-2] [-v] [-d <dev>] [-p <pgm>] [-V <vídeo>] [<arquivo>]

Opções:	-0: Instala/consulta/modifica o "boot0"
	-1: Instala/consulta o "boot1"
	-2: Consulta/modifica o "boot2"
	-w: Instala o "boot0"/"boot1" no <dispositivo>
	-f: Instala/modifica sem consultar o usuário
	-v: Verboso
	-i: A partição "default" do "boot0" será <índice>
	-d: O dispositivo "default" do "boot2" será <dev>
	-p: O programa "default" a executar do "boot2" será <pgm>
	-V: Usa o modo de <vídeo> dado no "boot2" (25x80, 113x48)

Obs.:	Sem opções nem argumentos, são impressos os parametros de "/boot"
.ec	/*************************************/


.in +3
.ip -0
Instala/consulta/modifica "boot0".
Sem as opções "-w" nem "-i", é dado o índice da partição "default" contido em "boot0".

.sp
Além disto, verifica se a versão instalada é a atual (contida no diretório "/etc/boot").

.ip -1
Instala/consulta o "boot1".
Sem a opção "-w", verifica se a versão instalada é a atual (contida no diretório "/etc/boot").

.ip -2
Consulta/modifica "boot2" (opção implícita).
Sem as opções "-d", "-V" nem "-p", são dados o dispositivo e programa "default"s contidos em "boot2".

.ip -w
Instala o "boot0"/"boot1" no <dispositivo> dado.

.sp
O programa "boot0" é construído a partir do arquivo "/etc/boot/h.boot0"
e a tabela de partições existente no bloco 0 do <dispositivo>.
O bloco 0 original do <dispositivo> é guardado no arquivo "/etc/boot/mbr".

.sp
Se o "boot0" não funcionar adequadamente, o bloco 0 original deve ser novamente
recolocado através do comando

.sp
.nf
		cat /etc/boot/mbr ><dispositivo>
.fi

.sp
e a carga do TROPIX deverá ser feita através de disquete.

.sp
O programa "boot1" é obtido a partir do arquivo
"/etc/boot/t1.l.boot1", "/etc/boot/t1.g.boot1",
"/etc/boot/v7.l.boot1" ou "/etc/boot/v7.g.boot1",
de acordo com o tipo do sistema de arquivo (T1 ou V7)
e se o dispositivo suporta as extensões da INT 13 da BIOS ou não.

.ip -f
Não pede a confirmação do usuário antes de instalar
o "boot0"/"boot1".

.ip -v
Verboso. Imprime outras informações, tais como as versões dos
programas.

.ip -i
Atribui o valor <índice> para a partição "default" do "boot0".
Se este valor for NULO, a partição "default" do "boot0"
será a primeira partição do tipo TROPIX (T1 ou V7) encontrado na tabela.

.ip -d
Atribui <dev> como o dispositivo "default" de "boot2".
O valor "NODEV" (ou "nodev") é interpretado como o dispositivo
usado para a leitura do "boot1", ou (na ausência desta informação)
como a primeira partição de tipo TROPIX encontrada durante
a análise das várias partições dos diversos discos.

.ip -p
Atribui <pgm> como o programa "default" de "boot2".

.ip -V
Atribui <vídeo> como o modo de vídeo do "boot2". Podem ser dados os valores
"25x80" (ou "vga") para o modo 720x400 (normal) ou
"113x48" (ou "svga") para o modo 1024x768.
Durante a carga, se o controlador de vídeo não possibilitar o modo
"svga", automaticamente será usado o "vga".

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
boot, fdisk
.br
.wo "(ref): "
install
.br

.in
.sp
.b ARQUIVOS
.in 5
.(l
/dev/hd?
/dev/sd?
/dev/root
/etc/boot/d.boot0		Para testar, em disquete
/etc/boot/h.boot0		Para disco rígido
/etc/boot/t1.d.boot1	Disquete 1.44 MB, T1
/etc/boot/t1.g.boot1	Disco rígido, sem as extensões, T1
/etc/boot/t1.l.boot1	Disco rígido, com as extensões, T1
/etc/boot/v7.d.boot1	Disquete 1.44 MB, V7
/etc/boot/v7.g.boot1	Disco rígido, sem as extensões, V7
/etc/boot/v7.l.boot1	Disco rígido, com as extensões, V7
/etc/boot/mbr		Cópia do primeiro setor do disco
				principal ("/dev/hda" ou "/dev/sda")
/boot			Versão única de "boot2"
.)l

.in
.sp
.b EXEMPLOS
.in 5
A execução de 
.nf

		edboot -0 -w

.fi
irá instalar o "boot0" nos setores iniciais de "/dev/hda" (ou "/dev/sda").

.sp
A execução de 
.nf

		edboot -0 -i 3 /dev/sda

.fi
irá alterar para 3 o índice do  "boot0"
contido no disco SCSI "/dev/sda".

.sp
A execução de 
.nf

		edboot -1 -v /dev/aroot

.fi
informa se a versão de "boot1" contida no primeiro bloco de "/dev/aroot" é a atual.

.sp
.(t
A execução de 
.nf

		edboot -2 -p new.tropix /new.boot

.fi
.)t
irá alterar para "new.tropix" o núcleo "default" do "boot2"
alternativo "/new.boot".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
