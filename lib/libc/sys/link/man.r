.bp
.he 'LINK (sys)'TROPIX: Manual de Referência'LINK (sys)'
.fo 'Atualizado em 16.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "link -"
cria um novo nome para um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	link (const char *old_path, const char *new_path);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "link" cria um novo nome
(uma entrada em um diretório, "hard-link") para um arquivo já existente.

.sp
O nome do arquivo é apontado por "old_path"
e o novo nome a ser criado é apontado por "new_path".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero em caso de sucesso.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Todos os nomes de um arquivo devem pertencer ao mesmo sistema
de arquivos.

.sp
Somente o superusuário pode criar mais um nome para um diretório.

.sp
Nem todos os sistemas de arquivos permitem a criação de novos nomes
para um arquivo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
symlink, unlink
.br
.wo "(cmd): "
ln, rm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
