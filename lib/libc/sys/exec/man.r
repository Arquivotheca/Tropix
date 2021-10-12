.bp
.he 'EXEC (sys)'TROPIX: Manual de Referência'EXEC (sys)'
.fo 'Atualizado em 13.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Executa um arquivo:
.sp
.wo "execl  -"
dado um vetor de argumentos (e ambiente implícito)
.br
.wo "execle -"
dado um vetor de argumentos e um ambiente
.br
.wo "execv  -"
dado um ponteiro para um vetor de argumentos
.br
.wo "         (e ambiente implícito)"
.br
.wo "execve -"
dado um ponteiro para um vetor de argumentos
.br
.wo "         e um ambiente"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	execl  (const char *path, const char *arg0, ...);
int	execle (const char *path, const char *arg0, ...);
int	execv  (const char *path, const char *argv[]);
int	execve (const char *path, const char *argv[],
					const char *envp[]);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "exec" em todas as suas formas substitui
o arquivo em execução (do processo que fez a chamada) por um novo
arquivo executável. Este arquivo é composto de um cabeçalho,
um segmento de texto, e um segmento de dados. O segmento de dados
é composto de uma parte inicializada ("data") e uma parte não 
inicializada ("bss").

.sp
Embora o processo continue sendo o mesmo (com o mesmo PID (veja "intro"),
apenas o programa em execução foi substituído),
usaremos os nomes de "processo antigo"
e "processo novo" para simplificar a explicação. O processo antigo
refere-se ao processo durante o momento da chamada,
e processo novo refere-se ao processo após a troca do arquivo executável.

.sp
Quando um programa em "C" é executado, ele recebe argumentos
na seguinte forma:

.sp
.(l
	extern const char	**environ;

	main (int argc, const char *argv[], const char *envp[])
.)l

.sp
.in 5
onde "argc" contém o número de argumentos, "argv" é um ponteiro
para um vetor de ponteiros para as cadeias dos argumentos,
e "envp" é um  ponteiro para o vetor de ponteiros para as cadeias do ambiente
(ver "environ" (fmt)).
A variável "environ" recebe uma cópia de "envp".
O vetor "argv" tem sempre pelo menos um elemento:
o ponteiro para a cadeia com o nome do arquivo em execução
(ou o seu último componente do caminho).
Ambos vetores de ponteiros para as cadeias terminam com um ponteiro NULO.

.sp
A chamada ao sistema "execl" executa o arquivo apontado pelo argumento "path".
os argumentos "arg0", ... são ponteiros para cadeias que irão formar o vetor
de argumentos disponíveis para o programa a executar ("argv[0]", ...).
Após o último ponteiro,
deve ser adicionado um ponteiro NULO (para indicar o final da lista de
argumentos). Por convenção, "arg0" deve apontar para
uma cadeia igual a "path" (ou ao seu último componente do caminho).
O ambiente recebido pelo programa a executar será o mesmo do que
o processo corrente (definido pela variável externa "environ").

.sp
A chamada ao sistema "execle" executa o arquivo apontado pelo argumento "path".
Ela é similar à chamada ao sistema "execl", mas contém um argumento adicional
(após o ponteiro NULO): um ponteiro para um vetor de ponteiros para cadeias
definindo o "ambiente"
(este vetor também deve ser terminado por um ponteiro NULO).
Deste modo, o ambiente recebido pelo programa a executar poderá ser
diferente do ambiente do processo corrente (definido por "environ").

.sp
A chamada ao sistema "execv" executa o arquivo apontado pelo argumento "path".
o argumento "argv" é um ponteiro para um vetor de ponteiros para cadeias
que irão formar o vetor de argumentos disponíveis para o programa a executar
(este vetor deve ser terminado por um ponteiro NULO).
Por convenção, o primeiro ponteiro deste vetor (que deve ter no mínimo um
ponteiro não NULO) aponta para uma cadeia igual a "path"
(ou ao seu último componente do caminho).
O ambiente recebido pelo programa a executar será o mesmo do que
o processo corrente (definido por "environ").

.sp
A chamada ao sistema "execve" executa o arquivo apontado pelo argumento "path".
Ela é similar à chamada ao sistema "execv", mas contém o argumento adicional
"envp": um ponteiro para um vetor de ponteiros para cadeias
definindo o "ambiente".
Deste modo, o ambiente recebido pelo programa a executar poderá ser
diferente do ambiente do o processo corrente (definido por "environ").

.sp
Os descritores dos arquivos abertos continuam abertos 
no processo novo, exceto para os quais foi ligado o indicador
de "feche na execução" (veja "fcntl" (sys)). Para os arquivos que
permanecem abertos, o ponteiro de posição não é alterado. 

.sp
Sinais com disposição para terminarem o processo ou serem ignorados,
continuarão neste estado no novo processo.
Sinais com disposição para serem interceptados são
alterados para terminarem o novo processo (veja "signal" (sys)). 

.sp
Se os indicadores S_ISUID  e/ou S_ISGID ("set user ID" e/ou "set group ID",
veja "chmod" (sys)) do arquivo do novo processo
estão ligados, o UID/GID efetivo do novo processo será
o UID/GID do arquivo do novo processo.  O UID/GID real
não é alterado.

.sp
.(t
O novo processo também herda os seguintes atributos do processo antigo:

.sp
.in 10
Valor do "nice" (veja "nice" (sys)).
.)t

.sp
Identificação do processo, e de seu pai.

.sp
Identificação do grupo de processos e grupo de terminais (ver "intro" (sys)).

.sp
Indicador de "trace" (ver "ptrace" (sys)).

.sp
Tempo restante para um sinal do alarme do relógio (ver "alarm" (sys)).

.sp
Diretório corrente, e diretório raiz.

.sp
Máscara de criação de arquivos (ver "umask" (sys)).

.sp
Tempos de processamento do processo e de seus descendentes.

.sp
Tranca dos arquivos (ver "lockf" (sys)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se a chamada ao sistema retornou, é porque ocorreu um erro;
o valor de retorno é -1 e é indicada em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Não pode haver retorno de um "exec" com sucesso, pois o
processo é sobreposto pelo conteúdo do novo arquivo executável.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
alarm, exit, fcntl, fork, intro,  nice, signal, times,
.wo "        umask"
.br
.wo "(libc):"
execvp
.br
.wo "(fmt): "
a.out, environ
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
