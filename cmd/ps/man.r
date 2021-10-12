.bp 1
.he 'PS (cmd)'TROPIX: Manual de Referência'PS (cmd)'
.fo 'Atualizado em 10.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "ps -"
imprime informações sobre processos
.br

.in
.sp
.b SINTAXE
.in 5
ps [-axlk] [-t <tty>] [<pid> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ps" fornece algumas informações sobre os processos ativos.

.sp
Sem opções nem argumentos, são dadas informações apenas sobre os processos
do usuário que está utilizando o comando "ps",
e que estão associados a algum terminal.

.sp
Neste modo, é dada uma lista curta de informações, contendo apenas
a identificação do terminal ("TTY"),
a identificação do processo ("PID")
e o nome do comando com os respectivos argumentos ("CMD").

.sp
O conteúdo da lista de informações pode ser modificado através das seguintes
opções/argumentos:

.in +3
.ip -a 7
Fornece informações sobre todos os processos associados a algum terminal
(e não apenas sobre os processos do usuário que está utilizando o comando "ps").
Esta opção só é permitida ao superusuário.

.ip -x 7
Fornece informações sobre todos os processos,
quer tenham um terminal associado ou não.
Esta opção só é permitida ao superusuário.

.ip -l 7
Fornece informações mais detalhadas  sobre cada processo: além das
informações da forma curta descrita acima ("TTY", "PID" e "CMD"),
dá também:

.in +9
.ip F 6
indicadores do processo (ver abaixo);

.ip S 6
estado do processo (ver abaixo);

.ip UID 6
dono do processo obtido a partir da variável "USER" do ambiente do processo
(ou, se não encontrado, o nome do usuário obtido no arquivo "/etc/passwd");

.ip US 6
indicação se o dono é superusuário ("SU") ou do grupo
do superusuário ("sys");

.ip PPID 6
identificação do  pai do processo;

.ip T 6
tipo do processo (reentrante ("n") ou não);

.ip PRI 6
prioridade do processo;

.ip NI 6
valor do "nice";

.ip UTIME 6
tempo total de CPU (em modo usuário) já utilizado pelo processo;

.ip STIME 6
tempo total de CPU (em modo de sistema) já utilizado pelo processo.

.ep
.in -9

.ip -ll 7
Com a opção "-l" dada duas vezes, algumas das informações descritas acima
(em "-l") são substituídas por:

.in +9
.ip RUID 6
nome do usuário obtido no arquivo "/etc/passwd";

.ip ADDR 6
endereço da UPROC do processo na memória/disco em KB;

.bc	/*************************************/
.ip SIZE 6
tamanho do processo em KB;
.ec	/*************************************/

.ip MSK 6
máscara de execução (isto é, uma indicação em quais CPUs
o processo pode ser executado);

.ip PGRP 6
identificação do processo líder do grupo ao qual este
processo pertence;

.ip TGRP 6
identificação do grupo de terminais do processo;

.ip RCs 6
número atual/máximo de recursos lentos que o processo
ocupa/ocupou.

.ep
.in -9

.ip -lll 7
Com a opção "-l" dada três vezes, algumas das informações descritas acima
(em "-l") são substituídas por:

.in +9
.ip ADDR 7
endereço da UPROC do processo na memória/disco em KB;

.ip TSIZE 7
tamanho da região TEXT do processo em KB;

.ip DSIZE 7
tamanho da região DATA (que inclui o BSS) do processo em KB;

.ip SSIZE 7
tamanho da região STACK do processo em KB;

.ip KFILE 7
número atual de arquivos abertos do processo.

.ep
.in -9

.ip -llll 7
Com a opção "-l" dada quatro vezes, algumas das informações descritas acima
(em "-l") são substituídas por:

.in +9
.ip ALARM 8
número de segundos faltando antes de enviar o sinal SIGALRM para o processo;

.ip SIGALRM 8
estado do sinal SIGALRM: (D) para SIG_DFL, (I) para SIG_IGN e um endereço
em hexadecimal para indicar a função de usuário que será chamada.

.ep
.in -9

.ip -lllll 7
Com a opção "-l" dada cinco vezes, algumas das informações descritas acima
(em "-l") são substituídas por:

.in +9
.ip KERNEL 8
O tamanho (em bytes) da parte da pilha de modo de supervisor do processo que
já foi usada.

.ip STACK 8
A percentagem correspondente ao uso da pilha (veja acima).

.ep
.in -9

.ip -k 8
Envia o sinal "SIGTERM", interativamente, para os processos desejados.
Se a opção for dada duas vezes ("-kk"), envia o sinal "SIGTERM".

.ip "-t" 8
Fornece informações apenas sobre processos associados ao terminal <tty>.
O nome do terminal pode ser dado com ou sem o prefixo "tty"
(como por exemplo "tty002" ou simplesmente "002").
Se <tty> for "?" , serão dadas
informações apenas sobre processos sem terminal associado. 

.ip "<pid>" 8
Fornece informações apenas sobre processos cujas
identificações são dadas pelos <pid>s.

.ep
.in -3

.in
.sp
.b "ESTADOS DOS PROCESSOS"
.in 5
Os vários estados dos processos são dados abaixo pelos códigos e
seus respectivos nomes simbólicos:

.in 8
.ip r 4
SSWAPRDY: pronto para executar, porém no disco;
.ip R 4
SCORERDY: pronto para executar, e na memória interna;
.ip s 4
SSWAPSLP: esperando alguma recurso, no disco;
.ip S 4
SCORESLP: esperando alguma recurso, na memória interna;
.ip i 4
SRUN:	executando na CPU "i";
.ip Z 4
SZOMBIE: o processo já terminou, mas o pai ainda não chamou "wait".
.ep

.in
.sp
.b "INDICADORES DOS PROCESSOS"
.in 5
Os indicadores dos processos dão informações auxiliares sobre o processo.
Os vários indicadores são dados abaixo em hexadecimal, juntamente
com seus nomes simbólicos:

.in 8
.ip 0x01 8
SSYS: processo especial;
.ip 0x02 8
SLOCK:	deve ficar sempre na memória principal;
.ip 0x04 8
SSCTXT:	o estado está em uma área alternativa;
.ip 0x08 8
SULOCK:	processo regular que deve ficar sempre na memória principal;
.ip 0x10 8
SIGWAKE: foi acordado sem esperar o recurso;
.ip 0x20 8
STRACE: processo em trace;
.ip 0x40 8
SWAITED: a informação de trace já foi dada;
.ip 0x80 8
SIGEXIT: envia o sinal "SIGHILD".
.ep

.in
.sp
.b OBSERVAÇÕES
.in 5
Um sistema operacional, juntamente com seus usuários e processos,
formam uma atividade dinâmica, e como a tarefa de coletar as diversas
informações sobre os  processos não é instantânea,
o comando "ps" pode fornecer informações possivelmente desatualizadas.

.sp
Algumas informações impressas para processos defuntos
(estado SZOMBIE) podem ser irrelevantes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
kill, nice
.br
.wo "(sys): "
intro, nice
.br

.in
.sp
.b ARQUIVOS
.in 5
/dev/console
 /dev/tty*
 /dev/ptyc*
 /dev/video*
 /dev/swap
 /etc/passwd

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
