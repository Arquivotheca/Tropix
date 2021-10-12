.bp
.he 'EVENT (sys)'TROPIX: Manual de Referência'EVENT (sys)'
.fo 'Atualizado em 13.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manuseia eventos:
.sp
.wo "eventctl  -"
operações de controle de eventos
.br
.wo "eventwait -"
espera pela ocorrência de um evento
.br
.wo "eventdone -"
sinaliza a ocorrência de um evento
.br
.wo "eventtest -"
obtém o número do último evento ocorrido
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <sys/unistd.h>

int	eventctl (int op, int fd, int n);
int	eventwait (int eventdescr, int eventordno);
int	eventdone (int eventdescr);
int	eventtest (int eventdescr);
.)l

.in
.sp
.b INTRODUÇÃO
.in 5
Eventos são formas de comunicação entre processos. Geralmente estão associados 
a acontecimentos assíncronos que, muitas vezes, são exteriores ao próprio
sistema. Quando um processo espera pelo final da leitura de um bloco
de disco, por exemplo, ele está esperando por um evento.

.sp
No TROPIX, eventos são coordenados pelo núcleo do sistema operacional
e estão disponíveis para o usuário.
Estão associados a um contador (número de ordem da ocorrência do evento)
que indica o número de ocorrências de eventos daquele tipo desde a sua
criação (o contador é sempre inicializado com o valor zero).

.sp
A identificação de um evento no TROPIX se faz através de um arquivo regular
(do qual apenas o nome é relevante) seguido do número de identificação
do evento. O par "fd" (descritor do arquivo aberto associado ao evento)
e "eventid" (número de identificação do evento)
identificam o evento de forma não ambígua.
Diversos eventos (com números de identificação diferentes) podem ser
associados a um mesmo arquivo. 

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "eventctl" com o argumento "op" igual a UE_ALLOC,
aloca "n" eventos ao arquivo cujo descritor é dado pelo argumento "fd".
Estes eventos são identificados pelos pares ("fd", 0), ("fd", 1), ...,
("fd", "n"-1). Em caso de sucesso, retorna o valor zero.

.sp
A chamada ao sistema "eventctl", com o argumento "op" igual a UE_GET,
retorna o descritor de evento (um número não negativo) associado ao
evento definido pelo par ("fd", "n"), que deverá ser usado para identificar
este evento nas chamadas ao sistema "eventwait",  "eventdone" e  "eventtest".

.sp
A chamada ao sistema "eventwait" bloqueia o processo até a ocorrência do
"eventordno"-ésimo evento indicado pelo descritor "eventdescr".
Se "eventordno" é menor ou igual ao número de ocorrências
atual, a chamada ao sistema "eventwait" retorna imediatamente.
A chamada ao sistema retorna o número de ocorrências atual.

.sp
A chamada "eventdone" sinaliza a ocorrência do evento indicado pelo descritor
"eventdescr". O contador de eventos correspondente é incrementado do valor 1.
A chamada ao sistema retorna o número de ocorrências atual.

.sp
A chamada "eventtest" retorna o número de ocorrências atual
do evento indicado pelo descritor "eventdescr".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Os valores de retorno das várias chamadas em caso de sucesso
já foram descritas acima.
Em caso de erro, elas devolvem -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Observe que a contagem dos eventos se faz de acordo com uma aritmética
circular: os contadores são inteiros de 32 bits, e desta forma,
em qualquer ponto da contagem, os 2^31 números
anteriores ao número de ocorrências atual são considerados menores 
que esse número, correspondendo, portanto, aos eventos
já ocorridos. Os 2^31 números seguintes ao número de ocorrências atual
são considerados maiores e correspondem aos eventos a ocorrer.

.sp
Observe, também, que se há diversos processos (ou "thread"s) esperando por
um mesmo "eventordno" de um determinado evento, assim que este evento
ocorrer ("eventdone"), TODOS os processos (ou "thread"s) esperando por
este evento são desbloqueados. Compare com o caso de semáforos onde
apenas um processo (ou "thread") é acordado quando um recurso é
liberado ("semafree").

.sp
No TROPIX os eventos existem enquanto o seu arquivo associado estiver aberto.
Quando o arquivo associado a um conjunto de eventos é fechado, o processo
perde o acesso a estes eventos. Quando o último processo fecha este arquivo
os eventos são destruídos.
Desta forma, a liberação de eventos é feita,
simplesmente, fechando-se seus arquivos associados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open, thread
.br

.in
.sp
.(t
.b EXEMPLO
.in 7
A aplicação usual das chamadas ao sistema para sinalização de eventos
entre processos é feita da seguinte forma:
.sp 2
.(l
		/* Parte comum aos dois processos */

	fd = open ("nome", ...);	/* Abre o arquivo associado */

	if (eventctl (UE_ALLOC, fd, 1) == -1)
		printf ( "Não consegui alocar o evento\n");

	if ((edesc = eventctl (UE_GET, fd, 0)) == -1)
		printf ( "Não consegui obter o descritor do evento\n");

			/* Parte diferente */
	/* Processo 1 */		/* Processo 2 */

	eno = eventwait (edesc, 1);	eno = eventdone (edesc);

	printf ("recebi o evento\n");	printf ("sinalizei o evento\n");

		/* Parte comum aos dois processos */

	close (fd);			/* Libera os eventos alocados */

.)l
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
