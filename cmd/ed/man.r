.bp 1
.he 'ED (cmd)'TROPIX: Manual de Referência'ED (cmd)'
.fo 'Atualizado em 27.04.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
ed - editor de textos orientado por linhas

.in
.sp
.b SINTAXE
.in 5
ed [nome]

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "ed" é um editor de textos orientado por linhas.

.br
.sp
Se o parâmetro "nome" estiver presente, "ed" lê o arquivo correspondente
simulando um comando "e" (edit), descrito abaixo.

.br
.sp
O editor "ed" opera em uma cópia do arquivo em edição.
Quaisquer alterações realizadas na cópia
não afetarão o arquivo até
que um comando "w" (write) seja executado.
Chamaremos a cópia do arquivo em edição de
.b
"buffer".
.r

.br
.sp
Os comandos de "ed" têm uma estrutura simples e regular:
zero, um ou dois endereços seguidos de um comando com apenas um caractere,
possivelmente acompanhado de argumentos e sufixos.
Os endereços especificam as linhas no "buffer" atingidas pelo comando.
Na falta de endereços são assumidos endereços pré-estabelecidos.

.br
.sp
.b Endereçamento:
.in 5
.sp
O endereçamento de linhas no "buffer"
baseia-se no conceito de linha corrente.
De modo geral a linha corrente é a última linha afetada
por um comando; entretanto o efeito exato na linha corrente
é explicado na discussão dos comandos.
Endereços podem ser construidos do seguinte modo:
.br

.in 10
.ip 1. 4
O caractere "." endereça a linha corrente.

.ip 2. 4
O caractere "$" endereça a última linha no "buffer".

.ip 3. 4
Um número decimal "n" endereça a linha de ordem "n'ésima" no "buffer".

.ip 4. 4
A sequência "'m" endereça a linha marcada com o nome "m",
que deve ser uma letra minúscula.
Linhas são marcadas com o comando "k" (mark), que será descrito adiante.

.ip 5. 4
Uma expressão regular entre barras ("/expressão/")
endereça a linha encontrada buscando-se avante após a linha corrente
ou após o endereço especificado até então
e parando na primeira linha que contem uma cadeia
que corresponda à expressão regular.
Se necessário a busca dá a volta, passando pelo fim do "buffer".

.ip 6. 4
Uma expressão regular entre interrogações ("?expressão?")
endereça a linha encontrada buscando-se retroativamente
antes da linha corrente
ou do endereço especificado até então
e parando na primeira linha que contem uma cadeia
que corresponda à expressao regular.
Se necessário a busca dá a volta, passando pelo início do "buffer".

.ip 7. 4
Um endereço seguido de um "+" (sinal de soma)
ou de um "-" (sinal de subtração)
seguido de um número decimal especifica um endereço mais ou menos
o número indicado de linhas.

.ip 8. 4
Se um endereço começa com "+" ou com  "-" a adição ou subtração
é tomada como em relação a linha corrente, isto é,
"-5" equivale à ".-5".

.ip 9. 4
Se um endereço termina com "+" (ou "-"), então um é somado (ou subtraido).
Como consequência desta regra e da regra 8,
o endereço "-" se refere à linha anterior a linha corrente.
Mais ainda, "-" e "+" no final de um endereço têm efeito cumulativo,
assim "--" se refere a linha corrente menos dois.
.lp

.br
.in 5
.sp
.b 'Expressões Regulares:'

.br
.in 5
.sp
O editor "ed" suporta uma forma limitada de expressões regulares.
Uma expressão regular especifica um conjunto de cadeias de caracteres.
Dizemos que um membro deste conjunto de cadeias
corresponde à expressão regular.
Na definição de expressão regular a seguir a palavra "caractere"
significa qualquer caractere à exceção de <nl>:

.in 10
.br
.ip 1. 4
Qualquer caractere exceto um caractere especial corresponde a si próprio.
Caracteres especiais são o delimitador da expressão regular
e mais os caracteres "\", "[", "." e em alguns casos "^", "*" e "$".

.ip 2. 4
Um "\" (barra invertida) seguido de qualquer caractere corresponde
ao próprio caractere.

.ip 3. 4
Um "." (ponto) corresponde a qualquer caractere.

.ip 4. 4
Uma cadeia não vazia "s" entre colchetes "[s]" (ou "[^s]")
corresponde a qualquer caractere presente (ou ausente) "s".
Uma sub-cadeia "a-b" dentro de "s",
com "a" e "b" em ordem crescente ISO,
significa o conjunto de caracteres ISO entre "a" e "b", inclusive.
Os caracteres "]", "-" e "\" podem ser especificados precedendo-os com "\".

.ip 5. 4
Uma expressão regular da forma 1 até 4 seguido de "*"
corresponde a zero ou mais ocorrências da expressão regular.

.ip 6. 4
Uma expressão regular "x" da forma 1 até 6, seguida
de uma expressão regular "y" da forma 1 até 5
corresponde a uma ocorrência de "x" seguida de uma ocorrência de "y"
com a correspondência para "x" sendo a mais longa
que permita a correspondência para "y".

.ip 7. 4
Uma expressão regular da forma 1 até 6 precedida por "^"
(ou seguida de "$"),
é restrita a correspondências no início (ou no fim)
de uma linha.

.ip 8. 4
Uma expressão regular da forma 1 até 7 corresponde
á mais longa das correspondências mais á esquerda da linha.

.ip 9. 4
Uma expressão regular vazia
equivale a cópia da última expressão regular procurada.
.lp
.in 5

Expressões regulares são usadas em endereços para especificar linhas
e no comando "s" (substitute), descrito abaixo,
para especificar a porção da linha que será substituida.
.br


.in 5
.br
.sp
.b 'Modo Entrada de Texto:'
.br

.in 5
.sp
Alguns comandos permitem a adição de texto novo no "buffer".
Enquanto "ed" está recebendo texto,
dizemos que ele está em modo de entrada de texto.
Neste modo, nenhum comando é reconhecido;
toda entrada é simplesmente acrescentada ao "buffer".
O modo de entrada de texto termina  quando entramos
uma linha contendo apenas "." (ponto).
Apos a execução destes comandos
a linha corrente será a última linha no texto entrado
ou, se este for nulo, a linha endereçada para o comando.
.br

.in 5
.br
.sp
.b Comandos:
.br

.in 5
.sp
Os comandos de "ed" podem requerer zero, um ou dois endereços.
Quando são fornecidos endereços a menos,
"ed" assume endereços pré-estabelecidos.
É um erro dar mais endereços do que o comando espera.
.br

.sp
Se não for especificado nenhum comando
após o endereçamento
o comando será "p" (print).
Uma linha em branco faz da linha seguinte à linha corrente
a nova linha corrente e a mostra.
.br

.sp
Endereços são tipicamente separados por uma vírgula (",").
Quando os endereços são separados por um ponto e vírgula (";"),
o segundo endereço é calculado fazendo-se antes
o primeiro endereço como linha corrente.
Esta característica pode ser usada para determinar 
a linha incial para buscas para frente e para trás ("/" e "?").
O segundo endereço de toda sequência de dois endereços
tem que corresponder a uma linha de ordem superior
a linha correspondente ao primeiro endereço.
.br

.sp
Na lista a seguir de comandos do "ed",
os valores pré-estabelecidos para os endereços
são mostrados entre parênteses.
.br
.sp
.(b
.in 5
(.) a
.br
<texto>
.br
 .
.br

.in 10
O comando 
"a" (append)
lê o texto dado
e o acrescenta ao "buffer" a partir da linha endereçada.
.)b

.sp
.(b
.in 5
(.,.) c
.br
<texto>
.br
 .
.br

.in 10
O comando 
"c" (change)
remove as linhas endereçadas,
e então recebe o texto entrado
que substitui estas linhas.
Se não houver texto entrado,
a nova linha corrente será a linha anterior às linhas removidas.
.)b

.sp
.(b
.in 5
(., .) d[p]
.br
.in 10
O comando 
"d" (delete)
remove as linhas endereçadas do "buffer".
A linha originalmente após a última linha removida
se tornará  a linha corrente;
se as linhas removidas estavam originalmente no final do "buffer",
a última linha no "buffer" será a nova linha corrente.
Se o sufixo "p" estiver presente a nova linha corrente é mostrada.
.br
.)b

.sp
.(b
.in 5
e [arquivo]
.br

.in 10
O comando 
"e" (edit)
causa a remoção de todas linhas no "buffer"
e a leitura de "arquivo" para o "buffer".
O número de linhas e caracteres lidos é escrito.
O nome "arquivo" é lembrado para poder ser usado em sub-sequentes
comandos "r", "w" e "e" como nome de arquivo pré-estabelecido.
.)b

.sp
.(b
.in 5

E [arquivo]
.br
.in 5
e! [arquivo]
.br
.in 10
Estes comandos são equivalentes ao comando "e",
exceto que nenhuma mensagem de erro é enviada
se o "buffer" já foi alterado
desde sua última atualização no sistema de arquivos.
.br
.)b

.sp
.(b
.in 5

f [arquivo]
.br
.in 10
O comando 
"f" (filename)
informa o nome lembrado para o arquivo em edição.
Se "arquivo" é dado, o nome correntemente lembrado é trocado para "arquivo".
.br
.)b

.sp
.(b
.in 5

(1,$) g/expressão regular/ [comando]
.br
.in 10
O comando 
"g" (global)
marca todas as linhas endereçadas
onde há uma ocorrência de "expressão regular"
e para cada uma dessas linhas
a torna a linha corrente
e executa "comando"
Os comandos "a", "i", "c",
"g" e "v" não são permitidos.
.)b

.sp
.(b
.in 5
g [comando]
.br
.in 10
O comando 
"g" (global)
sem especificação de expressão regular
utiliza as linhas marcadas no último comando "g" ou "v".
.)b

.sp
.(b
.in 5
(.) i
.br
<texto>
.br
 .
.br

.in 10
O comando 
"i" (insert)
acrescenta  o texto antes da linha endereçada.
Este comando difere do comando "a" apenas no local para colocação do texto.
.)b

.sp
.(b
.in 5
(., .+1) j
.br
.in 10
O comando
"j" (join)
transforma as linhas endereçadas
em uma única linha.
A linha resultante será a nova linha corrente.
.)b

.sp
.(b
.in 5
(.) k m
.br
.in 10
O comando 
"k" (mark)
marca a linha endereçada com a letra minúscula "m".
O endereçamento "'m" se refere a esta linha.
.)b

.sp
.(b
.in 5
(., .) l
.br
.in 10
O comando 
"l" (list)
mostra as linhas endereçadas
de uma forma não ambígua.
Os caracteres <bs> são indicados por "<"
e caracteres de tabulação (<ht>) são indicados por ">".
Linhas muito longas são quebradas
em linhas de até 72 colunas.
O final de cada linha é indicado com um "$".
.)b

.sp
.(b
.in 5
(., .) m endereço
.br
.in 10
O comando 
"m" (move)
reposiciona as linhas endereçadas para após a linha endereçada
por "endereço". A última das linhas movidas se tornará a linha
corrente.
.)b

.sp
.(b
.in 5
(., .) n
.br
.in 10
O comando 
"n" (number)
exibe as linhas endereçadas
com os números das linhas e um <ht> à esquerda.
.)b

.sp
.(b
.in 5
(., .) p
.br
.in 10
O comando
"p" (print)
mostra as linhas endereçadas.
A última linha mostrada será a nova linha corrente.
O comando "p" pode ser colocado na mesma linha que um comando
que não realize entrada ou saida.
.)b

.sp
.(b
.in 5
(., .) P
.br
.in 10
Este comando mostra uma página com a linha endereçada no centro.
A linha corrente permanece inalterada.
.)b

.sp
.(b
.in 5
q
.in 10
O comando 
"q" (quit)
faz com que "ed" termine.
O arquivo em edição não é automaticamente atualizado no disco.
.)b

.sp
.(b
.in 5
Q
.br
.in 5
q!
.in 10
Estes comandos são o mesmo que "q", exceto que
nenhuma mensagem de erro
ocorre se o "buffer" já foi alterado  desde
a última atualização do arquivo.
.)b

.sp
.(b
.in 5
($) r [arquivo]
.br
.in 10
O comando 
"r" (read)
lê  "arquivo" para o "buffer"
após a linha endereçada.
Se "arquivo" estiver ausente,
o nome corrente é utilizado.
O nome "arquivo"  é lembrado se ainda não houver um nome de arquivo corrente.
Se a leitura for bem sucedida,
o número de  linhas e caracteres lidos é mostrado.
A nova linha corrente será a última linha lida do arquivo.
A última linha lida do arquivo será a nova linha corrente.
.)b

.sp
.(b
.in 5
(., .) s[n]/expressão regular/substituição/ [p]
.br
.in 10
O comando 
"s" (substitute)
busca em cada linha endereçada
ocorrências de "expressão regular"
e substitue as de ordem "n" por "substituição".
Se a ordem "n" não for especificada,
todas as ocorrências de "expressão regular" são substituidas.
É um erro a não ocorrência de 'expressão regular'
em todas as linhas endereçadas.
.br
.sp
Qualquer caractere de pontuacão pode ser usado em lugar de "/"
para delimitar a expressão regular e a substituição.
.br
.sp
O caracter "&" em 'substituição' é substituido
pela cadeia correspondente à expressão regular.
O significado especial de "&" pode ser anulado
precedendo-o por "\".
.br
.sp
O caractere <nl> precedido por "\"
divide a linha substituida em duas.
.br
.sp
A última linha onde houve substituição será a nova linha corrente.
.)b

.sp
.(b
.in 5
(., .) t endereço
.br
.in 10
Este comando age como o comando "m" (move),
exceto que uma cópia das linhas endereçadas
é colocada após "endereço".
A última linha copiada será a nova linha corrente.
.)b

.sp
.(b
.in 5
(1, $) v/expressão regular/ [comando]
.br
.in 10
Este comando é o mesmo que o comando "g", exceto que
"comando" é executado para cada linha não contendo
"expressão regular".
.)b

.sp
.(b
.in 5
(1, $) w[!] [arquivo]
.br
.in 10
O comando 
"w" (write)
escreve as linhas endereçadas
em "arquivo".
Se o arquivo não existe, é criado.
Este comando não será obedecido se "arquivo" for o nome correntemente lembrado
e não tenha havido alteração no texto
desde sua última atualização no disco
ou se "arquivo" não for o nome correntemente lembrado
e já existir um arquivo com seu nome.
O comando seguido de exclamação só não é executado
em caso de erro de escrita no disco.
A linha corrente permanece inalterada.
Se o comando é bem sucedido
o número de linhas e caracteres escritos é mostrado.
.)b

.sp
.(b
.in 5
(1, $) W [arquivo]
.br
.in 10
Este comando é o mesmo que "w", exceto que as linhas endereçadas
são acrescentadas no final de "arquivo".
.)b

.sp
.(b
.in 5
($) =
.br
.in 10
O número da linha endereçada é mostrado.
A linha corrente permanece inalterada.
.)b

.sp
.(b
.in 5
! [Comando Shell]
.br
.in 10
O restante da linha após o "!" é enviado a "sh"(cmd)
para ser interpretado como um comando.
.)b

.sp
.(b
.in 5
(.+1, .+1)
.br
.in 10
Um endereçamento apenas mostra as linhas endereçadas.
A última linha mostrada será a nova linha corrente.
.)b

.in
.sp
.b OBSERVAÇÃO
.in 5
A versão inicial não dispõe do comando "undo".

.sp
Quando "ed" lê um arquivo, todos os caracteres nulos são descartados
e é acrescentado um <nl> ao final do arquivo,
se ele estiver ausente.

.sp
As linhas muito longas quando lidas de um arquivo
ou tecladas no modo de entrada de texto
são divididas em linhas de até 255 caracteres.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi, 
sh
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
