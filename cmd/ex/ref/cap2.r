.bp
.sp 5
.nf
     *********************************************************
     *                                                       *
     *                      CAPÍTULO 2                       *
     *                                                       *
     *                      MODO VISUAL                      *
     *                                                       *
     *********************************************************
.fi
.sp 4
.pp
O "vi" no modo Visual utiliza o vídeo do terminal
como uma janela sobre o texto em edição
e o cursor como um apontador no texto, localizando as operações de edição.

.pp
Os comandos no modo Visual são compostos de alguns poucos
(geralmente um) caracteres normalmente não ecoados.
O efeito dos comandos é percebido instantaneamente pelo usuário
através do movimento do cursor e/ou da alteração do texto na janela.
Os comandos, em sua maioria, podem ser precedidos de uma contagem.
Os comandos que requerem mais de um caractere
podem ser cancelados (se ainda não foram totalmente digitados)
teclando-se o caractere <esc>.

.sp 2
.b
2.1. Observação e Localização no Texto
.r
.pp
A maioria dos comandos do editor "vi" em seu modo Visual
move a janela sobre o texto ou movimenta o cursor pelo texto.
De modo geral, estes comandos podem ser precedidos por uma contagem,
para indicar uma repetição no efeito destes comandos.

.sp
.b
2.1.1. Movendo a Janela sobre o Texto
.r

.pp
A porção de texto mostrada a cada instante pela janela
é chamada de uma
.b página.
É possível mover a janela 
avançando ou retrocedendo páginas
ou, mais suavemente, deslocando a janela gradualmente.

.pp
Os comandos <^F> (move forward) e <^B> (move backward) movem a janela
em unidades de uma página, respectivamente avançando ou retrocedendo.
Avançando ou retrocedendo a janela de uma página,
o editor "vi" conserva duas linhas da página mostrada anteriormente,
de modo a possibilitar a leitura contínua do texto.
Após estes comandos o cursor se localiza
no início da primeira palavra
na primeira linha da nova página corrente.

.pp
Os comandos <^D> (sroll down) e <^U> (scroll up)
propiciam um modo mais confortável para percorrermos o texto.
Estes comandos respectivamente descem e sobem a janela sobre o texto,
"rolando" o vídeo e gradualmente expondo as linhas de texto adjacentes
à página mostrada antes do comando.
Quando especificamos uma contagem, esta se refere ao número de linhas 
em que a janela deve ser deslocada.
Quando não especificamos uma contagem,
a janela é deslocada de acordo com a última contagem
fornecida a estes comandos ou,
se até o momento não foram especificadas contagens para estes comandos,
de meia página.
Os comandos <^E> e <^Y>, respectivamente sobem e descem a janela de uma linha.

.pp
Se o caractere apontado pelo cursor permanecer na janela
após a execução dos comandos <^D>, <^U> <^E> ou <^Y>,
ele continuará sendo apontado;
em caso contrário, o cursor apontará
o início da primeira palavra
na primeira linha na janela.

.pp
Exemplos:
a seqüência de comandos "5<^D><^U>"
move a janela abaixo cinco linhas e retorna à posição original,
o comando <^B> retrocede uma página
e o comando "5<^F>" avança cinco páginas.

.sp
.b
2.1.2. Apontando uma Linha no Texto
.r

.pp
Os comandos "+" ou <cr> e "-" respectivamente avançam e retrocedem uma linha.
O comando "H" (home) move o cursor para a primeira linha na janela.
O comando "M" (medium) move o cursor para uma linha
aproximadamente no meio da janela.
O comando "L" (last) move o cursor para a última linha na janela.
O comando "G" (go) move o cursor para a linha especificada pela contagem;
na falta de uma contagem, "G" move o cursor para a última linha do texto.
O comando <^G> informa a ordem da linha apontada
e o número total de linhas no texto.

.sp
.b
2.1.3. Movendo o Cursor na Página Corrente
.r

.pp
As teclas "h", "j", "k" e "l" movem o cursor respectivamente 
para a esquerda, para baixo, para cima e para a direita.
Se há setas no teclado do seu terminal,
elas provavelmente são reconhecidas pelo editor "vi"
e também podem ser usadas para mover o cursor.

.sp
.b
2.1.4. Movendo o Cursor na Linha Corrente
.r

.pp
Os seguintes comandos movem o cursor sobre a linha corrente:
<bs> para o caractere à esquerda do cursor,
<sp> para o caractere à direita do cursor,
<ht> para o caractere ocupando a próxima coluna de tabulação,
"0" para o primeiro caractere,
"$" para o último caractere,
"w" e "W" (word) para o início da próxima palavra,
"e" e "E" (end) para o próximo fim de palavra,
"b" e "B" (begin) para o início de palavra imediatamente anterior e
"^" para o início da primeira palavra.

.pp
O editor "ex" distingue dois tipos de palavras:
para os comandos com letras maiúsculas
as palavras são seqüências de caracteres não incluindo
os separadores <sp> e <ht>;
para os comandos com letras minúsculas, seqüências alfanuméricas
ou seqüências de sinais de pontuação.
O alcance destes comandos não se limita à linha corrente.
Uma palavra é definida como uma seqüência de caracteres
diferentes de <sp> e <ht>.

.sp
.b
2.1.5. Busca de Expressões Regulares
.r

.pp
A busca de expressões regulares é
uma das formas mais práticas de localizar um trecho específico
dentro do texto em edição.
Durante a busca de uma expressão regular, o editor "vi" percorre o texto circularmente,
ou seja, o editor "vi" considera
que ao fim do texto segue-se o seu início e vice-versa.

.pp
O comando "/" busca uma expressão regular fornecida na última linha do vídeo
(como no modo Comandos descrito adiante)
avançando a partir do caractere apontado pelo cursor.
O comando "?" também requer uma expressão regular na última linha do vídeo
e procura-a nas linhas anteriores do texto.
O comando "n" (next) repete o último comando "/" ou "?" executado.
O comando "N" repete a último comando "/" ou "?" revertendo o sentido da busca.

.pp
O comando "&" move o cursor para o caractere
seguinte à cadeia correspondente à expressão regular.

.sp
.b
2.1.6. Busca de Caracteres
.r
.pp
Os comandos "f" e "F" (find) buscam o caractere fornecido a seguir,
repectivamente para frente e para trás.
Os comandos "t" e "T" (up to) são análogos aos comandos "f" e "F",
porém não movem o cursor para o caractere procurado,
mas sim para o caractere que o antecedeu na busca.

.sp
.b
2.1.7. Marcação de Linhas
.r
.pp
A marcação de linhas permite que linhas sejam referenciadas por letras.
Um comando na forma "mx", onde "x" é uma letra minúscula,
associa à linha corrente a marca "x".
Uma linha marcada pode ser localizada por um comando na forma
"'x", onde "x" é a marca associada.
Um comando na forma "`x" localiza a posição exata da marcação
(linha e ordem do caractere marcado).

.sp
.b
2.1.8. Verificação de Balanceamento
.r
.pp
O comando "%" busca o caractere correspondente ao balanceamento
do caractere corrente.
Os caracteres "[", "{" e "(" são balanceados respectivamente
pelos caracteres "]", "}" e ")" e vice-versa.

.sp 2
.b
2.2. Edição Básica do Texto
.r
.pp
Esta seção aborda todos os comandos do modo Visual
para alteração do texto.

.sp
.(t
.b
2.2.1. Inserção de Texto
.r

.pp
O editor "vi" dispõe de vários comandos
que permitem acrescentar caracteres teclados ao texto.
Estes comandos colocam o editor "vi" em seu modo Inserção.
.)t

.pp
No modo Inserção, o editor "vi" abre um espaço apontado pelo cursor,
indicando onde o próximo caractere será inserido.
O caractere <esc> encerra o modo Inserção.
O estado do texto sob a janela é constantemente mantido atualizado no vídeo
durante o funcionamento do modo Inserção
(para maiores detalhes, veja o Capítulo 4).

.pp
Existem quatro comandos básicos que iniciam o modo Inserção.
Os comandos "i" (insert) e "a" (append) inserem texto
respectivamente à esquerda e à direita do caractere apontado pelo cursor.
Os comandos "o" e "O" (open) permitem a inserção de texto
respectivamente abaixo e acima da linha apontada pelo cursor.

.sp
.b
2.2.2. Operadores
.r

.pp
Operadores manipulam extensões contíguas de texto.
Os operadores devem ser seguidos de um comando para movimento do cursor
que especifica o escopo da operação.
Este comando para movimento do cursor é chamado
.b
objeto
.r
quando utilizado neste contexto.

.pp
As operações podem abranger linhas inteiras
ou uma parte do texto entre dois caracteres,
de acordo com o tipo do movimento correspondente ao objeto.
No caso de repetição do operador, a operação abrange
um número de linhas inteiras dado pela contagem.

.pp
Os seguintes operadores estão disponíveis:
"d" (delete) para remoção,
"c" (change) para substituição
(remoção seguida de entrada no modo Inserção)
e ">" e "<" para o deslocamento de linhas
respectivamente para a direita e para a esquerda
(através da inserção e remoção de caracteres <ht>).
O operador "y" (yank) para armazenamento independente
de partes do texto está descrito adiante
(seção 2.3. Cópia e Transferência de Blocos).

.pp
Exemplos:
o comando "d$" remove do caractere apontado pelo cursor
até o final da linha corrente;
o comando "3cc" substitui a linha corrente
e mais duas abaixo por um texto inserido;
a seqüência "/ontem<esc>c&hoje<esc>"
busca a palavra "ontem" e a substitui pela palavra "hoje".

.sp
.b
2.2.3. Reversão e Repetição dos Comandos de Edição
.r

.pp
O comando "u" (undo) reverte o último comando de edição.
Isto significa que sempre é possível retornar o texto ao seu estado anterior.
O comando "u" também pode ser revertido.
A aplicação repetida do comando "u"
faz com que o texto alterne entre dois estados (o atual e o anterior).

.pp
O comando "." (ponto) repete o último comando de edição.

.sp
.(t
.b
2.2.4. Substituição de um Caractere
.r

.pp
O comando "r" (replace) substitui o caractere apontado pelo cursor
pelo caractere teclado a seguir.
.)t

.sp 2
.b
2.3. Cópia e Transferência de Blocos de Texto
.r
.pp
Blocos de texto são grupos de linhas de texto contíguas.
O editor "vi" dispõe de um conjunto de áreas de memória
capazes de armazená-los.
Estas áreas de memória são chamadas 
.b buffers.
Existem 27 buffers no editor "vi";
26 são referenciados por uma letra minúscula
(buffers com nomes de "a" até "z")
e um é acessado diretamente (buffer sem nome).

.pp
As operações básicas relativas aos buffers
são a cópia de uma parte do texto para um buffer
e sua recíproca - a cópia do conteúdo de um buffer para o texto.
O operador "y" (yank) copia o objeto especificado para um buffer.
Os comandos "p" e "P" (put) copiam o conteúdo de um buffer
para respectivamente após e antes o cursor.

.pp
O operador "y" e os comandos "p" e "P"
na forma mais simples trabalham com o buffer sem nome.
A especificação de um buffer antecede o comando
e é feita teclando aspas (") e a letra correspondente ao buffer desejado.

.pp
Os buffers não são preenchidos apenas pelo operador "y".
As partes do texto removidas pelo operador "d"
também são copiadas para os buffers.

.pp
Uma aplicação interessante do buffer sem nome
é um modo bastante prático
para a transferência de parte do texto,
realizando a seguinte série de comandos:
remoção da parte do texto desejada,
movimento do cursor
e cópia do buffer sem nome para o texto em edição.

.pp
Exemplos:
o comando
.b
yw
.r
guarda uma palavra no buffer sem nome;
o comando
.b
"ay'x
.r
copia para o buffer "a" as linhas
desde a linha corrente até a linha marcada como "x".

.sp 2
.b
2.4. Sinônimos
.r

.pp
Existem alguns sinônimos usando apenas um caractere
para os comandos ou seqüências de comandos utilizados
com maior frequência e que requerem dois ou mais caracteres
para serem executados.
A lista completa dos sinônimos e seus respectivos significados
está na tabela a seguir.

.sp
.(t
.ts
center
allbox;
c s s
c c l.
Alguns Sinônimos Úteis
Sinônimo	Comandos	Função
x	d<sp>	apaga o caractere sob o cursor
X	d<bs>	apaga o caractere anterior
s	c<sp>	substitui o caractere corrente
I	^i	insere no início da linha
A	$a	insere ao final da linha corrente
C	c$	substitui até o final da linha
D	d$	remove até o final da linha
Y	yy	armazena a linha corrente
S	cc	substitui a linha corrente
.te
.)t

