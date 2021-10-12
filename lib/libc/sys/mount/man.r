.bp
.he 'MOUNT (sys)'TROPIX: Manual de Referência'MOUNT (sys)'
.fo 'Atualizado em 04.10.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "mount -"
monta um sistema de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/common.h>
#include <sys/sync.h>
#include <sys/sb.h>
#include <sys/syscall.h>

int mount (const char *dev, const char *dir, int flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "mount" faz com que um sistema de arquivos
contido no arquivo especial de blocos "dev" seja
montado no diretório identificado por "dir". Tanto "dev" como
"dir" são ponteiros para cadeias de caracteres contendo caminhos. 

.sp
A chamada ao sistema "mount" tenta identificar automaticamente o
tipo do sistema de arquivos. Se houver um código correspondente na entrada
da tabela de partições, este é usado, e em caso contrário são tentados
todos os tipos suportados atualmente: TROPIX V7, TROPIX T1, FAT 12/16/32
e CDROM.

.sp
A variável "flags" contém indicadores contendo várias opções de montagem.
A definição dos vários indicadores está no arquivo <sys/sb.h>,
e uma explicação está em "getmntopt" (libc).

.sp
Se a montagem foi efetuada com sucesso, as referencias a "dir"
irão se referir à raiz do sistema montado. Toda a subárvore
cuja raiz é "dir" (que era acessível antes da montagem) fica oculta.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve zero.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Esta chamada ao sistema pode apenas ser utilizada pelo superusuário,
e é reservada para o utilitário "mount" (cmd).

.sp
Sistemas de arquivos em fita magnética ou em dispositivos
com proteção de escrita devem ser montados sempre
com permissão apenas de leitura.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
mount
.br
.wo "(sys):  "
umount
.br
.wo "(libc): "
getmntopt
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
