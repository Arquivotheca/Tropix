.bp 1
.he 'CHDIRIP (sys)'TROPIX: Manual de Referência'CHDIRIP (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "chdirip -"
troca de diretório corrente (dado o ponteiro)
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/inode.h>
#include <sys/syscall.h>

int	chdirip (const INODE *ip);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "chdirip" faz com que o diretório corrente
passe a ser o diretório cujo ponteiro para a estrutura
"INODE" é dado pelo argumento "ip". Este ponteiro deve ser
dado como um endereço virtual do espaço de endereçamento
do núcleo do sistema operacional.

.sp
Esta chamada é reservada para superusuários, e é destinada
para utilitários sofisticados.

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
chdir
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
