.bp
.he 'KERNEL (sys)'TROPIX: Manual de Modificações'KERNEL (sys)'
.fo 'Atualizado em 28.08.07'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "kernel -"
núcleo do sistema operacional TROPIX
.br

.in
.sp 2
.nf
/*
 ******	PLURIX **************************************************
 */
.fi
.ip "13.7.87   1.0" 20
VERSÃO CORRENTE
.sp

.ip "13.7.87   1.0.1" 20
Foi modificada a sincronização do "ioctl" de terminais:
antes, o par SLEEPLOCK & FREE ficava parte no "driver" e parte em
"ttyctl.c"; agora fica todo no "driver".
.sp
Com isto, não é mais possível escrever caracteres entre a modificação
da estrutura "termio" e a programação da pastilha, o que podia acontecer,
na versão anterior.

.ip "14.7.87   1.0.2" 20
Na rotina "ttyread" foi colocado um teste para o caso do evento
"t_inqnempty" estar ligado com um alarme falso. Isto pode ocorrer
no caso de interrupções em locais inconvenientes, que poderiam
ser sanados com mais um semáforo, mas a solução dada é mais simples.

.ip "27.7.87   1.0.3" 20
Foi consertado o problema da
rotina "pipe" que não estava zerando a área do inode referente ao
texto no close.
.sp
Foi consertado o problema da
rotina "sched" que não estava preparada para o fato de que um processo
SCORESLP pode ser transformar em SCORERDY durante a operação de swap.
Agora, um processo SCORESLP é transformado em SSWAPSLP antes do
início da operação de swap.

.ip "28.7.87   1.0.4" 20
O endereço da área de argumentos da chamada ao sistema "exec", que
ficava na stack do programa "exece" foi agora colocado na UPROC.
O motivo é permitir a desalocação desta área no caso do processo
receber um sinal fatal durante um "exec". Foram feitas modificações
em "sysexec.c", "sysfork.c" e "uproc.h".

.ip "31.7.87   1.0.5" 20
Foi ativada a escrita de "core" (modificação em "aux.c").
Foi colocado uma MSG (chaves = 18) para as chamadas ao sistema
"read" e "write" (modificação em "sysfio.c"). 

.ip "04.8.87   1.0.6" 20
Foram modificadas as rotinas "bflush" e "bumerase" para serem síncronas,
isto é, para só retornarem depois de terminadas as escritas nos
dispositivos. Isto faz com que a chamada ao sistema "sync" seja
síncrona. 

.ip "19.8.87   1.0.7" 20
Foi acrescentado na UPROC o "frame-pointer" do usuário, para que ele
esteja disponível ao "sdb" em um "core-dump" (modificação em "aux.c").

.ip "20.11.87  1.1.0" 20
Foi modificado o módulo "dev/si.c" para processar terminais virtuais.
.sp

.ip "09.12.87  1.1.1" 20
Foi modificado o módulo "dev/dis.c" para tentar recuperar êrros de
leitura/escrita de maneira mais eficiente, dando recalibrate entre
as diversas tentativas. 

.ip "12.12.87  1.1.2" 20
Foi consertado o problema do "cc[VMIN]" da estrutura "termio",
que ao ser alterado, faltava apagar o evento "inqnempty".
Foi alterado o módulo "ttyctl".

.ip "24.12.87  1.1.3" 20
Foram modificados os módulos "ttyctl" e "ttyread" para resolver
dois problemas: ficar preso no evento "outqstart" quando se passa
do modo cooked para raw e quando se
dá "intr" no final da página do terminal.

.ip "05.01.88  1.1.4" 20
Foi alterada a
rotina "itocore" do módulo "ialloc",
para que o campo "rdev" do
inode na memória agora conténha "0", se o arquivo não for especial. 

.sp
Foram alterados alguns "printf" em "prdev" do módulo "ialloc",
para também informar em qual dispositivo está ocorrendo o problema.

.ip "07.01.88  1.1.5" 20
Foi consertado o esquecimento de desligar o evento "outqempty"
no caso do "eco" das linhas de comunicação. Foram alteradas as
rotinas "ttyout" do módulo "ttyin" e a rotina "ttywrite" do
módulo "ttyread". 

.ip "08.01.88  1.1.6" 20
Foi consertado o FIFO, que agora (como o PIPE) funciona na memória.
Foi inserida uma sincronização,
para que o leitor (ou o escritor) espere pelo
"open" do seu parceiro. Foram alterados os módulos "pipe.c"
e "file.c".

.ip "13.01.88  1.1.7" 20
Foi colocado um teste (com #ifdef SEGURO) no módulo "fio/mount.c"
para verificar, depois de um "umount", se ainda sobraram
INODEs ou BUFFERs do dispositivo no kernel.

.sp
Agora, quando se dá o último "close" em um dispositivo especial
de blocos, são removidos todos os blocos remanescentes do
"cache". Deste modo, quando se troca de disquete (por exemplo),
não se fica com um bloco do disquete anterior na memória. 
Foi inserido uma chamada a "bumerase" no módulo "fio/iread.c".

.ip "12.02.88  1.1.8" 20
Foi implementado a chamada ao sistema "ptrace". Não foi necessário
inserir um estado novo; foi utilizado o estado SCORESLP/SSWAPSLP esperando
no evento "p_trace" para representá-lo. 
Foram realizadas modificações em "proc.h", "excep.c", "signal.c",  
"sysent.c", "sysexec.c", "sysfork.c",
além de criar o módulo novo "systrace.c".

.ip "01.03.88  1.1.9" 20
Faltou desligar o bite de "trace" do SR quando se mandava executar
o programa em STOP com o comando "7"; alterado "sys/systrace.c".

.sp
No módulo "sysfork.c", a chamada ao sistema "wait", em caso de
estado STOP, estava retornando (na parte baixa)
o valor "0x3F" ao invés de "0x7F".

.sp
A chamada ao sistema "wait" foi extendida para devolver
em "_fpc", o "pc" do BUS ERROR e do ADDR ERROR.

.ip "10.03.88  1.1.10" 20
Faltou desligar o bite de "trace" do SR quando
vinha a exceção do TRACE;
alterado "proc/excep.c".

.ip "08.04.88  1.1.11" 20
O Módulo "excep.c" foi alterado para devolver em "fa"
a instrução inválido no caso de SIGILL.

.sp
No caso de um sinal interceptado, agora é passado para o usuário
(através do "sigludium") o "pc" e "fa". Alterado o módulo "signal.c".

.ip "26.04.88  1.1.12" 20
Foi introduzido o processamento do indicador "NDELAY" para linhas
de comunicação. Foram alterados os módulos
uproc.h, sysfio.c e ttyread.c.

.sp
Foi criado a nova chamada ao sistema "sigchild".
Alterados os módulos
sysent.c, sysfork.c e  sysproc.c.

.sp
Foi criado o modo de comunicações para linhas de comunicação.
Alterados
tty.h, ttyin.c, ttyctl.c e ttyread.c.

.sp
Foi finalmente descoberto e corrigido o erro que causava processos
ZUMBI sobrarem em certas condições.
Alterados
sysfork.c e signal.c.

.ip "06.05.88  1.1.13" 20
Foi acrescentado o "driver" do RAM disk.

.sp
Foi alterado a chamada ao sistema "sigchild" para enviar também
o sinal quando o filho entra no estado "STOP".

.ip "28.06.88  1.1.14" 20
Foi alterado o valor inicial de "eol" para <esc>.
Envolveu mudança em "tty.h" e "ttyread".

.ip "08.07.88  1.1.15" 20
Agora, a máscara do hash de processos "nhpash" do SCB já está sendo
inicializada. Alteração em "scb.c".

.ip "02.08.88  1.1.16" 20
Foram feitas diversas pequenas alterações para compilar o
KERNEL com o compilador "plc".

.sp
Foi feito a chamada ao sistema "lockf".
Criação dos módulos "fio/lockf.c" e "h/lockf.h",
e alterações em diversos módulos,
principalmente em "sys/sysfio.c". 

.sp
Foi alterada a alocação de textos para usarem as regiões do
final da memória, o que permite a maior compactação
da memória. Alterado "fio/text.c".

.sp
Foram tirados os pontos de entrada "smul" e "usmul"
de "arit.s", pois o compilador "plc" não
gera código chamando estas rotinas.

.sp
Foram retiradas as restaurações dos PCWs do ICA.
Alterações em "smove.s" e "umove.s"

.sp
Foi implantado a chamada ao sistema "lockf".
Criado "fio/lockf.c", "h/lockf.h" e alterados
"fio/file.c", "sys/sysfio.c".

.sp
Foi alterado "proc/core.c" para que um processo
verifique se há memória adjacente na parte inferior
quando há necessidade de aumento do tamanho do processo.

.ip "12.08.88  2.0.00" 20
Foram inseridos os testes para sinais ("proc/signal.c")
e modo "raw" ("tty/ttyctl.c") para ser compatível
com os executáveis da versão "1.1".

.ip "24.08.88  2.1.00" 20
Foi retirado o "bootcode" do "etc/init", e colocado
em um arquivo separado "iconf/boot.s", inserindo logo
a tentativa de executar "/etc/oinit".

.ip "10.09.88  2.1.02" 20
Foi alterado a inicialização do sistema, podendo-se
dar o nome do programa "init". Mudanças em "iconf/boot.s"
e "etc/kinit.c".

.ip "22.09.88  2.1.03" 20
Foi alterada a programação das linhas de comunicação
para gerararem o sinal "request to send".
Alterado "dev/si.c".

.ip "29.09.88  2.1.04" 20
Foi consertado o pequeno erro no cálculo da conversão do endereço
virtual para físico no caso PEG. Alteradas as rotinas "uatopa"
e "satopa" do múdulo "conf/smove.s".

.ip "01.11.88  2.1.05" 20
As linhas de comunicação no modo "ICOMM" agora não são mais
fechadas, para evitar problemas no funcionamento com 5 fios.
Alterado o módulo "si.c".

.ip "03.12.88  2.1.06" 20
No módulo "tty/ttytl.c", a função "TCSETNR" não estava dando
o "eventdone" em "outqstart".

.sp
Foi estendido a chamada ao sistema "kcntl" para a nova função
"K_SEMAFREE", para liberar um recurso pelo qual um processo está esperando.

.ip "21.01.89  2.1.07" 20
Foi consertado o "dev/si.c" para não mais considerar trocas
de tela no modo "ICOMM".

.ip "25.01.89  2.1.08" 20
Foi consertado o problema do offset para fita magnética raw.
Modificado "dev/physio.c", "dev/sasi.c", "dev/win.c", "dev/dis.c".

.ip "26.01.89  2.1.09" 20
Foi consertado o detalhe do "nice" ser com sinal, e deve ter o
tipo "schar". Modificado "common.h", "proc.h".

.ip "06.03.89  2.1.10" 20
Foi modificado o driver do winchester, para realizar operações
com múltiplos blocos em uma operação, além de utilizar uma
estrutura para acessar os registros do dispositivo.
Alterado "dev/win.c".

.sp
Foi modificado o driver do disquete, para utilizar uma
estrutura para acessar os registros do dispositivo.
Alterado "dev/dis.c".

.sp
Foi introduzido o conceito de "update gradual", em que ao esvaziar
a fila de pedidos de um driver (por exemplo, "win.c"), é chamada
a rotina "boneflush", que procura um block "DIRTY", e inicia a escrita. 
Alterados "bio/bread.c", "proc/excep.c", e os drivers.

.ip "01.04.89  2.1.11" 20
Foram alterados os drivers de disquetes para exercer "open" exclusivo
duplicando a idéia do "O_LOCK", para que "/dev/d5a" e "/dev/rd5a"
tenham o "LOCK" em comum.

.ip "07.05.89  2.2.00" 20
Foi alterado a deteção de erro de "dis.c" para reparar que o
disquete está protegido para escritas.

.ip "21.07.89  2.2.01" 20
Foi criado o diretório "ica.ctl", e realizado uma pequena
reestruturação para modularizar mais o KERNEL, com vista
a atender vários clientes, com peculariedades próprias.

.ip "03.08.89  2.2.02" 20
Alterado o módulo "etc/prot.c" para aceitar "snf" coringa (== 0).

.sp
Feita uma redivisão de diretórios "conf" para que nestes diretórios
fiquem todos os detalhes particulares das máquinas personalizadas.

.ip "16.08.89  2.2.03" 20
Foram realizadas modificações nos módulos "proc/signal.c" e "umove.s"
para que os sinais "SIGBUS" e "SIGADDR" possam
ser interceptados corretamente.

.ip "20.08.89  2.2.04" 20
Foi modificada ligeiramente a "stack" de exceção para possibilitar
a compatibilidade "plurix/tropix". Retirado o "rot" da "stack".
Modificados "h/reg.h", "ctl.s".

.sp
Foram alterados "dis.c" e "delay.dis.c" para só imprimirem
mensagem de êrro após a segunda tentativa.

.sp
Foi estendido o módulo "etc/scb.c" para imprimir a tabela do
disco como "display c".

.ip "04.09.89  2.3.00" 20
Agora, quando uma linha de comunicações é aberta, o código
de caracteres é "ISO". Alterado "dev/si.c".

.ip "28.11.89  2.3.01" 20
Fora colocados os controles do "cache" do 68020.
Alterados "ctl.s", "kinit.c" e "dispat.c".

.ip "04.12.89  2.3.02" 20
Foram feitas as alterações para o controle de discos "winchester"
com controlador embutido. Alterados "disktb.h", "win.c", "scb.c".

.ip "15.12.89  2.3.03" 20
Foram introduzidas as extensões (apenas ICA) para utilizar a resolução
de microsegundos do relógio interno. Alterados "aux.c", "ctl.s",
"excep.c", "kinit.c" e "systime.c".

.sp
Foi estendido a chamada ao sistema "kcntl" para fornecer os tamanhos do 
processo. Alterado "kcntl.h" e "sysctl.c".

.sp
Foram corrigidos "win.c", "dis.c" e "delay.dis.c" para não exigir
o "count" múltiplo de 512.  

.sp
Foram corrigidos "dis.c" e "delay.dis.c" para processar o "byte count"
correto.

.ip "15.03.90  2.3.04" 20
Foi alterado "si.c" para trabalhar também com 38400 baud.

.ip "11.04.90  2.3.05" 20
Foram alterados "win.c", "dis.c" e "delay.dis.c" para não
aceitarem leituras/escritas de tamanho não múltiplo de 512.

.sp
Foram alterados "ttyread" e "ttyctl" para serem mais ortodoxos e conservativos
na manipulação do semáforo "t_inqnempty".

.sp
Foram alterados "excep.c" e "l.s" para conterem os vetores de
exceção do ponto flutuante.

.ip "08.08.90  2.3.06" 20
Foi introduzido um cálculo do TTYPANIC para o modo de comunicações,
em função do tamanho da CLIST. Alterados "scb.c" e "ttyin.c".

.sp
Foi estendida a análise de "cputype" para o MC-68881, mudando a codificação
para bites. Alterados "common.h", "ctl.s", "kinit.c", "dispat.c" e "excep.c".

.sp
Foram introduzidas os novos ioctl's TCSETCTS e TCRESETCTS em "dev/si.c",
"h/tty.h" e <termio.h>.

.sp
Foram atualizados os protótipos ANSI nos <includes> do "h",
e em razão disto feitas pequenas alterações em "sysuser.c",
"systree.c", "systime.c", "sysent.c",  "sysetc.c" e "stdio.c".

.sp
Já pode ser feito a formatação de disquetes "on-line". Alterados
"dis.c" e "delay.dis.c".

.sp
Consertado "delay.dis.c" para não prender todo o sistema quando
se tenta ler/escrever, mas não há disquete montado.

.sp
Foi introduzido o controle de sto/ld do MC 68881/68882 no kernel.
Alterados "ctl.s", "uproc.h", "common.h", "core.c", "dispat.c", 
"nproc.c", "excep.c", "systrace.c" e "sysexec.c".

.ip "15.09.90  2.3.07" 20
Foram feitas correções no controle de sto/ld do MC 68881/68882 no
kernel. Alterados "kinit.c" e "ctl.s".

.sp
Tentativa de evitar o "bloco residual" em "bumerase". Alterado
"bread.c".

.sp
Agora, pegando a tabela de discos da ROM. Alterado "c.c".

.sp
Inserido a opção de "close sempre" para dispositivos de caracteres.
Alterados "iotab.h", "file.c" e "c.c".

.sp
Adicionado campo de "endpoint" para o TCP/IP. Alterado "file.h".

.sp
Foi criado o sinal "SIGREAD", com todas as suas (deliciosas) conseqüências.
Alterados "h/common.h", "h/tty.h", "h/uproc.h", "h/file.h",
"tty/ttyread.c", "tty/ttyin.c", "tty/ttyctl.c", "dev/term.c",
"dev/si.c" e "proc/signal.c".

.sp
Foi feita uma revisão no "mmuunld". Alterados
"sys/sysetc.c", "fio/text.c", "etc/kinit.c", "etc/aux.c" e "proc/core.c".

.ip "20.12.90  2.3.8" 20
Alterado para trabalhar com símbolos externos de 31 caracteres.
Alterado "etc/aux.c", "sys/sysctl.c", "ica.ctl/smove.s".

.sp
Acertado o detalhe do ".const" do "boot.s".

.ip "10.02.91  2.3.9" 20
Criado e incorporado o driver de pseudo-terminal "dev/pty.c".
Alterados "etc/kinit.c", "h/tty.h" e "c.c".

.ip "02.05.91  2.3.10" 20
Incorporados controles de tamanho (5, 6, 7, 8) bits do caracter,
paridade e stop-bit. Alterado "dev/si.c".

.sp
Criado a chamada ao sistema "select". Alterados 
"h/ioctl.h", "h/tty.h", "h/proc.h",  "sys/sysent.c", "sys/sysetc.c",
"tty/ttyin.c", "tty/ttyread.c", "dev/si.c", "dev/pty.c".

.ip "18.06.91  2.3.11" 20
Foi trocado o nome da chamada ao sistema "select" por "attention".
Foi introduzido mais um argumento na chamada "attention" - o "timeout".
Alterados "sys/sysent.c" e "sys/sysetc.c".

.sp
Foi consertado um pequeno erro no "attention" de "dev/pty.c", para
o servidor.

.sp
Foi consertada a função "toutreset", que esquecia de adicionar o tempo
da entrada retirada na entrada seguinte, e adicionado mais um argumento.
Alterados "proc/excep.c" e "dev/delay.dis.c". 

.ip "16.07.91  2.3.12" 20
Iniciada a inclusão da "internet" no PLURIX.

.ip "07.10.91  ......" 20
Foi acrescentado o "\%P" em "etc/stdio.c".

.sp
Foi consertado um erro de "count" em "/dev/pty.c".

.in
.sp 2
.nf
/*
 ******	TROPIX Icarus *******************************************
 */
.fi
.ip "20.11.89   2.3.00" 20
VERSÃO INICIAL

.sp
Foi separado o tratamento de exceções como bus error, address error, etc
que continua no módulo "excep" do "tconf/ctl.s", do tratamento de chamadas ao
sistema, agora no "syscall" do "tconf/ctl.s, e do de interrupção que está no
"interrupt" do "tconf/ctl.s".

.sp
Foi alterada a rotina "siint" em "dev/si.c" para pegar o vetor de
interrupção diretamente do lugar na stack onde ele foi colocado originalmente
pela própria interrupção. Como a maioria das rotinas de interrupção não
precisam do vetor, não há necessidade de colocá-lo sempre na stack,
como era feito anteriormente, atrasando o atendimento de todas interrupções. 

.sp
Foi criada a rotina "tickfra" em "tconf/ctl.s" para calcular o número de
microssegundos dentro de um tick de relógio. Junto com a modificação
no módulo "clock" da "proc/excep.c" foi possível implementar a chamada ao
sistema "mutime" em "sys/systime.c" com resolução de 3,2 microssegundos.

.sp
Foi criada a rotina "prmutime" em "etc/aux.c" para ser usada na instrumentação
do kernel para análise de desempenho. Esta rotina imprime o tempo em
segundos e microsegundos no formato "\%d, \%6d\n".

.sp
Foi criada a variável "preemptionflag" em "tconf/ctl.s" para quando
estiver com valor zero impedir preemption de processo em modo supervisor.

.sp
Foi criada a variável "bestpripid" em "tconf/ctl.s" que dá a identificação
do processo que tem a prioridade mais alta, i.e., aquele cuja prioridade 
está em "bestpri".

.ip "02.12.89   2.3.01" 20

Foram modificados os módulos "etc/kinit.c", "fio/text.c", "proc/core.c",
"proc/dispat.c", "proc/swap.c", "proc/nproc.c", "sys/sysetc.c", 
"sys/sysexec.c" para ligar e desligar a variável "preemptionflag" de forma
a impedir preemption em modo supervisor nas seguintes condições:

.sp
.in +5
1) Entre um "ctxtsto" e um "ctxtld" porque as áreas de salvamento não
são empilháveis;
.sp
2) Durante a carga e descarga da MMU porque senão o contexto restaurado
ficaria incorreto pois a área da MMU é global e não pertence à pilha do
processo na "uproc";
.sp
3) Quando um processo não está no estado SRUN. Isto ocorre
na criação, no término e nas transições de estado de processos quando
o mesmo, embora rodando, tem momentaneamente estados SNULL, SZOMBIE,
SCORERDY, etc. O problema se dá porque trocar um processo que está
com o controle da UCP significa mudar o seu estado para SCORERDY e
quando ele volta a rodar o "dispatcher" muda o seu estado para SRUN.
Este problema pode ser contornado com uma modificação mais radical
dos estados e do "dispatcher". Ainda não foi feito.
.in -5

.ip "09.12.89   2.3.02" 20

Foi modificado o módulo "newproc" da "proc/nproc.c" para não
haver mais mudança no ponteiro "u.u_proc" do pai para o filho
durante um "fork". Agora o pai continua no estado SRUN e insere o
valor correto do ponteiro "u.u_proc" diretamente na "UPROC" do
filho. Isto permite que se libere a "preemption" em modo supervisor
durante a cópia do filho.

.sp
Verificou-se que o fato de os processos estarem em estados diferentes
de SRUN afeta pouquíssimo ou nada o tempo de troca de processos
quando se tem "preemption" em modo supervisor.
Por enquanto desistiu-se de implementar essas mudanças.

.ip "31.12.89   2.3.03" 20

Foi corrigido o problema da troca de contexto quando se executava um
"pipe". O problema, na realidade, estava na "unicopy" que altera
através da "setr34" diretamente registros da MMU. Estes registros não
eram salvos durante uma troca de contexto. Foram alteradas as rotinas
"mmuld" e "mmuunld" para que esses registros também sejam salvos.

.sp
Foram criadas no módulo "etc/aux.c" duas rotinas para fazer
trace do kernel em memória: "mktrace" e "prmktrace".
As rotinas usam um buffer "ktracearea" que é preenchido de forma circular.
A "prmktrace" imprime em hexa os dados (long) armazenados.

.sp
Foi criada a rotina "ktrace" para fazer trace do kernel mas colocar os
dados no arquivo "ktrace" do root. A "ktrace" fica no módulo "etc/aux.c"
e foi acrescentada a entrada "K_KTRACE" na chamada ao sistema "kcntl"
para ligar e desligar o trace do kernel. Foi alterada também a "kcntl.h".

.ip "28.11.90   2.3.04" 20
Foram acrescentados eventos, semáforos e memória compartilhada.

.sp
Foram acrescentados "h/ipc.h" e "sync/ipc.c" que controlam
eventos, semáforos e memória compartilhada.

.sp
Foram modificados "h/file.h" e "h/inode.h" que agora contém mais
ponteiros para o manuseio de eventos, semáforos e memória compartilhada.

.sp
Foram modificados "fio/file.h" e "fio/iget.c" para liberarem as
áreas relativas a eventos, semáforos e memória compartilhada
quando arquivos são fechados.

.sp
Foi modificado "sys/sysent.c"  criado "sys/ipc.c"
para conter as novas chamadas relativas a 
eventos, semáforos e memória compartilhada.

.sp
Foram modificados "h/default.h" e "h/common.h" para definir novas estruturas.

.sp
Foram modificados "h/scb.h" e "etc/scb.c" para definir novos parâmetros para
eventos, semáforos e memória compartilhada.

.sp
Foi modificado "etc/kinit.c" para innicializar as estruturas relativas a
eventos, semáforos e memória compartilhada.

.sp
Foi modificado "h/uproc.h" para conter um "short" que indica o no. total
de páginas utilizadas pelo processo em memória compartilhada.

.ip "1992-1993  3.0.00" 20
Implementada a INTERNET.

.ip "13.08.93   3.0.02" 20
Foi acertado o detalhe da paridade opcional do UDP.
Decodificado o caso particular de "destination unreachable". 

.ip "21.08.93   3.0.03" 20
Adicionadas mensagens de erro ICMP para "protocolo" e "port" desconhecidos.

.ip "09.12.93   3.0.04" 20
Agora, os tempos dos arquivos ficam no INODE da memória. Criadas as
novas chamadas ao sistema "instat" e "upstat". Alterados diversos
arquivos.

.sp
Agora só se pode dar um LOCK em um arquivo com i_count == 0.
Alterado "sys/sysfio.c".

.ip "28.01.94   3.0.05" 20
Realizadas alterações para a incorporação do "name resolver".
Alterados muitos módulos do "itnet".

.ip "25.08.94   3.0.06" 20
Alterado "ksnd.c" para satisfazer o protocolo TCP dúbio do PC, que
envia o ACK_of_SIN e SIN no mesmo segmento.

.ip "20.09.94   3.0.07" 20
Consertado o problema da comparação circular do TCP (alterados "ksnd.c",
"rcv.c" e "snd.c").
Simplificado o módulo "sysipc.c" para utilizar a comparação circular.

.ip "18.10.94   3.0.08" 20
Tentativa de conserto do problema do recebimento da retransmissão de
segmentos após perdas de segmentos. Alterado "rcv.c".

.sp
Mudado o tempo de "SILENCE" para estados NÃO "ESTABLISHED". Alterado
"daemon.c".

.ip "14.01.95   3.0.09" 20
Colocado um teste em "mmuunldregion" verificando se o ponteiro
corresponde a uma região inexistente.

.sp
Alterado "update" para operar para um "dev" dado, e esperar caso
o semáforo "updlock" esteja ocupado.
Eliminado o semáforo "bfreesema".

.ip "23.01.95   3.0.10" 20
Restaurado o local de onde o relógio é ligado, para permitir ao
disquete funcionar como ROOT.

.ip "02.02.95   3.0.11" 20
Estendida a funcão "GET_DEV_TB" de "kcntl" para fornecer também a tabela
toda.

.ip "08.04.95   3.0.12" 20
Agora, os datagramas das filas RAW são removidos depois de certo tempo.

.sp
Invertida a ordem de chamar "close" para dispositivos BLK em "fio/iread.c".

.ip "22.05.95   3.0.13" 20
Agora, os blocos para datagramas da INTERNET são alocados estaticamente,
no SCB. Alterados "etc/scb.c", "itnet/itn.c" e "itnet/itblock.c".

.in
.sp 2
.nf
/*
 ******	TROPIX PC ***********************************************
 */
.fi
.ip "01.04.96  3.0.1" 20
VERSÃO CORRENTE
.sp

.ip "12.04.96  3.0.2" 20
Foi alterado o endereço do próprio nó para "127.0.0.1".
Foi consertado "itnet/kaddr.c" para reconhecer os vários endereços próprios.
O teclado passou para "spl1" para não atrapalhar o "siosplip".

.sp
Foi estendido o tempo da chamada ao sistema "mutime" para microsegundos.
Alterados "etc/kinit.c" e "sys/systime.c".

.ip "11.08.96  3.0.3" 20
Foi alterada a filosofia de alocação de áreas da ITNET. Agora temos 3
classes: IT_IN, IT_OUT_DATA e IT_OUT_CTL.

.sp
Foram incluídas entradas de 3.5" na tabela de 5.25" dos disquettes,
para o caso de obter a informação errada do CMOS.

.sp
o TROPIX agora tem um "screen saver"! Alterados: "proc/clock.c", "etc/scb.c",
"dev/con.c" e criado "dev/screen_saver.c".

.sp
Consertado "ctl/vector.s": faltava a atualização dos dos ICUs.

.ip "05.01.97  3.0.5" 20
Foi modificada a execução das funções da fila de "timeout"
(módulo "proc/clock.c") para que cada função seja chamada com
a fila solta.

.sp
Feita a primeira versão do "driver" do ZIP paralelo.

.sp
Novo "pcntl": ENABLE_USER_IO e DISABLE_USER_IO.

.sp
Conserto do problema do ano bissexto em "etc/cmos_clock.c".

.sp
Primeira versão de "xcon.c".

.sp
Estendido "dev/hd.c" para usar transferências longas no PIO.

.sp
Revisão de "dev/zip.c" para ficar de acordo com a versão "ppa3.42"
do FreeBSD.

.sp
Elaborada a base de tempo de 1 us, e a medição da velocidade da CPU.

.sp
Estendida a estrutura HDINFO para conter o campo "flags" no momento
usando 16/32 bits.

.sp
Alinhada a tabela DISKTB para ficar com 64 bytes, o que facilita o
cálculo de endereço.

.sp
Eliminadas as colunas finais de "conf/c.c", que não eram usadas.

.sp
Modificado a função "iclose" para a função "close" dos drivers
serem chamados a cada chamada de "close".

.sp
Atualizados todos os drivers de blocos para utilizarem corretamento
os contadores de "open" das entradas da DISKTB e do dispositivo.

.ip "20.03.97  3.0.6" 20
Unificado em "dev/scsi.c" e "h/scsi.h" todos os procedimentos
comuns aos diversos drivers SCSI.

.sp
Aumentado (e muito) o tamanho da CLIST.

.sp
Alterado o "PAGING" para controlar a saída da CLIST (ao invés da
entrada).

.sp
Agora, a saída de "printf" do KERNEL pode ser desviado para
um pseudo-terminal. Introduzidas os IOCTLs "TC_KERNEL_PTY_ON"
e "TC_KERNEL_PTY_OFF".

.sp
O módulo "dev/hd.c" foi estendido para controlar também o segundo
canal IDE. A tabela "bcb.h" foi alterada para conter informações
sobre os novos dois discos IDE. A estrutura "disktb" foi estendida
para conter também o "target".

.ip "04.04.97  3.0.7" 20
Em decorrência da introdução do campo "p_target" na estrutura DISKTB, foi
realizada uma revisão dos módulos que utilizam "p_unit".

.ip "25.04.97  3.0.8" 20
Foi introduzido o teste do teclado estar pronto na interrupção
(em "dev/con.c").

.sp
Foi alterado o "timeout" de 1 s. para 60 ms. da espera pelo endereço
ETHERNET (em "itnet/frame.").

.ip "14.05.97  3.0.9" 20
Foram feitos pequenos consertos em "dev/meta_dos" ("close" faltando
e trocada a ordem "swap/root").

.sp
Introduzida a idéia de "NODEV" e "-" nas especificações de dispositivos
no boot do kernel.

.sp
Agora, se não tiver dispositivo de SWAP, os textos de " chmod -t"
simplesmente NÃO são colocados no SWAP (ao invés de PANIC).

.ip "07.06.97  3.0.10" 20
Alterações em "dev/pty.c": Nome agora sempre do cliente, confere melhor
"kernel_pty_tp", servidor pode dar IOCTL.

.sp
Introduzido "max_wait" e "max_silence" em "t_optmgmt".

.sp
No boot, analisa qual o disquete de 3½.

.ip "16.06.97  3.0.11" 20
Retirado os semáforos "t_outqstart" e "t_olock" da interface de terminais.
Além disto, retirado o estado PAGING (agora é TTYSTOP).

.ip "24.06.97  3.0.12" 20
Introduzido o "/dev/grave" e o seu respectivo ATTENTION.

.ip "26.06.97  3.0.13" 20
Desligando as "lâmpadas" de IDLE e INTR quando entra no X-Window.

.ip "19.07.97  3.0.14" 20
Agora descartando os segmentos DATA da entrada quando dá "t_sndrel".

.ip "22.07.97  3.0.15" 20
Criado o ATTENTION para PIPEs.

.ip "28.07.97  3.0.16" 20
Acrescentado o "_attention_index".

.ip "05.08.97  3.0.17" 20
Retirado o nome do evento das mensagens da XTI.

.ip "12.08.97  3.0.18" 20
Consertado o URGENT do TCP.

.ip "02.09.97  3.0.19" 20
Criada a chamada ao sistema "boot".
Simplificadas as mensagens durante o BOOT.

.ip "04.09.97  3.0.20" 20
Colocado no SCB as variáveis "preemption_mask" e "CSW".

.ip "23.10.97  3.0.21" 20
Inicializando agora o teclado em "dev/con.c".
Alterado o DELAY de todos os módulos para usar a função de 1 us.

.ip "18.11.97  3.0.22" 20
Usando o novo "vector.s" simplificado.
A estrura "vecdef" é agora inteiramente inicializada dinamicamente.

.ip "23.11.97  3.0.23" 20
Foi estendida a pesquisa do ROOT durante o "boot"; agora se o número
mágico não é válido, continua procurando.

.ip "30.11.97  3.0.24" 20
Revisão na parte de DNS para aceitar servidores de "mail" e "alias".

.ip "18.12.97  3.0.25" 20
Conserto no "unbind" para voltar corretamente ao estado anterior.

.ip "23.12.97  3.0.26" 20
Acrescentadas as funções F1-F12, setas, page up, down, ..., para o teclado.

.ip "11.01.98  3.0.27" 20
Revisão no "...nopen" e "...lock" dos vários dispositivos.

.ip "22.01.98  3.0.28" 20
Pequeno acerto no TCSETNR (agora espera a fila de saída esvaziar) e
"ttychars" (agora inicializando todos os campo).
Novos ioctls: TCLOCK e TCFREE.

.ip "21.02.98  3.0.29" 20
A chamada ao sistema "phys" foi interamente refeita; agora podem ser
alocados até 16 MB em regiões de tamanhos sem limites.

.ip "24.02.98  3.0.30" 20
Agora, podemos ter até 16 MB por região!

.ip "28.02.98  3.0.31" 20
Agora a "attention" está desarmando as chamadas. Dispositivo de "pipe"
não está mais sendo forçado a ser igual ao "root". O dispositivo "swap"
não pode mais ser de tipo LINUX.

.ip "06.03.98  3.0.32" 20
Iniciado o conserto da INTERNET: 1. Dois "attentions" faltando para
"tp_inq_nempty"; 2. Blocos sendo perdidos (problema do "last").

.ip "11.03.98  3.0.33" 20
Continuação do conserto da INTERNET: 3. Paradas repentinas (causadas
pela atualização não simultânea de "snd_una" e "snd_wnd").
4. Agora mandando segmentos de teste de 1 byte quando a janela fecha.

.ip "17.03.98  3.0.34" 20
Colocado um SPINLOCK em "tp_inq_lock" no módulo "itnet/snd.c".
Transformado "off_t" em "unsigned long".

.ip "14.07.98  3.1.0" 20
Reforma da INTERNET para usar a fila circular (RND).
Consertado o esquecimento de fechar o dispositivo em "mount".

.ip "20.07.98  3.1.1" 20
Acrescentado o PCI ED.
Consertado o detalhe do DELIM de "ttyread".
Alterados os DELAYs de "zip.c".

.ip "27.07.98  3.1.2" 20
Adicionado o código para uso dos discos e PPP ("sysmap").

.ip "28.07.98  3.1.3" 20
Alterado o "kaddr.c" para usar apenas letras minúsculas no
nome DNS, não reescrever entradas ainda válidas e não realocar entradas
do "/etc/itnetdev".

.sp
Adicionado o código para uso do ETHERNET ("sysmap").

.ip "25.08.98  3.1.4" 20
Colocado o "pgname" na tabela de processos. Tiradas algumas
variáveis não usadas da UPROC.

.sp
Reforma das regiões (1a. parte): usando páginas de MMU
exclusivas para o processo.

.ip "05.12.98  3.1.5" 20
Reformado o PHYS para também usar as 
páginas de MMU exclusivas para o processo.

.sp
Reformado o SHMEM para também usar as 
páginas de MMU exclusivas para o processo
(além de colocar o REGIONL no KFILE e só permitir
uma região por arquivo).

.ip "21.01.99  3.1.6" 20
Reformado a parte de "text": unificado "xalloc" com "xget" e
"xrelease" com "xput".

.sp
Consertado o problema dos servidores de correio sem endereço
e/ou mesma precedência.

.sp
Reformulada a gerência de memória: agora cada processo tem
diretórios e tabelas de páginas exclusivos.
Agora o registro "cr3" só é recarregado quando estritamente
necessário ("mmu_dirty").

.sp
Foram revistos o "phys" e a memória compartilhada.
Foram unificadas as regiões regulares com as "phys"
e memória compartilhada.

.sp
Refeitas as estruturas KFILE e INODE com uniões.

.sp
Retirado todo o código do SWAP.

.sp
Colocada a mensagem de "tentando ROOT ..." durante o "boot".

.sp
Revisão em "ed.c".

.sp
Revisão "itnet/kaddr.c", para retransmitir o pedido de DNS.

.ip "09.02.99  3.1.7" 20
Consertado o problema da falta de SWAP.

.sp
Em revisão as cores do "video.c".

.sp
O "screen saver" é suspenso para que sejam escritas
as mensagens de erro do kernel. Alterados: "etc/printf.c" e "dev/con.c".

.sp
Alterado "dev/fd.c" para continuar eternamente, no caso de erro de "overrun".

.ip "14.04.99  3.1.8" 20
Adicionado o FAT32 na tabela.

.ip "10.06.99  3.2.0" 20
Adicionado o FAT32 no arquivos "meta_dos.c".

.sp
Criado o "driver" da SB16.

.ip "02.07.99  3.2.1" 20
Revisão no DNS, para aceitar um computador e domínio com o mesmo nome.

.sp
Revisão no DNS, para fazer a busca reversa (nome do computador a partir do endereço).

.ip "24.07.99  3.2.2" 20
Colocado um teste de fragmentação em "ip.c".

.ip "23.10.99  3.2.3" 20
Instalado a nova versão das funções de "timeout" (em "proc/clock.c").

.ip "04.11.99" 20
Instalado a nova versão das funções de "malloc" (em "mem/malloc.c").

.ip "08.12.99" 20
Carregando "/lib/libt.o".

.sp
Postas no SCB dialog os nomes do "init", "sh" e "libt.o".

.sp
Agora, um processo poder ter região DATA de tamanho NULO.

.ip "13.02.00" 20
Introduzido o tratamento de dispositivos ATAPI.

.ip "13.03.00" 20
Introduzido o tratamento de bibliotecas compartilhadas.

.ip "13.04.00" 20
Alterado os tempos de "timeout" de "dev/fd.c".

.ip "17.04.00" 20
Consertado um pequeno problema do "timeout" (found) e
alterados os TIMEOUTs do "/dev/fd.c".

.ip "04.05.00" 20
Inserido o JAZ.

.ip "08.05.00" 20
Inserido o driver para a placa "ethernet" RealTek 100 Mbs.

.ip "06.06.00  4.0.0" 20
Atualizados os nomes das partições.

.ip "29.06.00" 20
O núcleo agora herda o valor de DELAY do "boot2".
A versão do BCB é conferida.

.ip "14.07.00" 20
Consertado o erro "Tipo inválido de exceção na pilha", na recepção
de sinais. Alterado "ctl/vector.s".

.ip "18.07.00" 20
Introduzido o uso do DMA para discos IDE.

.ip "01.08.00" 20
Adicionado a tabela ABNT para os teclados.
.sp
Adicionado o módulo para o reconhecimento de dispositivos PnP.

.ip "10.08.00" 20
Adicionado a conversão de endereço IP dos endereços do próprio
computador.

.ip "14.08.00" 20
Agora, o redirecionamento de datagramas IP já decrementa o TTL.

.ip "17.08.00" 20
Adicionado o endereço do próprio computador nas estruturas TCP, UDP e RAW.
Atualizado também no envio do ICMP ECHO.

.ip "12.10.00" 20
Introduzido o uso da instrução "invlpg" para invalidar entradas na TLB quando
regiões de um processo mudam de tamanho.
.sp
A troca de contexto foi agilizada, evitando recarregar sempre a gerência de
memória.

.ip "03.09.01  4.1.0" 20
Novo "driver" para Adaptec 29160.

.sp
Novo formato da tabela de símbolos SYM.

.sp
Nova chamada ao sistema "getdirent".

.sp
Consertado o problema dos "buracos" NULOs em diretórios.

.ip "10.10.01  4.2.0" 20
Criada a chamada ao sistema "select".

.ip "20.11.01" 20
Mudança do "cache" de blocos para 4 KB.

.ip "20.12.01" 20
Construção da camada de INODEs virtuais.

.ip "20.01.02" 20
Elaboração do sistema de arquivos FAT12/16/32.

.ip "20.03.02" 20
Elaboração do sistema de arquivos CD-ROM.

.ip "20.04.02" 20
Elaboração dos elos simbólicos.

.ip "05.06.02  4.3.0" 20
Nova versão das bibliotecas compartilhadas, com resolução
dinâmica das referências externas.

.ip "16.06.02" 20
Pequena modificação na decisão de usar LFN ou não nos nomes da FAT.

.ip "11.07.02" 20
O código para os dispositivos ATA/ATAPI foi integralmente reescrito.

.ip "15.08.02" 20
Início do uso do sistema de arquivos T1.

.ip "03.09.02" 20
Reformulado o PIPE, com um novo sistema de arquivos.

.ip "05.09.02" 20
Introduzida na parte de 16 bits, a mudança de algumas cores.
Revisão na parte das cores do vídeo em modo texto.

.ip "28.10.02  4.4.0" 20
Adicionado o sistema de arquivos EXT2 LINUX, somente para leituras.

.sp
Iniciado a chamada ao sistema "llseek".

.ip "09.11.02" 20
Completado o sistema de arquivos EXT2 LINUX, também para escritas.

.sp
Chamada ao sistema "llseek" terminada há muito tempo.

.ip "30.12.02" 20
Correção no TCP "snd" para colocar PUSH no segmento que
coincidiu de ficar igual ao tamanho de segmento máximo.

.ip "02.04.03" 20
Pequena correção no "rcvconnect" (código de erro).

.ip "03.04.03" 20
Correção em "sysexec.c" para o caso dos arquivos antigo e novo
serem o mesmo.

.ip "05.05.03" 20
Introduzidos os símbolos INTR_PATTERN e IDLE_PATTERN, nos módulos
"ctl/common.s", "ctl/intr.s", "ctl/vector.s" e "ctl/idle.s",
que permitem habilitar/desabilitar a impressão de padrões
("lampadinhas") no modo texto.

.ip "19.05.03  4.5.0" 20
Modificada a chamada ao sistema "unlink" para que não remova mais
diretórios.

.ip "26.05.03" 20
Modificada ligeiramente a interrupção de "dev/rtl.c".

.ip "28.05.03" 20
Agora as conexões remotas ficam no início da fila TCP, antes das
conexões locais.

.ip "02.06.03" 20
Criado o campo "tp_snd_fin" para verificar o ACK do FIN.

.ip "09.06.03" 20
Alterados vários arquivos para possibilitar o compartilhamento das
interrupções.

.ip "01.10.03" 20
Para a montagem com o sistema de arquivos FAT, cria os nomes DOS de até
"....~99" para o caso LFN.

.ip "03.10.03" 20
Para a montagem com SB_USER no sistema de arquivos T1, usa os "uid/gid"

.ip "10.10.03" 20
Introduzido o erro ENOTMNT.

.ip "21.10.03" 20
Introduzido o DMESG.

.ip "23.10.03" 20
Adicionado a busca de ALIAS do próprio computador em "itnet/kaddr.c".

.ip "05.11.03" 20
Correção da tabela do teclado ABNT.

.ip "27.11.03" 20
Primeiro dia do funcionamento triunfal do camundongo USB.

.ip "02.01.04" 20
Primeira revisão no driver do USB.

.ip "13.01.04" 20
Adicionado o driver para controladores USB do tipo OHCI.

.ip "26.01.04" 20
Adicionado o driver provisório para controladores USB do tipo EHCI.

.sp
Eliminado o "use_polling" na inicialização do USB.

.ip "27.01.04" 20
Modificada a criação de processos: agora as threads do processo 0
compartilham com o pai o diretório de páginas.
Alterados "proc/nproc.c" e "etc/main.c".

.ip "10.04.04" 20
Pequena correçao na criação de diretórios FAT.

.ip "14.05.04" 20
Acrescentado o cálculo correto do tamanho de um CD multisessão.

.sp
Acrescentada a obtenção de nome do volume durante a montagem de uma
partição FAT.

.ip "22.05.04" 20
Pequena correção na comparação dos nomes de CD ("_" igual a ".").

.ip "16.06.04" 20
Aumentado o valor de NUFILE para 32.

.ip "17.06.04" 20
Acrescentado o "KFILE" em mon.

.ip "05.07.04" 20
Começando a funcionar o sistema de arquivos NTFS.
.sp
Colocado o "0" no formato numérico de "printf" para usar como prefixo
o "0" ao invés de branco.

.ip "19.07.04" 20
Acrescentada a função "vprintf".

.ip "02.08.04" 20
Revisão dos MAJORs, deixando um intervalo.

.ip "06.08.04" 20
Montagem de imagens de sistemas de arquivos ("pseudo device").

.ip "16.08.04" 20
Revisão nos endereços virtuais, com o núcleo em 2 GB.

.ip "23.08.04" 20
Tabela dinâmica dos pseudo terminais (no SCB).

.ip "02.09.04" 20
O "driver" para discos (de memória) USB começou a funcionar.

.ip "04.09.04" 20
O núcleo atualiza os dispositivos de "/dev" a partir de distktb.

.ip "10.09.04" 20
Consertado detalhe do NTFS (nem sempre "ntfs_ntvattrget" considera
um erro o fato de NÃO encontrar um atributo).

.ip "16.09.04" 20
Consertado o "pseudo_dev.c".

.ip "19.09.04" 20
Agora, os novos blocos T1 alocados NÃO são mais zerados (isto deve
ser feito por quem chama "t1_block_map".

.ip "27.09.04" 20
Anexação/desanexação dinâmica de dispositivos USB começou a funcionar.

.ip "02.10.04" 20
Criada a função "realloc_byte".

.sp
O sistema de arquivos FAT agora tem um vetor com os valores da FAT.

.ip "05.10.04" 20
Acrescentado o campo "fs_rel_inode_attr" na tabela DIRENT.

.ip "06.10.04" 20
Também são consideradas partições lógicas na criação dinâmica de tabelas
de partições.

.ip "10.10.04" 20
Criados os novos caracteres de conversão "\%v", "\%r" e "\%m" para "printf".

.sp
Foram substituídos todas as referências a "id" para "str".

.ip "11.10.04" 20
Criado os novo caractere de conversão "\%g" para "printf".

.sp
Substituídas todas as referencias a "prpgnm" por "\%g".

.ip "08.11.04" 20
Agora o núcleo recebe o "dmesg" do "boot2".

.ip "02.12.04  4.7.0" 20
Corrigido o travamento do TCP/IP na desconexão (corrigido
"itnet/ksnd.c").

.ip "15.12.04" 20
Corrigido o erro de escrita dos diretórios do LINUX
(NULO ao final dos nomes), "ext2fs/ext2dir.c".

.ip "23.12.04" 20
Alterado o local do conteúdo do bloco para o final de ITBLOCK.

.ip "20.01.05  4.8.0" 20
Consertada a montagem de CDROMs (problema do "nojoliet,norock"),
e estendida para aceitar sessões com SUPERBLOCOs inválidos.

.sp
Introduzida a opção "lfn/nolfn" para sistemas de arquivos FAT.

.ip "10.03.05" 20
Consertado o problema do "timeout" para o comando IDENTIFY em
"ata-all.c".

.sp
Alterado o cálculo do cilindro na tripla geométrica da tabela de 
partição (1023).

.ip "29.06.05" 20
Acrescentado o tipo de partição PAR_DOS_EXT_L a "h/disktb.h"
e "dev/disktb.c".

.ip "01.07.05" 20
Ligado o "#ifdef" dos nomes dos dispositivos em "usb/usb_subr.c" (USB_KNOWNDEV).

.sp
Atualizado o cálculo da geometria, em "dev/disktb.c".

.ip "06.07.05" 20
Atualizado o módulo USB para ler um "hub" filho de outro "hub" ("usb/bus.c")
e aceitar discos com protocolo ATAPI ("usb/ud.c").

.ip "22.08.05" 20
Alterado "ohci.c" para encontrar adequadamente o endereço da porta.

.sp
Início dos trabalhos do protocolo NFS, versão 2.

.ip "02.09.05" 20
Nova conversão dos tempos da FAT.

.ip "30.11.05" 20
As filas HASH dos INODEs e BHEADs agora estão usando SPINLOCK.

.ip "21.12.05" 20
Foi retirado o SPINLOCK da fila da "cput" para tentar resolver o
problema do travamento dos "pty"s. Vamos ver ser resolve.

.ip "31.12.05" 20
Foram retirados os SPINFREE e SPINLOCK de "event.c", "sema.c" e "sleep.c", de tal
modo que durante a inserção na CORERDY o semáforo da fila HASH continua trancado.

.ip "01.07.06  4.9.0" 20
Implementação do modo texto SVGA (1024x768).

.ip "21.08.06" 20
Revista a fila "listen" TCP para remover entradas "esquecidas".

.ip "22.08.06" 20
Criado a nova opção "max_client_conn" da XTI e TCP.

.ip "30.08.06" 20
Versão preliminar do protocolo DHCP.

.ip "04.10.06" 20
Introduzido o INODE dirty count.

.ip "16.08.07" 20
Término da revisão do USB. Introduzido o suporte a controladores EHCI.

.ip "28.08.07" 20
Revisão da obtenção dos caracteres 8.3 (em "fatfs/fatlfn.c").
