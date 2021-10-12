.bp 1
.he 'INOPEN (sys)'TROPIX: Manual de Referência'INOPEN (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "inopen -"
abre um arquivo dados dispositivo e nó-índice
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	inopen (int dev, int ino);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "inopen" abre um arquivo que é identificado
não pelo seu "caminho" como "open" (sys), mas sim pelo par
(dispositivo, nó-índice). Este par pode ser obtido através
de uma chamada "stat" ou "fstat" (sys).

.sp
Esta chamada verifica apenas
a proteção do arquivo em si, e não do caminho que leva a ele,
e é destinada a utilitários especiais.

.sp
O arquivo é aberto apenas para leituras.

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
arquivo.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open, chmod, close, creat, dup, fcntl, intro, lseek,
.br
.wo "        read, umask, write"
.br

.in
.sp
.b ESTADO
.in 5
Efetivo
.in
