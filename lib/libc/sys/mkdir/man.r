.bp
.he 'MKDIR (sys)'TROPIX: Manual de Referência'MKDIR (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "mkdir -"
cria um diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	mkdir (const char *path, int mode);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "mkdir" cria um novo diretório cujo nome
é dado pela cadeia de caracteres apontada por "path".

.sp
O UID/GID do diretório será o UID/GID efetivo do processo.

.sp
O modo do diretório é obtido a partir do valor de "mode",
porém os 9 indicadores de mais baixa ordem (a proteção) são
modificados pela máscara de criação de arquivos do processo;
todos os indicadores ligados da máscara são zerados (veja "umask" (sys)).

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
mkdir
.br
.wo "(sys): "
chmod, stat, umask
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
