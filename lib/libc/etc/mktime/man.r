.bp
.he 'MKTIME (libc)'TROPIX: Manual de Referência'MKTIME (libc)'
.fo 'Atualizado em 27.01.99'Versão 3.1.6'Pag. %'

.b NOME
.in 5
Manipulação de tempos:
.sp
.wo "mktime    -"
conversão de "TM" no tempo padrão "time_t"
.br
.wo "strftime  -"
formatação de vários campos de tempo
.br
.wo "strtotime -"
conversão de uma cadeia externa e "TM" em "time_t"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <time.h>

time_t	mktime (TM *tp);
size_t	strftime (char *str, size_t maxsize,
				const char *format, const TM *tp);
time_t	strtotime (const char *str, TM *tp);
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
Nenhuma das funções aqui descritas realizam conversões de fuso horário.

.sp
A função "mktime" converte o tempo dado na estrutura "TM" apontada
por "tp" no tempo padrão interno do sistema ("time_t").
Além disto, os campos "tm_wday" e "tm_yday" são (re-)calculados e
recebem o valor correto.

.sp
A função "strftime" converte, formata e escreve caracteres no vetor apontado
por "str", de acordo com a cadeia apontada por "format", em um estilo
semelhante à função "sprintf" (libc).

.sp
Diversas cadeias tais como os dias da semana, nomes dos meses, ... são
influenciados pelo valor corrente da variável LC_TIME do "locale" (libc).

.sp
A cadeia "format" consistem em zero ou mais especificacões de conversão
e caracteres normais. Uma especificacão de conversão consiste de um
caractere "\%" seguido de um caractere que determina qual a conversão/formatação
a ser feita. Todos os caracteres normais (incluindo o caractere NULO
finalizador) são copiados sem modificações no vetor.
São colocados um máximo de "maxsize" caracteres no vetor.
Cada especificacão de conversão é substituída pelos caracteres apropriados
(como descrito na lista abaixo) a partir de valores obtidos na estrutura
apontada por "tp".

.sp
.(t
As especificações de conversão com os respectivas caracteres
de substituição são:

.in +3
.ip \%a
Nome do dia da semana abreviado, como por ex. "DOM".
.)t

.ip \%A
Nome do dia da semana completo, como por ex. "domingo".

.ip \%b
Nome do dia do mês abreviado, como por ex. "jan".

.ip \%B
Nome do dia do mês completo, como por ex. "janeiro".

.ip \%c
Representação da data e hora no estilo
.sp
.ce
"15:10:22 SAB 10 mar 1990".

.ip \%d
Dia do mês como um número decimal (01-31).

.ip \%H
Hora do dia (ciclo de 24 horas) como um número decimal (00-23).

.ip \%I
Hora do dia (ciclo de 12 horas) como um número decimal (01-12).

.ip \%j
Dia do ano como um número decimal (001-366).

.ip \%m
Mês como um número decimal (01-12).

.ip \%M
Minuto como um número decimal (00-59).

.ip \%p
Parte do dia (AM ou PM).

.ip \%S
Segundo como um número decimal (00-60).

.ip \%U
Número da semana no ano (semana começando pelo domingo) como um número
decimal (00-53).

.ip \%w
Dia da semana como um número decimal (0-6) (0 = domingo).

.ip \%W
Número da semana no ano (semana começando pela segunda feira) como um número
decimal (00-53).

.ip \%x
Representação da data no estilo "SAB 10 mar 1990".

.ip \%X
Representação da hora no estilo "15:10:22".

.ip \%y
Ano (sem século) como um número decimal (00-99).

.ip \%Y
Ano (com século) como um número decimal (1970-2037).

.ip \%Z
Fuso horário (obtido através de "gettzmin" (sys)) no formato "shhmm",
onde o "s" é o sinal "+" ou "-", "hh"
o número de horas e "mm" o número de minutos (por exemplo "-0300").

.ip \%\%
Caractere "\%"

.in -3
.ep

.sp
A função "strtotime" recebe como argumentos uma cadeia de caracteres
apontada por "str" representando uma data na forma
.sp
.ce
      [[[[[ano]mes]dia]hora]minuto][:segundo]
.sp
e uma estrutura "TM" apontada por "tp", já contendo uma
(possivelmente outra) data.
A função faz uma fusão destas informações dando prioridade às
informações da cadeia.
Ela retorna a data no tempo padrão interno do sistema ("time_t"),
além de atualizar a estrutura "TM".
Cada campo da cadeia deve ser numérico, e conter exatamente 2 dígitos
decimais. Ao invés de ":" pode ser usado ".".
Se forem dados todos os campos, não será utilizada nenhuma informação
original da estrutura, enquanto que se for dada uma cadeia vazia,
todas as informações serão obtidas da estrutura.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a função "mktime" devolve o tempo padrão interno
do sistema. Se este tempo não puder ser representado, ou se o conteúdo
da estrutura dada contiver erros, a função retorna -1.

.sp
Em caso de sucesso, a função "strftime" devolve o número de caracteres
colocados no vetor "str" (não incluindo o NULO finalizador).
No caso contrário, devolve ZERO, e o conteúdo do vetor é indeterminado.

.sp
Em caso de sucesso, a função "strtotime" devolve o tempo padrão interno
do sistema. Se este tempo não puder ser representado, ou se o conteúdo
da cadeia ou da estrutura dadas contiver erros, a função retorna -1.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "strftime" não se preocupa com a possibilidade dos vetores
apontados por "str" e "format" serem superpostos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
data
.br
.wo "(sys): "
time, gettzmin
.br
.wo "(libc):"
ctime, localtime, sprintf, locale
.br

.in
.sp
.(t
.b EXEMPLOS
.in 5
O modo usual de utilização da função "strtotime" é:
.sp
.nf
	#include <time.h>
		..........................

		time_t		tempo;
		char		*str;

		..........................

		/* obtém a cadeia "str" */

		..........................

		time (&tempo);
		tempo = strtotime (str, localtime (&tempo));
.fi
.)t
.sp
Repare que a própria estrutura "TM" local da função "localtime" está
sendo utilizada e modificada.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
