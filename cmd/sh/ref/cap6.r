.bp

.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 6			     *
     *							     *
     *	           CHAMADA À "SHELL" E AMBIENTE	             *
     *							     *
     *********************************************************

.fi

.sp 4


.pp
No TROPIX, após o "login", a "shell" é invocada para ler
e executar comandos do usuário em um terminal específico.
Poderá também ser chamada explicitamente pelo usuário, como qualquer
comando, ou implicitamente, como para a execução de um arquivo de
comandos ("script").


.sp 4
.b
6.1 - Chamada à "shell"
.r

.pp
A chamada à "shell" obedece à seguinte sintaxe:
.sp
.in +10
 [-]sh [-c <comando>] [-erv]
.in -10
.sp
 onde "cmdarq" é um comando ou um arquivo de comandos.


.pp
Se o primeiro caractere do argumento 0,
ou seja o nome do comando "sh", for precedido pelo caractere
"-", a "shell" executará inicialmente o arquivo de comandos
"$HOME/.profile",
passando depois a receber os comandos normalmente. 


.pp
As opções têm os seguintes significados:
.in +5

.ip -c
executa o <comando> dado como argumento.

.ip "-e"
Termina a execução quando um comando retorna erro.

.ip "-r"
Verifica a reescrita de arquivos no redirecionamento.

.ip "-v"
Lista cada linha de comandos lida do arquivo de entrada.

.in -5
.lp


.sp 4
.b
6.2 - "Prompt"
.r

.pp
O uso mais comum da "shell" é o interativo.
A "shell" fornece um "prompt" (caractere ou cadeia de caracteres),
definido pela variável "PS1", sempre que está apto a receber comandos.
Se esse comando deve continuar em outra linha, é fornecido um "prompt"
secundário, definido pela variável "PS2".
Os "prompt" podem ser modificados através
da atribuição de um novo valor às suas variáveis.


.sp 4
.b
6.3 - Ambiente
.r


.pp
A "shell" quando chamada recebe o ambiente de execução,
que nada mais é do que uma lista
de variáveis. Este ambiente é passado para os subprocessos criados por ela.

.pp
Por exemplo, as variáveis "HOME", "PATH", "PS1" e "PS2"
usadas pela "shell" (ver 3.1) são passadas a ela através do ambiente.

.in +5
.sp
"PS1" e "PS2" são os "prompts" principal e secundário da "shell".

.sp
"HOME" é o diretório corrente, representado por "."
e usado na resolução de referências a arquivos que não começam com "/".
O comando interno "cd" é usado para se mudar o diretório corrente.

.sp
"PATH" define os diretórios onde os comandos a executar devem ser
procurados e a ordem desta procura.
Se não for especificado será, por omissão, o diretório corrente,
"/bin" e "/usr/bin", nessa ordem. Assume-se,
"PATH=.:/bin:/usr/bin",  onde o caractere ":" separa os nomes dos diretórios.
Se o comando começa com "/", o "PATH" não é usado.

.in -5

.pp
O usuário pode atribuir novos valores para essas variáveis,
bem como criar outras.
Então, os "prompts" podem ser escolhidos pelo usuário
e os valores das variáveis "HOME" e "PATH" modificados sempre que necessário.

.pp
Essas modificações e adições de variáveis só são repassadas ao
ambiente através do comando interno "export". 
Assim, os processos criados pela "shell", para executar os comandos,
receberão o ambiente da "shell", somente com as modificações
e adições explicitadas através do comando interno "export".


.sp 4
.b
6.4 - Sinais
.r


.pp
Os sinais do processo que inicia a "shell" são também passados
a ela no ambiente
e serão  repassados aos processos criados para executar os comandos,
com uma exceção:
para os comandos terminados por "&", que deverão ser executados em
"background", o sinal
"SIGINT"  é passado  com o valor "SIG_IGN", que fará com que eles
não possam ser interrompidos pelo terminal teclando o caractere "intr"
(usualmente <^C>).
