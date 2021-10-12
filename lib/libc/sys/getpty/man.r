.bp
.he 'GETPTY (sys)'TROPIX: Manual de Referência'GETPTY (sys)'
.fo 'Atualizado em 14.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "getpty -"
aloca um pseudo terminal
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <termio.h>

int	getpty (PTYIO *);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getpty" aloca um pseudo terminal, e
devolve o ponteiro para a estrutura PTYIO preenchida com
os seguintes membros:

.sp
.nf
	t_fd_client:	Descritor do cliente
	t_fd_server:	Descritor do servidor
	t_nm_client:	Nome completo do dispositivo cliente
	t_nm_server:	Nome completo do dispositivo servidor
.fi

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função devolve zero em caso de sucesso;
em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b ESTADO
.in 5
Efetivo.
