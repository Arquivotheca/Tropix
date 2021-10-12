.bp 1
.he 'GETPASS (libc)'TROPIX: Manual de Referência'GETPASS (libc)'
.fo 'Atualizado em 03.06.97'Versão 3.0.2'Pag. %'

.b NOME
.in 5
getpass - lê uma senha do terminal

.in
.sp
.b SINTAXE
.(l
#include <sys/types.h>
#include <pwd.h>

char	*getpass (const char *prompt);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getpass" lê uma senha do terminal associado ao processo, após
escrever o "prompt" dado.

.sp
A senha pode ter até 80 caracteres
(repare no entanto, que a função "crypt" (libc), somente
considera os 14 primeiros caracteres).

.sp
A senha pode incluir quaisquer caracteres do código ISO
exceto <^S>, <^Q>, <nl>,  <cr> e o caractere "kill"
(normalmente <^U>, ver "termio" (fmt)).

.sp
O caractere "kill" pode ser usado em caso de erro, para
ler novamente a senha completa.
Repare que os demais caracteres de "termio"
(tais como "erase", "aerase", "intr", "eof", etc ...)
não são processados, podendo fazer parte de uma senha.

.sp
Se o processo não tiver terminal associado, devolve um ponteiro NULO.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "getpass" devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
a função ser chamada novamente.

.in
.sp
.b ARQUIVOS
.in 5
/dev/tty

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
crypt
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
