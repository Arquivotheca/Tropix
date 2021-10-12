.bp 1
.he 'OPEN (sys)'TROPIX: Manual de Referência'OPEN (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "open -"
abre um arquivo para leitura e/ou escrita
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int	open (const char *path, int oflag, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "open" abre para leitura e/ou escrita o arquivo
cujo nome é dado pela cadeia de caracteres apontada por "path".
Os indicadores do estado do arquivo são atribuídos de acordo com "oflag".

.sp
O valor de "oflag" é construído a partir
de um outório dos indicadores da
lista abaixo (os três primeiros são mutuamente exclusivos):

.in 8
.ip O_RDONLY 10
O arquivo é aberto apenas para leituras.

.ip O_WRONLY 10
O arquivo é aberto apenas para escritas.

.ip O_RDWR 10
O arquivo é aberto para leituras e escritas.

.ip O_NDELAY 10
Este indicador (cujo nome é O_NONBLOCK no padrão POSIX),
afeta leituras e escritas
subsequentes (veja "read" e "write" (sys)).

.ip O_APPEND 10
Se este indicador estiver ligado, o ponteiro de posição do
arquivo é colocado no final do arquivo antes de cada escrita.

.ip O_CREAT 10
Se o arquivo já existe, este indicador não tem efeito.
Em caso contrário, a chamada ao sistema espera o argumento adicional
"int mode"; o arquivo é criado, do mesmo modo do que a chamada
.sp
.nf
			"creat (path, mode)"

.fi
(veja "creat" (sys)).

.ip O_TRUNC 10
Se o arquivo já existe, o seu tamanho é truncado a zero
(isto é, o seu conteúdo é apagado), e o modo e dono/grupo
são conservados.

.ip O_EXCL 10
Se O_EXCL e O_CREAT estão ligados, a chamada "open" irá
falhar caso o arquivo já exista. A função deste indicador
é possibilitar executar a operação composta de "certificar-se
de que um arquivo não existe, e neste caso criá-lo" de
maneira indivisível. 

.ip O_LOCK 10
Abra o arquivo de maneira exclusiva, isto é, enquanto este descritor
não for fechado, não será possível um outro "open" neste arquivo.

.ep
.in 5
.sp 2
Alguns destes indicadores podem ser alterados posteriormente
ao "open" através da chamada "fcntl" (sys).

.sp
O ponteiro de posição do arquivo é colocado no início do
arquivo.

.sp
O descritor de arquivos retornado irá continuar
aberto através de chamadas
"exec" (veja "fcntl" (sys)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o descritor do
arquivo, que deve ser usado em todas as operações com o arquivo.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chmod, close, creat, dup, fcntl, intro, lseek, read
.br
.wo "        umask, write, signal"
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
