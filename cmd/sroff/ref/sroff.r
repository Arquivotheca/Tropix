.bp
.tp
.sp 10
.nf
      *******   *******  *******   *******    *    *     *
         *      *     *  *     *   *     *    *     *   *
         *      *     *  *     *   *     *    *      * *
         *      *******  *     *   *******    *       *
         *      *  *     *     *   *          *      * *
         *      *   *    *     *   *          *     *   *
         *      *    *   *******   *          *    *     *
.fi
.sp 10
.ce
SROFF
.sp 2
.ce
Versão 4.7.0
.sp 4
.ce
Manual de Referência
.sp 12
.ce
Julho de 1997

.bp
.sp 10
.ce
CONTEÚDO
.sp 5
.nf
	Cap. 1 - Introdução ..............................  2

	Cap. 2 - Definições Preliminares .................  3

	Cap. 3 - Controle de Página ....................... 6

	Cap. 4 - Controle de Linha ........................ 7

	Cap. 5 - Controle de Parágrafo .................... 9

	Cap. 6 - Controle de Blocos ...................... 10

	Cap. 7 - Controle do Tipo do Caractere ........... 12

	Cap. 8 - Formatação de Tabelas ................... 13
.fi

.bp
.he 'Sistema TROPIX'Manual de Referência'SROFF'
.fo 'Escrito em 07.07.97'Versão 4.7.0'Pag. %'
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 1			     *
     *							     *
     *	     		INTRODUÇÃO			     *
     *							     *
     *********************************************************
.fi
.sp 4
.pp
O SROFF é um processador de texto não interativo
baseado num subconjunto das especificações do NROFF
do UNIX da AT&T. O SROFF é não interativo porque
o texto formatado não aparece imediatamente na tela
do terminal após ser datilografado no teclado. O
texto de entrada é acompanhado de comandos para
o SROFF e gravado em um arquivo. O programa SROFF
lê este arquivo, interpreta os comandos e produz o
arquivo de saída com o texto formatado a partir
do texto de entrada.

.pp
O SROFF baseia-se num subconjunto das especificações
do NROFF e utiliza algumas características das macros
ME desenvolvidas na universidade de Berkeley, E.U.A..

.pp
Este manual apresenta os diversos comandos aceitos
pelo SROFF com as suas respectivas opções.
Alguns exemplos são dados para facilitar o
entendimento dos comandos.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 2			     *
     *							     *
     *		   DEFINIÇÕES PRELIMINARES		     *
     *							     *
     *********************************************************
.fi
.sp 4

.ip "2.1 -" 6
Identificação dos comandos e seus parâmetros.

.pp
Um comando para o SROFF é indentificado por um ponto (".")
na primeira coluna de uma linha de entrada. O comando
é indentificado por dois caracteres que seguem o ponto.
Os parâmetros do comando seguem a indentificação do
comando separados por brancos.
.sp
.ce
Ex:  .ip  "TÍTULO" 15
.sp

.ip "2.2 -" 6
Convenções.

.pp
Diversos comandos alteram características básicas do SROFF
(tamanho da linha de texto, por exemplo). Caso este comando
não seja especificado, o valor entre colchetes ("[", "]") é
assumido.

.pp
Em relação ao espacejamento, usa-se a convenção "v" para
um espacejamento vertical e "h" para um espacejamento 
horizontal.

.pp
O parâmetro "n" que aparece em muitos comandos é numérico
e pode ser especificado sem sinal ou com sinal "+" ou "-".
Se especificado sem sinal, representa o valor absoluto.
Com o sinal "+" ou "-", representa alterações relativas
ao valor corrente. O valor assumido para "n", caso ele seja
omitido, é dado entre colchetes.

.pp
Quanto à disposição do texto formatado, imagina-se que
cópias do texto em tamanho A4 serão tiradas usando-se apenas
uma das faces do papel. Versões futuras do SROFF permitirão
formatações mais sofisticadas.

.pp
Todos os comandos descritos causam uma quebra na linhas corrente,
recomeçando a formatação em uma nova linha. As exceções são o
.b .wo
e
.b .ul
(veja Controle de Linha), e os comandos relativos ao tipo do caractere
(veja Controle do Tipo do Caracter).

.bp
.ip "2.3 -" 6
Padronização vertical.

.pp
Os seguintes comandos especificam o espacejamento vertical
de uma página de texto típica.
.sp
.in 5

.b .pl
n (page-length) [75v]
 Especifica o tamanho total da página em linhas de texto
formatado.
.sp 2

.b .m1
n (margin 1) [4v]
 Especifica o espacejamento entre o topo da página e o
header (se houver) ou a primeira linha de texto.
.sp 2

.b .m2
n (margin 2) [2v]
 Especifica o espacejamento entre o header e a 
primeira linha de texto.
Se não houver header, este espacejamento é ignorado.
.sp 2

.b .m3
n (margin 3) [2v]
 Especifica o espacejamento entre a última linha 
de texto e o footer.
Se não houver footer, este espacejamento é ignorado.
.sp 2

.b .m4
n (margin 4) [1v]
 Especifica o espacejamento entre o footer (se houver)
ou a última linha de texto e o final da página.
.sp 2

.b .ls
n (line space) [1]
 Especifica o espacejamento entre duas linhas consecutivas no texto.
Se "n" é dois, produz-se um texto com espacejamento duplo.

.in
.sp
.ip "Obs: "
Os comandos 
.b ".pl, .m1, .m2, .m3"
e
.b .m4
quando usados no meio de uma página, causam uma quebra na mesma.
.sp
Uma página típica com header e footer terá:

.ip 
.(l
4  linhas em branco
1  linha  de header
2  linhas em branco
55 linhas de texto
2  linhas em branco
1  linha  de footer
1  linha  em branco
.)l
.sp

.bp
.ip "2.4 -" 6
Padronizacão horizontal.

.pp
Os comandos seguintes especificam o espacejamento
horizontal de uma linha típica.
.sp
.in 5

.b .ll
n (line length) [67h]
 Especifica o comprimento da linha de texto em caracteres.
.sp 2

.b .po
n (page offset) [0h]
 Especifica a margem a esquerda em número de caracteres
a partir da qual devem começar todas as linhas do texto.
.sp 2

.b .in
n (indentation) [0h]
 Especifica o número de caracteres a partir da margem esquerda
corrente que se deve deixar em branco quando uma nova linha
for impressa.
.in
.sp

.ip "Obs: "
Sem alterar os valores iniciais dos diversos parâmetros,
o SROFF permite a impressão de linhas que deixam 2,5cm
de margem a esquerda e 1,5cm de margem a direita em uma
página de tamanho A4.
.sp

.ip "2.5 -" 6
Header e footer.

.pp
Especifica o que deverá ser impresso a esquerda, no centro
e a direita no alto da página (header) e no pé da página
(footer). Pode-se especificar também se só se quer nas páginas
pares (".eh", ".ef") ou ímpares (".oh", ".of"). Ainda pode-se ter um
header (ou footer) para páginas pares, e outro para as ímpares.
.sp

.in 5
.b .he
'esquerda'centro'direita'
.sp
.b .eh
'esquerda'centro'direita'
.sp
.b .oh
'esquerda'centro'direita'
.sp
.b .fo
'esquerda'centro'direita'
.sp
.b .ef
'esquerda'centro'direita'
.sp
.b .of
'esquerda'centro'direita'
.sp
.in

.pp
Para que o header e/ou o footer sejam impressos em negrito,
basta que sejam definidos depois de um 
.b .b.
E para que sejam sublinhados basta que venham depois de um 
.b .ul.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 3			     *
     *							     *
     *		     CONTROLE DE PÁGINA			     *
     *							     *
     *********************************************************
.fi
.sp 4
.in 5

.b .bp
n (begin page) [+1]
 Especifica o término da página corrente e o início
de uma nova. O parâmetro "n" indica o valor do
número para ser usado na nova página.
.sp
.in

.ip "Obs: "
O número da página corrente é representado no texto por "\%".
Caso este caractere precise ser usado especifique "\\%".
.lp
.sp 2
.in 5

.b .pn
n (page number) [1]
 Especifica o número da página corrente.
.sp 2

.b .tp
(begin title page)
 Especifica o término da página corrente e o início de uma 
nova que não conterá header nem footer e não incrementará
o número da página.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 4			     *
     *							     *
     *		     CONTROLE DE LINHA			     *
     *							     *
     *********************************************************
.fi
.sp 4
.in 5

.b .br
(break)
 Produz uma quebra na linha corrente, isto é, esta é impressa
sem haver ajustamento da margem direita. Uma linha de texto
de entrada começando com um caractere branco, faz o mesmo efeito.
.sp 2

.b .wo
(word) 'palavra'
 Imprime literalmente o conjunto de letras (incluindo brancos) passado
como parâmetro entre
aspas duplas (") ou simples ('). Usado quando o número de brancos
entre palavras não pode ser alterado.
.sp 2

.b .ce
(centered) [1]
 As "n" linhas seguintes de texto de entrada são centradas.
Linhas de comando, em branco, de listas, literais ou linhas de tabelas
não são consideradas. Caso seja usado 0 como parâmetro, pára de centrar
antes das "n" linhas estabelecidas no comando
.b .ce
anterior.

.sp 2
.b .tl
'esquerda'centro'direita'
 O texto esquerda, centro, direita são escritos a esquerda,
no centro e a direita da linha como no header e footer.
Pode ser em negrito e/ou sublinhado da mesma forma que o header e o footer.
Bom para fazer índices.

.sp 2
.b .sp
n (space) [1]
 Especifica um "break" na linha corrente e um espacejamento
vertical de "n" linhas.
"n" deve ser positivo e diferente de 0.
.sp 2

.b .ul
n (underline) [1]
 Sublinha no texto formatado as próximas "n" linhas do texto
de entrada.
.sp 2

.b .nf
(no fill)
 Começa um trecho de texto sem ajustamento. As linhas do
texto de entrada são literalmente transferidas para
o texto formatado.
.sp 2

.b .li
(literal)
 O mesmo que ".nf".
.sp 2

.b .fi
(fill)
 Retornar ao modo de ajustamento normal.
.sp 2

.b .sh
"n" título x (section header)
 Faz a numeração automática dos capítulos. Nenhum parâmetro apenas
zera a numeração (nada é impresso). "n" indica o nível da numeração
(para 1.2.2 o nível seria 3). "título" indica o que vai ser colocado
ao lado do número (1.2.3 título).
Se "título" tiver <sp> ou <tb> deve vir entre '"' ou '''.
"x" é uma numeração específica. Deve ter o mesmo número de
dígitos que o nível indicado e os dígitos devem vir separados
por <sp>. Caso não haja o parâmetro "x", a numeração colocada
é a seguinte à anterior. O nível pode variar de 1 a 3.
.sp
Exemplo:
.sp
.in +3
 .sh 1 tit1 gerará
.sh 1 tit1
.sp

 .sh 2 tit1.1 gerará
.sh 2 tit1.1
.sp

 .sh 1 tit2 gerará
.sh 1 tit2
.sp

 .sh 3 tit2.1.1 gerará
.sh 3 tit2.1.1
.in -3
.sp 2


.b .nr
n [5]
 Indenta todos os títulos de capítulos em um valor
proporcional a "n", ou seja cada título será indentado
em "n" vezes o nível dado no comando ".sh" menos um.
.in

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 5			     *
     *							     *
     *		    CONTROLE DE PARÁGRAFO		     *
     *							     *
     *********************************************************
.fi
.sp 4
.in 5
.b .pp
(paragraph)
 Parágrafo padrão. Produz uma quebra na linha corrente, pula
um espaço vertical e indenta em cinco caracteres a próxima linha do texto
formatado a partir da margem esquerda.
.sp 2

.b .lp
(left paragraph)
 Idêntico a ".pp" sem a indentação de cinco caracteres.
.sp 2

.b .ip
título "n" (indented paragraph)
 Produz uma quebra na linha corrente, pula um espaço vertical,
coloca a palavra título na margem esquerda corrente e indenta
todas as linhas deste parágrafo. Se o título contiver brancos,
usar "título" ou 'título'. A indentação produzida é a padrão
de cinco caracteres. O parâmetro "n" indica a alteração deste
valor com validade para este parágrafo.
.sp 2

.b .ep
(end paragraph)
 Termina um parágrafo.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 6			     *
     *							     *
     *		     CONTROLE DE BLOCOS			     *
     *							     *
     *********************************************************
.fi
.sp 4

.in 5

.b .bc
 texto
 
.b .ec
 Usado para comentários. O texto será ignorado.
.sp 2

.b .(q
.wo "     (quote)"
 texto
 
.b .)q
 Usado para citações. Indenta o texto a esquerda e a direita
das margens correntes em cinco caracteres.
.sp 2

.b .(l
.wo "     (lista)"
 texto
 
.b .)l
 Usado para listas. Indenta o texto a esquerda em cinco caracteres
e transfere literalmente cada linha do texto de entrada para
o texto formatado.
.sp 2
.b .(b
f
.wo "   (block)"
 texto
 
.b .)b
 Usado para manter o texto em uma mesma página. Se o parâmetro "f"
não for especificado, o bloco é impresso na posição onde ele 
aparece no texto se couber na página corrente. Se não couber,
ele é impresso no início da página seguinte e a página corrente
é deixada com um espaço em branco.
 Se o parãmetro "f" for especificado, o bloco flutua dentro do texto.
Isto significa que o texto seguinte ao bloco continua a ser formatado
até chegar á posição de impressão do referido bloco. Esta posição
será o fim da página, se o bloco couber nela, ou o começo da próxima,
caso contrário.

.bp
.b .(t
.wo "     (text)"
 texto
 
.b .)t
 O mesmo que o bloco fixo, mantendo, entretanto, as condições
do sistema. Além disso, qualquer alteração feita dentro do
bloco permanece após o fim do mesmo.

.sp 2
.b .(f
.wo "     (footnote)"
 texto
 
.b .)f
 Usado para especificar uma nota de rodapé. O texto termina antes,
pula-se um espaço vertical definido em ".ls" e o texto da nota de
rodapé é impresso. Caso o texto da nota não possa ser impresso
inteiramente na página corrente, ele continuará na página seguinte.
Caso nem uma linha possa ser impressa na nota de rodapé, a
página é terminada antes e a nota de rodapé é impressa na 
página seguinte.

.in
.ip 'Obs. : ' 9
No caso de blocos (ou footnote) com tamanho acima do tamanho
da página, estes serão divididos, e impressos em blocos menores, como
se fossem todos blocos fixos.
.lp

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 7			     *
     *							     *
     *		CONTROLE DO TIPO DO CARACTERE		     *
     *							     *
     *********************************************************
.fi
.sp 4


.in 5
.b .r
(roman)
 Muda o tipo dos caracteres para romano.
É o tipo de caractere padrão.
.sp 2
.b .i
(italic)
 Muda o tipo dos caracteres para itálico.
.sp 2
.b .b
(bold)
 Muda o tipo dos caracteres para negrito.
.sp
.in
.ip "Obs: "
Se uma palavra é passada como parâmetro com um dos três
comandos acima, somente ela terá seus caracteres alterados.
.lp


.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 8			     *
     *							     *
     *		   FORMATAÇÃO DE TABELAS		     *
     *							     *
     *********************************************************
.fi
.sp 4

.ip "9.1 -" 6
Definição.

.in 5
.b .ts
 [opções;]
 formatos.
 dados
.br
.b .t&
 formatos.
 dados
.br
.b .te
.in 0

.pp
Tabelas são formadas de campos que são independentemente centrados,
ajustados a esquerda, a direita ou pelo ponto decimal.
Títulos podem ser colocados sobre uma coluna ou sobre um grupo de colunas.
.pp
Cada tabela em um texto é independente e deve conter informações
relativas á sua formatação. Estas informações são especificadas na
seção 'formatos' que vem entre as 'opções' (informações relativas à
tabela como um todo) e os 'dados' (elementos que comporão os campos
da tabela).
.pp
O comando '.t&' indica uma redefinição dos formatos das linhas da tabela.
Não é permitido alterar as 'opções'. Deve ser considerado o fato
de que algumas condições estabelecidas ao longo da leitura dos dados
anteriores ao '.t&' não serão desprezados. São elas:
.in 5
.ip - 5
Largura mínima de uma coluna.
.ip - 5
Largura mínima dos campos esquerdo e direito, além da existência do
ponto decimal, para os campos numéricos de cada coluna.
.ip - 5
O tamanho máximo dos campos pertencentes a uma subcoluna para
cada coluna.
.ip - 5
Colunas com indicação para o mesmo tamanho.
.in 0

.pp
Os tamanhos indicados acima somente serão aumentados,
reduções indicadas para os mesmos serâo ignoradas.
Quanto às colunas que devem ter o mesmo tamanho, elas
não perderão esta condição, entretanto colunas que não possuiam
este atributo podem passar a possuir.

.ip "9.2 -" 6
Opções.
.pp
As opções alteram a formatação da tabela como um todo.
Elas, se presentes, vêm seguindo o comando '.ts'.
São colocadas uma em cada linha e a última deve ser terminada por ';'.
.pp
São elas:
.in 5
.ip - 5
center: centra a tabela (o default é ajustar a esquerda).
.ip - 5
expand: formata a tabela no tamanho da linha (o default é o mínimo necessário).
.ip - 5
box: coloca a tabela em uma moldura.
.ip - 5
allbox: coloca todos os elementos da tabela em molduras.
.ip - 5
doublebox: coloca a tabela em uma moldura dupla.
.ip - 5
tab (c): 'c' indica o caractere separador dos dados (o default é <tb>).
.ip - 5
title n: a n-ésima linha de dados será usada como título
no início de cada página onde houver continuação da tabela.
.in

.ip "9.3 -" 6
Formatos.
.pp
A seção 'formatos' especifica exatamente a colocação dos elementos
na tabela. Cada linha desta seção corresponde a uma linha da tabela.
Exceto a última, que corresponde a todas as linhas seguintes
até o próximo '.t&' ou '.te'.
Cada linha contém uma letra chave para cada coluna.
O número de letras chave na primeira linha de formatos
indicará o número de colunas da tabela. Nas seguintes,
formatos a mais serão ignorados e a menos serão subentendidos
como 'l'.
.pp
As letras chave são as seguintes:
.in 5
.ip "l - "
Indica campo alinhado a esquerda.
.ip "r - "
Indica campo alinhado a direita.
.ip "c - "
Indica campo centrado.
.ip "n - "
Indica campo numérico, a ser alinhado com outros
campos numéricos (na mesma coluna) pelo ponto decimal.
.ip "a - "
Indica uma subcoluna. Todos os campos correspondentes são alinhados
a esquerda de forma que o maior campo correspondente esteja centrado
na coluna.
.ip "s - "
Indica que o campo da coluna anterior se extende até esta coluna
(não é permitido na primeira coluna).
.ip "^ - "
Indica campo em branco. Dados correspondentes a este formato
são ignorados.
.ip "'-' ou '=' - "
A coluna será totalmente preenchida pelo caractere '-' ou '=' respectivamente.
Dados correspondentes a esses formatos sâo ignorados.
.ip "| - "
Deve vir entre duas letras chave, indicando aí uma
(ou mais, dependendo do número de barras) coluna vertical.
.in

.pp
Quando o formato "n" é especificado, o '.' mais a direita do dado
é usado como ponto decimal. Se não houver '.', o caractere mais a 
direita é considerado como unidade.
.pp
A última linha de formatos deve vir seguida de um '.'.
.pp
Os seguintes atributos podem ser utilizados seguindo as letras chave,
visando mudar características das colunas:
.in 5
.ip "f - "
Deve vir seguido de 'b', 'r', ou 'i', para mudar o tipo do caractere
do campo correspondente para negrito, romano ou itálico respectivamente.
O default é o tipo corrente.
O 'f' pode ser omitido, pois o sroff aceita como mesmo atributo
apenas o 'b', o 'r' ou o 'i'.
.ip "e - "
Indica colunas com mesmo tamanho. São ajustadas pelo maior tamanho.
.ip "w(n) - " 
O número "n" (decimal) indica o tamanho mínimo da coluna correspondente.
O default é o tamanho do maior campo desta coluna.
.ip "n - "
O número "n" (decimal) indica a distância mínima entre esta coluna
e a próxima. O default é 3. A opção 'expand', expande a tabela
multiplicando as distâncias por uma constante de forma que
ocupe o máximo da linha.
.in 0

.ip "9.4 -" 6
Dados.
.pp
Os 'dados' da tabela vêm após os formatos. Cada linha da tabela
vem em uma linha diferente, com os campos separados por <tb> ou
outro caractere definido pela opção 'tab'.
.pp
Uma linha de dado pode conter:
.in 5
.ip "- "
Comando para o sroff.
.ip "- "
'-' ou '=': Indica uma linha inteira com o respectivo caractere.
.ip "- "
Campos separados por <tb>.
.in 0

.pp
Um campo pode ser:
.in 5
.ip "- "
'-' ou '=': Indica toda a coluna com o respectivo caractere. Para que
um deles seja o dado, usar "\&-" ou "\&=", ou colocar um <sp>
entre o caractere e o <tb>.
.ip "- "
'\a': todo o campo com o respectivo caractere.
.ip "- "
'\Rx': 'x' indica o caractere que ocupará toda a extensão do campo.
.ip "- "
'\^': indica campo em branco, todo ocupado por <sp>.
.ip "- "
's': 's' indica a cadeia de caracteres a ser colocada na tabela
de acordo com o formato correspondente.
.ip "- "
campo em branco (dois <tb> seguidos): indica campo em branco,
todo ocupado por <sp>.
.in 0

.pp
Deve ser levado em consideração que comandos para o sroff indicados
dentro da tabela (na seção de dados) terão seu efeito anterior
ou posterior à colocação da tabela no texto, uma vez que
a tabela (dados) é toda armazenada na memória antes de ser montada
e colocada no texto. Enquanto os comandos do sroff são executados
imediatamente após sua leitura.
.pp
Tabelas que estejam dentro de bloco não terão suas
"caixas" completadas nas extremidades das páginas.
Além disso o comando "title" não funciona para estas tabelas.

.ip "9.5 -" 6
Exemplos.

.(b
.nf

     .ts
     box
     tab (:);
     cb s s s s
     c  c c c c
     r2 | n2 | n2 | n2 | n.
     título
     =
     tit1:tit12:tit123:tit1234:tit12345
     -
     tit12:9.2:5.2:55:6.4
     tit123:4.34:666:7.65:5:43
     tit1234:6.43:-43:65.43:-34.65
     tit12345:23:5677:-7676:123.
     .te
.fi
.)b


.sp 2
.in 5
.ts
box
tab (:);
cb s s s s
c  c c c c
r2 | n2 | n2 | n2 | n.
título
=
tit1:tit12:tit123:tit1234:tit12345
-
tit12:9.2:5.2:55:6.4
tit123:4.34:666:7.65:5:43
tit1234:6.43:-43:65.43:-34.65
tit12345:23:5677:-7676:123.
.te
