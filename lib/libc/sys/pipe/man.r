.bp 1
.he 'PIPE (sys)'TROPIX: Manual de Referência'PIPE (sys)'
.fo 'Atualizado em 18.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "pipe -"
cria um canal entre processos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	pipe (int fd[]);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "pipe" cria um mecanismo de entrada/saída
entre dois processos chamado de canal ("pipe").
Ela recebe como argumento o ponteiro "fd" para um vetor de dois inteiros,
que são preenchidos por dois descritores de arquivos, "fd[0]" e "fd[1]".
O descritor "fd[0]" está aberto para leituras e "fd[1]" para
escritas.

.sp
A área interna do canal comporta até 5120 bytes de dados,
valor após o qual o processo que está escrevendo é bloqueado. 

.sp
O descritor de leituras "fd[0]" acessa os dados escritos em "fd[1]"
em um modo de fila ("FIFO").

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
É possível aguardar a disponibilidade de dados na leitura de um
"pipe" através da chamada ao sistema "attention" (sys).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(sys): "
read, write, attention
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
