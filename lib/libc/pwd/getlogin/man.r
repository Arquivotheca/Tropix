.bp 1
.he 'GETLOGIN (libc)'TROPIX: Manual de Referência'GETLOGIN (libc)'
.fo 'Atualizado em 07.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
getlogin - obtém o nome do usuário dado no login

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <pwd.h>

char	*getlogin (void)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getlogin" retorna um ponteiro para o nome do usuário, como foi
dado no comando de "login", e está registrado no arquivo "/etc/utmp". 
Pode ser usado conjuntamente com "getpwnam", para localizar
a entrada correta do arquivo de contas/senhas, quando vários
usuários tem a mesma identificação numérica. 

.sp
A melhor maneira de obter o nome do usuário é chamar "getlogin",
e se falhar, chamar "getpwuid" (ou "pwcache").

.in
.sp
.b DIAGNÓSTICOS
.in 5
Devolve um ponteiro NULO se não conseguiu obter o nome do usuário.
Isto pode ocorrer (por exemplo) se "getlogin" é chamado por um
processo que não tem um terminal associado.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "getlogin" devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
a função ser chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
pwent, pwcache
.br
.wo "(fmt): "
utmp
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/utmp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
