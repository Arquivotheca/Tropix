.bp
.he 'ZIPTOOL (cmd)'TROPIX: Manual de Referência'ZIPTOOL (cmd)'
.fo 'Atualizado em 23.12.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "ziptool -"
ferramenta para dispositivos Iomega ZIP
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ziptool [-rpwqe] [-c <code>] [-23] [<dev>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ziptool" executa algumas operações especiais disponíveis
nas unidades ZIP da Iomega.

Estas unidades são acionadores de discos removíveis de 100 MB,
conectados ao barramento IDE/ATAPI, SCSI ou à porta paralela.

.sp
Antes de poder utilizar as unidades ZIP, é necessário configurá-las
(veja abaixo em CONFIGURAÇÃO).

.sp
Em cada execução de "ziptool" é executada a operação definida pela
opção dada (veja abaixo); a operação é executada no dispositivo <dev>,
e caso este não seja dado, em "/dev/zip".

.sp
As opções do comando são:

.in +3
.ip --
Imprime a proteção atual. Sem opções, o comando imprime o estado
da proteção corrente do disco.

.ip -r
Altera a proteção do disco para permitir apenas leituras
(sem senha).

.ip -p
Altera a proteção do disco para permitir apenas leituras,
e atribui uma senha para posterior alteração da proteção.

.ip -w
Altera a proteção para permitir leituras e escritas.
Se o disco estava protegido com uma senha, ela é pedida.

.ip -q
Altera a proteção para permitir leituras e escritas,
porém somente enquanto o disco não for retirado do dispositivo.

.sp
Esta alteração não é possível a partir do estado que
permite apenas leituras com senha.

.ip -e
Retira o disco do dispositivo.

.ip -c
Consulta ou altera o código da partição do sistema de arquivos.
Se <code> for "-", o código é escrito em sua forma hexadecimal
(0x00 a 0xFF). Para os valores 0x06, 0xA8 e 0xA9 são também
dados os valores simbólicos ("FAT", "V7" e "T1").

.sp
Se <code> tiver um valor numérico ou um dos nomes simbólicos
acima, o código da partição é alterado para o valor dado.
O código da partição deve corresponder ao tipo do sistema de arquivos
contido no disco para que ele possa ser montado através do comando
"mount" (cmd".

.ip -2
Atribui o valor "/dev/zip0" a <dev>.

.ip -3
Atribui o valor "/dev/zip1" a <dev>.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Há um modo de proteção em que não são permitidas
leituras nem escritas. Este modo NÃO é suportado pelo TROPIX.

.sp
Os nomes "/dev/zip" e /dev/zip0" são sinônimos.

.in
.sp
.b CONFIGURAÇÃO
.in 5
Para a configuração das unidades, execute as seguintes etapas:

.in +3
.ip 1. 3
Durante a carga do TROPIX, certifique-se de que a unidade ZIP está
sendo reconhecida. Além disto, se for IDE/ATAPI ou SCSI, deverá ser impressa
uma mensagem relativo ao respectivo barramento.

.ip 2. 3
Se não for reconhecida (no caso da porta paralela),
experimente mudar o seu modo (EPP, PS2, NIBBLE, ...) na BIOS do computador.
Na presente versão, nem todas as pastilhas ("chips") de portas paralelas
são reconhecidas pelo TROPIX. Há uma chance da sua unidade ZIP não
poder ser usada.

.ip 3. 3
Uma vez detetado, ainda em mono-usuário, execute o comando
"mkdev" (veja o manual). Ele criará quatro novos arquivos
(provavelmente) denominados "/dev/sda", "/dev/sda4",
"/dev/rsda" e "/dev/rsda4" (confira através do comando "prdisktb").

.ip 4. 3
Crie nomes convenientes através do comando "ln":

.sp
.(l
		ln /dev/sda4  /dev/zip
		ln /dev/rsda4 /dev/rzip
		ln /dev/sda4  /dev/zip0
		ln /dev/rsda4 /dev/rzip0
.)l

.ip 5. 3
Teste o dispositivo através do comando "ziptool".
No caso da porta paralela, se não conseguir acessá-lo corretamente,
experimente alterar na BIOS do PC o modo da porta paralela (EPP, PS2, NIBBLE, ...).

.ip 6. 3
Crie uma entrada adequada no arquivo "fstab" (fmt), como por exemplo:

.sp
.(l
		/dev/zip  /zip	noauto,user
.)l

.ip 7. 3
Pronto! Você já poderá examinar/alterar a proteção através do
comando "ziptool", montar discos ZIP DOS através de "dosmp",
criar coleções GAR/TAR através de "gar", criar sistemas de
arquivos através de "mkfs", montá-los através de "mount" ...
Se o barramento for IDE/ATAPI ou SCSI, você poderá gerenciar
a tabela de partições através de "fdisk" contido no "boot2".

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
fdisk, mount
.br
.wo "(fmt): "
fstab
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
