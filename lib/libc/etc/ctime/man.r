.bp 1
.he 'CTIME (libc)'TROPIX: Manual de Referência'CTIME (libc)'
.fo 'Atualizado em 27.01.99'Versão 3.1.6'Pag. %'

.b NOME
.in 5
 Conversão de datas e tempos:
.sp
.wo "ctime      -"
fornece data e hora em inglês em uma cadeia
.br
.wo "btime      -"
fornece data e hora em português em uma cadeia
.br
.wo "localtime  -"
fornece data e hora locais
.br
.wo "gmtime     -"
fornece data e hora Universal (UT, GMT)
.br
.wo "asctime    -"
fornece data e hora em inglês em uma cadeia
.br
.wo "brasctime  -"
fornece data e hora em português em uma cadeia
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <time.h>

char	*ctime (const time_t *timer);
char	*btime (const time_t *timer);
TM		*localtime (const time_t *timer);
TM		*gmtime (const time_t *timer);
char	*asctime (const TM *tm);
char	*brasctime (const TM *tm);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções aqui descritas trabalham com tempos expressos em
dois formatos:

.in +3
.ip 1. 3
O tempo padrão interno do sistema ("time_t"),
que contém o número de segundos decorridos desde 00:00:00
do dia 1. de janeiro de 1970, horário UT
(Tempo Universal, antes chamado de GMT).
.ip 2. 3
A estrutura "TM", que contém o tempo
já convertido em horas, minutos, segundos, dia, dia da semana, mes, ano, etc..
(veja <time.h>).

.ep
.in -3

.sp
Internamente o tempo é sempre UT, enquanto que na estrutura "TM"
podemos ter o tempo UT ou LOCAL. O tempo LOCAL é obtido somando-se ao tempo
padrão interno do sistema o valor do fuso horário, retornado pela chamada ao sistema
"gettzmin" (sys).

.sp
As funções "ctime" e "btime" convertem um tempo padrão interno do sistema,
apontado por "timer" em uma cadeia de 25 caracteres representando o tempo
LOCAL, com o seguinte formato:
.ce
	ctime:	"Thu Sep 25 13:25:22 1986\n"
.sp
.ce
	btime:	"13:25:22 QUI 25 set 1986\n"

.sp 
A função "gmtime" converte um tempo padrão interno do sistema,
apontado por "timer" em uma estrutura "TM", contendo o tempo UT.

.sp
A função "localtime" converte um tempo padrão interno do sistema,
apontado por "timer" em uma estrutura "TM", contendo o tempo LOCAL.

.sp
As funções "asctime" e "brasctime" convertem o tempo dado em uma estrutura
"TM" apontada por "tm" em um cadeia de caracteres como no caso de
"ctime" e "btime" descritas acima.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Um apontador para uma cadeia de 25 caracters ou um apontador
para uma estrutura "TM" definida em <time.h>, conforme a função.

.in
.sp
.b OBSERVAÇÕES
.in 5
Estas funções não fazem qualquer ajuste automático para o
horário de verão já que o mesmo no Brasil é adotado de
forma casuística e sem qualquer planejamento.

.sp
Todas as funções devolvem um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso elas sejam chamadas novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
 (cmd) : date, data
 (sys) : time, gettzmin
 (libc): mktime, strftime, strtotime, getenv

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
