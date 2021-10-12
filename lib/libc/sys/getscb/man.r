.bp
.he 'GETSCB (sys)'TROPIX: Manual de Referência'GETSCB (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "getscb -"
obtem o bloco de controle do sistema operacional
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

SCB	*getscb (SCB *sp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getscb" obtém o bloco de controle do sistema
operacional. Este bloco contém diversas informações sobre o núcleo
do sistema, que podem ser vistas em <sys/scb.h>.

.sp
A chamada ao sistema devolve o endereço virtual do
bloco de controle do sistema, no espaço de endereçamento do núcleo,
que pode ser usado por "phys" (sys) para acessar o bloco.

.sp
Além disto, se "sp" for um ponteiro não NULO,
a chamada copia o bloco de controle
para a área apontada por "sp".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o endereço do
bloco de controle do sistema.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada pode ser utilizada por qualquer usuário, e é própria
para utilitários consultarem algumas informações do núcleo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
phys
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
