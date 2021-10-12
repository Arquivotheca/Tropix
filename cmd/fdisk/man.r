.bp
.he 'FDISK (cmd)'TROPIX: Manual de Referência'FDISK (cmd)'
.fo 'Atualizado em 02.01.08'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "fdisk -"
imprime/modifica a tabela de partições de um disco
.br

.in
.sp
.b SINTAXE
.in 5
.(l
-f			(invocado a partir do "boot2")

fdisk [-gv] <dev>	(Comando regular)
.)l

.bc	/*************************************/
fdisk - imprime e/ou modifica a tabela de partições de um disco
editor de partições dos discos

Versão: 4.9.0, de 06.07.06

Sintaxe:
	fdisk [-gv] <dev>

Opções:	-g: Tenta obter a geometria da tabela de partições
	-v: Verboso
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "fdisk" edita a tabela de partições de um disco. o TROPIX possui
2 versões do "fdisk": uma integrada ao "boot2" (ver "boot" (cmd)) e outra
como comando regular.
As duas versões são semelhantes; as diferenças serão indicadas no texto.

.bc	/*************************************/
.sp
As duas versões são semelhantes; as diferenças serão indicadas no texto.
Repare no entanto, que ao modificar uma tabela de partições durante a carga do sistema
operacional TROPIX através da versão integrada ao "boot2", as novas partições
já estarão disponíveis nesta própria execução do TROPIX, enquanto que usando-se
o comando regular, é necessário recarregar-se o TROPIX para disponibilizar a tabela
de partições alterada.
.ec	/*************************************/

.sp
Ao ser iniciado, o "fdisk"

.in +3
.ip 1.
do "boot2"
imprime a tabela de partições do disco corrente,
que é o primeiro disco IDE ("hda"), ou se não houver discos IDE, o primeiro
disco SCSI ("sda").

.ip 2.
(comando regular) 
imprime a tabela de partições do disco dado <dev>.

.ep
.in -3

.bc	/*************************************/
.sp
Assim como os outros sistemas operacionais, o TROPIX também possui o seu
editor de partições de discos, que (como é usual) é chamado de "fdisk".

.sp
No entanto, no TROPIX o "fdisk" está incorporado ao "boot2" (ver "boot" (cmd)),
e só pode ser executado durante a carga do sistema operacional
(e não com o sistema já executando).
A idéia é a de ressaltar o fato de que não é possível alterar as
partições com o sistema em funcionamento.

.sp
Ao ser iniciado, "fdisk" imprime a tabela de partições do disco corrente,
que é o primeiro disco IDE ("hda"), ou se não houver discos IDE, o primeiro
disco SCSI ("sda").

.sp
Em seguida, "fdisk" aguarda um comando (indicado através do "prompt" "fdisk> ").
Cada comando inicia em geral um diálogo durante o qual são pedidos detalhes
para a sua execução.
Durante qualquer um destes diálogos, a resposta "n" cancela o comando.
.ec	/*************************************/

.sp
Os blocos referenciados são sempre de 512 bytes.

.sp
A tabela de partições residente no disco só é efetivamente alterada
quando é dado o comando "w". Portanto você pode experimentar à vontade, criando,
alterando e removendo partições. Quando estiver finalmente
satisfeito com o resultado, use o comando "w" para gravar no disco a tabela editada
(veja as opções "w" e "q", abaixo).

.sp
Para uma explicação sobre a "geometria" de um disco, veja a seção
"GEOMETRIA dos DISCOS", abaixo.

.sp
A partir da versão 4.9.0, "fdisk" mantém uma cópia da tabela de partições
(veja abaixo, na seção "TABELA DE PARTIÇÕES DE EMERGÊNCIA").

.bc	/*************************************/
.sp
Repare no entanto, que ao modificar uma tabela de partições durante a carga do sistema
operacional TROPIX através da versão integrada ao "boot2", as novas partições
já estarão disponíveis nesta própria execução do TROPIX, enquanto que usando-se
o comando regular, é necessário recarregar-se o TROPIX para disponibilizar a tabela
de partições alterada.
.ec	/*************************************/

.sp 2
Os comando de "fdisk" são os seguintes:

.in +3
.ip - 4
Imprime a tabela de partições do disco corrente:

.sp
Inicialmente escreve o nome do disco corrente, o seu tamanho (em MB e blocos) e
a sua geometria (obtida através da BIOS): Número de cilindros, de cabeças, de setores, e
uma letra: "L" para indicar que a BIOS pode acessar o disco através da função INT 13 estendida,
ou a letra "G" para indicar o contrário (isto é, o acesso da BIOS fica limitado aos primeiros
8 GB do disco).

.bc	/*************************************/
A geometria impressa é a que o "fdisk" supõe ser a correta; em certos
(raros) casos, "fdisk" indica que não conseguiu obter a geometria; neste
caso é necessário usar o comando "e" (veja abaixo).
.ec	/*************************************/

.sp
Em seguida é impressa a tabela propriamente dita, com uma linha por partição.
A tabela contém as seguintes colunas:

.in +7
.ip 1. 3
O índice da partição. Este número é utilizado apenas para referenciar uma
partição durante o uso do "fdisk"; não confundir com o número da partição
(partições regulares/estendidas têm números de 1 a 4).

.ip 2. 3
O nome da partição. Durante o uso do TROPIX, cada partição é associada a
um dispositivo com o prefixo "/dev/" anteposto ao nome da partição.
Assim, por exemplo, a partição de nome "hda1" corresponderá ao dispositivo
"/dev/hda1".

.ip 3. 3
Um asterisco se a partição está "ativa", isto é, se é possível carregar
um sistema operacional desta partição. Durante o processo de "boot", o
estágio inicial "boot0" só considera partições "ativas".

.ip 4. 3
Um indicador de desalinhamento (somente se o alinhamente está ligado, veja
o comando "u" abaixo):
"1" se o início da partição estiver desalinhado,
"2" se o final estiver desalinhado e
"3" se ambos estiverem desalinhados.
Se ambos estiverem alinhados, a coluna aparece em branco.

.ip 5. 3
Número do bloco inicial da partição.

.ip 6. 3
Número do bloco final da partição.

.ip 7. 3
Tamanho da partição em blocos.

.ip 8. 3
Tamanho da partição em MB.

.ip 9. 3
Tipo da partição. É dado o código numérico e o tipo simbólico
(se houver). Veja o comando "t".

.ep
.in -7

.ip p 4
Imprime a tabela de partições do disco corrente: este comando é
similar ao anterior, mas são também indicadas (por linhas de pontos)
as áreas vagas do disco (veja também o comando "s").

.ip c 4
Troca de dispositivo (disco) corrente
(apenas para a versão do "fdisk" integrada ao "boot2").

.ip n 4
Cria uma partição nova no disco.
É pedido o tipo da partição a criar (regular, estendida ou lógica),
e em seguida inicia um diálogo pedindo características desejadas
da partição. As características são
influenciadas pelo alinhamento (veja o comando "u").

.ip d 4
Remove uma partição do disco.
Se for removida uma partição estendida, serão removidas todas as partições
lógicas nela contida.

.ip m 4
Altera o tamanho de uma partição do disco.
Este comando mantém o inicío da partição, mas possibilita mudar
o seu final. É possível dar tamanhos incrementais da forma +<n> ou -<n>
em relação ao tamanho original.
Os finais sugeridos/utilizados são
influenciados pelo alinhamento (veja o comando "u").

.ip a 4
Troca o estado (ativo/não ativo) da partição.

.ip l 4
Imprime os tipos/códigos das partições.

.ip t 4
Troca o tipo da partição.

.ip s 4
Imprime as áreas vagas do disco.
Se o alinhamento estiver ligado, só considera áreas de no mínimo
um cilindro (veja a opção "p").

.ip u 4
Liga/desliga o alinhamento (começa ligado):

.sp
Normalmente (isto é, com o alinhamento ligado),
as partições regulares e estendidas alocadas por "fdisk"
começam no início de um cilindro
e terminam no final de um cilindro (isto é, são alinhadas em cilindros).

.sp
Exceções são formadas pela primeira partição ocupada do disco, que começa
no início da segunda trilha do disco (para deixar espaço para a própria
tabela de partições do disco), e a última partição ocupada, que (opcionalmente)
pode terminar no final do disco.
Este final do disco possivelmente não coincide com o final de um cilindro
(isto é, o tamanho do disco pode não ser exatamente múltiplo do
tamanho de um cilindro).

.sp
Normalmente, as partições lógicas alocadas por "fdisk"
começam no início da segunda trilha
de um cilindro, e terminam no final de um cilindro.
Esta primeira trilha de uma partição lógica é utilizada para conter um
"ponteiro" para a partição lógica seguinte
(na realidade é utilizado apenas um setor desta trilha).

.sp
Com o alinhamento desligado, "fdisk" não realiza nenhum alinhamento,
possibilitando o usuário a escolher livremente o início e final de
cada partição.

.sp
A utilização do alinhamento como descrito acima, consiste no alocamento
"ortodoxo", que a maioria dos programas de edição de partições (de outros
sistemas operacionais) também adota.
A NÃO utilização deste esquema de alinhamento pode gerar ganhos
de espaço (por exemplo quase uma trilha no início de cada partição lógica),
mas pode também causar problemas para certos sistemas operacionais.
O TROPIX não exige nenhum alinhamento das partições, mas
antes de criar partições desalinhadas para outros sistemas operacionais, 
certifique-se de que ele as suporta.

.bc	/*************************************/
.ip e 4
Altera a geometria do disco:
A geometria do disco consiste de 3 números: o número de trilhas por cilindro
(isto é, o número de cabeças), o número de setores por trilha e o número de
cilindros.
Estes números são usados nas tabelas de partições, e são relevantes apenas
durante as etapas iniciais do "boot" (rodando no modo de 16 bits),
em que ainda são usadas as chamadas de entrada/saída da BIOS do PC
(INT 13).


.sp
Um disco IDE possui DUAS geometrias (que podem ser diferentes):
a "real" e a da BIOS. A "geometria real" é a que é usada para dar os comandos
de leitura/escrita nas portas dos controladores IDE. Como os discos
IDE modernos podem ter mais de 1023 cilindros, e a BIOS só reservou 10 bits
para este campo, foi criado uma "geometria da BIOS" (fictícia),
com a BIOS encarregando-se de realizar as conversões
necessárias. Esta geometria é normalmente chamada de LBA, e contém um número
de cabeças maiores (do que o real) para que o número de cilindros fique abaixo de 1024.

.sp
Na realidade, os discos IDE modernos (assim como os SCSI) usam densidade variável
(isto é, alocam mais setores por trilha na parte externa do que na parte interna), de
tal modo que a "geometria real" dos discos IDE pode também ser uma fantasia.

.sp
Os discos SCSI não possuem "geometria real", isto é, a geometria não é usada
nos comandos de leitura/escrita nas portas dos controladores SCSI
(os blocos dos discos SCSI são endereçados linearmente pelo próprio número do bloco).
Estes discos recebem geometrias "de fantasia" (normalmente
de 64 cabeças/32 setores por trilha para discos até de 1 GB,
e 255 cabeças/63 setores por trilha para discos acima de 1 GB).
Isto é necessário para satisfazer as chamadas da BIOS.

.sp
É preciso salientar que nos PC modernos, cada disco possui DUAS geometrias
(que podem ser diferentes): a REAL e a da BIOS.
A geometria real é a que é usada para dar os comandos nas portas dos
controladores IDE

.sp
Apenas os discos IDE possuem uma geo

.sp
No passado, estes números correspondiam à geometria real do disco, que realmente
tinha estes números de cabeças, setores, cilindros, etc ..., e durante um comando
de entrada/saída os números são usados.
Nos discos IDE modernos (em geral usando um esquema LBA), assim como nos
discos SCSI, estes 3 números são apenas "fantasias" para satisfazer o modo
de chamar as funções de entrada/saída da BIOS.

.sp
O "boot2" do TROPIX quase sempre consegue obter a geometria correta do disco,
a partir de uma tabela de partição já existente, ou da tabela contida na
memória CMOS do PC (que é atribuída/alterada através do "setup" do PC).
No caso de discos SCSI (se for necessário), são usados os valores
(fictícios) descritos acima.

.sp
Naturalmente, a geometria utilizada pelo "fdisk" deve ser idêntica à
geometria usada pela BIOS do PC.
Há somente um caso em que o TROPIX não consegue obter a geometria correta:
no caso de um disco IDE recém-instalado (isto é, ainda sem tabela de partições),
conectado ao segundo controlador IDE. Neste caso é necessário utilizar o
comando "e" para escolher entre as várias propostas de geometria, ou ainda,
se for necessário, dar manualmente os valores corretos da geometria.
.ec	/*************************************/

.ip w 4
Reescreve (atualiza) a tabela de partições no disco.

.ip q 4
Termina a execução do editor de partições, retornando ao "boot2".

.bc	/*************************************/
.sp
Repare no entanto, que ao modificar uma tabela de partições durante a carga do sistema
operacional TROPIX através da versão integrada ao "boot2", as novas partições
já estarão disponíveis nesta própria execução do TROPIX, enquanto que usando-se
o comando regular, é necessário recarregar-se o TROPIX para disponibilizar a tabela
de partições alterada.
.ec	/*************************************/

.sp
Com a versão integrada ao "boot2", as novas partições
já estarão disponíveis nesta própria execução do TROPIX, enquanto que usando-se
o comando regular, é necessário recarregar-se o TROPIX para disponibilizar a tabela
de partições alterada.


.bc	/*************************************/
Mesmo que tabelas de partições tenham sido alteradas,
é possível continuar com o processo de carga do TROPIX
(ele irá receber as tabelas atualizadas).
NÃO é necessário dar novo "boot".
.ec	/*************************************/


.ip g 4
Tenta obter a geometria através da própria tabela de partições, e a confere
com a geometria obtida da BIOS.

.ip e 4
Utiliza a tabela de partições de emergência (veja abaixo, na seção
"TABELA DE PARTIÇÕES DE EMERGÊNCIA").

.ip ? 4
Imprime um resumo dos comandos.

.ep
.in -3

.in
.sp
.b "GEOMETRIA dos DISCOS"
.in 5
A geometria do disco consiste de 3 números: o número de trilhas por cilindro
(isto é, o número de cabeças), o número de setores por trilha e o número de
cilindros.
Estes números são usados nas tabelas de partições, e são relevantes apenas
durante as etapas iniciais do "boot" (rodando no modo de 16 bits),
em que ainda são usadas as chamadas de entrada/saída da BIOS do PC
(INT 13).

.sp
Originalmente, as operações de leitura/escrita através da INT 13 eram feitas
através da tripla (cabeça, setor, cilindro).
A geometria definida para a INT 13 limitava o número de
cabeças a 255, o número de setores por trilha a 63 e o número de cilindros
a 1024. Com isto, havia um limite de (aproximadamente) 8 GB, após o qual não
se conseguia acessar os discos.

.sp
Em meados de 1995 foram criadas "extensões" para 
a INT 13, através das quais é dado o número linear do bloco do disco (de 64 bits),
ao invés da tripla (cabeça, setor, cilindro). Com isto, acabou-se
com o limite de 8 GB, além da "geometria" do disco ficar irrelevante.

.sp
Se o seu computador possuir estas extensões, você poderá dar "boot" em
partições acima do limite de 8 GB. Para verificar se é o caso, use o comando
"prdisktb" (cmd), ou então verifique em "boot0", durante a carga do sistema.
Além disto, através de "edboot" (cmd), você poderá verificar se está instalado
o "boot1" adequado. O comando "dmesg" lista as mensagens de carga do TROPIX,
onde (logo no início) haverá uma mensagem começando por "BIOS", e informando
se ela é "COM" ou "SEM" "Extensões da INT 13".

.sp
Normalmente, o TROPIX consegue obter a geometria dos discos através
de consulta à própria BIOS. Quando isto não é possível, ele tenta
calcular a geometria através de partições já existentes na tabela
de partições. Se não conseguir é impressa uma mensagem de erro, e neste
caso NÃO é prudente usar o "fdisk".

.in
.sp
.b "TABELA DE PARTIÇÕES DE EMERGÊNCIA"
.in 5
A tabela de partições de um disco está contido no bloco 0 do disco
(exceto os ponteiros das partições lógicas), e se for inadvertidamente 
escrito neste bloco, todas as partições (isto é, toda as informações)
deste disco estarão inacessíveis. Para evitar isto, a partir da versão
4.9.0, o "fdisk" escreve uma cópia da tabela de partições no último
bloco da primeira trilha do disco (denominada de "tabela de partições
de emergência) que pode ser acessada através do comando "e" (veja acima).

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
boot, prdisktb, edboot
.br
.wo "(ref): "
install
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
/boot

.in
.sp
.b EXEMPLO
.in 5
Em "install" (ref) há um exemplo de utilização do "fdisk" do "boot2".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
