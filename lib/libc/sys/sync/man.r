.bp
.he 'SYNC (sys)'TROPIX: Manual de Referência'SYNC (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "sync -"
atualiza o conteúdo do disco
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	sync (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "sync" atualiza os conteúdos dos discos
escrevendo todas as informações contidas no sistema operacional
que ainda estão mais atuais na memória do que nos discos.
Isto inclui superblocos modificados, nós-índices modificados
e blocos de E/S ainda não escritos.

.sp
Somente superusuários podem utilizar esta chamada,
que deve ser utilizada por programas que
examinam um sistema de arquivos, como por exemplo "fsck".
Sua utilização é essencial antes de um "boot".

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
No TROPIX esta chamada ao sistema é síncrona, isto é,
após o retorno com sucesso, as escritas no disco já foram
completadas.

.in
.sp
.b ESTADO
.in 5
Efetivo.
