.bp
.he 'SCB (fmt)'TROPIX: Manual de Referência'SCB (fmt)'
.fo 'Atualizado em 21.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "scb -"
parâmetros do bloco de controle do núcleo do TROPIX
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O núcleo do sistema operacional TROPIX contém uma tabela com diversos
parâmetros, cujo objetivo é definir a configuração do sistema.
Alterando um (ou mais) destes parâmetros podemos modificar vários
aspectos do sistema operacional,
e deste modo atender a diversos desejos e necessidades.

.sp
Esta tabela (chamada de SCB, ou seja "system control block") está contida
na imagem do núcleo do TROPIX em disco (normalmente o arquivo "/tropix"),
e é copiada para a memória (de onde é consultada) durante o processo
de "boot" (ou seja, a carga do sistema operacional).

.sp
Sem argumentos, o comando "edscb" (cmd) imprime os valores dos parâmetros do SCB
em uso efetivo no momento.
Durante a carga do sistema operacional é
possível alterar os parâmetros apenas para esta execução do sistema
(veja "boot" (sys)).
O mesmo comando "edscb" pode alterar os valores dos parâmetros de modo definitivo,
isto é, até uma nova execução de "edscb", alterando a tabela contida
na imagem do núcleo do TROPIX em disco.

.in
.sp
.b "LISTA DE PARÂMETROS"
.in 5
Damos a seguir uma lista dos parâmetros do SCB.
O valor inicial (padrão) dos parâmetros é dado entre parênteses:
.bc	/*************************************/
O valor inicial (padrão) da maioria destes parâmetros pode ser encontrada
no arquivo "/usr/include/sys/defaults.h".
.ec	/*************************************/

.in +3
.ip nproc 10
Número de entradas da tabela de processos (100).

.ip ninode 10
Número de entradas da tabela de nós-índice (150).

.ip nbuf 10
Número de áreas de entrada/saída para dispositivos estruturados
("cache" de "buffers"). Se este valor for NULO, ele será computado
durante a carga do sistema operacional, em função da memória disponível
(calculado em função do total de memória interna).

.ip nkfile 10
Número de entradas da tabela de arquivos abertos (150).

.ip nclist 10
Número de áreas de entrada/saída para terminais, vídeos, linhas de comunicações
e pseudo-terminais (1024).

.ip nmap 10
Número de entradas da tabela de alocação da memória interna (102).

.ip nhshlib 10
Número de entradas da tabela "hash" para os símbolos das bibliotecas
compartilhadas (10007).

.ip ntout 10
Número de entradas da tabela de "time-outs" (40).

.ip nlockf 10
Número de entradas da tabela de "trancas" para arquivos (100).

.ip neventl 10
Número de entradas da tabela de eventos de usuários (local) (50).

.ip neventg 10
Número de entradas da tabela de eventos de usuários (global) (25).

.ip nsemal 10
Número de entradas da tabela de semáforos de usuários (local) (50).

.ip nsemag 10
Número de entradas da tabela de semáforos de usuários (global) (25).

.ip nregionl 10
Número de entradas da tabela de regiões locais
(no momento NÃO sendo usada).

.ip nregiong 10
Número de entradas da tabela de regiões globais (400).

.ip ndns 10
Número de entradas da tabela de nomes/endereços de computadores
da INTERNET (200).

.ip nroute 10
Número de entradas da tabela de roteamento da INTERNET (20).

.ip nether 10
Número de entradas da tabela de endereços lógicos/físicos de controladores
"ethernet" (50).

.ip nrawep 10
Número de entradas da tabela de "endpoint" para o "modo-de-acesso-interno"
(veja "t_intro" (xti)) (20).

.ip nudpep 10
Número de entradas da tabela de "endpoint" para o "modo-sem-conexão"
(veja "t_intro" (xti)) (20).

.ip ntcpep 10
Número de entradas da tabela de "endpoint" para o "modo-com-conexão"
(veja "t_intro" (xti)) (64).

.ip rootdev 10
Código interno ("major" + "minor") do dispositivo raiz de sistema de
arquivos. Se o seu valor for NODEV, o dispositivo será determinado
durante a carga do sistema operacional, baseado na tabela de partições.

.ip pipedev 10
Código interno ("major" + "minor") do dispositivo onde serão alocados
os nós-índice para a criação de "pipe"s.
Se o seu valor for NODEV, o dispositivo será determinado
durante a carga do sistema operacional, baseado na tabela de partições.

.ip swapdev 10
Código interno ("major" + "minor") do dispositivo onde será alocada a
área de "swap".
Se o seu valor for NODEV, o dispositivo será determinado
durante a carga do sistema operacional, baseado na tabela de partições.

.ip swaplow 10
Deslocamento (em blocos de 512 bytes) do início da área de "swap" no
dispositivo "swapdev" (0).

.ip nswap 10
Tamanho (em blocos de 512 bytes) da área de "swap" no
dispositivo "swapdev".
Se o seu valor for NULO, ele será determinado
durante a carga do sistema operacional, baseado na tabela de partições.

.ip ramd0sz 10
Tamanho (em KB) da área de memória alocada para simular
a unidade de disco "0" na memória interna (512 KB).

.ip ramd1sz 10
Tamanho (em KB) da área de memória alocada para simular
a unidade de disco "1" na memória interna (0 KB).

.ip tslice 10
Intervalo de tempo ("time-slice") entre os recálculos
das prioridades dos diversos processos.
Este intervalo de tempo é dado em "ticks" do relógio (que são normalmente
de 10 ms.); portanto um valor de "10" significa 100 ms (10 ms).

.ip incpri 10
Número com o qual são incrementadas as prioridades dos processos
prontos para a execução (mas NÃO executando) a cada "time-slice" (10).

.ip decpri 10
Número com o qual são decrementadas as prioridades dos processos
em execução a cada "time-slice" (10).

.ip ttyinc 10
Tempo (em segundos) com o qual é incrementado o tempo de residência
no "swap" de processos que estavam aguardando entrada de teclado e
foram acordados (16).

.ip nfactor 10
Número que indica o grau de influência dos valores do "nice"
na prioridade dos processos (veja "nice" (cmd)) (10).

.ip nswapin 10
Número que determina o grau de influência do "nice" na operação de
trazer processos do "swap" para a memória interna (1).

.ip nswapout 10
Número que determina o grau de influência do "nice" na operação de
levar processos da memória interna para o "swap" (1).

.ip inage 10
Tempo (em segundos) que determina se um processo já está tempo suficiente
na memória interna para levá-lo para o "swap" (2).

.ip outage 10
Tempo (em segundos) que determina se um processo já está tempo suficiente
no "swap" para traze-lo de volta para a memória interna (3).

.ip maxppu 10
Determina o número máximo de processos por usuário
(no momento NÃO sendo controlado) (10).

.ip maxpsz 10
Determina o tamanho máximo de um processo, em KB (4 MB).

.ip umask 10
Máscara global de criação de arquivos, em octal.
Cada usuário pode posteriormente mudar este valor através do comando
"umask" do "sh" (cmd) ou da chamada ao sistema "umask" (sys) (007).

.ip ncexec 10
Número máximo de caracteres alocados pelo núcleo do TROPIX
para a passagem de argumentos e cadeias do ambiente durante a
execução de um programa (8 KB).

.ip stksz 10
Número de bytes alocados inicialmente na pilha de execução de um programa (3 KB).

.ip stkincr 10
Número de bytes alocados adicionalmente na pilha de execução de um programa,
cada vez que o espaço alocado se esgota (4 KB).

.ip nmount 10
Número máximos de dispositivos que podem ser montados
(além do dispositivo "raiz"). Veja "mount" (cmd) (16).

.ip hz 10
Freqüência de interrupção do relógio principal do sistema, em interrupções
por segundo (hertz) (100).

.ip nflush 10
Número máximo de áreas de entrada/saída do "cache" de "buffers"
que devem ser atualizados a cada segundo
(no momento NÃO sendo usada).

.ip nitblock 10
Número de áreas para entrada/saída de datagramas da INTERNET (120).

.ip scrsaver 10
Número de segundos esperados até a ativação do "screen-saver" (300).

.ip preempt 10
Realiza troca de processos mesmo quando está em modo supervisor
(0xFF, permitindo).

.ip tzmin 10
Fuso horário: número de minutos que devem ser somados ao tempo universal
(UT, antes chamado de GMT) para obtermos o tempo local (-180).

.ip utclock 10
Diferente de zero caso o relógio do computador está com o horário UT, zero em caso
contrário (horário local) (0).

.ip dmaenable 10
Diferente de zero caso deva ser habilitado o uso de DMA dos dispositivos ATA (IDE) (1).

.ip dma 10
Como "dmaenable", acima.

.ip key 10
Código da tabelas de teclado: 0 para teclado norte-americano (US) e
1 para teclado brasileiro ABNT-2 (0).

.ip dmesgsz 10
Tamanho da área em que serão armazenadas as mensagens da carga do sistema, e que
posteriormente poderão ser lidas através do programa "dmesg" (3072).

.ip synctime 10
Intervalo de tempo em segundos entre cópias consecutivas dos nós-índices e Superblocos
dos sistemas de arquivos para o "cache" de blocos (31).

.ip blocktime 10
Intervalo de tempo em segundos entre escritas consecutivas de até "maxflush" (ver abaixo)
blocos do "cache" de blocos para os dispositivos físicos (5).

.ip dirtytime 10
Tempo máximo permitido entre a última modificação de um bloco do
"cache" de blocos e a sua escrita no dispositivo físico (30).

.ip maxflush 10
Número máximo de blocos escritos a cada escrita de blocos do cache nos dispositivos físicos (40).

.ip sioport? 10
Número da porta dos portas seriais 0 a 7, correspondem aos dispositivos "/dev/tty00?".
As portas seriais 0 a 3 correspondem respectivamente às portas MS-DOS COM1 a COM4.
Os dispositivos seriais PCI são configurados automaticamente
(0x3F8, 0x2F8, 0x3E8, 0x2E8).

.ip sioirq? 10
Número da interrupção dos portas seriais 0 a 7 (4, 3, 4, 3).

.ip lpport? 10
Número da porta dos portas paralelas 0 a 3 (0x378, 0x278, 0x3BC, 0).

.ip lpirq? 10
Número da interrupção dos portas paralelas 0 a 3.
O número 0 significa porta paralela operando por estado (7, 0, 0, 0).

.ip edport? 10
Número da porta dos dispositivos "ethernet" 0 a 3.
Os dispositivos "ethernet" PCI são configurados automaticamente (0, 0, 0, 0).

.ip edirq? 10
Número da interrupção dos dispositivos "ethernet" 0 a 3 (0, 0, 0, 0).

.ip usb? 10
Igual a 1 caso deva ser habilitado o controlador "usb" 0 a 7 (1, 0, 0, 0, 0, 0, 0, 0).

.ip sysname 10
Nome do sistema operacional. Se o valor não for dado, o sistema assume
"TROPIX".

.ip systype 10
Variante do sistema operacional.
Se o valor não for dado, o sistema assume "INTEL 386/486".

.ip nodename 10
Nome do computador.

.ip version 10
Versão do sistema operacional TROPIX.

.ip date 10
Data de geração deste núcleo do sistema operacional TROPIX.

.ip time 10
Hora de geração deste núcleo do sistema operacional TROPIX.

.ip machine 10
Tipo do computador.
Se o valor não for dado, o sistema assume "PC".

.ip customer 10
Cliente do sistema operacional TROPIX.

.ip depto 10
Departamento de instalação do sistema operacional TROPIX.

.ip sysserno 10
Número de série deste exemplar do sistema operacional TROPIX.

.ep
.in -3

.sp 2
Finalmente, damos alguns parâmetros adicionais
impressos pelo comando "edscb":

.in +3
.ip ncpu 10
Número de CPUs ativas encontradas. Embora o sistema operacional
TROPIX seja multiprocessado, no momento, para PCs apenas um
processador é utilizado.

.ip initpid 10
Número (identificação, PID) do processo "/etc/init".
Este processo é o pai de todos os processos, e o seu PID é dependente
do número de CPUs ativas.

.ip cputype 10
O tipo da CPU: INTEL 386, 486 ou PENTIUM.

.ip pgsz 10
O tamanho da página em bytes.

.ip nblkdev 10
Número de dispositivos estruturados (de blocos) encontrados.

.ip nchrdev 10
Número de dispositivos não-estruturados (de caracteres) encontrados.

.ip endkernel 10
Endereço (em KB) do final da memória utilizada pelo núcleo do TROPIX
(incluindo todas as tabelas alocadas dinâmicamente).

.ip ucore 10
Endereço (em KB) do início da memória disponível para processos do usuário.

.ip enducore 10
Endereço (em KB) do final da memória disponível para processos do usuário.

.ip endpmem 10
Endereço (em KB) do final da memória física do computador.

.ip ramd1 10
Endereço (em KB) do início da área de memória alocada para simular
a unidade de disco "1" na memória interna.

.ip endramd1 10
Endereço (em KB) do final da área de memória alocada para simular
a unidade de disco "1" na memória interna.

.ip ramd0 10
Endereço (em KB) do início da área de memória alocada para simular
a unidade de disco "0" na memória interna.

.ip endramd0 10
Endereço (em KB) do final da área de memória alocada para simular
a unidade de disco "0" na memória interna.

.ip debug 10
Liga/desliga a depuração do núcleo.
A lista das opções de depuração é dada abaixo:

.in +11
.ip ' 0: ' 5
Depuração desligada.
.ip ' 1: ' 5
Sincronismo.
.ip ' 2: ' 5
Chamadas ao sistema.
.ip ' 3: ' 5
Informações sobre exceções de endereçamento e acesso à memória.
.ip ' 4: ' 5
Leitura adiantada de blocos ("readahead").
.ip ' 5: ' 5
Atividade de SWAP.
.ip ' 6: ' 5
Update, bflush.
.ip ' 7: ' 5
Open, creat, close, ....
.ip ' 8: ' 5
Chamadas obsoletas (iname, ioctl, ...).
.ip ' 9: ' 5
Desliga synccheck.
.ip '10: ' 5
Discos IDE.
.ip '11: ' 5
Terminais PEGASUS (ti).
.ip '12: ' 5
Iget, iput, ....
.ip '13: ' 5
Residual de entrada/saida.
.ip '14: ' 5
Deadlocks evitatos.
.ip '15: ' 5
Sinais.
.ip '16: ' 5
Área de data de um text lido da memoria.
.ip '17: ' 5
Atividade de timeout.
.ip '18: ' 5
Read/write.
.ip '19: ' 5
Lockf.
.ip '20: ' 5
Trace.
.ip '21: ' 5
Aumento do processo na parte inferior.
.ip '22: ' 5
Disquete.
.ip '23: ' 5
TCP/IP.
.ip '24: ' 5
IPC (eventos, semaforos e shmem).
.ip '25: ' 5
Regiões de memória.
.ip '26: ' 5
Preemption do Kernel (escreve @).
.ip '27: ' 5
Fast Read: escreve os segmentos lidos.
.ip '28: ' 5
Exec.
.ip '29: ' 5
Ethernet.
.ip '30: ' 5
PPP.
.ip '31: ' 5
Physio.
.ip '32: ' 5
Zip.

.ep
.in -11

.ep
.in -3

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
edscb, boot, chmod, nice, sh, mount
.br
.wo "(sys): "
umask
.br
.wo "(xti): "
t_intro
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
/tropix
 /usr/include/stat.h
 /usr/include/sys/default.h

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
