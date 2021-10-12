.bp
.he 'INSTAT (sys)'TROPIX: Manual de Referência'INSTAT (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.bc	/*************************************/
.wo "instat  -"
obtem o estado de um arquivo dados "dev" e "ino"
.br
.ec	/*************************************/
.wo "instat  -"
obtem o estado de um arquivo dados o dispositivo e o
.br
.wo "          nó-índice"

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

int	instat (int dev, int ino, STAT  *sp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "instat" obtém o estado (isto é, uma série de
informações) de um arquivo.
Com a chamada ao sistema "instat" o arquivo é identificado
não pelo seu "caminho" como em "stat" (sys), mas sim pelo par
(dispositivo, nó-índice).

.sp
Não é necessário ter nenhuma permissão de leitura, escrita ou execução
do arquivo.
.bc	/*************************************/
É necessário ter no mínimo uma das permissões para ler,
escrever ou executar o arquivo.
Esta chamada verifica apenas a proteção do arquivo em si,
e não do caminho que leva a ele, e é destinada a utilitários especiais.
.ec	/*************************************/

.sp
O par (dispositivo, nó-índice) normalmente é obtido atráves de um "stat"
anterior de um diretório, de onde se obtém o dispositivo "dev",
seguido da leitura deste diretório, de onde se obtém o nó-índice
"ino" de cada um de seus filhos.

.sp
O argumento "sp" é um ponteiro para uma estrutura "STAT", que
é preenchida com as informações sobre o arquivo
(que podem ser vistas em "stat" (sys)).

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
.wo "(sys): "
stat, upstat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo
