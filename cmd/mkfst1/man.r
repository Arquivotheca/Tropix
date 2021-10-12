.bp
.he 'MKFST1 (cmd)'TROPIX: Manual de Referência'MKFST1 (cmd)'
.fo 'Atualizado em 22.09.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "mkfst1 -"
gera um sistema de arquivos T1
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mkfst1 [-rfv] <dev> [<fs> [<vn> [<pn>]]]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O  comando "mkfst1" cria um sistema de arquivos T1 no dispositivo
<dev>, no formato adequado para conter arquivos e
ser "montado" (veja "mount" (cmd)).

.sp
Também pode ser criado um sistema de arquivos T1 em um arquivo regular
(veja a opção "r").

.sp
Um sistema de arquivos T1 é composto de uma ou mais zonas,
descritas por mapas de bits.
Cada zona compreende 8192 blocos de 4 KB, totalizando 32 MB.
Cada zona contém o seu mapa de bits no 3. bloco, e pode conter
blocos livres, de dados ou de nós-índice.
A primeira zona é especial, pois contém 
o bloco de "boot" (o "boot1", na seqüência de carga do sistema)
e o Superbloco em seus blocos iniciais.

.sp
Para a criação do sistema de arquivos,
é necessária a definição de 3 parâmetros.
Cada um dos parâmetros pode ser dado:

.in +3
.ip 1. 3
Com um valor <val>: O valor <val> será utilizado.

.ip 2. 3
Na forma "-": Será utilizado um valor "padrão", descrito abaixo,
em cada um dos parâmetros.

.ip 3. 3
Na forma ".": Supõe-se que <dev> já contenha um sistema de arquivos,
e o valor será herdado do sistema existente.
Se <dev> NÃO contiver um sistema de arquivos T1,
será utilizado o valor "padrão".

.ip 4. 3
Não ser dado: Como ".", acima.

.ep
.in -3

.sp
Os parâmetros são:

.in +3
.ip <fs>
Tamanho do sistema. Especifica o tamanho total do
sistema de arquivos (normalmente em blocos de 512 bytes;
com o sufixo "k" ou "K" em KB e com o sufixo "m" ou "M" em MB).
O valor padrão é o obtido da tabela de partições do disco
(veja "prdisktb" (cmd)).
Obviamente, se for dado um valor ele não pode ser maior
do que o espaço total da partição.

.sp
Repare que o sistema de arquivos T1 possui blocos de 4 KB,
e podem ocorrer truncamentos na conversão de 512 bytes para 4 KB.

.ip <vn>
Nome do Volume (até 14 caracteres).
Este parâmetro tem aspecto puramente documentacional,
e em geral é utilizado para descrever o conteúdo do sistema
de arquivos.
Este nome é dado nas saídas dos comandos "mount" e "fsname" (cmd).
O valor padrão é igual a "???"
(ele pode ser alterado posteriormente através do comando "fsname").

.ip <pn>
Nome do Dispositivo Físico (até 14 caracteres).
Este parâmetro tem aspecto puramente documentacional,
e em geral é utilizado para indicar o nome do dispositivo em
que reside o sistema de arquivos.
Este nome é dado nas saídas dos comandos "mount" e "fsname" (cmd).
O valor padrão é igual ao nome do dispositivo da tabela de partições
(ele pode ser alterado posteriormente através do comando "fsname").

.ep
.in -3

.sp
Normalmente, antes de iniciar a geração do sistema de arquivos T1,
"mkfst1" escreve todos os parâmetros que irá usar, pedindo então
uma confirmação do usuário para iniciar a geração (veja a opção "-f").

.sp
As opções do comando são:

.in +3
.ip -r
Arquivo regular. Caso o dispositivo <dev> não exista, cria um
arquivo regular com o sistema de arquivos T1.

.ip -f
Força:
Como a geração de um sistema de arquivos destrói todos os arquivos
que porventura já existam em <dev>, "mkfst1" normalmente
pede a confirmação do usuário, antes de iniciar a escrita em <dev>.
Com esta opção, a confirmação do usuário não é pedida.
Além disto, não é impressa a lista de parâmetros utilizados
na geração.

.ip -v
Verboso. Esta opção informa se o dispositivo já tem um sistema de
arquivos T1 (dando os seus parâmetros se for o caso).

.ep
.in -3

.sp
O comando "mkfst1" preenche o bloco de "boot" (primeiro bloco do
sistema de arquivos T1) com o arquivo "/etc/boot/t1.g.boot1" ou
"/etc/boot/t1.l.boot1", a não ser
para os disquetes de 1440 KB ("/dev/fd?"), em que utiliza o arquivo
"/etc/boot/t1.d.boot1". Este bloco pode posteriormente ser alterado
através do comando "edboot" ou "dd" (cmd).

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que, mesmo quando o sistema de arquivos T1 já existe,
e os parâmetros não são dados, NÃO é herdado o bloco de
"boot".

.sp
Por razões de segurança e consistência, o superbloco de um
sistema de arquivos T1 possui um campo que contém um "número mágico".
Deste modo o sistema operacional pode identificar
se naquele dispositivo existe realmente um sistema de arquivos
(por exemplo, em operações de "mount").

.sp
Antes de iniciar a criação do sistema de arquivos T1,
o comando "mkfst1" verifica se o dispositivo <dev> está
está montado sob algum diretório.
Se isto ocorrer, o novo sistema não será
criado, sendo enviada uma mensagem correspondente.
.bc	/*************************************/
Assim, sobre o dispositivo que for raiz ("root") do sistema no
momento, não poderá ser criado um sistema de arquivos.
.ec	/*************************************/

.sp
O comando gera automaticamente o diretório "lost+found",
utilizado pelo "fsckt1" ao reconectar arquivos T1.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mount, fsname, fsckt1, dd, prdisktb, edboot
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/boot/*

.in
.sp
.(t
.b EXEMPLOS
.in 5
.(l
	mkfst1 /dev/hd2a
.)l
.)t

.sp
Se em "/dev/hd2a" já existir um sistema de arquivos T1, serão
herdados todos os seus 3 parâmetros. Em caso contrário, os
parâmetros serão obtidos dos seguinte modo:
<fs> e <pn> da tabela de partições; <vn>: "???";

.sp
.(l
	mkfst1 /dev/sd3 10000
.)l

.sp
O sistema de arquivos T1 terá 10000 blocos de 512 bytes.
Os demais parâmetros serão herdados do sistema anterior, ou
caso o dispositivo ainda não possuir um sistema de arquivos,
o parâmetro <vn> terá o valor "???" e
<pn> será obtido da tabela de partições.

.sp
.(l
	mkfst1 /dev/hdb3c 30000 manuais -
.)l

.sp
O sistema de arquivos T1 terá 30000 blocos de 512 bytes.
o parâmetro <vn> terá o valor "manuais" e
<pn> será obtido da tabela de partições.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
