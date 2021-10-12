.bp 1
.he 'CPP (cmd)'TROPIX: Manual de Referência'CPP (cmd)'
.fo 'Atualizado em 24.08.01'Versão 4.1.0'Pag. %'

.b NOME
.in 5
cpp - préprocessador para fontes em "C"

.in
.sp
.b SINTAXE
.in 5
/lib/cpp [-PCTv] [-D <def> ...] [-U <id> ...] [-I <dir> ...]
.nf
					 <entrada> [<saída>]
.fi

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cpp" é o préprocessador para programas fontes na linguagem
"C" do TROPIX.
Usualmente, ele é invocado automaticamente
através do comando "cc" (cmd), 
como o primeiro passo de uma compilação.
O comando também pode ser utilizado separadamente, mas neste caso,
deve-se levar em conta o formato do arquivo de saída.

.sp
O argumento <entrada> é o nome do arquivo de onde o comando irá obter
a entrada.
Se o segundo argumento <saída> for fornecido, ele será utilizado como
nome do arquivo de saída; em caso contrário, a saída será posta na saída padrão.

.sp
Os diretórios implícitos de busca dos arquivos "#include" são:
"/usr/include", "/usr/xwin/include" e "/usr/xwin/include/X11" (veja a opção "-I").

.sp
Nesta versão do sistema, foi removido o limite do tamanho dos identificadores,
isto é, os nomes das macros, variáveis, ... podem ter tamanhos arbitrários.

.sp
As opções do comando são:

.in +3
.ip -P 10
Normalmente, o préprocessador gera uma série de informações de controle,
que são esperadas pelo compilador "C"
para acompanhar a origem de cada linha do programa fonte.
Com esta opção, esta informação não é gerada.

.ip -C 10
Normalmente, o préprocessador suprime todos os comentários
(no estilo da linguagem "C") na saída.
Com esta opção, os comentários são incluídos na saída.

.ip -T 10
Emite mais diagnósticos.

.ip -v 10
Verboso. Ao final do préprocessamento, imprime uma tabela com
as macros definidas.

.ip '-D <def>' 10
Prédefine macros;
<def> é uma definição de macro do tipo "identificador" ou
"identificador=valor"
(no primeiro caso, a macro é definida com valor "1").

.sp
A opção "-D" tem precedência mais alta do que as definições de
macros através de "pragma getenv" (ver abaixo), isto é, se a mesma macro for
definida dos dois modos, a que prevalece é a dada pela opção "-D".

.ip '-U <id>' 10
Não permite macros com o nome <id>.
Mesmo que hajam definições da macro <id>
no decorrer do texto, elas são ignoradas.

.ip '-I <dir>' 10
Acrescenta <dir> à lista de diretórios de busca de arquivos "#include"
cujos nomes não começam por "/".

.sp
Arquivos "#include" cujos nomes estão cercados por "" são 
procurados em primeiro lugar no diretório do arquivo que contém
a diretiva "#include", em seguida nos diretórios dados pelas
opções "-I" e finalmente nos diretórios implícitos (veja acima).

.sp
Para arquivos "#include" cujos nomes estão cercados por <>, a
busca é análoga, exceto que o diretório do arquivo que contém
a diretiva "#include" não é consultada.

.ep
.in -3

.sp 2
A diretiva "#pragma" reconhecida nesta versão de "cpp" é apenas o "getenv".
Através desta diretiva podemos definir variáveis do ambiente como macros
(ver ("environ" (fmt)).
Ela pode ser usado em duas formas:
.sp
.nf
		#pragma getenv (<id1>, <id2>, ...)
.fi
.sp
em que são definidas as variáveis <id1>, <id2>, ... e
.sp
.nf
		#pragma getenv
.fi
.sp
em que todas as variáveis do ambiente são definidas como macros.

.sp
.in
.b
DIAGNÓSTICOS
.r
.in 5
As incorreções são classificadas em "erros" e "advertências".
Para cada incorreção é impressa a linha, com um ponteiro
indicando o local provável da incorreção, seguida de uma
mensagem explicativa.
Para alguns tipos de incorreções são impressos apenas o número da linha
e nome do arquivo ao invés da linha propriamente dita. 

.sp
.in
.b
VEJA TAMBÉM
.r
.nf
     (cmd): cc
     (fmt): environ
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
