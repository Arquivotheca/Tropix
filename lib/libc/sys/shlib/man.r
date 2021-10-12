.bp
.he 'SHLIB (sys)'TROPIX: Manual de Referência'SHLIB (sys)'
.fo 'Atualizado em 29.12.99'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "shlib -"
gerência de bibliotecas compartilhadas
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/common.h>
#include <sys/shlib.h>

int	shlib (int op, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "shlib" efetua diversas operações com bibliotecas compartilhadas.

.sp
Dependendo do argumento "op", a operação efetuada é:

.in +3
.ip 1. 3
SHLIB_LOAD: Carrega uma biblioteca compartilhada.

.sp
Neste caso, a chamada tem a forma

.sp
.(l
		int shlib (SHLIB_LOAD, const char *lib_nm);
.)l

.sp
A biblioteca compartihada "lib_nm" é carregada.
O índice é obtido do cabeçalho da própria biblioteca.

.ip 2. 3
SHLIB_UNLOAD: Descarrega uma biblioteca compartilhada.

.sp
Neste caso, a chamada tem a forma

.sp
.(l
		int shlib (SHLIB_UNLOAD, int index);
.)l

.sp
A biblioteca compartihada de índice "index" é descarregada.

.ip 3. 3
SHLIB_TABLE: Preenche uma tabela com dados sobre as bibliotecas carregadas.

.sp
Neste caso, a chamada tem a forma

.sp
.(l
		int shlib (SHLIB_TABLE, SHLIB *ptr);
.)l

.sp
A área apontada por "ptr" é preenchida com NSHLIB estruturas SHLIB
fornecendo informações sobre as bibliotecas carregadas.
Esta estrutura está definida no arquivo <sys/shlib.h>.

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função devolve zero se conseguiu efetuar a operação.
No caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
