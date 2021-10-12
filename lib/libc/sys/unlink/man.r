.bp
.he 'UNLINK (sys)'TROPIX: Manual de Referência'UNLINK (sys)'
.fo 'Atualizado em 23.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "unlink -"
remove uma entrada de um diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	unlink (const char *path);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "unlink" remove uma referência a um arquivo,
ou seja, remove a entrada do diretório cujo nome é dado pela cadeia
de caracteres apontada por "path".

.sp
Quando todos as referências a um arquivo foram removidas,
e nenhum processo está com o arquivo aberto, o espaço ocupado
por ele é liberado e ele deixa de existir.

.sp
Se um ou mais processos
estiverem com o arquivo aberto quando a última referência
for removida, a sua remoção é adiada até que todos os processos
fechem o arquivo.

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
.wo "(cmd): "
rm
.br
.wo "(sys): "
close, link, open
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
