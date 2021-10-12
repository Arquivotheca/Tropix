.bp
.he 'GETTZMIN (sys)'TROPIX: Manual de Referência'GETTZMIN (sys)'
.fo 'Atualizado em 27.01.99'Versão 3.1.6'Pag. %'

.b NOME
.in 5
.wo "gettzmin -"
Obtém o fuso horário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	gettzmin (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "gettzmin" retorna o valor do fuso horário em minutos.
Este valor indica o tempo que deve ser acrescido ao Tempo Universal (UT, antes
chamado de GMT) para obter-se o tempo local.

.sp
No Rio de Janeiro, este valor
é de -180 exceto durante o horário de verão, quando passa para -120.

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que o valor de retorno é dado em minutos, e que em geral será
necessário multiplicar por 60 antes de somar/subtrair com os
diversos tempos do sistema (que são dados em segundos).

.in
.sp
.b ESTADO
.in 5
Efetivo.
