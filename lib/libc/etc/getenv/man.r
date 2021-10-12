.bp 1
.he 'GETENV (libc)'TROPIX: Manual de Referência'GETENV (libc)'
.fo 'Atualizado em 01.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
getenv - obtém o valor de uma variável do ambiente

.in
.sp
.b SINTAXE
.in 5
.(l
const char	*getenv (const char *name);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getenv" pesquisa o ambiente ("environment"),
procurando uma cadeia da forma
"name=value".

.sp
Devolve um ponteiro para "valor" caso 
encontre a cadeia, ou um ponteiro NULO em caso contrário.

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
putenv, delenv
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
