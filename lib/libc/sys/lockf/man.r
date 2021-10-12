.bp
.he 'LOCKF (sys)'TROPIX: Manual de Referência'LOCKF (sys)'
.fo 'Atualizado em 16.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "lockf -"
gerência do uso exclusivo de regiões de arquivos
.br

.in
.sp
.b SINTAXE
.(l
#include <sys/syscall.h>
#include <unistd.h>

int	lockf (int fd, int function, off_t size);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "lockf" permite o uso exclusivo de regiões
de arquivos através da colocação de trancas nas regiões desejadas.
A filosofia de implementação seguida pelo TROPIX é bastante próxima
das idéias descritas no POSIX, apêndice "E".
As diferenças mais significativas são as de que
as trancas só podem ser aplicadas a arquivos regulares, e de que
sempre é realizado um teste de acesso
(independente dos atributos do arquivo)
durante as operações de leitura/escrita ("mandatory file record
locking" ou "enforcement mode record locks").

.sp
As trancas nas regiões de um arquivo são possuídas pelo processo
que as criou, e elas são associadas ao arquivo, que deve estar aberto.
Se o processo fechar um arquivo do qual ele ainda possui trancas de regiões,
elas são liberadas.
Analogamente,
se o processo terminar ainda com arquivos abertos dos quais possui
trancas,
todas as trancas são liberadas
como parte do procedimento automático de fechamento dos seus arquivos.

.sp
Regiões trancadas não são herdadas por processo filhos, mas são
conservadas através de uma chamada ao sistema "exec" (sys),
a não ser que o arquivo tenha sido fechado durante o "exec".

.sp
O parâmetro "fd" é o descritor de um arquivo aberto,
obtido por uma das chamadas "creat", "open",
"dup" ou "fcntl".
É necessário que o descritor tenha permissão de escrita para que
o processo possa trancar uma região.
Por outro lado, não há distinção entre trancas de leitura ou
escrita; uma vez trancada uma região, um outro processo não
pode ler nem escrever naquela região do arquivo.

.sp
O parâmetro "function" controla a ação a ser executada. 
Os valores válidos são definidos em <unistd.h>, e significam:

.in +5

.ip F_ULOCK: 10
Libera uma região trancada.

.ip F_LOCK: 10
Tranca uma região do arquivo.
Se a região já estiver trancada por outro processo, aguarda
a sua liberação pelo outro processo.

.ip F_TLOCK: 10
Testa e tranca uma região do arquivo.
Se a região já estiver trancada por outro processo,
retorne imediatamente com erro.

.ip F_TEST: 10
Testa se a região tem trancas colocadas por outros processos.

.ep
.in -5

.sp
O parâmetro "size" indica o tamanho (em bytes)
da região a ser trancada/liberada.
O começo da região é dado pelo ponteiro de posição corrente do arquivo
(ver "lseek" (sys)), e se estende para a frente com "size" positivo
e para trás com "size" negativo.
Neste segundo caso, a região termina no byte anterior à posição
corrente (mas não incluindo o byte apontado pelo ponteiro de posição).
Se "size" for NULO, a região termina na posição indicada pelo
final de um arquivo de tamanho máximo possível
(isto é, ela começa na posição corrente e se estende até
final do arquivo qualquer que seja seu tamanho, no presente ou futuro).
Uma região não precisa estar efetivamente alocada ao arquivo
para poder ser trancada, e portanto podemos trancar regiões
após o final do arquivo.

.sp
Regiões trancadas com "F_LOCK" ou F_TLOCK" podem incluir ou
serem incluídas totalmente ou parcialmente por um região
trancada previamente pelo mesmo processo.
Se isto ocorrer, ou haver regiões adjacentes, elas são
combinadas em apenas uma região.
Se uma ação necessitar de mais uma entrada na tabela de
trancas ativas, e ela estar esgotada, a ação não é
executada, e é retornado o erro "EDEADLK".

.sp
Cada ação "F_ULOCK" pode liberar, integralmente ou em parte,
uma ou mais regiões trancadas pelo processo. 
Quando uma região não é liberada integralmente, as regiões
remanescentes continuam trancadas pelo processo. A liberação
do centro de uma região necessita de uma entrada adicional
da tabela de trancas ativas, o que pode acarretar um erro,
como descrito acima.

.sp
Quando se tenta ler/escrever de um arquivo no qual se acessaria
uma região trancada por um outro processo:
.sp
.in +5
Se O_NDELAY (veja "open" (sys)) estiver ligado,
a leitura/escrita retorna imediatamente
com o erro "EAGAIN".
.sp
Se O_NDELAY estiver desligado, a leitura/escrita é bloqueada até
o processo liberar a região.
.ep
.in -5

.sp
A chamada ao sistema "lockf" é uma fonte potencial de "deadlocks".
Isto pode ocorrer quando um processo, dono de uma região trancada,
aguardar a liberação de uma região trancada por um outro processo.
Por este motivo, todas as chamadas ao sistema "read", "write"
e "lockf" tentam prever (e evitar) um "deadlock" antes de
aguardar a liberação da região trancada.
É importante salientar que esta previsão 
deteta os casos usuais de "deadlock", mas não os mais
complexos, envolvendo recursos possuídos por vários processos.

.sp
Processos aguardando a liberação de trancas podem ser interrompidos
através de qualquer sinal, portanto a chamada ao sistema "alarm" (sys)
pode ser usado para providenciar uma interrupção após um tempo
limite de espera por um recurso.


.sp
Uma chamada ao sistema "creat" (sys) ou "open" (sys) com o indicador
"O_TRUNC" ligado retornará com o erro "EAGAIN" se algum outro processo
tiver trancado parte do arquivo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve 0.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
A chamada ao sistema "lockf" deve ser usada preferencialmente
com as chamadas "open", "read", "write", ... e não
"fopen", "fread", "fwrite", ... da "stdio".
Isto porque as rotinas da "stdio" contém "buffers" fora do controle
do sistema operacional, podendo ser lidos/escritos dados
que estão contidos em regiões trancadas
(caso em que serão manipulados dados desatualizados). 
Se for essencial usar estas rotinas, deve-se utilizá-las sem
"buffers" (ver "setbuf" (libc)) ou então descarregá-los
após cada operação (ver "fflush" (libc)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
alarm, creat, dup, exec, fcntl, lseek, open 
.br
.wo "(libc):"
setbuf, fflush
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
IEEE Trial-Use Standard Portable Operating System for
Computer Environments (POSIX), IEEE.

.in
.sp
.b EXEMPLOS
.in 7
A aplicação usual da chamada ao sistema é feito da seguinte forma:
.sp 2
.(l
	lseek (...);		/* Posiciona no registro desejado */

	lockf (F_LOCK, ...);	/* Tranca a região */

	read (...);		/* le o registro */

	lseek (...);		/* Se for necessário, atualiza */
	write (...);		/* o registro */

	lseek (...);		/* Libera a região */
	lockf (F_ULOCK, ...);
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
