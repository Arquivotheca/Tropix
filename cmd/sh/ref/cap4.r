.bp

.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 4			     *
     *							     *
     *	           COMANDOS INTERNOS E HISTÓRICO	     *
     *							     *
     *********************************************************

.fi

.sp 4
.b
4.1 - Histórico
.r

.pp
Uma das facilidades mais interessantes oferecidas pela  "shell"
quando em uso interativo é a manutenção de um histórico.
Seu propósito é guardar os últimos comandos fornecidos pelo usuário,
visando facilitar a edição e reexecução de comandos grandes
e de difícil digitação, bem como manter controle do trabalho
realizado no terminal.

.pp
O histórico consiste numa lista cujo tamanho é dado pela variável "HIST",
que inicialmente tem valor igual a 10, mas pode ser facilmente modificada
pelo usuário através de uma atribuição.

.pp
Cada comando recebe um número, através do qual pode ser posteriormente
refereciado.
O histórico pode então ser consultado e seus comandos editados,
agrupados e reexecutados através dos comandos internos "h", "eh", "gh" e "xh",
respectivamente.

.sp 2
.b
4.2 - Comandos Internos
.r

.pp
Os comandos internos são executados pela "shell" sem a criação de um
subprocesso.
.in +5

.sp
.ip ":"
Este comando não faz nada. Produz um valor de retorno igual a zero.

.ip ". arquivo"
Lê e executa os comandos em "arquivo" e retorna. Os caminhos de busca
especificados em "PATH" são utilizados para achar o diretório contendo
"arquivo".

.ip "cd [dir]"
Troca o diretório corrente para "dir". O valor da variável "HOME"
é o argumento assumido por omissão.

.ip "eh [num]"
Permite a edição do comando "num" do histórico. Se "num" não for
especificado, é assumido o último comando do histórico.
Os comandos de edição são os do editor visual "ex" que fazem
sentido para uma linha. Os comandos do editor e a edição terminam
respectivamente por <esc> ou "q".

.ip "eval [arg ...]"
Os argumentos são lidos como entrada para a "shell"
e os comandos resultantes executados.

.ip "exec [arg ...]"
O comando especificado pelos argumentos é executado no lugar
desta "shell", sem que seja criado um subprocesso. Redirecionamentos
de entrada e saída podem aparecer, e se nenhum outro argumento
for dado, causa a modificação da entrada/saída da "shell".

.ip "exit [n]"
Provoca o término da "shell" com valor de retorno "n".
Se "n" for omitido, o valor de retorno do último comando executado
é utilizado.

.ip "export [nome ...]"
As variáveis mencionadas são marcadas como exportáveis, o que provoca
a inclusão de uma cadeia definindo seu valor nos ambientes
dos programas executados posteriormente.
Se não for mencionado nenhum nome de variável, a "shell" mostra as variáveis
que são exportáveis.

.ip "false  "
Produz um valor de retorno diferente de zero.

.ip "gh     "
O histórico é  mostrado na tela e o usuário poderá selecionar o
comando desejado através das setas. Escolhido o comando, três opções são
possíveis: <enter> para executar o comando, <e> para editar o comando ou
<q> para desistir.

.ip "h      "
Apenas mostra o histórico com os últimos comandos executados.

.ip "login   "
Termina a sessão corrente da "shell".

.ip "read [nome ...]"
Este comando é usado para ler valores de variáveis.
Uma linha é lida da entrada padrão e a primeira palavra
é atribuída ao primeiro nome, a segunda palavra ao segundo nome,
e assim sucessivamente.
As palavras que sobrarem são todas atribuídas ao último nome.
Qualquer caractere pode ser quotado precedendo-o por barra invertida
('\'), inclusive <nl>. O valor de retorno é zero, a menos que
seja encontrado um fim de arquivo.

.ip "readonly [nome ...]"
As variáveis cujos nomes são mencionados tornam-se inalteráveis,
não podendo ser afetadas por atribuições posteriores.
Se nenhum argumento for dado, uma lista de todos as variáveis neste estado
é mostrada.

.ip "rehash"
Por questão de eficiência, o "sh" mantém internamente uma tabela HASH
contendo os nomes de todos os arquivos executáveis encontrados nos diretórios
especificados no conteúdo da variável PATH. Assim, se um destes diretórios
for modificado através da inclusão ou exclusão de algum
executável, o comando "rehash" deve ser utilizado, a fim de manter
atualizada a estrutura de dados interna do "sh".

.(t
.ip "set [--derv] [arg ...]"

.in +7

.ip "-d"
Permite que a execução se encerre com <control-D>.
.)t

.ip "-e"
Termina a execução imediatamente, se um comando não retornar zero.

.ip "-r"
Verifica a reescrita de arquivos no redirecionamento.

.ip "-v"
Lista cada linha de comandos lida do arquivo de entrada.

.ep

.in -7

.ip "shid"
Imprime a versão corrente da "shell".

.ip "shift [n]"
Os parâmetros posicionais de $n+1 ... são renomeados de $1 ...
Se "n" não for dado, ele é assumido que é 1.

.ip "time arg ..."
Marca e mostra o tempo de execução do comando formado pelos argumentos
dados.

.ip "times "
Mostra os tempos de usuário e sistema acumulados para os processos
executados pela "shell".

.ip "trap [arg] [n] ..."
O comando "arg" será lido e executado quando a "shell".
receber o(s) sinal(is) cujos números foram dados.
(Note que "arg" é varrido quando o comando
é dado e quando o sinal é recebido.) Os comandos dados são
executados na ordem dos números dos sinais. Qualquer tentativa
de preparação para a recepção de um sinal que estava sendo ignorado
na entrada da "shell" é ineficaz.
Se "arg" não for dado então os "traps" dos números dados
são restaurados aos seus valores originais. Se "arg" é
uma cadeia de caracteres nula, então os sinais relacionados
são ignorados pela "shell" e pelos comandos que ela evocar.
Se for especificado o sinal zero, então o comando relacionado
é executado ao fim da execução da shell, senão ao recebimento
de um sinal de número "n", conforme especificado em "signal" (sys).
Se não forem dados argumentos "trap" mostra os comandos
associados a cada sinal.

.ip "true  "
Não faz nada: apenas produz o valor de retorno igual a zero.

.ip "umask [nnn]"
A máscara de criação de arquivos do usuário é modificada para "nnn"
(veja umask (sys)). Se "nnn" for omitido, o valor corrente de "umask"
é mostrado.

.ip "unset [nome ...]"
Para cada nome dado, remove a variável correspondente.

.ip "wait [n]"
Espera pelo processo assíncrono (em "background") cuja identificação
seja "n" e mostra seu valor de retorno . Se "n" for omitido todos os
processos assíncronos correntemente ativos são esperados e o
valor de retorno será zero.

.ip "xh [num]"
Reexecuta o comando de número dado do histórico. Assume por omissão
o último comando.

.ep
.in -5
