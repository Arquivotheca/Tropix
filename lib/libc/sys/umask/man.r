.bp
.he 'UMASK (sys)'TROPIX: Manual de Referência'UMASK (sys)'
.fo 'Atualizado em 22.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "umask -"
obtem e modifica a máscara de criação de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/syscall.h>
#include <stat.h>

int	umask (int cmask);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "umask" atribui o valor "cmask" à máscara
de criação de arquivos do processo, e devolve o valor anterior
da máscara.

.sp
Somente os 9 indicadores de mais baixa ordem são usados,
ou seja, a parte referente à proteção do arquivo
(a descrição completa dos indicadores é dado em <stat.h>).

.sp
A máscara de criação do processo é utilizado durante as chamadas
ao sistema "open", "creat", "mknod". Indicadores presentes em "cmask"
são zerados no modo do arquivo criado.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkdir, sh
.br
.wo "(sys): "
chmod, creat, mknod, open
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
