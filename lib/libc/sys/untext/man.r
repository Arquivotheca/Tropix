.bp
.he 'UNTEXT (sys)'TROPIX: Manual de Referência'UNTEXT (sys)'
.fo 'Atualizado em 23.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "untext -"
libera os recursos de um programa reentrante 
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	untext (const char *path);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Um programa reentrante ("text") ocupa uma série de recursos do sistema
operacional, e em particular, memória interna e espaço na área de
"swap" no disco. Isto pode ocorrer, mesmo que no momento não haja nenhum
usuário do programa.

.sp
A chamada ao sistema "untext" libera os
recursos ocupados pelo programa reentrante cujo nome é dado pela cadeia
de caracteres apontados por "path"
(desde que no momento ele não esteja sendo utilizado).

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
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
untext, cc, ld (opção "-n"), chmod (opção "-m")
.br
.wo "(sys): "
chmod
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
