.bp
.he 'T_STREVENT (xti)'TROPIX: Manual de Referência'T_STREVENT (xti)'
.fo 'Escrito em 27.11.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_strevent -"
obtém uma cadeia com a descrição de um evento
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

char *t_strevent (int fd, int event);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "strevent" devolve um ponteiro para uma cadeia contendo uma
descrição correspondente ao código numérico "event" de um evento
ocorrido em um "endpoint".
A cadeia não contém o <nl> de final de linha.

.sp
Para a utilização da função, é necessário já haver algum "endpoint"
de transporte disponível (argumento "fd"), isto é, ela só pode ser
utilizada após um "t_open" (xti).

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Lembre-se de que esta função não faz parte da XTI
e portanto seu uso deve ser evitado se for desejada portabilidade
para outras implementações.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A função "strevent" devolve um ponteiro para a mensagem
"Evento Desconhecido" caso o argumento "event" contenha um código
numérico de evento inválido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_intro, t_open, t_look
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
