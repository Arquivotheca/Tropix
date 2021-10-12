.bp
.he 'CHMOD (sys)'TROPIX: Manual de Referência'CHMOD (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "chmod -"
modifica a permissão de acesso a um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

int	chmod (const char *path, int mode);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "chmod" modifica a parte de  permissão de acesso
do modo do arquivo "path", de acordo com o padrão de indicadores dado em
"mode".
.sp

O arquivo <sys/stat.h> contém a definição completa dos diversos indicadores
do modo de um arquivo.
A seguir damos os significados dos principais indicadores
de permissão de acesso:

.in 10
.ip S_ISUID 10
O UID efetivo do processo passa a ser igual ao
dono do arquivo quando o arquivo é executado como um programa
("set user ID"). 

.ip S_ISGID 10
O GID efetivo do processo passa a ser igual ao
grupo do arquivo quando o arquivo é executado como um programa
("set group ID"). 

.ip S_IMETX 10
Uma imagem do programa é guardada na memória interna
após a execução, mesmo que não haja mais usuários do programa.

.ip S_ISVTX 10
Uma imagem do programa é guardada na área de SWAP (em disco)
após a execução, mesmo que não haja mais usuários do programa.

.ip S_IRUSR 10
Permissão de leitura pelo usuário.

.ip S_IWUSR 10
Permissão de escrita pelo usuário.

.ip S_IXUSR 10
Permissão de execução (busca, se diretório) pelo usuário.

.ip S_IRWXG 10
Permissão de leitura, escrita, execução (busca) pelo grupo. 

.ip S_IRWXO 10
Permissão de leitura, escrita, execução (busca) pelos outros.

.ep

.sp 1
.in 5
Para poder trocar 
a parte de permissão de acesso do modo de um arquivo,
o usuário efetivo do processo  
deve ser o dono do arquivo ou então ser o superusuário.

.sp
Se o usuário efetivo do processo não for o superusuário,
os indicadores para guardar a imagem do programa após a execução
(S_IMETX e S_ISVTX) são zerados.

.sp
Se o usuário efetivo do processo não for o superusuário,
e o grupo efetivo do processo não é o grupo do arquivo,
o indicador S_ISGID ("set group ID") é zerado.

.sp
Se um programa reentrante tem um dos indicadores para
guardar a imagem após a execução ligado, o sistema
operacional conserva uma imagem do programa na área de SWAP
ou na memória interna
quando o último usuário do programa termina.
Isto faz com que o programa já esteja disponível
quando o próximo usuário executar o programa,
tornando sua execução mais rápida.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se conseguiu alterar 
a permissão de acesso do arquivo.
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chmod
.br
.wo "(sys): "
chown, creat, exec, fcntl, mknod, open, read, write 
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/sys/stat.h

.in
.sp
.b ESTADO
.in 5
Efetivo.
