.bp
.he 'IOCTL (sys)'TROPIX: Manual de Referência'IOCTL (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "ioctl -"
controla dispositivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	ioctl (int fd, int cmd, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "ioctl" efetua uma série de controles em
dispositivos de blocos (estruturados) ou de caracteres (não estrurados).
Estes controles são normalmente dependentes do dispositivo em questão.

.sp
O Argumento "fd" é um descritor de um arquivo aberto, que se refere
ao dispositivo desejado.
O argumento "cmd" seleciona a função de controle
a ser executada no dispositivo.

.sp
Dependendo da função de controle e do tipo de dispositivo podem 
ser necessários argumentos e/ou arquivos de inclusão adicionais.
Por exemplo, para linhas de comunicação, a chamada "ioctl" toma a
forma (ver "termio" (fmt)):

.(l

		#include <sys/types.h>
		#include <sys/syscall.h>

		#include <termio.h>

		int	ioctl (int fd, int cmd, TERMIO *tp);

.)l

Como outro exemplo, para determinar se um dispositivo é
uma fita magnética, a chamada "ioctl" toma a
forma (ver <sys/ioctl.h>):

.(l

		#include <sys/ioctl.h>
		#include <sys/syscall.h>

		int	ioctl (int fd, MTISATAPE);

.)l

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve um valor que depende
da função de controle e do dispositivo, mas sempre é um valor positivo. 
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(fmt): "
termio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
