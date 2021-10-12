.bp
.he 'EXIT (sys)'TROPIX: Manual de Referência'EXIT (sys)'
.fo 'Atualizado em 14.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Termina um processo:
.sp
.wo "exit  -"
processando "atexit" e atualizando a "stdio"
.br
.wo "_exit -"
sem processar "atexit" nem atualizar a "stdio"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

void	exit (int status);
void	_exit (int status);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "exit" termina o processo com as seguintes
consequências:

.sp
.in 10
As funções registradas através de "atexit" (libc) são chamadas,
na ordem inversa em que foram registradas.

.sp
As áreas de saída da "stdio" são atualizadas através de "fflush" (libc).

.sp
Todos os descritores de arquivos abertos são fechados.

.sp
Se o processo pai utilizou a chamada ao sistema "sigchild" (sys),
é enviado o sinal "SIGCHLD" para o processo pai.

.sp
O processo pai deste processo é
notificado do término deste processo (através da chamada "wait"),
e os 8 bites de mais baixa ordem
de "status" são colocados à sua disposição. 

.sp
O novo pai dos filhos do processo passa a ser o processo "init"
(ver "intro" (sys)).

.sp
.in 5
A chamada ao sistema "_exit" é análoga a "exit", exceto que 
as funções registradas por "atexit" não são chamadas, nem são
atualizados as áreas de saída da "stdio".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Nenhum; não pode haver um retorno destas chamadas.

.in
.sp
.b OBSERVAÇÃO
.in 5
Um processo que termina, mas o seu processo pai não chama
"wait", permanece em um estado chamado "ZOMBIE", que
ocupa uma entrada da tabela de processos, o que é
indesejável.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro, signal, sigchild, wait
.br
.wo "(libc):"
atexit, fflush
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
