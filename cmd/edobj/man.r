.bp
.he 'EDOBJ (cmd)'TROPIX: Manual de Referência'EDOBJ (cmd)'
.fo 'Atualizado em 15.01.99'Versão 3.1.0'Pag. %'

.b NOME
.in 5
.wo "edobj -"
editor de módulos objeto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
edobj [-f] -p "<busca>[:<substituição>]" <mod>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "edobj" realiza operações simples de busca e substituição
de cadeias em um módulo objeto <mod>. O comando pode também ser utilizado
para realizar estas operações em arquivos de texto, mas para isto
outros editores, tais como "subst", "vi" ou "ed" (cmd)
são provavelmente mais adequados.

.sp
Cada execução de "edobj" realiza a busca de apenas uma cadeia de <busca>
em um módulo objeto <mod>. Para efetuar buscas/substituições de várias
cadeias diferentes ou em vários módulos objeto, são necessárias várias
execuções de "edobj".

.sp
A busca/substituição é definida por uma opção "-p", que sempre deve ser dada
(ver abaixo).
É importante notar que as cadeias de busca NÃO são expressões
tais como as utilizadas no comando "grep" (cmd), mas apenas
cadeias de caracteres que serão procuradas (após processadas as seqüências
de escape) no arquivo dado.

.sp
Cada vez que a cadeia de <busca> é encontrada no arquivo, é impressa uma
linha com a região da cadeia em hexadecimal e em ISO (ASCII), seguida por
uma linha contendo uma indicação da cadeia encontrada.
Se foi dada também a cadeia de <substituição>, é impressa mais
uma linha mostrando a linha modificada, e finalmente o usuário é consultado
para confirmar se modificação deve ser feita no arquivo.  

.sp
As opções do comando são:

.in +3
.ip -p
Par de cadeias de <busca> e <substituição>.
A opção "-p" define a operação a ser feita no módulo objeto.
Se a opção for da forma "-p <busca>" (isto é, sem o delimitador ":"
e sem a cadeia de <substituição>), são listadas todas as linhas
que contém a cadeia <busca> (operação de busca). 

.sp
Se a opção for da forma completa "-p <busca>:<substituição>",
são modificadas todas as linhas que contém a cadeia <busca>,
substituindo-a por <substituição> (operação de substituição).
Neste caso, as duas cadeias devem ter o mesmo tamanho.

.sp
Nas duas cadeias, são permitidas todas as seqüências de escape
da linguagem "C", ou seja, os caracteres especiais (tais como
"\n", "\t", ...), caracteres octais ("\123", ...) e hexadecimais
("\x45", ...).

.ip -f
Normalmente, o usuário é consultado para confirmar a substituição.
Com esta opção, a substituição é realizada sem consultar o usuário.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
Tome cuidado com um possível conflito do sh" (cmd) com a utilização do
caractere de escape "\".
Se forem utilizadas seqüências de escape, o mais seguro é colocar as
cadeias de <busca> e <substituição> entre apóstrofes,
como por exemplo '1234\n:5678\r'.

.sp
Repare que se o caractere delimitador ":" fizer parte de uma das cadeias,
ele deve ser escapado, como por exemplo '1\:23:5678'.

.sp
Para realizar substituições de instruções de máquina deve-se utilizar
as seqüências de escape com caracteres dados em octal ou hexadecimal.
Em versões futuras, possivelmente "edobj" terá uma sintaxe mais
própria para dar seqüências longas de dígitos octais/hexadecimais.

.sp
Em versões futuras, possivelmente "edobj" será capaz de realizar
buscas/substituições de várias cadeias e/ou em vários módulos objeto.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
subst, vi, ed, grep
.br

.in
.sp
.b EXEMPLOS
.in 5
.nf
		edobj -p 'INCS\:\n' alpha.o
.fi
.sp
Mostra todas as regiões onde o módulo "alpha.o" contém a cadeia
de caracteres "INCS:\n".

.sp 2
.nf
		edobj -f -p '\x20\x39:\x10\x39' teste/beta.o
.fi
.sp
Substitui (sem consultar o usuário) todas as ocorrências da seqüência
"0x2039" (um "movl" do processador MC-68010) em
"0x1039" (um "movb") do módulo "teste/beta.o".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
