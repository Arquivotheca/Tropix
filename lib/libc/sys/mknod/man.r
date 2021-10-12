.bp
.he 'MKNOD (sys)'TROPIX: Manual de Referência'MKNOD (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "mknod -"
cria um arquivo de um tipo qualquer
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

int	mknod (const char *path, int mode, int dev);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "mknod" cria um novo arquivo cujo nome
é dado pela cadeia de caracteres apontada por "path".

.sp
O modo do arquivo é obtido através de "mode",
e é semelhante ao argumento "mode" utilizado na chamada ao sistema "chmod"
(sys), com a exceção de que é necessário dar adicionalmente
o tipo do arquivo:

.sp
.in 12
.wo "S_IFREG:  "
Regular
.br
.wo "S_IFDIR:  "
Diretório
.br
.wo "S_IFIFO:  "
FIFO
.br
.wo "S_IFBLK:  "
Especial de Blocos
.br
.wo "S_IFCHR:  "
Especial de Caracteres

.sp
.in 5
O arquivo <stat.h> contém a definição completa dos diversos indicadores
do modo de um arquivo.

.sp
O UID do arquivo será o UID efetivo do processo, e o GID
do arquivo será o GID efetivo do processo.

.sp
Valores de tipos de arquivo não dados acima são indefinidos,
e não devem ser usados.

.sp
Os 9 indicadores de mais baixa ordem do "mode" (a proteção) são
modificados pela máscara de criação de arquivos do processo;
todos os indicadores ligados da máscara são zerados (veja "umask" (sys)).

.sp
Se "mode" indica um arquivo especial de blocos ou caracteres,
o argumento "dev" indica a especificação de identificação
do dispositivo ("major" + "minor").
Se o arquivo não for especial de blocos ou caracteres,
"dev" é ignorado.

.sp
Esta chamada ao sistema só pode ser usada por processos
não superusuários para a criação de FIFOs.

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
.b OBSERVAÇÃO
.in 5
Para a criação de diretórios, deve ser preferida a chamada ao sistema
"mkdir" (sys), pois "mknod" não cria as entradas "." e ".." necessárias
para o funcionamento de um diretório.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkdir, chmod
.br
.wo "(sys): "
mkdir, chmod, exec, umask
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
