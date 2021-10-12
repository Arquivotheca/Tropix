.bp
.he 'CHROOT (sys)'TROPIX: Manual de Referência'CHROOT (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "chroot -"
troca de diretório raiz
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	chroot (const char *path);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "chroot" muda o diretório raiz, para 
o diretório apontando por "path". O diretório raiz é o diretório que
é usado como ponto inicial de busca para caminhos que
começam com "/".

.sp
O diretório corrente do processo não é modificado.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se conseguiu mudar o diretório raiz. 
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Apenas processos cujo usuário efetivo é o superusuário
podem utilizar esta chamada ao sistema.

.sp
A entrada ".." do diretório raiz é interpretado como
o próprio diretório raiz. Portanto, ".." não pode ser usado
para acessar arquivos fora da subárvore cuja raiz é o diretório
raiz.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chdir
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
