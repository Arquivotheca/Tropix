.bp
.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 5			     *
     *							     *
     *	       A SHELL COMO LINGUAGEM DE PROGRAMAÇÃO	     *
     *							     *
     *********************************************************

.fi

.sp 4

.pp
Além de seu uso como um interpretador interativo de comandos,
a "shell" pode ser usada também como linguagem de programação.
Duas de suas características como linguagem de programação já foram
abordadas: arquivos de comandos e variáveis.
Outras facilidades como as estruturas de controle tradicionais estão
disponíveis, possibilitando a criação de arquivos de comandos
("scripts"), que desempenham tarefas complexas. Estas características
incluem o seguinte:

.in +5
.ip "-"
Arquivos de comandos: um arquivo pode conter uma seqüência de comandos
para a execução posterior.

.ip "-"
Variáveis:
um arquivo de comandos pode ter declarações, atribuições e
expansões de variáveis.

.ip "-"
Argumentos:
um arquivo de comandos pode receber os argumentos passados
na linha de comando de sua evocação.

.ip "-"
Execução condicional:
a seqüência de comandos a executar pode variar,
podendo ser definida por condições externas (comandos if..then..else..elif).

.ip "-"
Seleção de casos:
uma entre várias seqüências de comandos pode ser
selecionada para a execução, dependendo de condições externas.

.ip "-"
Repetição enumerada:
é possível a repetição da execução de uma
seqüência de comandos, enumerando-se os valores de uma variável
de controle do laço.

.ip "-"
Repetição condicional:
é possível a repetição de uma seqüência de
comandos, enquanto persistir uma condição, ou até ocorrer uma
condição.

.ip "-"
Blocos de comandos:
uma seqüência de comandos pode ser agrupada e assim as mesmas condições
de execução podem ser especificadas para toda a seqüência.

.ip "-"
Tratamento de Exceções:
é possível explicitar quais ações devem ser realizadas quando
da ocorrência de uma exceção.

.ip "-"
Comentários:
textos explicativos podem ser inseridos como comentários em um
arquivo de comandos.

.ep

.in -5

.pp
Os comandos de programação e os blocos de comandos a serem
vistos a seguir, podem substituir comandos simples em uma lista de
comandos, podem sofrer redirecionamentos de entrada e saída como
um comando simples, ou podem ser executados em "background" como
um comando simples. Quando não especificado o valor de retorno
de um bloco de comandos ou de um comando de programação é o do
ultimo comando simples executado.


.sp 4

.b
5.1 - Passagem de Argumentos
.r

.pp
Como já visto em "Comandos" (capítulo 2), a chamada de um
comando é constituída por uma ou mais seqüências de caracteres.
A primeira indica o nome do comando a ser evocado, as próximas,
desde que iniciadas pelo caractere '-', indicam opções de
funcionamento do comando, ou seja o "modo" de execução do comando,
e as últimas indicam os objetos sobre os quais o comando vai agir.
Todas estas seqüências de caracteres posteriores ao nome do
comando são consideradas como argumentos do comando.

.pp
A "shell" permite que um arquivo de comandos
que precisa ser interpretado por uma "shell" para executar,
seja externamente indistinguível de um comando tradicional com um
programa objeto executável diretamente. Seguindo este objetivo
a "shell" passa os argumentos dados para um arquivo com comandos,
que pode acessá-los através dos parâmetros posicionais $0, $1, $2, ..., $9.
Os nomes dessas variáveis foram inspirados pelo vetor de argumentos
"argv" passados a primeira função a executar de um programa na
linguagem de programação "C".

.pp
Os parâmetros posicionais podem ser redefinidos pelo
comando interno "set" e reordenados pelo comando interno "shift" (capítulo 4).

.sp 4

.b
5.2 - Execução Condicional
.r

.pp
A determinação da execução ou não de listas de comandos pode ser
realizada a tempo de execução. Para tanto existem os comandos de
programação "if", "else", "elif" e "fi". A sua sintaxe é:

.sp
.b if
lista 
.b then
lista [

.b elif
lista 
.b then
lista]... [
.b else
lista]
.b fi
.br

.pp
A lista seguinte ao
.b if
é executada e, se ela tiver um valor de retorno igual a zero,
então a lista após o
.b then
é executada. Senão a lista após o
.b elif
se dada é executada e se o seu valor de retorno for zero a lista após o
próximo
.b then
é executada. Falhando a lista do 
.b elif
, a lista após o
.b else
é executada. Se nenhuma lista associada a um
.b then
ou a um
.b else
for executada o valor de retorno do
.b if
será zero.

.sp 4

.b
5.3 - Seleção de Casos
.r

.pp
Para a seleção de uma lista de comandos entre várias para
a execução a "shell" tem o comando "case". Sua sintaxe é:

.sp
.b case
palavra 
.b in
[padrão [|padrão] ...) lista ;;]
.b esac
.br

.pp
O comando
.b case
executa a lista associada ao primeiro padrão que
tenha correspondência com a palavra dada. A forma dos padrões
é a mesma que é usada para a geração de nomes de arquivos
exceto que uma barra ('/'),
um ponto ('.') inicial, ou um ponto ('.') imediatamente após
uma barra ('/') não precisam ser explicitados.

.sp 4

.b
5.4 - Repetição Enumerada
.r

.pp
A repetição enumerada permite a execução de uma lista de comandos
para cada um de uma lista de nomes dados. A cada iteração a variável
de controle assume um dos nomes dados e assim os comandos da lista
podem realizar uma determinada operação sobre ele. A sintaxe do
comando de repetição enumerada é:

.sp
.b for
nome [
.b in
palavra ...]
.b do
lista
.b done
.br

.pp
Este comando faz a repetição da execução da lista
dada, por um número de vezes definido a priori. A cada repetição
à variável identificada por "nome" é atribuído como valor a palavra
cuja posição entre as palavras dadas corresponde ao número da
iteração corrente. Se a parte "
.b in
palavra ..." não for dada será assumido como palavras dadas
os parâmetros posicionais (veja
.b
"Substituição de Parâmetros"
.r
adiante). A execução deste comando termina quando à "nome"
já houver sido atribuído todas as palavras dadas.


.sp 4

.b
5.5 - Repetição Condicional
.r

.pp
A "shell" tem construções de controle para a repetição de uma
lista de comandos enquanto persistir uma condição ("while"), ou
até que ocorra uma condição. Sua sintaxe é:

.sp

.b while
lista
.b do
lista
.b done
.br

.pp
Um comando
.b while
executa repetidamente a primeira e a segunda lista. Se o valor
de retorno do último comando da primeira lista for diferente de zero,
a segunda lista não é executada e a repetição
termina. Se nenhum comando da segunda lista for
executado, então o comando
.b while
retorna zero como valor de retorno . O comando
.b until
pode ser utilizado ao invés de
.b while
para negar o teste de término de repetição.
.in 0
.sp


.sp 4

.b
5.6 - Blocos de Comandos
.r

.pp
Uma lista de comandos pode formar um bloco, com o objetivo
de ser executada sob as mesmas condições, porém diferentes das dos
demais. Como exemplo destas condições podem ser citadas o redirecionamento
de entrada e saída, o ambiente, o tratamento de exceções, etc...

.pp
Um bloco de comandos pode ser executado pela própria "shell" corrente,
ou por uma "shell" "filha" criada especialmente para executá-lo.
Uma lista de comandos entre parênteses é executada por uma "shell" "filha",
enquanto uma terminada por ';' e entre chaves é
executada pela própria "shell" corrente.
Modificações das condições de execução da "shell" realizadas por
comandos componentes de um
bloco de comandos executado pela "shell" corrente mantêm-se para
a execução dos próximos comandos. Já estas modificações realizadas
por um bloco de comandos executados por uma "shell" filha só são
válidas durante a a existência da "shell" filha, ou seja durante a
execução do bloco.


.sp 4

.b
5.7 - Tratamento de Exceções
.r

.pp
A "shell" tem um mecanismo que permite especificar as ações
a serem realizadas quando da ocorrência de uma exceção.
São consideradas exceções a recepção de um sinal (veja signal (sys))
e o fim de arquivo da entrada padrão da "shell".

.pp
O comando interno "trap" posibilita a associação de uma linha
de comando para cada exceção. As exceções são especificadas pela sua 
constante inteira correspondente, por exemplo: a recepção de um sinal
de alarme de relógio provoca a exceção "SIGALRM". O fim de arquivo da
entrada padrão da "shell" corrente corresponde a exceção 0.
Mais de uma exceção pode ser associada a uma mesma linha de comando.
É possível especificar exceções que devem ser ignoradas e exceções
cujo tratamento deve ser restaurado ao estado inicial. Uma descrição
detalhada da sintaxe de "trap" está no capítulo 4.

.sp 6

.b
5.8 - Comentários
.r

.pp
O caractere '#' delimita o início de um comentário. Todos os
caracteres posteriores até o primeiro <nl> posterior são ignorados.
