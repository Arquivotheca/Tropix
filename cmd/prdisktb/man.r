.bp
.he 'PRDISKTB (cmd)'TROPIX: Manual de Referência'PRDISKTB (cmd)'
.fo 'Atualizado em 14.04.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "prdisktb -"
Imprime a tabela de discos/partições
.br

.in
.sp
.b SINTAXE
.in 5
.(l
prdisktb [-av]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "prdisktb" imprime a tabela de discos/partições do
núcleo do sistema operacional TROPIX.

.sp
Esta tabela é construída durante o processo de carga do sistema
("boot") e posteriormente alterada/estendida durante o seu funcionamento.
Ela é de vital importância, pois contém a visão do
sistema relativos a todos os discos e partições presentes.

.sp
Normalmente (isto é, sem a opção "-a"), "prdisktb" imprime os 
seguintes itens a respeito de cada disco/partição:

.in +3
.ip NAME 8
Nome do disco/partição.
Se for da forma "fd?", "hd?", "sd?" ou "ramd?" refere-se a um disco
(considerado integralmente).
Se contiver um sufixo de um dígito e (possivelmente)
uma letra, refere-se a uma partição de um disco.

.ip A 8
Contém um asterisco se a partição for ativa, isto é, se ela é visível
durante o estágio inicial da etapa de carga dos diversos
sistemas operacionais
("boot0").

.ip OFFSET 8
Deslocamento do começo da partição (em relação ao início do disco)
em blocos de 512 bytes.

.ip SIZE 8
Tamanho da partição/disco em blocos de 512 bytes.

.ip MB 8
Tamanho da partição/disco em MB.

.ip DEV 8
Número do dispositivo (Major + Minor), como referenciado dentro do
sistema operacional.

.ip U/T 8
Número do controlador + número do dispositivo lógico.
Estes números são utilizados para identificar qual o controlador e/ou
disco.

.ip TYPE 8
Tipo da partição, com o código numérico e simbólico.

.ep
.in -3

.sp
Com a opção "-a", "prdisktb" substitui alguns dos itens da lista acima
por:

.in +3
.ip HEAD 8
Número de cabeças, segundo a geometria do disco. Esta não é
necessariamente a geometria real do disco, mas sim a geometria usada
pela BIOS do PC e na tabela de partições contida no disco.

.ip SECT 8
Número de setores por trilha, segundo a geometria do disco.
Veja acima.

.ip CYL 8
Número de cilindros, segundo a geometria do disco.
Veja acima.

.ip G/L 8
Indicador da presença das extensões da INT 13 da BIOS.
Um "L" (linear) indica a presença das extensões,
um "G" (geometria) a ausência e um branco significa que
não foi possível obter informações da BIOS relativas a este disco.

.ip BLSZ 8
Tamanho do bloco do dispositivo.

.ip OPEN 8
Número de "open"s desta partição/disco.

.ip LOCK 8
Indica se houve um "open" de uso exclusivo nesta partição/disco.

.ep
.in -3

.sp
As opções do comando são:

.in +3
.ip -a
Imprime informações alternativas (veja acima).

.ip -v
Verboso.

.ep
.in -3

.bc	/*************************************/
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
c
.br
.wo "(sys): "
s
.br
.wo "(libc):"
l
.br
.wo "(xti): "
f
.br
.wo "(fmt): "
f
.br
.ec	/*************************************/

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
