.bp
.he 'CLOSE (sys)'TROPIX: Manual de Referência'CLOSE (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "close -"
fecha um arquivo (dado o descritor)
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	close (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "close" fecha o arquivo associado ao descritor
"fd". O descritor "fd" é obtido a partir das chamadas "creat", "open",
"dup", "fcntl" ou "pipe".

.sp
Todas as trancas associadas ao descritor são liberadas (ver "lockf" (sys)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se conseguiu fechar arquivo. 
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, dup, exec, fcntl, lockf, open, pipe, signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
