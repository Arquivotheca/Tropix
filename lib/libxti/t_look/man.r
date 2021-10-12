.bp
.he 'T_LOOK (xti)'TROPIX: Manual de Referência'T_LOOK (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_look -"
indica a ocorrência de um evento em um "endpoint"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_look (int fd);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_look" retorna um evento ocorrido no "endpoint" de
transporte especificado por "fd".

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
Esta função permite ao sistema de transporte notificar um usuário
da ocorrência de um evento assíncrono quando o usuário está chamando
as funções no modo síncrono.

.sp
Certos eventos necessitam de notificação imediata do usuário
e são indicados pelo erro [TLOOK] no retorno da função corrente
ou da próxima a ser chamada.

.sp
Esta função também permite a um usuário testar periodicamente
a presença de eventos assíncronos.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Para facilitar a impressão de diagnósticos,
nesta implementação está disponível a função "t_strevent", que
fornece uma cadeia contendo a descrição de um evento.
Lembre-se no entanto, que esta função não faz parte da XTI,
e portanto seu uso deve ser evitado se for desejada portabilidade
para outras implementações.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de erro, a função devolve -1 e indica em "t_errno" a causa do erro.
Em caso de sucesso devolve zero se nenhum evento ocorreu,
ou um identificador de eventos da lista abaixo:

.sp
.(t
.nf
   	T_LISTEN	Foi recebido um pedido de conexão
	T_CONNECT	Foi recebido um resposta a um pedido de
			conexão
	T_DATA		Foram recebidos dados normais
	T_EXDATA	Foram recebidos dados urgentes
	T_DISCONNECT	Foi recebida uma indicação de desconexão
	T_UDERR		Ocorreu um erro no envio de datagrama
	T_ORDREL	Foi recebida uma indicação de desconexão
			ordenada
	T_GODATA	O fluxo de dados normais foi restabelecido
	T_GOEXDATA	O fluxo de dados urgentes foi restabelecido
.fi
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_intro, t_open, t_snd, t_sndudata, t_sndraw,
.br
.wo "				    t_strevent"
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in
