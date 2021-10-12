.bp
.he 'SETTGRP (sys)'TROPIX: Manual de Referência'SETTGRP (sys)'
.fo 'Atualizado em 28.10.95'Versão'3.0 Pag. %'

.b NOME
.in 5
.wo "settgrp -"
altera o terminal associado a um processo
.bc	/*************************************/
gerencia grupos de processos associados a um terminal
.ec	/*************************************/
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	settgrp (int flag);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Cada processo ativo pode ser membro de um grupo de processos
associado a um terminal.
A identificação deste grupo de processos associados a um terminal
é o PID do processo que abriu o terminal (ver "intro" (sys)).

.sp
Se for chamada com o argumento "flag" igual a zero,
"settgrp" zera a identificação do grupo ao qual o processo pertence.
Deste modo o processo passa a não pertencer mais a nenhum grupo de
processos associados a um terminal.

.sp
Se for chamada com o argumento "flag" igual a um,
"settgrp" faz com que seja criado um novo grupo de processos associados
a um terminal.
Este novo grupo consiste inicialmente do processo
que chamou "settgrp", e irá futuramente conter os seus filhos.
Neste caso, o processo já deverá ter um terminal aberto.

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
.b OBSERVAÇÃO
.in 5
Esta chamada ao sistema somente pode ser utilizada pelo superusuário.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro
.br
.wo "(cmd): "
stty
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
