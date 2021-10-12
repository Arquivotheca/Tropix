.bp
.he 'SETSCB (sys)'TROPIX: Manual de Referência'SETSCB (sys)'
.fo 'Atualizado em 23.10.98'Versão 3.1'Pag. %'

.b NOME
.in 5
.wo "setscb -"
atualiza o bloco de controle do sistema operacional
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

int	setscb (SCB *sp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "setscb" permite alterar (apenas alguns) campos
do bloco de controle do sistema operacional.
Este bloco contém diversas informações sobre o núcleo
do sistema, que podem ser vistas em <sys/scb.h>.

.sp
No momento, apenas o campo "y_tzmin" pode ser alterado.

.sp
O modo usual da utilização desta chamada ao sistema é a de
(inicialmente) obter o estado atual do bloco de controle
(através de "getscb" (sys)), alterar os campos desejados, e
finalmente chamar "setscb".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve 0.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada pode ser utilizada apenas por superusuários.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
getscb, gettzmin
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
