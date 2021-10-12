.bp
.he 'INTRO (sys)'TROPIX: Manual de Referência'INTRO (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "intro -"
introdução aos conceitos fundamentais do sistema
.br

.in
.sp
.b DESCRIÇÃO
.in 5
Esta introdução define uma série de conceitos importantes do
TROPIX, que serão utilizados em todos o sistema, e são de particular
importância para a utilização das chamadas ao sistema.

.ip " 1." 4
.b "PROCESSO:"
Um espaço de endereçamento, um programa em execução neste espaço
e seus recursos necessários do sistema.

.ip " 2." 4
.b "IDENTIFICAÇÃO DO PROCESSO:"
Cada processo ativo do sistema é identificado univocamente por
um inteiro positivo, cujo valor máximo é 2147483647 (2 ** 31 - 1).
Repare que neste ponto o TROPIX difere do UNIX,
no qual as identificações só vão até 30000.
Usualmente, abreviamos a identificação de um processo 
como "ID" do processo, ou simplesmente "PID".

.ip " 3." 4
.b "IDENTIFICAÇÃO DO PAI DO PROCESSO:"
Um processo é criado por um processo ativo já existente
(veja "fork" (sys)).
A identificação do pai do processo é o PID do criador do processo,
durante o tempo de vida do criador.
Após o término do criador, a identificação do pai do processo
é o PID do processo especial "init" (ver abaixo).

.ip " 4." 4
.b "TEMPO DE VIDA DE UM PROCESSO:"
Após a criação de um processo através da chamada "fork",
o processo é considerado ativo, e
o programa associado e espaço de endereçamento existem
até o seu término.
Ao terminar, o processo entra em um estado inativo
("SZOMBIE", ver "ps" (cmd)),
quando alguns de seus recursos são liberados, enquanto
que outros ainda são mantidos (como por exemplo o seu PID).
Quando um (outro) processo executa um "wait" (sys) para um processo
inativo, os recursos remanescentes são liberados, e o
processo deixa de existir. 

.ip " 5." 4
.b "GRUPO DE PROCESSOS:"
Cada processo ativo é membro de um grupo de processos
(que é identificado por um PID). 
Durante a criação de um processo, o novo processo passa
a pertencer ao grupo do qual o criador é um membro. 
O objetivo do conceito de grupo de processos é 
o envio de sinais para processos relacionados
(ver "kill" (sys)).

.ip " 6." 4
.b "IDENTIFICAÇÃO DO GRUPO DE PROCESSOS:"
A identificação do grupo de processos é o PID do líder do grupo
(ver abaixo).

.ip " 7." 4
.b "LÍDER DO GRUPO DE PROCESSOS:"
O líder de um grupo de processos é o criador do grupo.
O seu PID é igual ao PID do seu grupo de processos.
Qualquer processo pode desligar-se do seu grupo atual, e
tornar-se líder de um novo grupo de processos através
da chamada ao sistema "setpgrp". 

.ip " 8." 4
.b "GRUPO DE TERMINAIS:"
Cada processo ativo pode ser membro de um grupo de processos
associado a um terminal.
Durante a criação de um processo, o novo processo passa
a pertencer ao grupo do qual o criador é um membro. 
O grupo é criado pelo processo que abriu o terminal.
O objetivo do conceito de grupo de processos associados a um terminal é 
o envio de sinais para todos os processos associados a um terminal,
como por exemplo, o "SIGINT" e "SIGQUIT" (ver "termio" (fmt)). 

.ip " 9." 4
.b "IDENTIFICAÇÃO DO GRUPO DE TERMINAIS:"
A identificação do grupo de processos associados a um terminal
é o PID do processo que abriu o terminal.

.ip "10." 4
.b "IDENTIFICAÇÃO DO USÁRIO REAL E GRUPO REAL:"
Cada usuário do sistema é identificado por um inteiro positivo
(0 a 65535) chamado de identificação do usuário real (UID real).
Cada usuário é também membro de um grupo, que é também identificado
por um inteiro positivo chamado de identificação do grupo real (GID real).
Cada processo ativo têm
um UID real e um GID real, que são herdados do processo 
criador deste processo.

.ip "11." 4
.b "IDENTIFICAÇÃO DO USÁRIO EFETIVO E GRUPO EFETIVO:"
Cada processo ativo têm
um UID efetivo e um GID efetivo, que são utilizados para determinar
as permissões de acesso aos arquivos (ver adiante).
O UID efetivo e o GID efetivo são iguais, respectivamente,
ao UID real e GID real do processo, a não ser que o processo
(ou um de seus antecessores) seja a execução de um arquivo
que tem o indicador "set user ID" ou "set group ID" ligado
(veja "exec" (sys)).

.ip "12." 4
.b "SUPERUSUÁRIO:"
Se o UID efetivo de um processo é "0" (zero), ele é considerado
um processo superusuário, e recebe previlégios especiais, tais
como a permissão de acesso a qualquer arquivo. 

.ip "13." 4
.b "PROCESSOS ESPECIAIS:"
Supondo que o sistema tenha "n" CPUs, os processos de PID "0" a "n+1"
são processos especiais (os demais são regulares).
O processo "0" é o "scheduler"; os processos "1" a "n" são os
"dispatchers" das várias CPUs, e o processo "n+1" é o "init",
reponsável pela inicialização do sistema, e pai de todos os processos
que se tornam órfãos.
O PID do processo "init" é chamado de "initpid".

.ip "14." 4
.b "DESCRITORES DE ARQUIVOS:"
Um descritor de arquivo é um inteiro pequeno utilizado para realizar
operações de entrada/saída em arquivos.
Existe um limite de arquivos abertos simultaneamente
por um processo (no momento 20).
Um descritor de arquivos é obtido por chamadas ao sistema
tais como "open" ou "pipe" (sys), e pode ser usado como
argumento em chamadas ao sistema tais como "read", "write",
"ioctl" e "close" (sys).

.ip "15." 4
.b "NOMES DE ARQUIVOS:"
Nomes consistindo de até 255 caracteres podem ser usados
para qualquer arquivo regular, especial, diretório ou FIFO. 
Podem ser usados quaisquer caracteres, exceto <nul> ('\0')
e '/'.
Repare no entanto, que não é aconselhável utilizar os caracteres
'*', '?', '!', '[' ou ']' como partes do nome de um arquivo,
em virtude do significado especial destes caracteres para o "sh" (cmd).
Também não é recomendado o uso de caracteres sem representação
gráfica.

.ip "16." 4
.b "CAMINHOS:"
Um caminho ("pathname") é um cadeia de caracteres (terminada por <nul>),
começando por um '/' opcional, seguido de zero ou mais nomes de diretórios
separados por '/', possivelmente seguido de um nome de arquivo.

.sp
Se um caminho começa por '/', a busca do arquivo começa da raiz
do sistema de arquivos. Em caso contrário, começa do diretório corrente.

.sp
Um caracter '/' sozinho representa a raiz do sistema de arquivos. 

.ip "17." 4
.b "DIRETÓRIOS:"
Diretórios são arquivos destinados exclusivamente para dar nomes
aos arquivos. Cada entrada de um diretório contém um nome de
um arquivo (ou outro diretório) e um número que identifica o
arquivo naquele sistema de arquivos.

.ip "18." 4
.b "DIRETÓRIO CORRENTE:"
A cada processo é associado um diretório corrente, que é   
utilizado para a busca dos arquivos através do caminhos.
Ele pode ser mudado através da chamada ao sistema "chdir".

.ip "19." 4
.b "PERMISSÕES DE ACESSO AOS ARQUIVOS:"
Para que um processos possa ler, escrever
ou executar/caminhar em um arquivo
é necessário que uma ou mais das condições abaixo sejam válidas:

.sp
.in +5
O UID efetivo do processo é superusuário, e o processo deseja
ler, escrever em um arquivo
ou caminhar em um diretório.

.sp
O UID efetivo do processo é superusuário, e o processo deseja
executar um arquivo
e um dos 3 bites de execução estão ligados. 

.sp
O UID efetivo do processo é igual ao UID do arquivo e o bite
adequado de permissão da parte do "dono" (0700) do modo do
arquivo está ligado.

.sp
O UID efetivo do processo não é igual ao UID do arquivo,
mas o GID efetivo do processo é igual ao GID do arquivo,
e o bite
adequado de permissão da parte do "grupo" (0070) do modo do
arquivo está ligado.

.sp
O UID efetivo do processo não é igual ao UID do arquivo,
o GID efetivo do processo não é igual ao GID do arquivo,
mas
o bite
adequado de permissão da parte dos "outros" (0007) do modo do
arquivo está ligado.

.in -5
.sp
Em caso contrário, a permissão correspondente é negada.

.ip "20." 4
.b "ERROS:"
Cada chamada ao sistema devolve um valor, que pode indicar 
um possível erro. Em geral, um valor de retorno NULO ou positivo
indica um chamada ao sistema sem erro, e um valor de retorno
negativo (em geral "-1") indica que ocorreu um erro.
Neste caso, a variável externa "errno" contém o número do erro
detetado. Esta variável não é zerada por chamadas ao sistema
corretas, de tal modo que o seu valor só deve ser consultado
se houver ocorrido um erro.

.sp
As funções "strerror" e "perror" (libc) podem ser usadas
para imprimir um texto explicativo do erro.

.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
init
.br
.wo "(sys): "
fork, wait, setpgrp
.br

.in
