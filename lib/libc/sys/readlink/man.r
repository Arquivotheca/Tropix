.bp
.he 'READLINK (sys)'TROPIX: Manual de Referência'READLINK (sys)'
.fo 'Atualizado em 16.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "readlink -"
lê o valor de um elo simbólico
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	readlink (const char *path, char *area, int count);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "readlink" armazena o conteúdo do elo simbólico
"path" na área "area", que contém o tamanho "count". Se o espaço o
permitir, o conteúdo é terminado com um caractere NULO.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve o número de caracteres armazenados
na área (incluíndo o possível NULO final).
Em caso de êrro, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ln
.br
.wo "(sys): "
stat, lstat, symlink
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
