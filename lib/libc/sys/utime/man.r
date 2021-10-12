.bp
.he 'UTIME (sys)'TROPIX: Manual de Referência'UTIME (sys)'
.fo 'Atualizado em 24.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "utime -"
modifica os tempos de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/syscall.h>

int	utime (const char *path, const time_t tp[]);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "utime" permite alterar os tempos de acesso,
modificação e criação do arquivo cujo nome é dado pela cadeia de
caracteres apontada por "path". O processo tem que ser o dono do arquivo
ou ser superusuário para poder utilizar esta chamada ao sistema.

.sp
Os tempos são medidos pelo número de segundos decorridos
desde 00:00:00, do dia primeiro de janeiro de 1970, horário UT
(Tempo Universal, antes chamado de GMT).

.sp
Se "tp" for um ponteiro NULO, os tempos de acesso e modificação
são alterados para o tempo corrente (o tempo de criação não é
alterado).
.sp

Se "tp" for um ponteiro não NULO, ele é interpretado como sendo
um ponteiro para um vetor de 3 elementos do tipo "time_t", contendo
os tempos de acesso, modificação e criação (nesta ordem) que
devem ser postos no arquivo.

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
Repare que no TROPIX, o arquivo possui o tempo de criação
(e não o tempo de modificação do nó-índice), e
esta chamada ao sistema pode modificar todos os três tempos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
stat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
