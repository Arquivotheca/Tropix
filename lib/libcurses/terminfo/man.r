.bp
.he 'TERMINFO (fmt)'TROPIX: Manual de Referência'TERMINFO (fmt)'
.fo 'Atualizado em 04.01.91'Versão 2.3'Pag. %'

.b NOME
.in 5
terminfo

.in
.sp
.b DESCRIÇÃO
.in 5
O banco de dados "terminfo"
contém as descrições dos diversos tipos de terminais
presentes em uma instalação.

Este banco de dados possibilita que
os programas construídos a partir da biblioteca "curses",
como por exemplo, o "vi" (editor de textos)
e o "wsdb" (depurador simbólico),
funcionem em uma variedade de terminais sem que seja necessário
qualquer tipo de alteração ou recompilação.

.sp
Este texto apresenta a linguagem empregada para descrever um terminal.
O utilitário "tic"
converte a descrição de um terminal desta linguagem para um formato interno,
que pode ser acessado mais rapidamente pela biblioteca "curses".

.sp
A descrição de um terminal é uma seqüencia de campos
terminados por vírgulas (",").
O primeiro campo de uma descrição
contém os nomes pelos quais o terminal é conhecido.
Os campos seguintes são ocupados pelas características do terminal,
informando, por exemplo,
se o terminal apresenta determinados comportamentos,
como realizar certas operações no terminal
e as seqüências de caracteres enviadas por suas teclas especiais.

.sp
.b
Nomes do Terminal
.r
.sp
O primeiro campo inicia com os nomes (separados por "|")
pelos quais o terminal é conhecido
pela "curses";
um destes nomes deve ser atribuído à variável "TERM" no ambiente
(veja "environ" (fmt)).
Estes nomes não devem conter espaços.
O último item no primeiro campo deve ser um nome longo
que identifique completamente o terminal;
este nome não é utilizado pela "curses"
e pode conter espaços para melhorar a sua legibilidade.

.sp
.b
Capacidades do Terminal
.r
.sp
Os terminais são descritos em termos de capacidades
que podem ser dos tipos booleano, númerico,
cadeia simples ou cadeia paramétrica.

.sp
A presença do nome de uma capacidade booleana
indica que o terminal possui o correspondente recurso ou comportamento.
Por exemplo: o fato de um terminal ter margem automática
(isto é, um avanço para a primeira posição na linha seguinte
quando um fim de linha é alcançado)
é indicado pela presença de um campo contendo "am".

.sp
As capacidades numéricas são expressadas
através de seu nome seguido do caractere "#"
e dos valores númericos associados.
Exemplo: se um terminal dispõe de oitenta colunas,
isto é indicado por um campo na forma "cols#80".

.sp
As capacidades do tipo cadeia são expressadas
através de seu nome seguido do caractere "="
e da cadeia delimitada pela vírgula que separa os campos.
Exemplo: a descrição de um terminal que limpa o seu vídeo
quando recebe a cadeia formada pelos caracteres
<esc>, "H", <esc> e "J"
contém um campo "clear=\EH\EJ".

.sp
O conjunto de campos de uma descrição pode estar disperso
em várias linhas e todos os campos, exceto o primeiro,
podem ser livremente precedidos de espaço
(caracteres <sp>, <ht> e <nl>).
Linhas iniciadas pelo caractere "#" são consideradas comentário e,
portanto, têm efeito nulo.

.sp
Os valores para as capacidades numéricas devem ser expressos em base decimal,
podendo ser precedidos de um sinal ("+" ou "-").
Os caracteres de controle podem ser expressos pela notação "^X",
ou "escapados" pelo caractere "\", de acordo com a seguinte codificação:

.sp
.ts
center
allbox;
c c c.
Seqüência	Alternativa	Significado
\E	\e	<esc>
\n	\l	<nl>
\r		<cr>
\t		<ht>
\b		<bs>
\f		<np>
\s		<sp>
\0		<nul>
 \^ 		^
\\		\
.te

.sp
.b
Cadeias Paramétricas
.r
.sp
O posicionamento do cursor e outras capacidades do tipo cadeia
que requerem parâmetros são expressados através das cadeias paramétricas.
Estas cadeias utilizam uma notação similar à empregada
pela rotina "printf" para especificar o formato em que estes
parâmetros são enviados.
Um exemplo:
o posicionamento do cursor é descrito pela capacidade "cup",
que utiliza dois parâmetros: a linha e a coluna.
As linhas e colunas são sempre numeradas da esquerda para a direita
e de cima para baixo a partir de zero
e se referem ao vídeo efetivamente visível ao usuário.

.sp
Uma cadeia paramétrica permite especificar algumas operações aritméticas
a serem realizadas sobre os parâmetros antes de enviá-los ao terminal.
Para isto existe um mecanismo para armazenamento dos parâmetros
em uma pilha e a realização de operações aritméticas
em Notação Reversa Polonesa.
No caso mais simples, uma seqüência empilha um parâmetro
e o envia no formato especificado.
Uma operação aritmética é feita empilhando seus dois operandos
e indicando a operação.
São usados os seguintes códigos:
.in +3

.ip "\%p[1-9]" 10
empilha o parâmetro especificado.

.ip "\%'x'" 10
empilha o caractere "x".

.ip "\%{n}" 10
empilha o número "n"
(em decimal, podendo ser precedido por um sinal "+" ou "-").

.ip "\%c" 10
desempilha e envie em formato caractere (um octeto).

.ip "\%d" 10
desempilha e envie em formato decimal.

.ip "\%o" 10
desempilha e envie em formato octal.

.ip "\%x" 10
desempilha e envie em formato hexadecimal com letras minúsculas.

.ip "\%X" 10
desempilha e envie em formato hexadecimal com letras maiúsculas.

.ip "\%+" 10
Desempilha dois valores da pilha, soma-os e empilha o resultado.

.ip "\%*" 10
Desempilha dois valores da pilha, multiplica-os e empilha o resultado.

.ip "\%-" 10
Desempilha dois valores da pilha, subtrai o primeiro do segundo
e empilha o resultado.

.ip "\%/" 10
Desempilha dois valores da pilha, divide o segundo pelo primeiro
e empilha o resultado.

.ip "\%m" 10
Desempilha dois valores da pilha, divide o segundo pelo primeiro
e empilha o resto da divisão.

.ip "\%i" 10
Incrementa os dois primeiros parâmetros ("%p1" e "%p2").

.ep
.in -3
.sp
Exemplos:
.sp
Uma seqüência de comando para o endereçamento do cursor
consistindo dos caracteres <esc> e "=" e mais dois caracteres
indicando respectivamente a linha e a coluna,
cujos valores são somados ao caractere <sp>,
é indicada da seguinte forma:
.in +5
.sp
cup=\E=\%'\s'\%p1\%+\%c%'\s'\%p2\%+\%c,
.in -5

.in
.sp
.b "VARIÁVEIS EM USO"
.in 5
A relação abaixo contém as capacidades correntemente consultadas
pela "curses".
O tipo das capacidades é indicado entre parênteses,
sendo "(b)" para capacidades booleanas,
"(n)" para capacidades numéricas,
"(c)" para cadeias e
"(p)" para cadeias paramétricas.

.sp
Dimensões
.in +3
.ip "cols (n)" 12
Número de colunas no vídeo.

.ip "lines (n)" 12
Número de linhas no vídeo.
.ep
.in -3

.sp
Movimento relativo do cursor (*)

.(f
(*) Estes movimentos nunca são usados a partir de posições que
deslocariam o cursor para fora do vídeo.
.)f

.in +3
.ip "home (c)" 12
Move para o canto superior esquerdo do vídeo (linha 0 e coluna 0).
.ip "cub1 (c)" 12
Move o cursor para a coluna imediatamente à esquerda na linha corrente.
.ip "cud1 (c)" 12
Move o cursor para a linha imediatamente abaixo na coluna corrente.
.ip "cuf1 (c)" 12
Move o cursor para a coluna imediatamente à direita na linha corrente.
.ip "cuu1 (c)" 12
Move o cursor para a linha imediatamente acima na coluna corrente.
.ip "cr (c)" 12
Move o cursor para a primeira coluna da linha corrente.
.ip "ht (c)" 12
Move para a próxima coluna de tabulação (múltipla de 8).
.ep
.in -3

.sp
Movimento direto do cursor
.in +3
.ip "smcup (c)" 12
Cadeia para preparar o terminal para movimento direto do cursor.
.ip "rmcup (c)" 12
Cadeia para anular o efeito da cadeia "smcup".
.ip "cup (p)" 12
Movimento do cursor.
Parâmetros: linha e coluna (contados a partir de 0).
.ep
.in -3

.sp
Atributos
.in +3
.ip "blink (c)" 12
Caracteres piscantes.
.ip "bold (c)" 12
Caracteres reforçados.
.ip "rev (c)" 12
Caracteres em vídeo reverso.
.ip "smso (c)" 12
Inicia caracteres em evidência.
.ip "rmso (c)" 12
Termina caracteres em evidência.
.ip "smul (c)" 12
Inicia caracteres sublinhados.
.ip "rmul (c)" 12
Termina caracteres sublinhados.
.ip "uc (c)" 12
Sublinhe o próximo caractere.
.ip "sgr0 (c)" 12
Termina qualquer campo.
.ip "xmc (c)" 12
Número de posições ocupadas pelo início ou término de um campo de atributos.
.ip "ms (c)" 12
Seguro mover o cursor enquanto escrevendo caracteres com atributos.
.ep
.in -3

.sp
Caracteres semigráficos:
.in +3
.ip "enacs (c)" 12
Habilita a utilização de caracteres semigráficos.
.ip "smacs (c)" 12
Inicia modo semigráfico.
.ip "rmacs (c)" 12
Termina modo semigráfico.
.ip "ac (c)" 12
O próximo caractere é semigráfico.
.ip "acsc (c)" 12
Mapeamento dos caracteres semigráficos aos caracteres comuns.
Esta cadeia é formada de pares de caracteres,
em que o primeiro especifica um caractere semigráfico
e o segundo especifica qual caractere o representa quando
o terminal está em modo semigráfico.
A tabela a seguir relaciona os caracteres semigráficos
com os caracteres que os especificam.
A terminologia utilizada corresponde ao terminal VT100 da DEC.

.sp
.(b
.ts
center
allbox;
c c.
Caracteres Semigráficos	Especificação
Seta apontando para a direita	+
Seta apontando para a esquerda	, (vírgula)
Seta apontando para baixo	. (ponto)
Seta apontando para cima	- (menos)
bloco quadrado cheio	0
lanterna	I
diamante	' (apóstrofo)
checker board (stipple)	a
símbolo de grau	f
mais/menos	g
board of squares	h
canto inferior direito	j
canto superior direito	k
canto superior esquerdo	l
canto inferior esquerdo	m
mais	n
scan line 1	o
linha horizontal	q
linha vertical	x
scan line 9	s
tê esquerdo	t
tê direito	u
tê para baixo	v
tê para cima	w
bala	~
.te
.)b


.ep
.in -3

.(t
.sp
Inicialização
.in +3
.ip "is1 (c)" 12
Cadeia enviada quando os programas que utilizam a "curses" iniciam.
.ip "is2 (c)" 12
Cadeia enviada a cada vez os programas entram em modo "curses".
.ep
.in -3
.)t

.sp
Deslocamentos
.in +3
.ip "ind (c)" 12
Rolamento do vídeo no sentido convencional.
.ip "ri (c)" 12
Rolamento do vídeo no sentido anticonvencional.
.ip "il1 (c)" 12
Insere uma linha.
.ip "dl1 (c)" 12
Remove a linha corrente.
.ip "csr (p)" 12
Definição da região para rolamento.
Parâmetros: linhas superior e inferior.
.ep
.in -3

.sp
Teclas especiais
.in +3
.ip "khome (c)" 12
Tecla indicando o canto superior esquerdo do vídeo.
.ip "kcub1 (c)" 12
Seta para a esquerda.
.ip "kcuf1 (c)" 12
Seta para a direita.
.ip "kcuu1 (c)" 12
Seta para cima.
.ip "kcud1 (c)" 12
Seta para baixo.
.ip "kf[1-9] (c)" 12
Teclas de funções, normalmente indicadas no teclado como
F1, F2, ..., F9.
.ep
.in -3

.sp
Controle de Star/Stop
.in +3
.ip "xon (b)" 12
Este terminal utiliza controle start/stop de transmissão.
.ip "xonc (c)" 12
Caractere XON (reinício da transmissão).
.ip "xoffc (c)" 12
Caractere XOFF (suspensão da transmissão).
.ep
.in -3

.sp
Alterações no cursor
.in +3
.ip "civis (c)" 12
Torna o cursor invisível.

.ip "cnorm (c)" 12
Torna o cursor visível.
.ep
.in -3

.sp
Outras Capacidades
.in +3
.ip "am (b)" 12
Margem automática.
Indica que após a última coluna
o cursor avança para a primeira coluna da próxima linha.

.ip "xt (b)" 12
Caracteres de tabulação destrutivos.
Isto é, eles escrevem espaços
da posição atual até a próxima coluna de tabulação.

.ip "bel (c)" 12
Sinal sonoro.
.ip "clear (c)" 12
Limpa o vídeo.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
Na versão atual não estão implementados "paddings".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):   "
tac, tic
.br
.wo "(libc):  "
printf
.br
.wo "(curses):"
curses, incurses, newterm
.br

.in
.sp
.b ARQUIVOS
.in 5
.ip /usr/lib/terminfo/* 22
arquivos onde estão armazenadas as descrições.
.ep

.in
.sp
.b EXEMPLOS
.in 5
Apresentamos abaixo uma descrição simplificada
do terminal TV3000-BR fabricado pela Sisco Computadores S.A.

.sp
.nf
       tv3000br|Sisco TV3000,
               lines#24, cols#80, am,
               cuf1=^L, cub1=\b, cud1=\l, cuu1=^K,
               home=^^, ht=\t, cr=\r,
               cup=\E=\%'\s'\%p1\%+\%c%'\s'\%p2\%+\%c,
               bel=^G, clear=^Z,
               rev=\EH, dim=\EG, blink=\EF,
               smul=\EI, rmul=\EP,
               sgr0=\EK,
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
