.bp
.he 'SH (cmd)'TROPIX: Manual de Referência'SH (cmd)'
.fo 'Atualizado em 24.03.04'Versão 4.5.0'Pag. %'

.b NOME
.in 5
sh - interpretador de comandos programável
.br

.in
.sp
.b SINTAXE
.in 5
sh [-c <comando>] [-derv] [<arg> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "sh" é um interpretador de comandos programável que
executa comandos lidos de um terminal ou arquivo.
Os comandos devem ser dados em uma linguagem própria, que é descrita abaixo.

.sp
As opções são:

.in +3 
.ip "-c" 5
Executa o <comando> dado como argumento e encerra.

.ip "-d" 5
Aceita também <^D> para terminar o "sh". Normalmente, para
terminar a execução, devem ser usados os comandos internos
"exit", "login" ou "logout".

.ip "-e" 5
Termina a execução do "sh" imediatamente, se um comando retornar não-zero.

.ip "-r" 5
Verifica a reescrita de arquivos no redirecionamento.

.ip "-v" 5
Lista cada linha de comandos lida do arquivo de entrada.

.ep
.in -3 

.in
.sp 2
.b "DESCRIÇÃO DA LINGUAGEM"
.in 5

.in
.sp
.in 2
.b Definições
.in 5
.sp
Um "branco" é um <sp> ou um <ht>. Uma "palavra" é uma seqüência de
caracteres que não incluem brancos.
Um "nome" é uma seqüência de letras, dígitos, ou "sublinhado"s ("_")
iniciada por uma letra ou "sublinhado".
Um "parâmetro" é um nome, um dígito,
ou um dos caracteres "*", "@", "#", "?", "-", "$", "!", ":" e "\%".

.in
.sp
.in 2
.b Comandos
.in 5
.sp
Um "comando-simples" é uma seqüência de palavras separadas por brancos.
A primeira palavra especifica o nome do comando a ser executado.
Salvo exceções (que serão vistas abaixo), as palavras restantes são
passadas como argumentos ao comando invocado.
O nome do comando é passado como argumento de número 0 (veja "exec" (sys)).

.sp
Se um comando-simples terminar normalmente, o seu "valor de retorno"
é dado pelo argumento fornecido à chamada "exit" (sys).
Caso contrário, o seu "valor de retorno" é dado por 0x80 + código de retorno
(veja "wait" e "signal" (sys) para uma lista dos códigos de retorno).

.sp
Uma "linha de canais" ("pipeline") é uma seqüência de dois ou mais comandos
separados por "|".
Um "pipeline" é construído conectando-se a saída padrão de cada comando
(com a exceção do último), à entrada padrão do comando seguinte
através da chamada ao sistema "pipe" (sys).

Cada comando é executado como um processo independente;
"sh" espera pelo término do último comando.
O valor de retorno de um "pipeline" é o valor de retorno do último comando.

.sp
Uma "lista de pipelines" (ou simplesmente uma "lista")
é uma seqüência de um ou mais "pipeline"s separados por ";", "&", "&&" ou "||"
e, opcionalmente, terminado por ";" ou "&".

Destes quatro símbolos, ";" e "&" têm a mesma precedência,
que é menor do que as precedências de "&&" e "||".
Os símbolos "&&" e "||" também têm a mesma precedência.

.sp
Um ponto e vírgula (";") causa a execução seqüencial do "pipeline"
precedente; um "e comercial" ("&") provoca uma execução assíncrona do
"pipeline" precedente (isto é, "sh" não espera pelo término daquele "pipeline").

.sp
O símbolo "&&" ("||") faz com que a lista que o
segue seja executada apenas se o "pipeline" anterior tiver
um valor de retorno igual a zero (diferente de zero).
Um número arbitrário de <nl> pode aparecer em
uma lista, ao invés de ponto e vírgulas, para delimitar comandos.

.sp
Um "comando" é uma lista, um "comando-interno" ou
um dos comandos-de-programação descritos adiante
(veja "comandos internos" e "comandos de programação", abaixo).
A menos que seja estabelecido de outro modo, o valor de retorno de
um comando é aquele do último comando-simples executado.

.sp
.in 2
.b
Comandos-de-Programação
.r
.in 5

.sp
for <nome> [in <palavra> ...] do <lista> done
.br

.in +8
.sp
Este comando executa a <lista> para cada <palavra> dada.
Em cada execução, <nome> recebe o valor da <palavra> seguinte.

.bc	/*************************************/
atribuindo <nome> com o valor da respectiva <palavra>. 
.ec	/*************************************/

Se a parte "in <palavra> ..." for omitida, a <lista> será executada
para cada "parâmetro posicional" definido
(veja "substituição de parâmetros", abaixo).

.in -8
.sp
.(l
case <palavra> in
    [<padrão> [|<padrão>] ...) <lista> ;;] ...
esac
.)l
.in +8
.br

.sp
O comando "case" executa a <lista> associada ao primeiro <padrão> que
seja aceito (tenha correspondência) com a <palavra> dada.
A forma dos <padrões> é a mesma que é usada para a geração de nomes de arquivos
(veja "geração de nomes de arquivos"),
exceto que uma barra ("/'), um ponto (".") inicial,
ou um ponto imediatamente após
uma barra não precisam ser aceitos explicitamente.

.in -8
.sp
.(l
if <lista> then <lista>
    [ elif <lista> then <lista> ] ...
    [ else <lista> ]
fi
.)l
.br

.in +8
A <lista> após o "if" é executada,
e caso tenha um valor de retorno igual a zero,
é executada a <lista> após o "then".
Em caso contrário, a <lista> após o "elif" (se dada) é executada,
e caso tenha um valor de retorno igual a zero,
é executada a <lista> após o "then" seguinte.
Falhando todos os "elif"s, é executada a <lista>
após o "else" (se dada).
Caso não seja executada nenhuma <lista> de "else" ou "then",
o comando termina com um valor de retorno diferente de zero.

.in -8
.sp
.(t
while <lista> do <lista> done
.sp
until <lista> do <lista> done
.br

.sp
.in +8
O comando "while" executa repetidamente a primeira e a segunda <lista>.
Se o valor de retorno do último comando da primeira <lista> for
diferente de zero, a segunda <lista> não é executada e a malha termina.
Se nenhum comando da segunda <lista> for executado, o comando
"while" termina com o valor de retorno igual a zero.
A palavra chave "until" pode ser utilizado ao invés de "while"
para negar o teste de término de repetição.
.)t
.in -8

.sp
( <lista> )
.sp
.in +8
Cria um processo filho "sh" para executar a <lista>. 
.in -8

.sp
{ <lista>; }
.in +8
.sp
Executa a <lista> neste "sh".
.in -8

.sp
As seguintes palavras são sempre consideradas reservadas:
.sp
.(l
    if then else elif fi case esac for while until do done { }
.)l

.sp
Se, em algum momento, for necessário utilizá-las de outro
modo, elas devem ser incluídas entre aspas ou apóstrofos.

.in
.sp
.in 2
.b Comentários
.in 5
.sp
Todos os caracteres após o "#" até o primeiro <nl> posterior são ignorados.

.in
.sp
.in 2
.b
Substituição de Comandos
.r
.in 5
.sp
O comando "sh" lê comandos de uma cadeia de caracteres entre dois acentos
graves (`...`), executa estes comandos, e a sua saída padrão
pode ser usada como parte integral ou parcial de uma palavra.
Os caracteres <nl> da saída padrão são removidos.

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

.sp
Não é realizada qualquer interpretação dos caracteres da cadeia
lida, exceto para a remoção de barras invertidas ("\"), usadas para
desabilitar o significado especial de caracteres. Uma barra invertida ("\")
pode ser usada para desabilitar o significado especial de um acento
grave ou outra barra invertida ("\") e é removida antes que a cadeia
de caracteres seja lida.
Se a substituição de comandos encontra-se entre um par de aspas
.wo '(" ... ` ... ` ... "),'
então uma barra invertida usada para desabilitar o significado especial
de uma aspas será removida; em outros casos ela será mantida intacta.


.bc	/*************************************/
A desabilitação do significado especial de
acentos graves permite o aninhamento de substituições de comandos.
.ec	/*************************************/


.sp
Se uma barra invertida ("\") é usada para anular o significado especial
de um <nl>, tanto a barra invertida quanto o <nl> serão
removidos (veja a seção posterior sobre "mecanismos de quotação").
Além disto, barras invertidas usadas para anular o significado do
sinal de dollar ("$") são removidos. Já que não é realizada nenhuma
interpretação na cadeia de caracteres do comando antes que ele
seja lido, a inserção de uma barra invertida antes de um sinal de
dollar não tem efeito. Barras invertidas que precedam caracteres
outros que "\", "`", '"', <nl> e "$" são mantidos intactos quando
é lida a cadeia de caracteres do comando.

.in
.sp
.in 2
.b
Substituição de Parâmetros
.r
.in 5
.sp
O caractere '$' é usado para introduzir parâmetros substituíveis.
Temos dois tipos de parâmetros: posicionais e de palavras-chaves.
Se um parâmetro é um dígito, ele é um parâmetro posicional.
Podemos atribuir valores aos parâmetros posicionais com o comando
"set".
Atribuimos valores a parâmetros de palavras-chaves (também chamadas
de variáveis) escrevendo:

.sp
.in +5
    <nome>=<valor> [<nome>=<valor> ...]
.in -5

.sp
A expansão de nomes não é realizada em <valor>.
.bc	/*************************************/
NÃO podemos ter uma função e uma variável com o mesmo nome.
.ec	/*************************************/

.ip "${<parâmetro>}" 16
O valor, se algum, do <parâmetro> é substituído.
As chaves só são necessárias se o nome do <parâmetro>
for seguido por uma letra, dígito, ou "sublinhado" ('_'),
que não deve ser interpretado como parte do nome.

.ep

.sp
Os seguintes parâmetros têm seus valores automaticamente
atribuídos pelo comando "sh":

.in +3
.ip "#" 3
O número de parâmetros posicionais (em decimal).

.ip "-" 3
Opções dadas ao comando "sh" ou dadas através do comando "set".

.ip "?" 3
O valor de retorno em decimal retornado pelo último comando
executado sincronamente.

.ip "$" 3
A identifiçação do processo (PID) deste "sh".

.ip "!" 3
A identifiçação do processo (PID) do último comando em "background" executado.

.ip ":" 3
O número do próximo comando a entrar no histórico (veja "histórico", abaixo).

.ip "@" 3
Os parâmetros posicionais (veja adiante) à exceção de $0.

.ip "*" 3
Uma cadeia com os parâmetros posicionais à exceção de $0.

.ip "\%" 3
Uma cadeia com o nome do nó (nome do próprio computador na rede).

.ip "~" 3
O nome do diretório corrente.

.ip "&" 3
O caractere "#" se o usuário for um superusuário, e "$" em caso contrário.

.in -3
.ep

.sp
.(t
Os seguintes parâmetros são utilizados pelo comando "sh":

.in +3
.ip "HOME" 6
O diretório "home", o argumento assumido por omissão para o comando "cd".
O caractere '~' é equivalente a $HOME, quando ocorre em caminhos.
.)t

.ip "PATH" 6
Os caminhos de busca para os comandos (veja execução abaixo).

.ip "PS1" 6
A cadeia de caracteres com o "prompt", normalmente "$ ".

.ip "PS2" 6
A cadeia de caracteres com o "prompt" secundário, normalmente "> ".

.bc	/*************************************/
.ip "IFS" 6
Separadores de campos utilizados pelo comando "sh",
normalmente  <sp>, <ht> e <nl>.
.ec	/*************************************/

.in -3
.ep

.sp
A 
O comando "sh" atribui os valores iniciais para as variáveis
"PATH", "PS1" e "PS2".
O valor inicial de "HOME" é atribuído pelo comando "login".

.in
.sp
.in 2
.(t
.b
Interpretação de brancos
.r
.in 5
.sp

Após a substituição de parâmetros e de comandos, o resultado da substituição
é examinado à procura de caracteres separadores de campos
e separado em argumentos distintos
onde estes são encontrados.
Argumentos explicitamente nulos ("" ou '') são mantidos.
Argumentos nulos implícitos (aqueles decorrentes de parâmetros sem valor)
são removidos.
.)t

.in
.sp
.in 2
.b
Redirecionamento de Entrada e Saída
.r
.in 5
.sp
As entradas e saídas de um comando podem ser redirecionadas
usando uma notação especial interpretada pelo comando "sh".
As construções apresentadas a seguir
podem aparecer em qualquer lugar de um comando-simples,
e não são passados como argumentos ao comando invocado.
Note que a substituição de comandos e parâmetros é realizada
antes que <nome> ou <dígito> seja usado.

.in +3
.ip "< <nome>" 12
Usa o arquivo <nome> como a entrada padrão
(relativo ao descritor de arquivos 0).

.ip "> <nome>" 12
Usa o arquivo <nome> como a saída padrão
(relativo ao descritor de arquivos 1).
Se o arquivo não existe ele é criado; em caso contrário é truncado.

.ip ">> <nome>" 12
Usa o arquivo <nome> como saída padrão.
Se o arquivo já existe a saída é adicionada ao seu final;
em caso contrário o arquivo é criado.

.ip "<< <nome>" 12
Após a substituição de parâmetros e de comandos ser realizada em <nome>,
a entrada do comando "sh" é lida até a primeira linha que corresponda
literalmente ao <nome> resultante, ou até um fim de arquivo.

.ip "< &<dígito>" 12
Usa o arquivo associado ao descritor de arquivos dado pelo <dígito>
como a entrada padrão.

.ip "> &<dígito>" 12
Usa o arquivo associado ao descritor de arquivos dado pelo <dígito>
como a saída padrão.

.ip "<&-" 12
Fecha a entrada padrão.

.ip ">&-" 12
Fecha a saída padrão.

.in -3
.ep

.sp
Se qualquer uma das cadeias acima for precedida por um dígito,
o descritor de arquivos que será associado ao arquivo é aquele
dado pelo dígito (ao invés dos valores 0 ou 1 assumidos por omissão).
Por exemplo:

.sp
.in +8
 ... 2>&1
.in -8

.sp
associa o descritor de arquivos 2 ao arquivo correntemente associado
com o descritor de arquivos 1.

.sp
A ordem em que são dadas as redireções é significante;
o comando "sh" avalia as redireções da esquerda para direita.
Por exemplo:

.sp
.in +8
 ... 1> <arquivo> 2>&1
.in -8
.sp

em primeiro lugar associa o descritor de arquivos 1 ao <arquivo>;
em seguida associa o descritor de arquivos 2 ao arquivo associado
ao descritor de arquivos 1 (isto é <arquivo>).

.sp
Se a ordem de redireções fosse a reversa, o descritor de arquivos 2
seria associado ao terminal/vídeo (assumindo que o descritor de arquivos
1 o tinha sido) e em seguida, o descritor de arquivos 1 seria associado
ao <arquivo>.

.sp
Usando a terminologia introduzida na primeira página, em "comandos",
se um comando é composto de vários comandos-simples, as redireções
são avaliadas para o comando como um todo, antes que sejam avaliadas
para cada comando-simples.
Em outras palavras, o comando "sh" avalia os redirecionamentos
para a lista inteira, em seguida para cada "pipeline" dentro da lista e
finalmente para cada lista dentro de cada comando.

.sp
Se um comando é seguido por "&", a entrada padrão para o
comando será o arquivo vazio "/dev/null".
Em caso contrário, o ambiente para a execução de um comando
contém os descritores de arquivos do comando "sh"
que o invocou, segundo as modificações dadas pelas
especificações de entrada e saída.

.in
.sp
.in 2
.b
Geração de Nomes de Arquivos
.r
.in 5
.sp
Antes que um comando seja executado, cada palavra do comando é
examinada à procura dos caracteres "*", "?", "!" e "[".
Se algum destes caracteres estiver presente, a palavra é considerada
um padrão.
A palavra é substituída pela lista ordenada alfabeticamente
de nomes de arquivos que correspondem ao padrão dado. Se não for
encontrado nomes de arquivos que correspondam ao padrão, o comando
"sh" indica o erro. O caractere "." no início de um nome de arquivo
ou imediatamente após uma barra ("/"), assim como o caractere barra
devem corresponder explicitamente ao padrão.

.in +3
.ip ? 8
Corresponde a (um) caractere qualquer.

.ip * 8
Corresponde a zero ou mais caracteres quaisquer.

.ip !c 8
Corresponde a qualquer caractere diferente de "c".

.ip [...] 8
Corresponde a um caractere igual a um dos caracteres entre os colchetes.
Podemos também ter construções do tipo "x-y"
entre os colchetes,
que representa todos os caracteres de "x" a "y" (inclusive). 

.ip ![....] 8
Corresponde a um caractere que deve ser diferente de todos os caracteres
entre os colchetes. Assim como acima, "x-y" representa todos os
caracteres de "x" a "y".

.ip \c 8
Corresponde ao caractere "c". Esta construção é útil para desfazer
o efeito especial dos meta-caracteres.

.in +3
.ep

.bc	/*************************************/
.sp
.in +5

.ip "*" 8
Corresponde a qualquer cadeia de caracteres, inclusive a cadeia nula.

.ip "?" 8
Corresponde a qualquer (único) caractere.

.ip "[...]" 8
Corresponde a qualquer um dos caracteres entre os colchetes.
Um par de caracteres separados por "-" corresponde a qualquer
caractere lexicamente entre o par (incluindo o primeiro o último).
Se o primeiro caractere após o abre colchete for um "!" qualquer caractere
não especificado entre os colchetes é correspondido.

.ip "!" 8
Corresponde a qualquer caractere a exceção do caractere especificado
após.
.ep
.in -5
.ec	/*************************************/

.in
.sp
.in 2
.b
Mecanismos de Quotação
.r
.in 5

.sp
Os seguinte caracteres têm um significado especial para o comando "sh"
e causam o fim de uma palavra a menos que estejam quotados:
.sp
.in +5
.b
;  &  (  )  |  <  >  <nl>  <sp>  <ht>
.r
.in -5

.sp
Um caractere pode ser "quotado"
(isto é, fazer com que perca o seu significado especial)
precedendo-o por uma barra invertida ("\") ou inserindo-o entre um par de
marcas de quotação ('' ou "").
Durante o processamento, o comando "sh" pode quotar alguns caracteres
para evitar que eles tenham um significado especial.
Barras invertidas usadas para quotar um caractere são removidas
da palavra antes que o comando seja executado.
O par "\<nl>" é removido antes da substituição de comandos e parâmetros.

.sp
Todos os caracteres entre um par de acentos agudos (''), exceto
um acento agudo, são quotados pelo comando "sh".
Uma barra invertida não tem um significado especial dentro de um
par de acentos agudos.
Um acento agudo pode ser quotado dentro de um par de
aspas (por exemplo "'").

.sp
Dentro de um par de aspas (""), é realizada a substituição de
parâmetros e comandos, mas sobre o resultado o comando "sh"
não faz interpretação de brancos, nem geração de nomes de arquivos.
Se "$*" está entre um par de aspas ("$*"), os parâmetros
posicionais são substituídos e formam uma única palavra,
considerada como quotada, com um espaço entre cada par de
parâmetros ("$1 $2 $3 ...").
No entanto, se "$@" estiver entre um par de aspas ("$@"),
os parâmetros posicionais são
substituídos e quotados separados por espaços
("$1" "$2" "$3" ...).
Uma barra invertida ("\") retira
o significado especial dos caracteres "\", "'", '"' e "$".
O par "\<nl>" é removido antes da substituíção de parâmetros
e comandos.
Se a barra invertida precede caracteres diferentes de "\", "'", '"' e "$",
ela própria é quotada pelo comando "sh".

.in
.sp
.in 2
.(t
.b '"Prompt"'
.in 5

.sp
Quando usado interativamente, o comando "sh" mostra o "prompt" dado
pelo valor da variável "PS1", antes de ler o comando.
Se a qualquer tempo um <nl> for teclado e for
necessária mais entrada para completar o comando, o "prompt"
secundário (isto é, o valor de "PS2") é exibido.
.)t

.in
.sp
.in 2
.b Ambiente
.in 5

.sp
O ambiente (veja environ (fmt)) é uma lista de pares (<nome>, <valor>)
que é passada a um programa executado, do mesmo modo que a lista de
argumentos.
O comando "sh" interage com o ambiente de vários modos.
Na sua invocação, o comando "sh" examina o ambiente,
e cria uma variável para cada <nome> encontrado,
associando-a ao <valor> respectivo.
Se o usuário modificar o <valor> de qualquer dessas variáveis ou
criar novas variáveis, nenhuma destas afeta o ambiente a menos
que o comando "export" seja utilizado para direcionar a variável
do comando "sh" para o ambiente (veja também "set -a").
Uma variável pode ser removida do ambiente através do comando "unset".
O ambiente recebido por qualquer comando executado é então composto dos
pares (<nome>, <valor>) não modificados, originalmente herdado pelo
comando "sh", menos os pares removidos pelo comando "unset",
mais quaisquer modificações ou adições,
realizadas através do comando "export".

.bc	/*************************************/
.sp
O ambiente para qualquer comando-simples pode ser acrescido
prefixando por uma ou mais atribuições a variáveis. Logo:
.sp
.in +5
$ TERM=450 cmd
.wo "                         e"
.br
$ (export TERM; TERM=450; cmd)
.in -5
.sp
são equivalentes no que concerne a excecução de "cmd".

.sp
Se a opção "-k" está ativa, todas as variáveis são colocadas no
ambiente dos comandos, mesmo que elas ocorram após o nome do comando.
Por exemplo:
.sp
.in +5
$ echo a=b c
.br
$ set -k
.br
$ echo a=b c
.sp
mostra "a=b c" e então "c".
.ec	/*************************************/

.in
.sp
.in 2
.b Sinais
.in 5

.sp
Os sinais SIGINT e SIGQUIT (veja "signal" (sys))
para um comando executado são ignorados
se o comando for seguido de um "&";
os demais sinais têm seus valores herdados pela
comando "sh" do seu pai, com a exceção do sinal SIGBUS
(veja o comando "trap" abaixo).

.in
.sp
.in 2
.b Execução
.in 5

.sp
Para cada comando executado, são efetuadas as substituições dadas acima.
Se o nome do comando corresponde a um dos comandos-internos
dados abaixo, ele é executado no processo do próprio comando "sh"
(ao invés de criar um comando "sh" filho).

.bc	/*************************************/
Se o nome do comando não corresponde ao nome de um comando-interno nem
o nome de uma função definida, um novo processo é criado
e uma tentativa é efetuada para executar o comando via exec (sys).
.ec	/*************************************/

.sp
A variável "PATH" do comando "sh" define os caminhos
dos diretórios onde os comandos serão procurados.
Os nomes dos diversos diretórios devem vir separados por dois-pontos (":").
O "PATH" padrão é ".:/bin:/usr/bin:/etc" (especificando o diretório
corrente ".", "/bin", "/usr/bin" e "/etc", nesta ordem).

.sp
Se o nome do comando começar por "/", trata-se de um nome absoluto,
e a variável "PATH" não é utilizada.
Em caso contrário, cada diretório dado em "PATH" é examinado à procura
de um arquivo executável com o nome do comando.
Se o arquivo tem permissão de execução,
mas não é um arquivo com o formato "a.out"
(veja o arquivo "/usr/include/a.out.h"),
assume-se que seja um arquivo contendo comandos para o "sh",
ou seja, um procedimento de comando "sh".
Neste caso, um "sh" filho é criado para processá-lo.
Um comando entre parênteses também é executado por um comando "sh" filho.

.sp
Por questão de eficiência, o "sh" mantém internamente uma tabela HASH
contendo os nomes de todos os arquivos executáveis encontrados nos diretórios
especificados no conteúdo da variável PATH. Assim, se um destes diretórios
for modificado através da inclusão ou exclusão de algum
executável, o comando "rehash" deve ser utilizado, a fim de manter
atualizada a estrutura de dados interna do "sh".

.in
.sp
.in 2
.b
Histórico
.r
.in 5
.br
O comando "sh" armazena em um "histórico", um certo número
dos últimos comandos executados.
Com isto, é possível re-executar comandos executados anteriormente,
inclusive com alterações.
Os comandos internos que gerenciam o hístórico são "h", "eh" e "xh"
(veja abaixo).
O número de comandos armazenados no histórico é dado pela variável
"HIST".

.in
.sp
.in 2
.b
Comandos Internos
.r
.in 5
.br

.sp
Os comandos internos são executados pela "shell" sem a criação de um
subprocesso.
O redirecionamento de entrada/saída também é permitido para os
comandos-internos.

.sp
.in +3
.ip ":"
.sp
Sem efeito; este comando nada faz.
Produz um valor de retorno igual a zero.

.ip ". <arquivo>"
.sp
Lê e executa os comandos do <arquivo> e retorna.
Os caminhos de busca especificados em "PATH" são utilizados
para achar o diretório contendo <arquivo>.

.ip "cd [<dir>]"
.sp
Troca o diretório corrente para "<dir>".
Se <dir> não for dado, troca o diretório corrente para o valor
da variável "HOME".

.ip "eh [<num> ...]"
.sp
Permite a edição do comando <num> do histórico.
Se forem dados mais de um <num>, permite a junção, edição e execução
de vários comandos do histórico.
Se <num> não for especificado, edita o último comando do histórico.
Os comandos de edição são os do editor visual "ex" (cmd) que fazem
sentido para a edição de uma única linha.
Os comandos do editor e a edição terminam respectivamente por <esc> ou "q".

.ip "eval [<arg> ...]"
.sp
Os argumentos são lidos como entrada para o comando "sh"
e os comandos resultantes executados.

.ip "exec [<arg> ...]"
.sp
O comando especificado pelos <arg>s é executado no lugar
deste comando "sh", sem que seja criado um novo processo.
Podem ser dados redirecionamentos de entrada e saída,
e se nenhum outro <arg> for dado,
causa a modificação da entrada/saída do comando "sh".

.ip "exit [<n>]"
.sp
Termina a execução do presente comando "sh",
com o valor de retorno <n>.
Se <n> for omitido, o valor de retorno é o do último comando executado.
De acordo com a opção "-d", um fim de arquivo (normalmente <^D>) também
pode terminar o "sh" (veja o comando-interno "set").

.ip "export [<nome> ...]"
.sp
Os <nome>s dados são marcados para a exportação automática para
o ambiente dos comandos executados posteriormente.
Se não forem dados argumentos, são listados os <nome>s das variáveis
que são exportáveis.

.ip "false"
.sp
Produz um valor de retorno diferente de zero
(para ser usado em conjunto com os comandos-de-programação).

.ip "gh"
.sp
O histórico é  mostrado na tela e o usuário poderá selecionar o
comando desejado através das setas. Escolhido o comando, três opções são
possíveis: <enter> para executar o comando, "e" para editar o comando ou
"q" para desistir.

.ip "h"
.sp
Mostra o histórico com os últimos $HIST comandos executados.

.ip "login"
.sp
Termina a sessão corrente do comando "sh" (veja o comando-interno "exit").

.(t
.ip "read [<nome> ...]"
.sp
Uma linha é lida da entrada padrão e, usando os separadores
de campo internos (<sp> ou <ht>), para
delimitar as fronteiras entre as palavras, a primeira palavra
é atribuída ao primeiro <nome>, a segunda palavra ao segundo <nome>,
e assim sucessivamente.
As palavras que sobrarem são todas atribuídas ao último <nome>.
Qualquer caractere pode ser quotado precedendo-o por barra invertida
("\"), inclusive <nl>. O valor de retorno é zero, a menos que
seja encontrado um fim de arquivo.
.)t

.ip "readonly [<nome> ...]"
.sp
Os <nome>s dados são considerados como variáveis apenas para a leitura,
e os seus valores não podem ser alterados por atribuições posteriores.
Se nenhum argumento for dado, é impressa uma lista de
todas as variáveis "readonly".

.ip "set [-derv] [<arg> ...]"

.sp
Altera alguns indicadores do "sh":

.in +7
.ip "-d" 4
Aceita também <^D> para terminar o "sh". Normalmente, para
terminar a execução do presente "sh", deve-se usar os comandos-internos
"exit", "login" ou "logout.

.ip "-e" 4
Termina a execução do "sh" imediatamente, se um comando retornar não-zero.

.ip "-r" 4
Verifica a reescrita de arquivos no redirecionamento.

.ip "-v" 4
Lista cada linha de comandos lida do arquivo de entrada.

.ep
.sp
Com "+" ao invés de "-", os indicadores são desligados.
Este indicadores também podem serem usados na invocação do "sh".
O estado atual destes indicadores pode ser consultada através de "$-".
Os <arg>s dados são valores atribuídos aos parâmetros posicionais
"$0", "$1", ... Se não forem dados argumentos, são impressos os
valores de todos os nomes.

.in -7

.ip "shid"
.sp
Imprime a versão do comando "sh".

.(t
.ip "shift [<n>]"
.sp
Os parâmetros posicionais $<n+1> ... recebem os novos nomes $1 ...
Se <n> não for dado, assume-se o valor 1.
.)t

.ip "time <arg> ..."
.sp
Marca e mostra o tempo de execução do comando formado pelos <arg>s
dados.

.ip "times"
.sp
Mostra os tempos de usuário e sistema acumulados para os processos
executados pelo comando "sh".

.ip "trap [<arg>] [<n> ...]"
.sp
O comando <arg> é lido e executado quando o comando "sh"
receber o(s) sinal(is) de número(s) <n> ...
(note que <arg> é examinado quando o comando
é dado e quando o sinal é recebido).
Os comandos dados são executados na ordem dos números dos sinais.
Qualquer tentativa para interceptar a recepção de um sinal
que estava sendo ignorado na invocação do comando "sh" é ineficaz.
Se <arg> não for dado, então os "traps" dos números dados
são restaurados aos seus valores originais.
Se <arg> é uma cadeia de caracteres nula, então os sinais relacionados
são ignorados pelo comando "sh" e pelos comandos que ele invocar.
Se <n> for zero, então o comando especificado
é executado ao fim da execução do comando "sh".
Se não forem dados <arg>s, "trap" lista os comandos associados a cada sinal
(veja "signal" (sys)).

.ip "true"
.sp
Produz o valor de retorno igual a zero
(deve ser usado em conjunto com os comandos-de-programação).

.ip "umask [<nnn>]"
.sp
A máscara de criação de arquivos do usuário é modificada para <nnn>
(veja umask (sys)). Se <nnn> for omitido, é impresso o valor corrente.
Todos os valores <nnn> são dados em octal.

.(t
.ip "unset [<nome> ...]"
.sp
Para cada <nome> dado, remove a variável correspondente.
As variáveis "PATH", "PS1" e "PS2" não podem ser removidas.
.)t

.(t
.ip "xh [<num>]"
.sp
Reexecuta o comando de número <num> do histórico.
Assume por omissão o último comando.
.)t

.ip "wait [<n>]"
.sp
Espera pelo processo assíncrono (em "background") cuja identificação
(PID) é <n>, e imprime seu valor de retorno.
Se <n> for omitido, todos os processos assíncronos correntemente
ativos são esperados e o valor de retorno será zero.

.in -3
.ep

.in
.sp
.in 2
.b
Invocação
.r
.in 5
Se um comando "sh" é invocado através de "exec" (sys)
e o primeiro caractere do argumento zero é "-",
então inicialmente o comando "sh"
lê comandos do arquivo "/etc/.profile" e de "$HOME/.profile
(caso estes arquivos existam).
A partir deste ponto, o comando "sh" lê comandos como descrito anteriormente,
que é também o caso quando o comando "sh" é invocado como "/bin/sh".
Ao término da execução do comando "sh", ele
executa os comandos de "$HOME/.logout" se o primeiro caractere
do argumento zero for "-".

.bc	/*************************************/
.sp
As opções a seguir são interpretadas pelo
comando "sh" apenas na sua invocação.
Note que, a menos que a opção "-c" seja dada, assume-se
que o primeiro argumento é o nome de um arquivo
contendo comandos, e os demais argumentos passados como parâmetros
posicionais daquele arquivo de comandos.

.in +3
.ip "-c <linha>"
São lidos e executados comandos da cadeia <linha>.

.in -3
.ep

.sp
As opções restantes são descritas na parte referente
ao comando-interno "set".
.ec	/*************************************/

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Erros detetados pelo comando "sh" (como erros de sintaxe),
acarretam um valor de retorno diferente de zero do "sh".
Se o comando "sh" está sendo utilizada não interativamente,
a execução do arquivo passado ao comando "sh" é abandonada.
Em caso contrário, a comando "sh" devolve o valor de retorno
do último comando executado (veja também o comando "exit" acima).

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
echo, intro, login, pwd, test, ex
.br
.wo "(sys): "
dup, exec, fork, pipe, signal, umask, exit
.br
.wo "(libc):"
edit
.br
.wo "(fmt): "
environ
.br
.)t

.in
.sp
.(t
.b ARQUIVOS
.in 5
/etc/.profile
 $HOME/.profile
 $HOME/.logout
 /tmp/sh*
 /dev/null
 /usr/include/a.out.h
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo

.in
