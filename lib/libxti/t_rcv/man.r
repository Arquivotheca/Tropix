.bp
.he 'T_RCV (xti)'TROPIX: Manual de Referência'T_RCV (xti)'
.fo 'Escrito em 08.01.93'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_rcv -"
recebe dados através de uma conexão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_rcv (int fd, void *buf, int nbytes, int *flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_rcv" recebe dados normais ou urgentes através de uma conexão.

.sp
.(l
	    +-------------------+------------+------------+
	    |   Argumentos	| Na chamada | No retorno |
	    +-------------------+------------+------------+
	    | fd		|     x      |     /      |
	    | buf		|     x      |    (x)     |
	    | nbytes		|     x      |     /      |
	    | flags		|     ?      |    (?)     |
	    +-------------------+------------+------------+
.)l

.sp
O argumento "fd" identifica o  "endpoint" local de transporte,
através do qual os dados são recebidos, "buf" aponta para a área
onde os dados serão colocados e "nbytes" especifica o tamanho da
área. O argumento "flags" aponta para uma variável onde serão
retornados indicadores opcionais, como descritos abaixo. Se "flags"
for um ponteiro NULO, não serão retornados os indicadores.

.sp
Normalmente, "t_rcv" opera no modo síncrono e espera a chegada
de dados, caso não haja dados disponíveis no momento da chamada.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_rcv" executa no modo assíncrono, e se não houver dados disponíveis
retorna imediatamente com -1 e atribui o valor [TNODATA]  a "t_errno".

.sp
Como nesta implementação o sistema de transporte utiliza o protocolo
TCP (INTERNET) para o modo-com-conexão, os indicadores "T_EXPEDITED"
e "T_MORE" não são suportados. Em compensação são disponíveis os
indicadores "T_URGENT" e "T_PUSH"
(veja "t_intro" e os DETALHES DE IMPLEMENTAÇÃO abaixo).

.sp
No modo síncrono, se o usuário deseja evitar ficar bloqueado em uma
chamada a "t_rcv", pode utilizar a função "t_look" (verificando
se ocorreu um dos eventos "T_DATA" ou "T_EXDATA").

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta implementação utiliza o protocolo TCP (INTERNET) para o
modo-com-conexão (veja "t_intro"), e portanto o envio de dados é
orientado a uma seqüência de bytes, o que não preserva as
fronteiras entre as mensagens
(isto é, não são suportados os conceitos de TSDU e ETSDU).

.sp
Pela mesma razão, os indicadores "T_EXPEDITED" e "T_MORE" não são
suportados; em compensação são disponíveis os indicadores
"T_URGENT" e "T_PUSH", (que embora semelhantes, não são iguais):

.in +3
.ip - 2
Se houver dados suficientes para completar a área "buf" antes de
utilizar um segmento TCP com "PUSH", a área será completada, mas o indicador
"T_PUSH" de "flags" não será ligado. Por outro lado, se o segmento
recebido tiver o indicador "PUSH",  a área será completada parcialmente,
e o indicador "T_PUSH" será ligado.

.ip - 2
Se o indicador "T_URGENT" de "flags" estiver ligado, significa que os
dados armazenados em "buf" são "urgentes".

.ep
.in -3

.sp
Repare que (no modo síncrono) a função "t_rcv" só retornará com sucesso se
a área "buf" tiver sido completada ou tiver sido recebido um segmento
com o indicador "PUSH".

.sp
Nesta implementação, também é possível a utilização da chamada ao
sistema "read" (sys) para receber dados através de uma conexão.
A chamada 

.sp
.(l
	  read (fd, buf, nbytes);
.)l

.sp
é equivalente a

.sp
.(l
	  t_rcv (fd, buf, nbytes, (int *)NULL);
.)l

.sp
Um outro modo disponível nesta implementação,
de não ficar bloqueado em uma chamada a "t_rcv"
(no modo síncrono) é a utilização da função "t_nread",
que fornece o número de bytes de dados disponíveis.

.sp
A chamada ao sistema "attention" (sys) pode ser usada para aguardar
a disponibilidade de dados para "t_rcv".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve um inteiro não negativo,
indicando o número de bytes de dados efetivamente armazenados em "buf".
Um valor NULO indica o "final de arquivo", que ocorre quando o usuário
remoto chama a função "t_sndrel" para terminar a conexão.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
read, fcntl, attention
.br
.wo "(xti): "
t_intro, t_getinfo, t_look, t_open, t_sndrel,
.br
.wo '        t_rcvrel, t_snd, t_nread'
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
