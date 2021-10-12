.bp
.he 'BOOT (cmd)'TROPIX: Manual de Referência'BOOT (cmd)'
.fo 'Atualizado em 29.08.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
Carga do sistema operacional:
.sp
.wo "boot0 -"
Estágio inicial
.br
.wo "boot1 -"
Estágio intermediário
.br
.wo "boot2 -"
Estágio final
.br

.in
.sp
.b SINTAXE
.in 5
.(l
boot1:      > [-sSvV] [<boot2>]

boot2:  boot> [-x] [-v] [-<dev>] [<pgm>]
        boot>  -?  [-v]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O processo de carga do sistema operacional TROPIX é composto de
4 etapas: BIOS (em ROM), "boot0", "boot1" e "boot2".

.sp
O "boot0" e "boot1" operam ainda no modo de 16 bits, utilizando as chamadas
"INT 13" da BIOS para realizar a leitura dos blocos dos discos.
.bc	/*************************************/
O "boot0" tenta, automaticamente, utilizar as extensões da INT 13, e
há duas versões para o "boot1": "lboot1" (usando) e "hboot1" (sem usar)
as extensões da INT 13. Se a BIOS do seu computador suportar estas
extensões, você poderá carregar sistemas operacionais (pelo menos o TROPIX)
em partições acima do limite de 8 GB (números de cilindro maiores do que 1024).
.ec	/*************************************/

.sp
Na última fase de "boot", "boot2" o modo é passado para 32 bits, quando
então a entrada/saída passa a ser realizada através de "drivers" incluídos
em "boot2". No momento, "boot2" contém "drivers" para disquete, discos IDE e
os controladores de discos SCSI Adaptec ISA 1542 e PCI 2940/29160.

.sp
Os diversos programas de "boot" tentam fornecer para a
etapa seguinte qual foi o dispositivo do qual foram lidos.
O objetivo consiste em escolher um dispositivo durante a segunda etapa
("boot0"), e este mesmo dispositivo ser usado para a leitura de "boot1",
"boot2", o núcleo do TROPIX e finalmente ser usado como sistema de arquivos
"raiz" do sistema operacional.

.sp
Repare que, a partir da versão 4.3.0, há dois tipos de sistemas de arquivos
nativos do TROPIX: o "antigo" (V7) com blocos de 512 bytes (em obsolescência)
e o "novo" (T1) com blocos de 4 KB.

.sp
A descrição do processo de "boot" dada abaixo refere-se à carga através de
disco rígido. Se for dado "boot" em um disquete com um sistema de arquivos
TROPIX (V7 ou T1), este conterá no primeiro bloco (de 512 bytes ou 4 KB)
o "boot1" (não havendo o "boot0").

.in
.sp
.b BIOS
.in 5
A BIOS do PC (após diversos testes e inicializações), le o primeiro
bloco (de 512 bytes) de um disco ("/dev/hda" ou "/dev/sda") para a memória,
e passa o controle para o programa contido neste bloco.

.in
.sp
.b BOOT0
.in 5
Este primeiro bloco contém o início de "boot0", que por sua vez
le os seus blocos restantes do mesmo disco.
O "boot0" imprime uma tabela com as partições ativas dos diversos
discos encontrados no sistema, e pede o índice da partição desejada
para prosseguir o procedimento de carga.

.sp
São impressos também o índice "default" (valor por omissão), e o número
de segundos remanescentes até que seja carregada
(se nada for teclado)
a partição indicada pelo índice "default".
Este índice "default" pode ser alterado através de "edboot" (cmd).
Repare que, se o índice "default" for NULO, ele será substituído pela primeira
partição de tipo TROPIX (V7 ou T1) da tabela.

.sp
O "boot0" permite carregar diversos sistemas operacionais,
e já foi testado com MS-DOS/Win3.1/Win95/Win98/WinNT/Win2000, Linux
e FreeBSD (além, naturalmente, do TROPIX).

.sp
Dependendo do índice escohido, iremos carregar o TROPIX ou um
outro sistema operacional.
Se for escolhida a continuação da carga do TROPIX, o "boot0" carrega
o "boot1", contido no início da partição ativa escolhida.

.in
.sp
.(t
.b BOOT1
.in 5
O "boot1", por sua vez, aguarda que seja teclado o nome do arquivo
que contém o "boot2" (normalmente "/boot", que é o valor "default"
usado se for teclado <enter>).

.sp
Normalmente, o modo de vídeo usado a partir de "boot2" é o "svga"
(113 colunas e 48 linhas, 1024x768).
Isto pode ser modificado para todas as cargas do sistema através
de "edboot" (cmd), ou apenas para esta carga através de uma das opções do "boot1":
"-v" ou "-V" para o modo "vga" (80 colunas e 24 linhas, 720x400) e
"-s" ou "-S" para o modo "svga"

.sp
Durante a carga, se o controlador de vídeo não possibilitar o modo
"svga", automaticamente será usado o "vga".

.sp
Em seguida, "boot1" le o "boot2" e passa o controle para ele.
.)t

.in
.sp
.b BOOT2
.in 5
A última etapa, o "boot2", permite carregar o arquivo contendo o núcleo 
do TROPIX, editar as tabelas de partições, além de outras funções.
Vários comandos de "boot2" iniciam diálogos, pedindo informações sobre
o comando a ser executado. Neste caso, a qualquer momento, o comando pode
ser cancelado através de uma resposta "n".

.bc	/*************************************/
A última etapa, o "boot2", permite escolher o nome do programa que
contém o núcleo do TROPIX (normalmente "/tropix")
e também o dispositivo no qual ele deve ser procurado (normalmente a primeira
partição de tipo TROPIX encontrado durante a análise das várias partições
dos diversos discos). 
Tanto o nome do programa como o dispositivo "default" podem ser alterados
através de "edboot" (cmd).
.ec	/*************************************/

.in
.sp
.b "EXTENSÕES da INT 13"
.in 5
O "boot0" tenta, automaticamente, utilizar as extensões da INT 13, e
para discos rígidos, há ao total 4 versões do "boot1" (veja a lista abaixo).
.sp
Se a BIOS do seu computador suportar estas
extensões, você poderá carregar sistemas operacionais (pelo menos o TROPIX)
em partições acima do limite de 8 GB (números de cilindro maiores do que 1024).

.in
.sp
.b 'COMANDOS de "BOOT2"'
.in 5
Os comandos aceitos por "boot2" são:

.in +3
.ip "[-x] [-<dev>] [<pgm>]" 3
.sp
Carrega (e executa) <pgm> (normalmente um núcleo do TROPIX),
contido no dispositivo <dev>.
Tanto <dev> como <pgm> tem valores "defaults", que podem ser alterados
através de "edboot" (cmd). Se não foram alterados, estes valores
"default" são "a partição usada por boot1" e "/tropix".
.bc	/*************************************/
"a primeira partição de tipo TROPIX encontrada" e "/tropix".
.ec	/*************************************/

.sp
Após a carga de <pgm> (e antes de sua execução) ainda é pedida uma
confirmação para a execução. No caso de uma resposta negativa, o
"boot2" retém o controle, e pode ser executada qualquer uma das funções
aqui descritas (inclusive, por exemplo, carregar outro <pgm>.

.ip -t 3
Imprime a tabela de partições em uso, com uma linha por partição.
A tabela contém as seguintes colunas:

.in +5
.ip 1. 3
O nome da partição. Durante o funcionamento do TROPIX, cada partição é associada a
um dispositivo com o prefixo "/dev/" anteposto ao nome da partição.
Assim, por exemplo, a partição de nome "hda1" corresponderá ao dispositivo
"/dev/hda1".

.ip 2. 3
Um asterisco se a partição está "ativa", isto é, se é possível carregar
um sistema operacional desta partição. Durante o processo de "boot", o
estágio inicial "boot0" só considera partições "ativas".

.ip 3. 3
Número do bloco (de 512 bytes) inicial da partição.

.ip 4. 3
Tamanho da partição em blocos.

.ip 5. 3
Tamanho da partição em MB.

.ip 6. 3
Código numérico do dispositivo associado à partição. Este código é composto
de duas partes chamadas de "major" e "minor".

.ip 7. 3
Número da unidade e alvo físicos do dispositivo.

.ip 8. 3
Nome simbólico do tipo da partição.

.ep
.in -5

.ip -f 3
Executa o editor de tabelas de partições "fdisk" (cmd).

.bc	/*************************************/
.ip -l 3
Imprime o conteúdo de um diretório.
O nome do dispositivo e o nome do diretório são pedidos através de um
diálogo.
.ec	/*************************************/

.ip -d 3
Imprime uma listagem em hexadecimal e caracteres de um dispositivo.
O nome do dispositivo e o endereço inicial são pedidos através de um
diálogo.

.ip -m 3
Imprime uma listagem em hexadecimal e caracteres da memória.
O endereço inicial é pedido através de um diálogo.

.ip "-r [-<dev>] [<arq>]" 3
.sp
Descompata  <arq> (normalmente a imagem comprimida de um sistema de arquivos
T1 contendo uma RAIZ), contido no dispositivo <dev> para uma área de RAMD.
O dispositivo <dev> tem um valor "default", que é "a partição usada por
"boot1", mas pode ser alterado através de "edboot" (cmd).
O valor "default" de <arq> é "boot.dsk.gz".

.sp
Após esta carga, deve-se utilizar o comando "-x" com o dispositivo "ramd0",
para executar o núcleo do TROPIX, com a RAIZ no RAMD.

.ip -i 3
Inicia uma instalação ou conserto de sistemas de arquivos.
É equivalente a "-r -fd0 boot.dsk.gz" seguido de "-x -ramd0 tropix".
Isto decompacta uma imagem comprimida de uma RAIZ de um disquete
para o RAMD, e em seguida executa o TROPIX, com a RAIZ no RAMD.


.bc	/*************************************/
.ip -c 3
Copia um dispositivo ou arquivo regular para um dispositivo.
A cópia é feita inicialmente para a memória, e desta para o dispositivo de
saída.
O nome do dispositivo/arquivo de entrada, deslocamento da entrada,
nome do dispositivo de saída, deslocamento da saída
e o número de blocos a copiar são pedidos através
de um diálogo.

.ip -r 3
Copia um dispositivo para uma área de RAMD.
O nome do dispositivo de entrada é pedido através de um
diálogo (se não for dado, é usado "fd0"). 
É copiado a imagem inteira do dispositivo no final da memória física.
.ec	/*************************************/

.ip -s 3
Reavalia a velocidade da CPU.
Com isto, são recalculados a freqüência da CPU (MHz) e o valor
adequado para DELAY (a função que permite esperas a nível de microsegundos).
Ao final, são impressas 10 mensagens com intervalos de um segundo,
para verificar se o valor de DELAY está correto.

.bc	/*************************************/
.ip -p 3
Imprime os parâmetros dos discos IDE.
São impressos a geometria real e a utilizada pela BIOS dos discos IDE
(Esta última apenas para os discos do primeiro controlador).
.ec	/*************************************/

.ip -v 3
Verboso.

.ip -? 3
Imprime um resumo dos comandos.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
edboot, fdisk
.br
.wo "(ref): "
install
.br

.in
.sp
.b ARQUIVOS
.in 5
.(l
/etc/boot/d.boot0		Para testar, em disquete
/etc/boot/h.boot0		Para disco rígido
/etc/boot/t1.d.boot1	Disquete 1.44 MB, T1
/etc/boot/t1.g.boot1	Disco rígido, sem as extensões, T1
/etc/boot/t1.l.boot1	Disco rígido, com as extensões, T1
/etc/boot/cd.boot1		Disco compacto (CD)
/etc/boot/v7.d.boot1	Disquete 1.44 MB, V7
/etc/boot/v7.g.boot1	Disco rígido, sem as extensões, V7
/etc/boot/v7.l.boot1	Disco rígido, com as extensões, V7
/boot			Versão única de "boot2"
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
