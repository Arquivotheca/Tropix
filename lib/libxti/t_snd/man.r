.bp
.he 'T_SND (xti)'TROPIX: Manual de Referência'T_SND (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_snd -"
envia dados através de uma conexão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_snd (int fd, const void *buf, int nbytes, int flags);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_snd" envia dados normais ou urgentes através de uma conexão.

.sp
.(l
	    +-------------------+------------+------------+
	    |   Argumentos	| Na chamada | No retorno |
	    +-------------------+------------+------------+
	    | fd		|     x      |     /      |
	    | buf		|    x(x)    |     /      |
	    | nbytes		|     x      |     /      |
	    | flags		|     x      |     /      |
	    +-------------------+------------+------------+
.)l

.sp
O argumento "fd" identifica o  "endpoint" local de transporte
através do qual os dados são enviados, "buf" aponta para a área
de dados do usuário, "nbytes" especifica o número de bytes de
dados que devem ser enviados e o argumento "flags" especifica
um (ou mais) indicadores opcionais, como descritos abaixo.

.sp
Como nesta implementação o sistema de transporte utiliza o protocolo
TCP (INTERNET) para o modo-com-conexão, os indicadores "T_EXPEDITED"
e "T_MORE" não são suportados. Em compensação são disponíveis os
indicadores "T_URGENT" e "T_PUSH"
(veja "t_intro" e os DETALHES DE IMPLEMENTAÇÃO abaixo).

.sp
Normalmente, "t_snd" opera no modo síncrono e pode esperar caso
haja retrições de controle de fluxo.
No entanto, se o indicador O_NONBLOCK foi ligado
(através de "t_open" ou "fcntl" (sys)),
"t_snd" executa no modo assíncrono, e se houver
retrições de controle de fluxo
retorna imediatamente com -1 e atribui o valor [TFLOW]  a "t_errno".

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
Se o indicador "T_URGENT" estiver ligado, os dados serão enviados como dados
"urgentes", e o usuário remoto será notificado disto
quando recebê-los através de "t_rcv".

.ip - 2
Normalmente (isto é, se o indicador "T_PUSH" NÃO estiver ligado),
o sistema de transporte só envia efetivamente os dados para o usuário remoto
quando um segmento TCP for completado.
No entanto, se o indicador "T_PUSH" estiver ligado, os dados serão
enviados imediatamente (possivelmente juntos com dados anteriores
ainda não enviados; veja também a função "t_push").
O usuário remoto será notificado disto
quando receber estes dados através de "t_rcv".

.ep
.in -3

.sp
Nesta implementação, também é possível a utilização da chamada ao
sistema "write" (sys) para enviar dados através de uma conexão.
A chamada

.sp
.(l
	  write (fd, buf, nbytes);
.)l

.sp
é equivalente a

.sp
.(l
	  t_snd (fd, buf, nbytes, NULL);
.)l

.sp
Além disto, nesta implementação a função "t_snd" só opera no modo
síncrono, isto é, ela sempre irá esperar caso haja retrições de
controle de fluxo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve um inteiro não negativo,
indicando o número de bytes de dados aceitos pelo sistema de transporte.
Normalmente este número é igual ao número de bytes especificado
em "nbytes".
No entanto, se o indicador O_NONBLOCK estiver ligado, é possível
que apenas parte dos dados sejam aceitos pelo sistema de transporte.
Neste caso, "t_snd" retornará um valor que é menor do que "nbytes".

.sp
Em caso de falha, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
write, fcntl
.br
.wo "(xti): "
t_intro, t_open, t_rcv, t_push
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
