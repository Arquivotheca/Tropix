.bp
.he 'SEMA (sys)'TROPIX: Manual de Referência'SEMA (sys)'
.fo 'Atualizado em 18.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manuseia semáforos:
.sp
.wo "semactl   -"
operações de controle de semáforos
.br
.wo "semalock  -"
obtém um recurso associado ao semáforo
.br
.wo "semafree  -"
libera um recurso associado ao semáforo
.br
.wo "sematestl -"
o mesmo que "semalock" mas sem bloquear o processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <unistd.h>

int	semactl (int op, int fd, ...);
int	semalock (int semadescr);
int	semafree (int semadescr);
int	sematestl (int semadescr);
.)l

.in
.sp
.b INTRODUÇÃO
.in 5
Semáforos são formas de comunicação entre processos. Geralmente estão associados 
a recursos críticos do sistema que só podem ser utilizados com exclusividade
por um processo de cada vez.
Quando um processo atualiza uma estrutura de dados que é compartilhada por
vários processos ele deve utilizar um semáforo de valor inicial igual a um para
garantir indivisibilidade do procedimento de atualização.

.sp
No TROPIX, os semáforos são coordenados pelo núcleo do sistema operacional
e estão disponíveis para o usuário. Eles possuem contadores associados,
que indicam os números de recursos disponíveis de cada tipo.
A medida em que os processos vão utilizando os recursos, este contadores são
decrementados. A partir do momento em que um certo contador atinge o valor 0,
os processos que pedirem o recurso associado a este contador são bloqueados,
até que (pelo menos) um processo libere um recurso deste tipo.

.sp
A identificação de um semáforo no TROPIX se faz através de um arquivo regular
(do qual apenas o nome é relevante) seguido do número de identificação
do semáforo. O par "fd" (descritor do arquivo aberto associado ao semáforo)
e "semaid" (número de identificação do semáforo)
identificam o semáforo de forma não ambígua.
Diversos semáforos (com números de identificação diferentes) podem ser
associados a um mesmo arquivo. 

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "semactl" com o argumento "op" igual a US_ALLOC
e o argumento (adicional) "n", aloca "n" semáforos ao arquivo cujo
descritor é dado pelo argumento "fd".
Estes semáforos são identificados pelos pares ("fd", 0), ("fd", 1), ...,
("fd", "n"-1). Em caso de sucesso, retorna o valor zero.

.sp
A chamada ao sistema "semactl", com o argumento "op" igual a US_GET
e argumentos (adicionais) "n" e "initval",
retorna o descritor de semáforo (um número não negativo) associado ao
semáforo definido pelo par ("fd", "n"), que deverá ser usado para identificar
este semáforo nas chamadas ao sistema "semalock", "semafree", "sematestl" e
"semactl" com as operações US_AVAIL e US_WAIT.
O quarto argumento "initval", dá o valor inicial do semáforo, isto é, o número
de recursos associados ao semáforo.
Na presente implementação do TROPIX, este valor inicial não pode
ser maior que 32767.

.sp
A chamada ao sistema "semactl", com o argumento "op" igual a US_AVAIL,
retorna o número de recursos associados ao semáforo definido pelo descritor
de semáforos "fd" que ainda se encontram disponíveis para alocação.

.sp
A chamada ao sistema "semactl", com o argumento "op" igual a US_WAIT,
retorna o número de processos e "threads"  que aguardam pela liberação
de recursos associados ao semáforo definido pelo descritor
de semáforos "fd".

.sp
A chamada ao sistema "semalock" bloqueia o processo até a 
liberação do recurso associado ao semáforo indicado pelo descritor
"semadescr".
O valor retornado é o número de recursos ainda disponível após o sucesso
na obtenção do recurso.

.sp
A chamada "semafree" libera um recurso associado ao semáforo indicado pelo
descritor "semadescr".
A chamada sempre retorna imediatamente dando o número de recursos ainda
disponíveis após a liberação do recurso em questão.

.sp
A chamda "sematestl" é equivalente a "semalock" com a única diferença que
o processo não fica bloqueado quando não há recursos disponíveis. Se 
"sematestl" retorna um valor não negativo, significa que o recurso foi
obtido e indica o número de recursos ainda disponíveis.
O retorno do valor -1 indica que o recurso não foi obtido.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Os valores de retorno das várias chamadas em caso de sucesso
já foram descritas acima.
Em caso de erro, elas devolvem -1 e indicam em "errno" a causa do erro.

.bc	/*************************************/
Em caso de sucesso,  a não ser a "semactl" que devolve 0 com
"op" igual a US_ALLOC e o descritor do semáforo com "op" igual a US_GET,
todas as chamadas ao sistema devolvem o número de recursos
associados a um determinado semáforo disponíveis para alocação.
Em caso contrário, devolvem -1 e indicam em "errno" a causa do erro.
.ec	/*************************************/

.in
.sp
.b OBSERVAÇÕES
.in 5
NÃO confunda "descritor do arquivo" com  "descritor do semáforo":
a chamada ao systema "semactl", quando executa operações
relativas a semáforos ainda não existentes (US_ALLOC e US_GET)
recebe como argumento o descritor do arquivo associado;
quando as operações são executadas sobre semáforos já existentes
(US_AVAIL e US_WAIT) ela recebe diretamente o descritor do semáforo.

.bc	/*************************************/
.sp
O arquivo associado a um conjunto de semáforos pode ser utilizado
normalmente a qualquer instante. Observar, porém, que um "close" no
mesmo faz o conjunto de semáforos ficar inacessível.
.ec	/*************************************/

.sp
No TROPIX os semáforos existem enquanto o seu arquivo associado estiver aberto.
Quando o arquivo associado a um conjunto de semáforos é fechado, o processo
perde o acesso a estes semáforos. Quando o último processo fecha este arquivo
os semáforos são destruídos.
Desta forma, a liberação de semáforos é feita,
simplesmente, fechando-se seus arquivos associados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open
.br

.in
.sp
.b EXEMPLOS
.in 7
A aplicação usual da chamada ao sistema em procedimento de exclusão mútua
é feita da seguinte forma:
.sp 2
.(l
	fd = open ("nome", ...); /* Abre o arquivo associado */

	if (semactl (US_ALLOC, fd, 1) == -1)
		printf ("Não consegui alocar o semáforo\n");

	if ((sdesc = semactl (US_GET, fd, 0, 1)) == -1)
		printf ("Não consegui obter o descritor\n");

	semalock (sdesc);

	printf ("peguei recurso exclusivo para este processo\n");

	semafree (sdesc);

	close (fd);	/* Libera os semáforos alocados */

.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.
