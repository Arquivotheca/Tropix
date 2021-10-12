.bp
.he 'UMOUNT (sys)'TROPIX: Manual de Referência'UMOUNT (sys)'
.fo 'Atualizado em 05.10.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "umount -"
desmonta um sistema de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	umount (const char *dev, int flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "umount" faz com que o sistema de arquivos
contido no arquivo especial de blocos "dev" seja desmontado.

.sp
Após a desmontagem deste sistema de arquivos, o diretório
sobre o qual o sistema estava montado volta a ter o seu significado
normal.

.sp
A variável "flags" contém indicadores contendo várias opções de montagem.
A definição dos vários indicadores está no arquivo <sys/sb.h>,
e uma explicação está em "getmntopt" (libc).

.sp
Se "flags" dado for diferente de "-1", serão usados os indicadores dados
nesta variável; em caso contrário serão usados os indicadores dados
na montagem (pela chamada ao sistema "mount" (sys).
Na presente versão, somente é relevante o indicador SB_EJECT.

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
.b OBSERVAÇÃO
.in 5
Esta chamada ao sistema pode ser utilizada apenas por superusuários,
e é reservada para o utilitário "umount" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
umount
.br
.wo "(sys): "
mount
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
