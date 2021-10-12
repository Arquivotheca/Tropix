
.bp
.sp 4
.nf
     *********************************************************
     *                                                       *
     *                      CAPÍTULO  3                      *
     *                                                       *
     *                     MODO COMANDOS                     *
     *                                                       *
     *********************************************************
.fi
.sp 4
.pp
Tanto o modo Visual como o modo Comandos são suficientes
para a realização da maioria das operações de edição normalmente necessárias,
entretanto em determinados contextos pode ser mais vantajoso
trabalhar em um ou no outro modo.
O modo Visual é mais adequado quando é necessária uma melhor visualização
do texto e para a realização de correções localizadas.
O modo Comandos é mais prático
quando as operações de edição podem ser mais facilmente localizadas
linha a linha ou abrangem um número inteiro de linhas.
Este capítulo detalha a funcionalidade do modo Comandos.

.pp
Os comandos do modo Comandos podem requerer zero, um ou dois endereços.
Quando são fornecidos endereços a menos,
o "vi" assume endereços pré-estabelecidos.
É um erro dar mais endereços do que o comando espera.
.br

.pp
Se não for especificado nenhum comando
após o endereçamento
o comando será "p" (print) (explicado mais adiante).
Uma linha em branco faz da linha seguinte à linha corrente
a nova linha corrente e a mostra.

.pp
Endereços são tipicamente separados por uma vírgula (",").
Quando os endereços são separados por um ponto e vírgula (";"),
o segundo endereço é calculado fazendo-se antes
o primeiro endereço como linha corrente.
Esta característica pode ser usada para determinar 
a linha incial para buscas para frente e para trás ("/" e "?").
O segundo endereço de toda seqüência de dois endereços
tem que corresponder a uma linha de ordem superior
a linha correspondente ao primeiro endereço.

.pp
As seções a seguir descrevem a especificação de endereços
e todos os comandos disponíveis.

.sp 2
.b
3.1. Endereçamento
.r

.pp
O endereçamento de linhas no texto em edição
baseia-se no conceito de linha corrente.
De modo geral a linha corrente é a última linha afetada por um comando;
entretanto a escolha exata da nova linha corrente
após a execução de cada comando
é explicada na discussão dos comandos.
Endereços podem ser construídos do seguinte modo:

.in +3
.ip 1. 4
O caractere "." endereça a linha corrente.

.ip 2. 4
O caractere "$" endereça a última linha no texto em edição.

.ip 3. 4
Um número decimal "n" endereça a linha de ordem "n'ésima" no texto em edição.

.(t
.ip 4. 4
A seqüência "'m" endereça a linha marcada com o nome "m",
que deve ser uma letra minúscula.
No modo Visual linhas são marcadas através do comando "m",
no modo Comandos através do comando "k" (mark).
.)t

.(t
.ip 5. 4
Uma expressão regular entre barras ("/expressão/")
endereça a linha encontrada buscando-se avante após a linha corrente
ou após o endereço especificado até então
e parando na primeira linha que contém uma cadeia
que corresponda à expressão regular.
Se necessário, a busca dá a volta, passando pelo fim do texto em edição.
.)t

.(t
.ip 6. 4
Uma expressão regular entre interrogações ("?expressão?")
endereça a linha encontrada buscando-se retroativamente
antes da linha corrente
ou do endereço especificado até então
e parando na primeira linha que contém uma cadeia
que corresponda à expressão regular.
Se necessário, a busca dá a volta, passando pelo início do texto em edição.
.)t

.(t
.ip 7. 4
Um endereço seguido de um "+" (sinal de soma)
ou de um "-" (sinal de subtração)
seguido de um número decimal especifica um endereço
somado ou subtraído
do número indicado de linhas.
.)t

.(t
.ip 8. 4
Se um endereço começa com "+" ou com  "-" a adição ou subtração
é tomada como em relação a linha corrente, isto é,
"-5" equivale à ".-5".
.)t

.(t
.ip 9. 4
Se um endereço termina com "+" (ou "-"), então um é somado (ou subtraído).
Como conseqüência desta regra e da regra 8,
o endereço "-" se refere à linha anterior a linha corrente.
Mais ainda, "-" e "+" no final de um endereço têm efeito cumulativo,
assim "--" se refere a linha corrente menos dois.
.)t
.ep
.in -3

.sp 2
.b
3.2. Comandos
.r
.pp
Estão listados a seguir todos os comandos do modo Comandos.
Os valores pré-estabelecidos para os endereços
indicando a abrangência dos comandos
são mostrados entre parênteses à esquerda dos comandos.
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
e o acrescenta ao texto em edição a partir da linha endereçada.
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
remove as linhas endereçadas do texto em edição.
A linha originalmente após a última linha removida
se tornará  a linha corrente;
se as linhas removidas estavam originalmente no final do texto em edição,
a última linha no texto em edição será a nova linha corrente.
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
causa a remoção de todas linhas no texto em edição
e a leitura de "arquivo" para o texto em edição.
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
se o texto em edição já foi alterado
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
(1, $) g!/expressão regular/ [comando]
.br
.in 10
Este comando é o mesmo que o comando "g", exceto que
"comando" é executado para cada linha não contendo
"expressão regular".
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
utiliza as linhas marcadas no último comando "g".
.)b

.sp
.(b
.in 5
(.) i
 <texto>
 .

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
faz com que "vi" termine.
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
ocorre se o texto em edição já foi alterado  desde
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
lê  "arquivo" para o texto em edição
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
todas as ocorrências de "expressão regular" são substituídas.
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
divide a linha substituída em duas.
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
v
.in 10
O comando "v" (visual) retorna o editor ao modo Visual.
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
são acrescenatadas no final de "arquivo".
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

.in 0
