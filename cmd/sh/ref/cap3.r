.bp

.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 3			     *
     *							     *
     *	         VARIÁVEIS E OUTRAS SUBSTITUIÇÕES            *
     *							     *
     *********************************************************

.fi

.sp 4


.pp
Antes da execução dos comandos, seus argumentos são analisados
quanto à existência de variáveis que devam ser substituídas por
seus valores e, posteriormente, quanto à necessidade de expansão
de nomes de arquivos.

.pp
Na análise de um comando a executar, a "shell" leva sempre em consideração
o caractere "\" precedendo imediatamente um metacaractere. Este
procedimento anula o efeito especial do metacaractere.

.sp 4
.b
3.1 - Variáveis
.r

.pp
A "shell" permite que sejam definidas variáveis para posterior uso.
O nome de uma variável é uma cadeia composta de caracteres alfanuméricos,
podendo incluir também o caractere "_" (sublinhado).
O valor atribuído a ela é qualquer cadeia de caracteres.


.pp
A atribuição de uma cadeia a uma variável é realizada
escrevendo-se o nome da variável, sucedido pelo caractere "=" e da cadeia.
As variáveis são declaradas na primeira atribuição.
A variável será substituída por seu valor,
quando esta for precedida pelo metacaractere "$".

.sp
Assim, dado

.sp
.in +5
$ DIR=a/b/c

.sp
$ mv pgm $DIR

.in -5
.sp
moverá o arquivo "pgm" do diretório corrente para o diretório "a/b/c".

.sp 2

.pp
Para a substituição, o nome da variável a ser substituída é procurado a partir
do caractere "$" até um caractere não alfanumérico (exceto "_").
Se esse caractere for <sp> ou <ht>, significa o fim do nome.
Se esse caractere for qualquer outro não alfanumérico, os caracteres a seguir
serão concatenados ao valor da variável.

.sp
Assim, para

.sp
.in +5

$ A=a
$ AB=ab

.in -5
.sp

.nf
     $A          equivale a	a
     $AB         equivale a	ab
     $A.c        equivale a	a.c
     $AB.pgm     equivale a	ab.pgm
.fi


.pp
Os seguintes parâmetros têm seus valores automaticamente atribuídos pela
"shell":
.in +5

.ip "#"
O número de parâmetros posicionais em decimal.

.ip "-"
Opções de funcionamento fornecidas a "shell"  na evocação ou através do
comando "set".

.ip "?"
O valor decimal de término retornado pelo último comando executado sincronamente.

.ip "$"
A identifiçação do processo desta "shell".

.ip "!"
A identificação do processo do último comando em "background" evocado.

.ip ":"
O número do próximo comando a entrar no histórico.

.ip "@"
Os parâmetros posicionais (veja adiante) à exceção de $0.

.ip "*"
Uma cadeia com os parâmetros posicionais à exceção de $0.

.ip "\%"
Uma cadeia com o nome do nó (nome do computador na rede).

.ip "~"
O nome do diretório corrente.

.ip "&"
O caractere "#" se o usuário for um superusuário, e "$" em caso contrário.

.ep
.in -5


.pp
Algumas variáveis têm um sentido especial para a "shell" e são usadas por ela,
que já as recebe definidas. (Ver Cap. 6). São elas:

.in +5
.ip HOME 7
diretório assumido por omissão como argumento para o comando interno "cd"

.ip PATH 7
diretórios a serem pesquisados, em ordem, na procura do comando a executar

.ip PS1 7
"prompt" principal da "shell", fornecido quando 
está apto a receber novo comando 

.ip PS2 7
"prompt" secundário da "shell", fornecido quando o comando
continuará em outra linha

.ep
.in -5

.pp
A "shell" atribui valores iniciais para as variáveis
"PATH", "PS1" e "PS2".
O valor da variável "HOME" é atribuído pelo comando "login".
O caractere '~', quando ocorre em caminhos, é equivalente a $HOME.

.sp 4
.b
3.2 - Expansão de Nomes de Arquivos
.r

.pp
Muitos comandos aceitam argumentos que são nomes de arquivos.
A "shell" fornece um mecanismo para geração de uma lista,
em ordem lexicográfica, de nomes de arquivos que atendam a certo padrão.
Para gerar os padrões, existem os metacaracteres 
"*", "?", "[", "]" e "!", que significam:


.in +5
.ip * 8
qualquer cadeia de caracteres (exceto as iniciadas por ".")

.ip ? 8
qualquer caractere simples (exceto ".")

.ip [...] 8
qualquer caractere no intervalo [...]

.ip ! 8
qualquer caractere diferente do que segue "!"

.ep
.in -5

.sp
Por exemplo:

.sp
.in +5
$ ls -l *.c

.in -5
.sp
lista, em ordem alfabética, informações sobre todos os nomes de arquivos,
do diretório corrente que terminam com o sufixo ".c"

.sp
.in +5

$ ls /cmd/*/teste

.in -5
.sp
lista todos os arquivos de nome "teste" de qualquer subdiretório de "/cmd"

.sp
.in +5

$ ls /a/?

.in -5
.sp
lista todos os arquivos do diretório "/a", cujos nomes têm um só caractere

.sp
.in +5
$ ls [a-d]*

.in -5
.sp
lista todos os arquivos começados com as letras "a", "b", "c" e "d"


.sp 4
.b
3.3 - Metacaracteres como Caracteres Comuns
.r

.pp
Os seguintes caracteres têm um significado especial para a
"shell" e causam o fim de uma palavra a menos que estejam quotados:
.sp
.in +5
.b
;  &  (  )  |  ^  <  >  <nl>  <sp>  <ht>
.r
.in -5

.pp
Um caractere pode ser quotado (i.e. fazê-lo siginificar ele mesmo)
precedendo-o por uma barra invertida ou inserindo-o entre um par de
marcas de quotação ('' ou ""). Durante o processamento, a "shell"
pode quotar alguns caracteres para evitar que eles tenham um
significado especial. Barras invertidas usadas para quotar um
caractere são removidas da palavra antes que o comando seja
executado. O par "\<nl>" é removido antes da substituição de
comandos e parâmetros.

.pp
Todos os caracteres entre um par de acentos agudos, exceto
um acento agudo, são quotados pela "shell".
Barra invertida
não tem um significado especial dentro de um par de acentos
agudos. Um acento agudo pode ser quotado dentro de um par de
aspas (por exemplo "'").

.pp
Dentro de um par de aspas (""), ocorre a substituição de
parâmetros e comandos, mas sobre o resultado a
"shell" não faz interpretação de brancos, nem geração de nomes de arquivos.
Se "$*" está entre um par de aspas ("$*"), os parâmetros
posicionais são substituídos e formam uma única palavra,
considerada como quotada, com um espaço entre cada par de
parâmetros ("$1 $2 $3 ...").
Uma barra invertida ('\') retira
o significado especial dos caracteres '\', ''', '"' e '$'.
O par "\<nl>" é removido antes da substituíção de parâmetros
e comandos.
A barra invertida só tem o significado especial em vigor
quando precede os caracteres anteriores.

.pp
Assim, a saída de

.sp
.in +5
$ echo \?
.in -5
.sp
será o caractere "?"

.sp
e a de
.sp
.in +5
$ echo xx'||||'xx
.in -5
.sp
será
.sp
.in +5
xx||||xx
.in -5
.sp 2

.sp 4
.b
3.4 - Substituição de Comandos
.r

.pp
A saída padrão de um comando que aparece entre um par de acentos graves (``)
pode ser usada como parte, uma, ou mais palavras. Assim, o comando

.sp
.in +5
$ grep -l driver `walk -s .`
.in -5
.sp
faz com que a lista de arquivos produzida pelo comando "walk" seja
utilizada pelo comando "grep".

.sp
Um comando útil para a utilização com os acentos graves é o "from",
como no exemplo abaixo:

.sp
.nf
	for i in `from 1 to 10`
	do
		echo $i
	done
.fi

.sp
em que o comando "echo" é executado 10 vezes, com os valores consecutivos
de "1" a "10" para a variável "i".

.pp
Não é realizada qualquer interpretação dos caracteres da cadeia
lida, exceto para a remoção de barras invertidas ('\'), usadas para
desabilitar o significado especial de caracteres.
Uma barra invertida ('\') pode ser usada para desabilitar o significado
especial de um acento grave ou outra barra invertida ('\') e
é removida antes que a cadeia de caracteres seja lida.

.pp
Se uma barra invertida ('\') é usada para anular o significado especial
de um <nl>, tanto a barra invertida quanto o <nl> serão
removidos.
Em adição, barras invertidas usadas para anular o significado do
cifrão ('$') são removidos. Já que não é realizada uma
interpretação na cadeia de caracteres do comando antes que ele
seja lido, a inserção de uma barra invertida antes de um cifrão
não tem efeito. Barras invertidas que precedam caracteres
outros que '\', '`', '"', <nl> e '$' são mantidos intactos quando
a cadeia de caracteres do comando é lida.
