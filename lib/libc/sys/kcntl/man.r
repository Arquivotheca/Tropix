.bp
.he 'KCNTL (sys)'TROPIX: Manual de Referência'KCNTL (sys)'
.fo 'Atualizado em 22.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "kcntl -"
funções de controle do núcleo do sistema operacional
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/kcntl.h>
#include <sys/syscall.h>

int	kcntl (int cmd, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "kcntl" engloba uma série de funções auxiliares
do núcleo do sistema operacional TROPIX, visando efetuar um controle
dinâmico sobre certos funções do núcleo, assim como auxiliar o
implementação de alguns utilitários especiais.

.sp
O argumento "cmd" especifica a função desejada,
e tanto o tipo do valor de retorno da chamada, assim como os tipos
dos argumentos adicionais são variáveis, dependendo da função
específica.

.sp
As funções disponíveis são:

.in +3
.ip 1. 4
K_GETSYMGN: preenche a estrutura apontada por "sym" com uma cópia
da entrada da tabela de símbolos do núcleo do sistema operacional
TROPIX referente ao identificador contido na estrutura.

.sp
No argumento "len" deve ser dado o comprimento do identificador.

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/types.h>
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		#include <a.out.h>

		int kcntl (K_GETSYMGN, int len, SYM *sym);
.)l

.sp
Esta função é similar à função "nlist" (libc) utilizada com o
arquivo "/tropix", só que é bem mais rápida, pois utiliza
a tabela de símbolos da memória interna. Naturalmente,
só pode ser utilizada se a carga do sistema operacional
foi realizada com a tabela de símbolos.

.sp
Em caso de sucesso, a função retorna zero.

.ip 2. 4
K_GETSYMGA: preenche a estrutura apontada por "sym" com uma cópia
da entrada da tabela de símbolos do núcleo do sistema operacional
TROPIX referente ao identificador cujo endereço é o mais próximo de
"addr" (mas não superior).

.sp
No membro "s_nm_len" da estrutura deve ser dado o comprimento
do identificador máximo para o qual a estrutura "sym" foi alocada.

.sp
.(t
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/types.h>
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		#include <a.out.h>

		int kcntl (K_GETSYMGN, ulong addr, SYM *sym);
.)l
.)t

.sp
Esta função só pode ser utilizada se a carga do sistema operacional
foi realizada com a tabela de símbolos.

.sp
Em caso de sucesso, a função retorna zero.

.ip 3. 4
K_GETDEV: obtem a identificação ("major" + "minor") do dispositivo
especial de blocos cujo nome é apontado por "dev_nm" (sem o "/dev").

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GETDEV, const char *dev_nm);
.)l

.sp
Esta função consulta a tabela interna "disktb" do núcleo do
sistema operacional TROPIX. O Utilitário "prdisktb" (cmd) fornece
uma listagem completa desta tabela.

.sp
Em caso de sucesso, a função retorna a identificação do dispositivo.

.ip 4. 4
K_GETDEVNM: obtem o nome (sem o "/dev") do dispositivo especial de
blocos cuja identificação ("major" + "minor") do dispositivo é
dada por "dev". O nome é armazenado na área apontada por "dev_nm"
(que deve ter espaço para no mínimo 16 caracteres).

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GETDEVNM, int dev, char *dev_nm);
.)l

.sp
Esta função consulta a tabela interna "disktb" do núcleo
do sistema operacional TROPIX. O Utilitário "prdisktb" (cmd)
fornece uma listagem completa desta tabela.

.sp
Em caso de sucesso, a função retorna o próprio endereço de "dev_nm"
(deve-se usar um "cast" para "char *").

.ip 5. 4
K_PUTCVTB: modifica uma das tabelas de conversão de caracteres
das linhas de comunicação.

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/types.h>
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		#include <termio.h>

		int kcntl (K_PUTCVTB, int tb, const char *addr);
.)l

.sp
O código de caracteres padrão do TROPIX é o ISO (CBII),
mas esta função permite a utilização de terminais/impressoras
com outros códigos de caracteres.

.sp
O argumento "tb" deve ter um dos valores "USER1" ou "USER2", modificando
as tabelas correspondentes a estes parametros do comando
"stty" (cmd). As outras duas tabelas não podem ser modificadas. 

.sp
O argumento "addr" é um ponteiro para um vetor de 256
caracteres, onde "addr[i]" deve ter o código desejado
correspondente ao caracter ISO "i".

.sp
Em caso de sucesso, a função retorna zero.

.ip 6. 4
K_SEMAFREE: libera o recurso pelo qual o processo "pid" está
esperando.

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_SEMAFREE, long pid);
.)l

.sp
Esta função força a liberação do recurso pelo qual o processo "pid"
está esperando, seja qual for o seu tipo ("SLEEP", "SEMA" ou "EVENT"),
fazendo com que o processo seja "acordado".
Isto é uma operação de "pânico", só permitida ao superusuário,
e devendo ser empregada apenas em situações graves,
pois pode causar inconsistências na gerência do sincronismo.

.sp
Em caso de sucesso, a função retorna zero.

.ip 7. 4
K_GETPSZ: preenche a estrutura apontada por "kpsz", forncecento os
tamanhos das diversas partes do processo que chamou "kcntl".

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GETPSZ, KPSZ *kpsz);
.)l

.sp
A estrutura "KPSZ" contém os membros "k_tsize" (tamanho do texto),
"k_dsize" (tamanho do data+bss) e  "k_ssize" (tamanho da pilha de execução),
todos em bytes (ver <sys/kcntl.h>).
Estes tamanhos são obtidos a partir das regiões de memória efetivamente
alocadas pelo sistema operacional para o processo.

.sp
Repare que, no entanto, os tamanhos são arredondados de acordo com a gerência
de memória do processador e que o texto de um processo não reentrante
é considerado como parte do "data".

.sp
Em caso de sucesso, a função retorna zero.

.ip 8. 4
K_GETMMU: preenche uma estrutura "UPROC" e uma área com informações
relativas à gerência de memória.

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/common.h>
		#include <sys/dir.h>
		#include <sys/uerror.h>
		#include <sys/uproc.h>
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GETMMU, UPROC *up, void *mmu);
.)l

.sp
A área apontada por "up" é preenchida com a estrutura UPROC
do processo.

.sp
No caso do ICARUS, a área apontada por "mmu" é preenchida com os
4096 "shorts" que compõe a "PCT" (Page control table).

.sp
No caso do PC, a área apontada por "mmu" é preenchida seqüencialmente
com 5 tabelas de páginas (cada uma com 4096 bytes): a de "text", "data",
"phys", "ipc" e "stack".

.sp
De posse destas tabelas, um programa de usuário pode verificar
se a gerência de memória está corretamente carregada.

.sp
Em caso de sucesso, a função retorna zero.

.ip 9. 4
K_SETPREEMTION: liga/desliga a "preemption", ou seja, a capacidade da
troca de processos durante a execuçao do sistema operacional em
modo "supervisor".

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_SETPREEMPTION, int flag, int cpuid);
.)l

.sp
Com um valor positivo para "flag", a "preemption" da CPU "cpuid" é ligada;
com um valor negativo, a "preemption" é desligada; um valor NULO
possibilita obter o estado atual da "preemption" sem alterá-lo.

.sp
Em caso de sucesso, a função retorna o estado atual (após a chamada) da
"preemption": 1 se ligada, 0 se desligada.

.ip 10. 4
K_GET_DEVTB: obtem uma entrada ou a tabela completa "disktb" de dispositivos,
disponível no núcleo do sistema operacional TROPIX.

.sp
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/types.h>
		#include <sys/disktb.h>
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GET_DEV_TB, int dev, DISKTB *dp);
.)l

.sp
Se "dev" for diferente de NODEV, a chamada preenche a estrutura
apontada por "dp" com a entrada correspondente ao dispositivo "dev".
Neste modo, em caso de sucesso, a função retorna zero.

.sp
Se "dev" for igual a NODEV, e o ponteiro "dp" for NULO,
a chamada retorna o número de entradas da tabela
(incluindo uma entrada vazia ao final).

.sp
Se "dev" for igual a NODEV, e o ponteiro "dp" for não NULO, a chamada
copia a tabela inteira (incluindo uma entrada vazia ao final) para a
área apontada por "dp".
Neste modo, em caso de sucesso, a função retorna o número de entradas
da tabela (incluindo uma entrada vazia ao final).

.ip 11. 4
K_GET_PARTNM: obtem o nome correspondente a um tipo de partição
dos discos do PC.

.sp
.(t
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GET_PARTNM, int type, char *nm);
.)l
.)t

.sp
O vetor de caracteres apontado por "nm" é preenchido com o nome
simbólico correspondente ao valor numérico "type" dado.
Este valor "type" pode ser obtido através do membro "p_type" da
estrutura DISKTB (ver K_GET_DEVTB).
O vetor "nm" deve ter espaço para (no mínimo) PARTNM_MAXSZ caracteres.
 
.sp
Em caso de sucesso, a função retorna zero.

.ip 12. 4
K_GET_FD_TYPE: obtem os tipos dos disquetes.

.sp
.(t
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_GET_FD_TYPE);
.)l
.)t

.sp
O inteiro retornado contém no seu byte de mais baixa ordem o tipo
de "/dev/fd0" e no byte seguinte o tipo de /dev/fd1".

.sp
O tipo é codificado do seguinte modo:

.sp
.(l
		0: Unidade de disquete não disponível.
		1: Unidade de 5¼, 360 Kb.
		2: Unidade de 5¼, 1.2 Mb.
		3: Unidade de 3½, 720 Kb.
		4: Unidade de 3½, 1.44 Mb.
.)l

.ip 13. 4
K_DEBUG: consulta/atribui o valor de depuração do núcleo do TROPIX.

.sp
.(t
Neste caso, a chamada tem a forma:

.sp
.(l
		#include <sys/kcntl.h>
		#include <sys/syscall.h>

		int kcntl (K_DEBUG, valor);
.)l
.)t

.sp
Se "valor" for negativo, a função retorna o valor atual de depuração.

.sp
Se "valor" for nulo ou positivo, a função atribui o "valor" dado à depuração,
e retorna este valor.
Isto só é permitido ao superusuário.

.ep
.in -3

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, os respectivos valores de retorno já foram dados.
Em caso contrário, a chamada retorna -1 e indica em "errno" a causa do erro.
.)t

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada ao sistema não é padrão, e pode sofrer
extensões/alterações conforme as necessidades.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
stty, prdisktb
.br
.wo "(libc): "
nlist
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
