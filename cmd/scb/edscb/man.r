.bp
.he 'EDSCB (cmd)'TROPIX: Manual de Referência'EDSCB (cmd)'
.fo 'Atualizado em 21.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "edscb -"
imprime/edita o bloco de controle do núcleo do TROPIX
.br

.in
.sp
.b SINTAXE
.in 5
.(l
edscb [-v] [<kernel> [<scbfile> ...] ]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O núcleo do sistema operacional TROPIX contém um bloco de controle
(SCB) com diversos parâmetros,
cujo objetivo é definir a configuração do sistema.
Alterando um (ou mais) destes parâmetros podemos modificar vários
aspectos do sistema operacional,
e deste modo atender a diversos desejos e necessidades.
Os parâmetros do SCB são descritos em "scb" (fmt).

.sp
O comando "edscb" edita o bloco de controle do núcleo do TROPIX, cuja
imagem está no arquivo <kernel>. Se <kernel> não for dado, será
impresso o bloco de controle do núcleo em execução.

.sp
Se o TROPIX foi instalado em arquivos do MS-DOS, o arquivo contendo
a imagem do núcleo estará no disquete BOOT (veja a observação, abaixo).

.sp
Se um ou mais arquivos de configuração <scbfile> forem dados, eles
devem conter comandos de configuração. Em caso contrário, os comandos
de configuração são lidos da entrada padrão (normalmente o teclado) -
veja o "modo interativo" e "modo não-interativo", abaixo.

.sp
Os novos valores dos parâmetros somente entrarão em vigor
após uma nova carga do sistema operacional, e valerão enquanto
não for utilizado novamente o comando "edscb".

.sp
Após a carga, pode ser utilizado o comando "prscb" (cmd)
(que imprime os parâmetros em uso efetivo no momento),
para verificar se as modificações realizadas através de "edscb"
foram as desejadas.

.sp
É também possível alterar os parâmetros de forma provisória,
durante a carga do sistema, respondendo afirmativamente a pergunta
"Deseja alterar parâmetros?".
Estas alteracões, no entanto, serão válidas apenas para aquela execução
do sistema operacional, já que é apenas alterada a tabela na memória
interna do computador, e não no arquivo imagem do sistema.
 
.sp
Repare que podemos ter várias imagens do núcleo com diversas configurações
(por exemplo, "/new.tropix", "/big.tropix", ...) e selecionar a configuração
desejada durante o "boot", escolhendo um dos arquivos.

.sp
No momento (ainda) não podemos alterar dinâmicamente os parâmetros. Sempre
necessitamos recarregar o sistema para ativar efetivamente um
SCB modificado.

.bc	/*************************************/
Esta tabela (chamada de SCB, ou seja "system control block") está contida
na imagem do núcleo do TROPIX em disco (normalmente o arquivo "/tropix"),
e é copiada para a memória (de onde é consultada) durante o processo
de "boot" (ou seja, a carga do sistema operacional).
.ec	/*************************************/

.sp
As opções do comando são:

.in +3
.ip -v
Verboso.

.ep
.in -3

.in
.sp
.(t
.b "MODO INTERATIVO"
.in 5
Se NÃO for dado nenhum arquivo de configuração <scbfile>, os comandos
de configuração são lidos da entrada padrão (normalmente teclado).
Neste caso, o comando utiliza o modo interativo, no qual "edscb" imprime uma
tela contendo alguns parâmetros,
e em seguida aguarda um comando do usuário após o "prompt"
"> ".
.)t

.sp
Como uma tela apenas não é suficiente para todos os parâmetros, são usadas
3 telas: "A", "B" e "C".
Para consultar uma outra tela, basta teclar o nome da tela desejada.

.sp
Um comando da forma "<param>=<valor>" atribui o valor <valor> ao parâmetro
<param>. O <valor> pode ser um número decimal, hexadecimal ou octal (de acordo
com a sintaxe de "strtol" (libc)) com o sufixo opcional "k", "K", "m" ou "M" indicando
que o número deve ser multiplicado por 1024 ou (1024 * 1024).

.bc	/*************************************/
Para alterar um parâmetro, deve-se dar um comando da forma "<param>=<valor>".
.ec	/*************************************/


.sp
Para encerrar "edscb", com a possibilidade de atualizar o SCB da
imagem do núcleo, temos os comandos "w" e "q" (ou basta teclar <^D>).
Repare que a imagem do núcleo só é atualizada ao final da execução
de "edscb", e somente se for respondida afirmativamente
a confirmação de atualização do arquivo <kernel>.

.in
.sp
.b "MODO NÃO-INTERATIVO"
.in 5
Também é possível ler os comandos de configuração dos parâmetros de um
(ou mais) arquivo(s) de configuração <scbfile>.
Cada comando deve ser da forma "<param>=<valor>" (ver acima), e dados um por linha.
NÃO é necessário incluir os comandos "w" ou "q" ao final.

.in
.sp
.b OBSERVAÇÃO
.in 5
Se o TROPIX foi instalado em arquivos do MS-DOS, o arquivo contendo
a imagem do núcleo estará no disquete BOOT, e os parâmetros do SCB
podem ser modificados do seguinte modo:

.sp
.nf
	mount -w /dev/fd0 /mnt
	edscb /mnt/tropix ... 
	umount /dev/fd0
.fi

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc): "
strtol
.br
.wo "(cmd):  "
prscb
.br
.wo "(ref):  "
install
.br
.wo "(fmt):  "
scb
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
