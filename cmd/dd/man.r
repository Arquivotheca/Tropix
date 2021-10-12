.bp 1
.he 'DD (cmd)'TROPIX: Manual de Referência'DD (cmd)'
.fo 'escrito em 14.10.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
dd - copia e converte arquivos

.in
.sp
.b SINTAXE
.in 5
dd [-v] [<argumento>=<valor>] ...

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "dd" copia o arquivo de entrada
para a saída especificada, efetuando
possíveis conversões.
Por omissão, são utilizadas a entrada/saída padrão.

.sp
Se o arquivo de saída for regular, e NÃO for dado o argumento
"seek", ele será truncado antes da cópia.

.sp
Podemos especificar os tamanhos dos blocos de entrada e de saída
para blocar/desblocar arquivos e/ou
aproveitar as vantagens das operações de
entrada e saída em modo físico não-estruturado ("raw").

.sp
Este comando, ao contrário do usual, utiliza uma sintaxe com
palavras chaves, onde são dados grupos, contendo cada um
o nome de um argumento, seguido do caracter "=" e do seu respectivo valor.
Estes grupos podem vir em qualquer ordem.

.sp
As opções do comando são:

.in +3
.ip -v
Verboso: escreve uma estatística ao final da execução,
contendo o número de blocos completamente/parcialmente lidos/escritos.

.ep
.in -3

.sp
Os argumentos são as seguintes:

.in +3
.ip 'if=<arq>' 15
Nome do arquivo de entrada (por omissão, a entrada padrão).

.ip 'of=<arq>' 15
Nome do arquivo de saída (por omissão, a saída padrão).

.ip 'ibs=<n>' 15
Tamanho do bloco de entrada (por omissão 4096).

.ip 'obs=<n>' 15
Tamanho do bloco de saída (por omissão 4096).

.ip 'bs=<n>' 15
Tamanho do bloco de entrada e de saída;
este argumento tem prioridade sobre "ibs" e "obs",
e é útil especialmente quando não for especificada nenhuma
conversão, pois não serão feitas cópias entra áreas da memória.

.ip 'cbs=<n>' 15
Tamanho da área de conversão.

.ip 'skip=<n>' 15
Pule os <n> blocos iniciais do arquivo de entrada,
antes de iniciar a cópia.

.ip 'seek=<n>' 15
Inicie a cópia, no arquivo de saída, a partir do bloco <n>
(numeração começando a partir de 0).

.ip 'count=<n>' 15
Copie apenas <n> blocos da entrada.

.ip 'conv=ascii' 15
Converta o código EBCDIC para ASCII.

.in +5
.ip 'ebcdic' 10
Converta o código ASCII para EBCDIC.

.ip 'ibm' 10
Conversão, um pouco diferente, de ASCII para EBCDIC.

.ip 'lcase' 10
Converta letras maiúsculas em minúsculas.

.ip 'ucase' 10
Converta letras minúsculas em maiúsculas.

.ip 'swab0' 10
Inverta bytes consecutivos
(converte "abcd" em "badc").

.ip 'swab1' 10
Inverta pares de bytes consecutivos
(converte "abcd" em "cdab").

.ip 'swab2' 10
Realize, para pares de bytes consecutivos, swab0 e swab1 simultaneamente
(converte "abcd" em "dcba").

.ip 'noerror' 10
Não interrompa o programa se ocorrer algum erro.

.ip 'sync' 10
Complete todo bloco de entrada para o tamanho "ibs".

.ip '..., ...' 10
Várias opções de conversão podem ser separadas por vírgula.

.in -5
.in -3
.ep

.sp 2
Quando for especificado um número (valor <n>), normalmente espera-se
um valor decimal; no entanto podemos usar
hexadecimal (se o número começar com "0x" ou "0X"),
ou octal (se o número começar com "0").
Além disto, o número pode terminar com
"c" ou "C" para especificar multiplicação por 2048,
"k" ou "K" para especificar multiplicação por 1024,
"b" ou "B" para especificar multiplicação por 512,
"l" ou "L" para especificar multiplicação por 4 e
"w" ou "W" para especificar multiplicação por 2.
Lembre-se no entanto, que "b" e "B" são também algarismos hexadecimais!

.sp
O argumento "cbs=<n>" (área de conversão) é usado apenas se for especificada
uma conversão "ascii", "ebcdic", ou "ibm".
No primeiro caso, são colocados "n" bytes na área de conversão
(convertidos para ASCII),
os brancos mais à direita são suprimidos, e um caractere <nl> é adicionado
antes de escrever o bloco na saída.
Nos outros dois casos, o procedimento é o inverso:
é lida uma linha do arquivo de entrada ASCII
(delimitada por um <nl>) na área de conversão
(convertida para EBCDIC), e a área é completada com brancos.

.sp
Ao final, escreve uma estatística (veja a opção "-v", acima).

.in
.sp
.b
OBSERVAÇÃO
.r
.in 5
A conversão "ibm" é uma variante da conversão ASCII para EBCDIC,
que apesar de não corresponder exatamente ao padrão, adapta-se
melhor aos casos usuais.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
(cmd): cp, tr

.in
.sp
.b EXEMPLOS
.in 7
.sp
dd if=arq1 of=arq2 conv=ucase
.sp
.in +3
Copia "arq1" para "arq2" convertendo as letras minúsculas para maiúsculas.
.sp 2

.in -3
dd if=/dev/rmt0 of=arq ibs=800 cbs=80 conv=ascii,swab0
.sp
.in +3
A unidade de fita contém um arquivo,
cujos registros lógicos de 80 bytes em EBCDIC estão blocados em 800 bytes.
Este arquivo será lido, os bytes consecutivos serão invertidos
e convertidos para ASCII, os brancos no final de cada registro de 80 bytes
serão eliminados, e finalmente serão acrescentados <nl>.
O arquivo convertido é escrito em "arq".
.in -3

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
