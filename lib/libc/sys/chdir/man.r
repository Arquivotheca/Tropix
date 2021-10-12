.bp
.he 'CHDIR (sys)'TROPIX: Manual de Referência'CHDIR (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "chdir -"
troca de diretório corrente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	chdir (const char *path);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "chdir" faz com que o diretório corrente
passe a ser o diretório apontado por "path".

.sp
O diretório corrente é o ponto de partida para buscas de caminhos
que não começam por "/".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se conseguiu trocar de diretório
corrente. 
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chroot
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
