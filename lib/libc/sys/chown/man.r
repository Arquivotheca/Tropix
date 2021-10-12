.bp
.he 'CHOWN (sys)'TROPIX: Manual de Referência'CHOWN (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "chown -"
modifica o dono e o grupo de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	chown (const char *path, int owner, int group);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "chown" muda o dono e o grupo
do arquivo "path", atribuindo os novos valores numéricos
"owner" e "group".

.sp
Somente processos com o usuário efetivo igual ao dono do arquivo
ou o superusuário podem trocar o dono/grupo de um arquivo.

.sp
Se a chamada ao sistema é utilizada por um usuário que não é
superusuário, os indicadores S_ISUID ("set user ID") e S_ISGID ("set group ID")
são zerados (ver "chmod" (sys)). 

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se conseguiu alterar 
o dono/grupo do arquivo.
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chown
.br
.wo "(sys): "
chmod
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
