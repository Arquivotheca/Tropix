.bp
.he 'BOOT (sys)'TROPIX: Manual de Referência'BOOT (sys)'
.fo 'Atualizado em 01.09.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "boot -"
Reinicializa o computador
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	boot (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "boot" reinicializa o computador, transferindo o controle para o
programa de carga da EPROM.

.sp
Somente pode ser utilizada pelo SUPERUSUÁRIO.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema NÃO retorna.
Em caso contrário, retorna -1 e indica em "errno" a causa do erro.

.in
.sp
.b ESTADO
.in 5
Efetivo.
