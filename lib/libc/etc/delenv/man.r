.bp 1
.he 'DELENV (libc)'TROPIX: Manual de Referência'DELENV (libc)'
.fo 'Atualizado em 30.05.95'Versão 3.0'Pag. %'

.b NOME
.in 5
delenv - remove uma variável do ambiente

.in
.sp
.b SINTAXE
.in 5
.(l
int	delenv (const char *name);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "delenv" pesquisa o ambiente ("environment"),
procurando uma cadeia da forma "name=value".

.sp
Se a cadeia for encontrada ela é removida e "delenv" retorna 0.
No caso contrário, a função retorna -1.

.in
.sp
.b OBSERVAÇÕES
.in 5
O espaço (do vetor de ponteiros) liberado pela remoção de uma cadeia
do ambiente é reaproveitado por uma futura chamada a "putenv" (libc).

.sp
A cadeia da forma "name=value" não é mais referenciada pelo
ambiente, e o seu espaço pode ser usado para outros fins.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec
.br
.wo "(libc):"
putenv, getenv
.br
.wo "(fmt): "
environ
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
