.bp

.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 2			     *
     *							     *
     *	     		     COMANDOS			     *
     *							     *
     *********************************************************

.fi

.sp 4


.pp
O usuário se comunica com o sistema operacional TROPIX por meio dos
comandos fornecidos à "shell". Sempre que a "shell" está com o controle
do terminal, ela os interpreta e executa. A indicação de que a "shell"
está com o controle do terminal é dada pelo "prompt" escrito pela
"shell" no terminal. O "prompt" é um caractere ou seqüência de
caracteres definida pelo usuário. Por omissão, a seqüência "$ " é
utilizada.

.sp 4
.b
2.1 - Comandos simples
.r

.pp
Um comando simples é formado por palavras (seqüência de caracteres diferentes
de branco e dos metacaracteres apresentados no decorrer do texto), separadas 
pelos caracteres de espacejamento <sp> ou <ht> (branco ou tab).
A primeira palavra é o nome do comando a ser executado (o argumento 0)
e as demais são argumentos para o comando.
Por exemplo:

.sp
.in +5

$ date

.in -5
.sp
é um comando que fornece a data atual.

.sp
.in +5

$ ls -l

.in -5
.sp
lista os arquivos do diretório corrente. O argumento "-l" faz com que sejam
listadas também outras informações, como tamanho e data de criação de cada
arquivo.


.sp 4
.b
2.2 - Comandos em "background"
.r


.pp
Para executar um comando a "shell" normalmente cria um 
subprocesso e espera pelo seu término.
O caractere "&" evita a espera pelo término do comando:
a "shell" cria o subprocesso, escreve na tela sua identificação (PID)
e volta a ter o controle do terminal.

.sp
Assim, ao receber:

.sp
.in +5

$ cc prog.c &

.in -5
.sp
a "shell" invoca o compilador "C" para compilar "prog.c", fornece o
número do  subprocesso e, imediatamente, escreve o "prompt".
Enquanto isso, a compilação se processa concorrentemente e,
quando terminar, não dará qualquer indicação ao usuário.
Pelo número do subprocesso, o usuário
poderá consultar o seu estado através do comando "ps".


.sp 4
.b
2.3 - Lista de comandos
.r


.pp
Os comandos simples podem vir em seqüência, formando uma lista de comandos,
desde que separados pelos símbolos ";", "&&" ou "||",
que são alguns dos metacaracteres da "shell" (um ou mais
caracteres que têm significado especial para a "shell").


.pp
Os comandos (ou listas de comandos) separados por
";" são simplesmente executados em seqüência, como se fossem fornecidos em
linhas distintas.
O símbolo "||" separando dois comandos (ou duas listas de comandos) 
faz com que o segundo seja executado somente se o primeiro terminar
incorretamente.
Comandos (ou listas de comandos) separados por "&&" serão executados
seqüencialmente até que um dos comandos termine incorretamente.


.pp
As listas de comandos também podem ser executadas em "background",
se terminadas por "&".


.pp
Os metacaracteres "||" e "&&" têm a mesma precedência, maior do que a de 
";" e "&", iguais entre si.

.sp
Por exemplo em:

.sp
.in +5

$ ls pg.h && cc -c pg1.c

.in -5
.sp
pg1.c será compilado apenas se pg.h (por exemplo contendo declarações usadas
em pg1.c) existir.

.sp
E em

.sp
.in +5

$ cc -c src/pg1.c || cc -c fonte/pg1.c

.in -5
.sp
pg1.c do diretório "fonte" será compilado se, por exemplo, não existir
pg1.c no diretório "src".


.sp 4
.b
2.4 - Redirecionamento de entrada e saída
.r


.pp
Os dados de entrada de um comando são em geral obtidos da entrada
padrão que, normalmente, está associada ao terminal.
Da mesma forma, os dados de saída são dirigidos para a
saída padrão, também associada ao terminal.
É possível, no entanto, redirecionar a entrada e/ou saída dos comandos
através de alguns metacaracteres da "shell".
As construções apresentadas a seguir podem aparecer em qualquer
lugar em um comando simples
que não serão passados como argumentos aos comandos invocados.
Note que a substituição de comandos e parâmetros
é realizada antes que a notação a seguir seja analisada.

.in +5
.ip "< nome" 12
Usa o arquivo de nome dado como a entrada padrão
(relativo ao descritor de arquivos 0).

.ip "> nome" 12
Usa o arquivo de nome dado como a saída padrão
(relativo ao descritor de arquivos 1).
Se o arquivo não existe ele é criado; senão ele é truncado a zero
caracteres.

.ip ">> nome" 12
Usa o arquivo de nome dado como saída padrão. Se o arquivo já existe,
a saída é anexada a ele; caso contrário, o arquivo é criado.

.ip "<< nome" 12
A entrada da "shell" é lida até a primeira linha que corresponda
literalmente ao "nome" resultante, ou até um fim de arquivo.

.ip "<&número" 12
Usa o arquivo associado com o descritor de arquivos dado pelo número
como a entrada padrão. Similarmente para a saída padrão com ">&número".

.ip "<&-" 12
Fecha a entrada  padrão. Similarmente para a saída padrão com ">&-".

.ep
.in -5

.pp
Se qualquer das cadeias acima for precedida por um número, o descritor de
arquivos que será associado ao arquivo é aquele dado pelo número (ao invés
dos assumidos por omissão 0 e 1). Por exemplo:
.sp
.in +12
 ... 2>&1
.sp
.in -12

associa o descritor de arquivos 2 ao arquivo correntemente associado
com o descritor de arquivos 1.

.pp
A ordem na qual as redireções são especificadas é significante. A
"shell"
avalia as redireções da esquerda para direita. Por exemplo:
.sp
.in +12
 ... 1> xxx 2>&1
.in -12

.pp
primeiro é associado o descritor de arquivos 1 com o arquivo "xxx".
Depois é associado o descritor de arquivos 2 com o arquivo associado
ao descritor de arquivos 1 (i. e. "xxx"). Se a ordem de redireções
fosse reversa, o descritor de arquivos 2 seria associado com o
terminal (assumindo que o descritor de arquivos 1 tinha sido) e
depois o descritor de arquivos 1 seria associado ao arquivo "xxx".

.pp
Usando a terminologia introduzida anteriormente,
se um comando é composto de vários comandos simples, as redireções
serão avaliadas para o comando como um todo, antes que seja avaliada
para cada comando simples. Isto é a "shell" avalia o redirecionamento
para a lista inteira, então para cada "pipeline" dentro da lista e
então para cada lista dentro de cada comando.

.pp
Se um comando é seguido por '&', a entrada padrão assumida para o
comando é o arquivo vazio "/dev/null".

.pp
Exemplos:

.sp
.in +5

$ ls > file

.in -5
.sp
o arquivo "file" conterá os nomes dos arquivos do diretório corrente.

.sp
.in +5

$ wc < file

.in -5
.sp
o arquivo "file" será lido e serão listados o número de caracteres, palavras
e linhas encontradas.


.sp 4
.b
2.5 - "Pipes"
.r


.pp
A saída de um comando pode ser concatenada à entrada de outro através do 
operador "|".

.sp
Assim,

.sp
.in +5

$ ls -l | wc

.in -5
.sp
tem efeito similar a

.sp
.in +5

$ ls -l > file ; wc < file

.in -5
.sp
com as vantagens de que o arquivo "file" não precisa ser especificado
e os dois comandos podem executados concorrentemente.

.pp
Vários comandos podem ser concatenados através do "pipe".

.sp
Exemplo:

.sp
.in +5

$ ls | grep teste | wc -l

.in -5
.sp
listará o número de arquivos no diretório corrente que contém a palavra
"teste" em seu nome.


.pp
Numa lista de comandos, "pipes" podem ser especificados e, neste
caso, a precedência do metacaractere "|" é
maior que a dos metacaracteres "||", "&&", ";" e "&".


.sp 4
.b
2.6 - Valor de Retorno
.r

.pp
Através da chamada ao sistema "exit", os comandos retornam à "shell" um
valor inteiro, indicando seu estado de término.
O valor 0 significa que o programa terminou normalmente;
qualquer valor distinto de 0 indica término anormal.
Numa lista de comandos ou numa linha de "pipes", o valor que a "shell"
finalmente retorna é o do último comando executado.

.sp 4
.b
2.7 - Resumo
.r

.pp
Os comandos interpretados pela "shell" devem obedecer à seguinte sintaxe:

.sp
CMD [delim CMD] ...

.sp
onde:

.sp
CMD = cmd [arg] ... [<ent] [>sai]

.in +2

.ip cmd 6
nome do comando

.ip arg 6
argumento para o comando

.ip ent 6
arquivo de entrada

.ip sai 6
arquivo de saída

.lp


.sp 2

.in -2 
delim = |  ;  &  &&  ||

.ip 'CM1 ; CM2    ' 13
CM1 e CM2 são executados seqüencialmente.

.ip 'CM1 | CM2    ' 13
As execuções de CM1 e CM2 são iniciadas simultaneamente.
A saída de CM1 é a entrada de CM2.
A "shell" espera CM2 terminar. ("Pipe")

.ip 'CMD &        ' 13
A execução de CMD é iniciada, mas seu término não é
esperado. ("Background")

.ip 'CM1 && CM2   ' 13
CM2 é executado se CM1 terminar corretamente. ("And")

.ip 'CM1 || CM2   ' 13
CM2 é executado se CM1 nao terminar corretamente. ("Or")

.lp


.sp 2
A precedência dos metacaracteres é:

.sp
.in +5
|
.sp
|| &&

.sp
| ;

.in -5

.sp 4
.b
2.8 - Arquivos de Comandos
.r


.pp
Em uso interativo, os comandos são lidos do terminal e executados.
Entretanto, esses comandos podem ser provenientes de um arquivo de
comandos ("script"), previamente editado.
Assim, se o arquivo "arq" contem comandos para a "shell",

.sp
.in +5
$ sh arq

.in -5
.sp
criará um subprocesso em que uma "shell" filha da original 
lerá os comandos de "arq" e os executará.

.pp
Se esse arquivo de comandos tiver o atributo de modo indicando
que ele é executável (veja "chmod(cmd)") a chamada

.sp
.in +5
$ arq

.in -5
.sp
será equivalente à anterior. Em qualquer hipótese, é criado um
subprocesso que executa uma "shell" filha.

.pp
O comando interno "." evita a criação de um subprocesso. Assim

.sp
.in +5
$ . arq

.in -5
.sp
os comandos são executados diretamente pelo "shell".

.pp
Erros detectados pela "shell", quando da execução
de um arquivo de comandos, causam o término da execução desse arquivo.


.bp
.b
2.9 - Comentários e Linhas de Continuação
.r

.pp
O metacaractere "#" inicia um comentário, que se estende até o final da linha.
É útil em arquivos de comandos.

.pp
Quando for necessário usar varias linhas para a entrada de um
comando, as linhas que têm continuação deverão ser terminadas
pela seqüência "\<nl>" (barra invertida seguida de <nl>). (Ver 3.3).

.sp 4
.b
2.10 - Comandos Internos
.r

.pp
Alguns comandos especiais não causam a criação de um subprocesso para
sua execução, sendo esta realizada internamente pela própria "shell".
São eles:

.sp
":", ".", "cd", "eh", "eval", "exec", "exit", "export", "false", "gh",
"h", "login",  "read", "readonly", "rehash", "set", "shid", "shift",
"time", "times", "trap", "true", "umask", "unset", "wait" e "xh".


.pp
A utilidade de alguns desses comandos será apontada no decorrer do texto e uma
explicação mais detalhada pode ser encontrada no Capítulo 4.
