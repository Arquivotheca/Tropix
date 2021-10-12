.bp 1
.he 'XD (cmd)'TROPIX: Manual de Referência'XD (cmd)'
.fo 'escrito em 02.04.02 'Versão 4.2.0'Pag. %'

.b NOME
.in 5
xd - realiza um "dump" em hexadecimal e ISO (ASCII)

.in
.sp
.b SINTAXE
.in 5
xd [-e] [-s <start>] [-a <ender>] [<arquivo>]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "xd" realiza um "dump" em hexadecimal e em ISO (ASCII)
de <arquivo>. Se <arquivo> não for dado, será feito o "dump" da entrada padrão.

.sp
A listagem produzida contém 3 colunas: no lado esquerdo o endereço
inicial da linha (em hexadecimal), no meio o conteúdo (também em hexadecimal),
e do lado direito o mesmo conteúdo em ISO.
Se um caractere não tiver representação gráfica, ele é substituído
por um ponto (".") na coluna da direita.

.sp
Normalmente, o "dump" é feito do início até o final do arquivo.

.sp
O comando possui as seguintes opções:

.in +3
.ip -e 12
Esta opção deve ser usada se se tratar de um arquivo no código EBCDIC.
Os caracteres são convertidos de EBCDIC para ISO (apenas) na coluna da direita.

.ip "-s <start>" 12
Pode ser dado para iniciar o "dump" a partir de <start>.
Este número é interpretado normalmente em decimal; no entanto podemos usar
hexadecimal (se o número começar com "0x" ou "0X"),
ou octal (se o número começar com "0").

.sp
Além disto, o número pode terminar com
"c" ou "C" para especificar multiplicação por 2048,
"k" ou "K" para especificar multiplicação por 1024,
"b" ou "B" para especificar multiplicação por 512,
"l" ou "L" para especificar multiplicação por 4 e
"w" ou "W" para especificar multiplicação por 2.

.sp
Lembre-se no entanto, que "b", "B", "c" e "C" são também algarismos hexadecimais!

.ip "-a <ender>" 12
Normalmente, a coluna de endereços começa no mesmo endereço em que foi
pedido o início do "dump".
Com a opção "-a", a coluna de endereços começa de <ender>. 
Este número é interpretado da mesma forma do que <start> (ver acima). 

.ep
.in -3

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
