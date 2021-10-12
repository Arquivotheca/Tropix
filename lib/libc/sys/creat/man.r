.bp
.he 'CREAT (sys)'TROPIX: Manual de Referência'CREAT (sys)'
.fo 'Atualizado em 12.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "creat -"
cria um arquivo ou reescreve em um antigo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

int	creat (const char *path, int mode);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "creat" cria um arquivo novo ou prepara para
a reescrita em um arquivo já existente. Em ambos casos, o nome
do arquivo é dado pela cadeia de caracteres apontada por "path".

.sp
Se o arquivo já existe, o tamanho é truncado a zero (isto é,
o seu conteúdo é apagado), mas o modo e UID/GID são conservados.

.sp
Em caso contrário, o UID/GID do arquivo será o UID/GID
efetivo do processo, e o modo do arquivo
(exceto o seu tipo "S_IFMT", ver <sys/stat.h>),
é obtido a partir do valor de "mode", com as seguintes
modificações:

.sp 1
.in 10
Todos os indicadores ligados da máscara de criação de arquivos
são zerados (ver "umask" (sys)). 

.sp
Os indicadores para guardar a imagem dos programas reentrantes
("S_ISUID" e "S_ISGID", ver "chmod" (sys)),
são zerados.

.sp 1
.in 5
Após um "creat" com sucesso, a chamada devolve um descritor de arquivos
que permite apenas escritas, mesmo que o modo dado não permita escritas.

.sp
O ponteiro do arquivo está apontando
para o começo do arquivo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve o descritor do arquivo (um número
não negativo) se
conseguiu criar o arquivo novo ou preparar
a reescrita em um arquivo já existente.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Um arquivo novo pode ser criado com um modo que proíba a escrita.

.sp
O descritor do arquivo é programado para que o arquivo
permaneça aberto através das chamadas ao sistema "exec".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chmod, close, dup, fcntl, lseek, open, read, umask,
.br
.wo "        write"
.br

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in
