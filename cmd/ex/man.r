.bp 1
.he 'EX (cmd)'TROPIX: Manual de Referência'EX (cmd)'
.fo 'Atualizado em 18.05.97'Versão 3.0.2'Pag. %'

.b NOME
.in 5
ex - editor visual de textos

.in
.sp
.b SINTAXE
.in 5
ex [-v] [<nome>]
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ex" é um editor de texto que pode utilizar toda
a tela do terminal para mostrar o arquivo em edição.
.sp
Opções:
.in 8
.ip -v 8
Funcionamento verboso. Quando esta opção está ativa,
todos os comandos no modo Visual são ecoados.
.ep
.in 5

.sp
Argumentos:
.in 8
.ip <nome> 8
Nome do arquivo a ser editado.
Se este nome não corresponde a um arquivo já existente,
o "ex" inicia a edição de um arquivo novo com o nome dado.
Se nenhum nome é fornecido, o "ex" inicia a edição 
de um arquivo novo, cujo nome poderá ser indicado
mais tarde através dos comandos "f" (file), "r" (read) ou "w" (write),
disponíveis no modo Comandos.
.ep
.in 5

.sp
O editor "ex" tem dois modos principais de operação:
o modo 
.b Visual
(pelo qual ele inicia)
e o modo
.b Comandos.
Em cada modo existem comandos para a mudança de modo
ou para a execução de um comando no outro modo.

.in 5
.sp
.b 'Modo Visual:'
.sp
.in 5
No modo Visual o editor "ex" utiliza o vídeo do terminal
como uma janela para a visualização do texto em edição
e o cursor como um apontador, indicando onde serão realizadas
as operações de edição.

.sp
Os comandos no modo Visual são compostos de alguns poucos
(geralmente um) caracteres normalmente não ecoados.
O efeito dos comandos é percebido instantaneamente pelo usuário
através do movimento do cursor e/ou da alteração do texto na janela.
Os comandos, em sua maioria, podem ser precedidos de uma contagem,
indicando a repetição de seu efeito.
Os comandos que requerem mais de um caractere
podem ser cancelados (se ainda não foram totalmente digitados)
teclando-se o caractere <esc>.

.sp
As tabelas a seguir contém os comandos do modo Visual,
divididos em grupos com funcionalidade semelhante.

.(t
.sp
Os comandos a seguir reposicionam a janela sobre o texto em edição,
permitindo uma completa visualização do mesmo.
.sp
.ts
center
allbox;
c s
c l.
Movendo a Janela
Comando	Função
<^F>	avança uma janela de texto
<^B>	retrocede uma janela de texto
<^D>	avança suavemente meia janela
<^U>	retrocede suavemente meia janela
<^E>	avança uma linha
<^Y>	retrocede uma linha
G	mostra a linha dada a sua ordem
<^G>	informa a ordem da linha corrente
.te
.)t

.(t
.sp
Os comandos a seguir reposicionam o cursor,
passeando com o cursor pelo texto nas quatro direções básicas.
.sp
.ts
center
allbox;
c s
c l.
Movendo o Cursor nas Direções Básicas
Comando	Função
h	retrocede um caractere
j	avança uma linha na coluna corrente
k	retrocede uma linha na coluna corrente
l	avança um caractere
.te
.)t

.(t
.sp
Os comandos a seguir reposicionam o cursor,
passeando com o cursor por linhas próximas a linha corrente.
.sp
.ts
center
allbox;
c s
c l.
Movendo o Cursor para Linhas Próximas
Comando	Função
 - 	retrocede uma linha
+	avança uma linha
H	primeira linha na janela
M	linha central na janela
L	última linha na janela
.te
.)t

.(t
.sp
Os comandos a seguir reposicionam o cursor,
passeando com o cursor pela linha corrente.
.sp
.ts
center
allbox;
c s
c l.
Movendo o Cursor na Linha Corrente
Comando	Função
0	primeiro caractere
^	início (primeiro caractere não branco)
<bs>	retrocede um caractere
<sp>	avança um caractere
<ht>	próxima coluna de tabulação
$	último caractere
.te
.)t

.(t
.sp
O editor "ex" distingue dois tipos de palavras:
para os comandos com letras maiúsculas
as palavras são sequências de caracteres não incluindo
os separadores <sp> e <ht>;
para os comandos com letras minúsculas, sequências alfanuméricas
ou sequências de sinais de pontuação.
O alcance destes comandos não se limita à linha corrente.
.sp
.ts
center
allbox;
c s
c l.
Movendo o Cursor por Palavras
Comandos	Função
w  W	avança uma palavra
e  E	avança até o final de uma palavra
b  B	retrocede ao inicio de uma palavra
.te
.)t

.(t
.sp
Os comandos a seguir movem o cursor para a primeira ocorrência
do caractere especificado.
.sp
.ts
center
allbox;
c s
c l.
Busca de Caracteres
Comando	Função
f<ch>	avança até o caractere <ch> (inclusive)
F<ch>	retrocede até o caractere <ch> (inclusive)
t<ch>	avança até o caractere <ch> (exclusive)
T<ch>	retrocede até o caractere <ch> (exclusive)
;	repete a última busca de caractere
,	repete no sentido inverso
.te
.)t

.(t
.sp
Os comandos a seguir permitem que algumas linhas do texto
sejam referenciadas por letras minúsculas.
.sp
.ts
center
allbox;
c s
c l.
Marcação de Linhas
Comando	Função
m<letra>	marca a posição corrente como <letra>
'<letra>	move para a linha marcada como <letra>
`<letra>	move para a posição marcada como <letra>
''	retorna à posição corrente conseqüente
-	ao último movimento relativo
.te
.)t

.(t
.sp
O editor "ex" é capaz de buscar expressões regulares no texto.
As expressões regulares aceitas estão descritas resumidamente
adiante (seção Expressões Regulares);
uma descrição completa
pode ser encontrada no guia ("ex" (ref)).
.sp
.ts
center
allbox;
c s
c l.
Busca de Expressões Regulares
Comando	Função
/<expressão><nl>	busca avançando
?<expressão><nl>	busca retrocedendo
n	repete a última busca com "/" ou "?"
N	repete no sentido inverso
&	avança até o primeiro caractere após
-	a última expressão regular encontrada
.te
.)t

.sp
Os comandos para a movimentação do cursor
podem ser utilizados para especificar a abrangência
das operações de edição
(geralmente da região apontada pelo cursor
até a região para onde o cursor iria).
Quando utilizados neste contexto, estes comandos são chamados de
.b
objetos.
.r

.(t
.sp
O "ex" dispõe de vários buffers, onde podem ser armazenadas partes do texto.
Os comandos para remoção de texto e manipulação de buffers
normalmente se referem a um buffer sem nome;
os outros buffers são nomeados por uma letra minúscula.
.sp
.ts
center
allbox;
c s
c l.
Manipulação de Buffers
Comando	Função
["<buf>]y<objeto>	operador para armazenamento
["<buf>]p	copia buffer após o cursor
["<buf>]P	copia buffer antes do cursor
.te
.)t

.(t
.sp
Através dos operadores de edição podemos remover, substituir
e deslocar partes do texto.
.sp
.ts
center
allbox;
c s
c l.
Operadores de Edição
Comando	Função
["<buf>]d<objeto>	remoção
c<objeto>	substituição (remoção e inserção)
><objeto>	deslocamento para a direita
<<objeto>	deslocamento para a esquerda
.te
.)t

.(t
.sp
Através dos comandos para inserção de texto
introduzimos o texto teclado no terminal
no texto em edição.
.sp
.ts
center
allbox;
c s
c l.
Inserção de Texto
Comando	Função
i	insere antes do cursor
a	insere após o cursor
o	insere em uma nova linha abaixo
O	insere em uma nova linha acima
<eol>	termina inserção de texto
.te
.)t

.(t
.sp
Todos os comandos de edição podem ser repetidos no mesmo ou em outro contexto
ou ter seu efeito anulado.
.sp
.ts
center
allbox;
c s
c l.
Repetição e Retroação dos Comandos de Edição
Comando	Função
 . (ponto) 	repete o último comando de edição
u	desfaz a última alteração no texto
.te
.)t

.(t
.sp
Algumas operações mais utilizadas e que normalmente demandam
mais de um caractere para a sua especificação podem
ser especificados por comandos sinônimos,
que consistem de apenas um caractere.
.sp
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
<^N>	j	move para baixo
<nl>	j	move para baixo
<^P>	k	move para cima
<cr>	+	avança para a próxima linha
.te
.)t

.(t
.sp
A tabela a seguir contém alguns comandos de funções diversas,
que ainda não foram mencionados.
.sp
.ts
center
allbox;
c s
c l.
Outros Comandos
Comando	Função
J	agrupamento de linhas
<^R> <^L>	reescreve a janela
!	envia um comando para a shell
rx	substitui o caractere corrente por "x"
%	busca o caractere par: () [] {}
.te
.)t

.in 5
.sp
.(t
.b 'Modo Inserção'
.r
.sp
Durante a inserção de texto (que pode ser ativada a partir do modo Visual)
os comandos do modo Visual não são válidos
e alguns caracteres especiais podem ser usados para editar
linha a linha o texto teclado,
permitindo a correção de erros de digitação.
Os caracteres que comandam a edição da entrada são listados
a seguir com os nomes pelos quais eles podem ser alterados
através do comando "stty" e as suas respectivas funções.
.sp
.ts
center
allbox;
l l.
Caractere	Função
<erase>	remove o último caractere inserido
<aerase>	alternativa a <erase>
<word>	remove a última palavra inserida
<kill>	remove todo o texto inserido nesta linha
<retype>	reescreve a janela
<eol>	termina a inserção
<eof>	alternativa a <eol>
<^V>	anula a função especial do próx. caractere
.te
.)t

.in 5
.sp
.(t
.b 'Expressões Regulares'
.r
.sp
O editor "ex" reconhece um subconjunto de expressões regulares.
Uma expressão regular é especificada como uma cadeia de caracteres,
em que, com a execeção de alguns caracteres especiais,
um caractere normalmente corresponde a ele mesmo.
Os caracteres especiais são ".", "[", "]", "\" e,
em alguns casos, "^", "$" , "*" e "+".
A tabela a seguir resume a funcionalidade dos caracteres especiais.
.)t

.sp
.(t
.ts
center
allbox;
c l.
Carac. Especial	Significado
^	início de uma linha
$	final de uma linha
 . (ponto) 	qualquer caractere
[s]	um caractere presente na cadeia "s"
[^s]	um caractere ausente da cadeia "s"
x*	repetição de "x" (zero ou mais vezes)
x+	repetição de "x" (uma ou mais vezes)
\x	caractere "x"
.te
.)t

.in 5
.sp
.(t
.b 'Modo Comandos'
.r
.sp
.in 5
Os comandos do editor "ed" estão disponíveis no modo Comandos.
Entrada para o modo Comandos a partir do modo Visual: "Q".
Saída do modo Comandos e retorno para o modo Visual: "v".
.)t

.in
.sp
.b OBSERVAÇÕES
.in 5
O editor "ex" identifica o tipo de terminal onde ele está
sendo executado através da variável TERM, presente no ambiente.
Portanto antes de usar o "ex" é importante garantir
que o nome de seu terminal já foi corretamente atribuído a variável TERM
e que esta variável seja exportável.

.sp
A recepção dos sinais "SIGINT" e "SIGQUIT"
normalmente interrompe as operações do "ex".
A recepção do sinal "SIGTERM" faz com que o texto em edição seja salvo
em um arquivo denominado "VI.SAVE" sob o diretório corrente
e que o "ex" termine a sua execução.

.sp
O comando "ex" identifica se o arquivo em edição é do estilo UNIX
(linhas terminadas por <nl>) ou estilo DOS (linhas terminadas por <cr><nl>),
e processa corretamento os dois estilos.

.in
.sp
.(t
.b "VEJA TAMBÉM"
.r
.in 5
(cmd): ed, sh
.)t
.br
(ref): ex

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
