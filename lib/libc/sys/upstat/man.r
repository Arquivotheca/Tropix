.bp
.he 'UPSTAT (sys)'TROPIX: Manual de Referência'UPSTAT (sys)'
.fo 'Atualizado em 24.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "upstat  -"
obtem o estado de um diretório montado
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/syscall.h>
#include <stat.h>

int	upstat (dev_t dev, STAT *sp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "upstat" obtém o estado (isto é, uma série de
informações) de um diretório oculto sob um ponto de montagem do
sistema de arquivos.

O diretório é identificado pelo número do dispositivo "dev"
do sistema de arquivos nele montado.
O argumento "sp" é um ponteiro para uma estrutura "STAT", que
é preenchida com as informações sobre o arquivo
(que podem ser vistas em "stat" (sys)).

.sp
Não é necessário ter nenhuma permissão de leitura, escrita ou execução
do arquivo.

.sp
Esta chamada ao sistema é especialmente útil quando se caminha no
sistema de arquivos no sentido das folhas em direção à raiz
(através de ".."), para ultrapassar um ponto de montagem.
Ao atingir um ponto de montagem, isto é, a raiz do sistema de arquivos
montado, pode-se usar "upstat" para obter o estado do diretório
oculto sob o ponto de montagem (em especial o seu nó-índice), e
o nó índice de seu diretório pai (que é o valor de retorno da chamada).
Lendo-se este diretório pai e procurando-se o nó índice do filho,
obtem-se o nome deste diretório oculto.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o número do nó-índice
do diretório pai do diretório oculto pela montagem.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada ao sistema foi criada principalmente para facilitar
a função da biblioteca "getcwd" (libc) (e portanto o utilitário "pwd" (cmd)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
pwd
.br
.wo "(libc): "
getcwd
.br
.wo "(sys):  "
stat, instat
.br

.in
.sp
.b ESTADO
.in 5
Experimental
