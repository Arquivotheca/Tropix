.bp
.he 'SELECT (sys)'TROPIX: Manual de Referência'SELECT (sys)'
.fo 'Atualizado em 16.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "select -"
multiplexação de entrada síncrona
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/times.h>
#include <sys/select.h>

int	select (int n, fd_set *read, fd_set *write,
				fd_set *excep, const MUTM *timeout);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "select" examina os conjuntos de descritores de arquivos
cujos endereços são dados em "read", "write" e "excep", para ver se algum
dos descritores estão prontos para a leitura, escrita, ou se há alguma
condição de erro pendente.

.sp
Os primeiros "n" descritores são examinados de cada conjuntos, isto é,
são examinados os descritores de números "0" a "n-1" de cada conjunto.
Após o retorno, "select" substitui os conjuntos por subconjuntos contendo
os descritores que estão prontos para as operações correspondentes.
É retornado o número total de descritores prontos de todos os conjuntos.

.sp
Os conjuntos de descritores são armazenados em campos de bits de um inteiro.
As segunites macros estão disponíveis para a manipulação destes campos:

.in +3  
.ip "FD_ZERO (&fdset):" 23
Inicializa o conjunto de descritores "fdset" como o conjunto vazio.

.ip "FD_SET (fd, &fdset):" 23
Inclui o descritor "fd" no conjunto "fdset".

.ip "FD_CLR (fd, &fdset):" 23
Remove o descritor "fd" do conjunto "fdset".

.ip "FD_ISSET (fd, &fdset):" 23
Retorna NÃO NULO se o descritor "fd" pertence ao conjunto "fdset".

.ep
.in -3  

.sp
Estas macros só são definidas se o valor do descritor for não negativo
e menor do que FD_SETSIZE.

.sp
Se "timeout" for um ponteiro NÃO NULO, ele especifica um intervalo
máximo de tempo para que a chamada a "select" retorne. Se "timeout"
for um ponteiro NULO, a espera será ilimitada. Se "timeout" for um
ponteiro apontando para um valor NULO, o retorno será imediato
("polling").

.sp
Cada um dos ponteiros "read", "write" ou "excep" pode ser um ponteiro
NULO, se os respectivos descritores não forem de interesse.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Normalmente, a chamada ao sistema "select" devolve
o número total de descritores prontos de todos os conjuntos;
em caso de êrro, devolve "-1". Se o limite de tempo for expirado,
devolve "0". Se o valor retornado não foi positivo, os conjuntos
de descritores não são modificados.

.in
.sp
.b OBSERVAÇÕES
.in 5
A chamada ao sistema "select" NÃO está disponível para todos os dispositivos.
No momento, só está implementada para terminais (linhas de comunicações,
dispositivos "/dev/console", "/dev/tty*", "/dev/video*"),
pseudo-terminais (tanto do lado do cliente como do servidor, 
dispositivos "/dev/ptyc*" e "/dev/ptys*"), na internet (dispositivos
"/dev/udp", "/dev/tcp", "/dev/raw"), arquivos "pipe"
e na espera por um processo filho
terminado (dispositivo "/dev/grave").

.sp
Além disto, nesta versão, só está implementado o conjunto "read".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
attention, open, read, wait, pipe
.br
.wo "(fmt): "
termio
.br
.wo "(xti): "
t_open
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
