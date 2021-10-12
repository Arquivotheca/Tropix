.bp
.he 'REDUCE (cmd)'TROPIX: Manual de Referência'REDUCE (cmd)'
.fo 'Escrito em 16.09.90'Versão 2.3'Pag. %'

.b NOME
.in 5
Comprime/expande um arquivo segundo o algoritmo LZW:
.sp
.wo "reduce -"
comprime ou expande, de acordo com as opções
.br
.wo "rcat   -"
apenas expande
.br

.in
.sp
.b SINTAXE
.in 5
.(l
reduce [-b <n>] [-rfv] <original> [<comprimido>]

reduce -u [-ft] <comprimido> [<original>]

rcat <comprimido> [<original>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "reduce" tenta reduzir o tamanho do arquivo <original>,
armazenando uma forma reduzida no arquivo <comprimido>.
Se o nome do arquivo <original> for "-", ele é lido da entrada
padrão. Se o nome do arquivo <comprimido> não for dado, ele é escrito
na saída padrão.

.sp
Um arquivo <comprimido> pode ser restaurado à sua forma original
através de "rcat" (ou "reduce" com a opção "-u").
Se o nome do arquivo <comprimido> for "-", ele é lido da entrada
padrão. Se o nome do arquivo <original> não for dado, ele é escrito
na saída padrão.

.sp
O comando "reduce" usa um algoritmo baseado no método de
"Lempel-Ziv-Welch" (LZW), um processo adaptativo de compressão de dados.
Subcadeias comuns do arquivo <original> são substituídas inicialmente
por códigos de 9 bits, a partir de 257. Quando é alcançado o código 512,
passa-se para 10 bits, e assim sucessivamente, até o máximo de 16 bits.
Periodicamente, o fator de compressão é avaliado, e se está decrescendo,
a tabela de subcadeias é esvaziada, recomeçando a codificação a partir
de 9 bits. Isto faz com que "reduce" se adapte a arquivos longos,
em que o estilo (isto é, o repertório de subcadeias) varia.

.sp
O fator de compressão obtido depende do tamanho do arquivo <original>, o
número de bits utilizado na codificação, e a distribuição das
subcadeias comuns.
Para programas fontes, são típicos fatores de compressão de 50 a 66 \%
(isto é, o arquivo <comprimido> tem da metade à terça parte do tamanho do
<original>). Para módulos objeto, os fatores de compressão são menores,
situando-se em geral na faixa de 30 a 40 \%.

.sp
A redução máxima de tamanho é obtida com a utilização de códigos de 16
bits. Para isto, no entanto, é necessária uma quantidade apreciável de memória
(principalmente durante a compressão).
A quantidade da memória necessária para a tabela de compressão/expansão
é uma função exponencial do tamanho máximo do código -
cada bit a mais exige o dobro de memória.

.(t
Na tabela abaixo damos o tamanho total necessário por "reduce",
para alguns valores de tamanho máximo do código: 

.sp
.ts
center
allbox;
c|c|c.
Bits	Compressão	Expansão
.t&
c|c|c.
 9	 48 Kb	 52 Kb
13	120 Kb	 84 Kb
14	196 Kb	116 Kb
16	656 Kb	308 Kb
.te
.)t

.sp
Repare que se forem gerados códigos de 16 bits, existe a possibilidade de não
ser possível a operação de expansão em um outro computador com menos memória
disponível (veja a opção "-b").

.sp
Normalmente, são adicionadas informações de CRC durante o processo de
compressão, que são verificadas durante a expansão. Isto oferece
uma garantia bem alta de que o arquivo recuperado durante o processo de
expansão é de fato idêntico ao original que foi comprimido (veja a opção "-r").  

.sp
Existe uma possibilidade de o arquivo <comprimido> ficar maior
do que o <original>. Isto ocorre principalmente com arquivos
pequenos (menores do que 256 bytes). Se isto ocorrer, é emitida uma
mensagem de advertência.

.sp
As opções do comando são:

.in +3
.ip -b
Tamanho máximo do código (em bits).
Esta opção deve ser utilizada apenas na compressão
(o arquivo <comprimido> já contém o tamanho máximo a ser utilizado na expansão).
Este valor deve ser dado na faixa de 9 a 16 bits
(se for omitido, será usado um tamanho máximo de 14 bits, que é um bom
compromisso entre o fator de compressão e a memória necessária para arquivos
da 30 Kb ou menores).

.ip -r
Gera o arquivo <comprimido> SEM os CRCs. Com isto,
a velocidade de compressão/expansão fica aproximadamente
.wo '20 \%'
maior e o arquivo <comprimido> fica da ordem de
.wo '1 \%'
menor. Por outro lado, a segurança proporcionada pelos cheques
de CRCs é perdida.  

.ip -f
Normalmente, o usuário é consultado para decidir se um arquivo destino
já existente deve ser reescrito.
A opção "-f" suprime esta consulta, recriando o arquivo.

.ip -v
Estatística.
Imprime os totais de caracteres processados e o fator de compressão obtido.

.ip -u
Operação de expansão.

.ip -t
Imprime a tabela de cadeias utilizada.

.ep

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se ocorreu qualquer tipo de erro, o código do "exit" é 1;
se durante uma operação de compressão o arquivo teve o seu
tamanho aumentado, o código do "exit" é 2;
em caso contrário, o código do "exit" é 0.

.in
.sp
.bc	/*************************************/
.b OBSERVAÇÃO(ÇÕES)
.ec	/*************************************/
.b OBSERVAÇÃO
.in 5
O arquivo <comprimido> contém codificado um indicador de final de arquivo.
Isto significa que ele pode residir também em arquivos especiais (de blocos
ou caracteres), e ser corretamente recuperado.
Aliás, como o arquivo <comprimido> contém normalmente CRCs, comprimir
um arquivo diretamente para um disquete ou fita magnética é um modo
conveniente de armazenamento/transporte de arquivos, como no exemplo abaixo:

.sp
.(t
.nf
		reduce alpha /dev/d5a
		rcat /dev/d5a .....
.fi
.)t
.sp

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
.wo "(fmt): "
f
.br
.ec	/*************************************/

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"A Technique for High Performance Data Compression",
Terry A. Welch, "IEEE Computer,"
vol. 17, no. 6 (June 1984), pp. 8-19.

.bc	/*************************************/
.in
.sp
.b EXEMPLOS
.in 7
.sp 2
e
.ec	/*************************************/

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
