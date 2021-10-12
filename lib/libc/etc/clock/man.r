.bp 1
.he 'CLOCK (libc)'TROPIX: Manual de Referência'CLOCK (libc)'
.fo 'Atualizado em 05.01.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
Rotinas para a medição de tempo:
.sp
.wo "clock       -"
fornece o tempo de processador usado
.br
.wo "clockstamp  -"
marca o inicio da contagem de tempo
.br
.wo "clocktostr  -"
converte o tempo decorrido em uma cadeia 
.br
.wo "difftime    -"
calcula a diferença entre dois tempos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <time.h>

clock_t	clock (void);
int	clockstamp (void);
char	*clocktostr (char *line);
double	difftime (time_t time1, time_t time0);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "clock" retorna o tempo de processamento utilizado pelo programa
desde o seu início.
Neste cálculo são incluídos os tempos em modo usuário e supervisor  
tanto do próprio programa como de todos os seus filhos terminados.
Para obter o tempo em segundos, o valor retornado deve ser dividido
por CLK_TCK. 

.sp
O objetivo das funções "clockstamp" e "clocktostr"
é a medição dos tempos gastos em trechos de programas.
Ao contrário de "clock", os tempos dos filhos não são computados.

.sp
A função "clockstamp" marca o tempo inicial, a partir
do qual será feita a contagem.

.sp
A função "clocktostr" escreve na área fornecida através do ponteiro "line"
uma cadeia de 65 bytes, no formato

.sp
.nf
  REAL  0:00:06.868       USER  0:00:06.622       SYS   0:00:00.098
.fi

.sp
contendo o tempo real transcorrido, o tempo do trecho do programa
em modo usuário e o tempo em modo do sistema.

.sp
Os tempos são dados em
horas, minutos, segundos e milésimos de segundos.

.sp
A função "clocktostr" pode ser chamada repetidas vezes, para obter
o tempo transcorrido desde uma marca de tempo comum.
Novas marcas de tempo
podem ser obtidas chamando-se "clockstamp" repetidas vezes.

.sp
A função "difftime" calcula a diferença de dois tempos na representação
padrão interna do sistema (tipo "time_t", ver "ctime" (libc)).
Ela devolve a diferença "time1 - time0" expressado em segundos como um "double". 

.in
.sp
.b
VALORES DE RETORNO
.r
.in 5
A função "clock" retorna o tempo de processamento em "ticks".

.sp
A função "clockstamp" retorna 0 se conseguiu alocar as áreas necessárias,
e -1 em caso contrário.

.sp
A função "clocktostr" retorna o argumento "line" dado.

.sp
A função "difftime" retorna o intervalo do tempo em "double".

.in
.sp
.(t
.b OBSERVAÇÕES
.in 5
Apesar de os valores retornados conterem milisegundos, a precisão deste 
valor depende do relógio interno do computador, que nem sempre
tem ciclo tão pequenos.
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
time, mutime, mutimes
.br
.wo "(libc):"
ctime
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
