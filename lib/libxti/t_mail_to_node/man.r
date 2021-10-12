.bp
.he 'T_MAIL_TO_NODE (xti)'TROPIX: Manual de Referência'T_MAIL_TO_NODE (xti)'
.fo 'Escrito em 29.11.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "t_mail_to_node -"
obtém um servidor de correio para um domínio
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

char *t_mail_to_node (int fd, const char *domain,
				int preference, int &new_preference);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Esta função fornece um servidor de correio eletrônico para o domínio "domain" dado.

.sp
Será fornecido o servidor de menor código numérico de preferência disponível, porém
de código igual ou maior do que o valor "preference" dado.
Repare que quanto maior este código numérico, menor a "preferência".

.sp
Se em "new_preference" for dado um ponteiro não NULO, o seu conteúdo receberá
a "preferência" do servidor fornecido.

.sp
Os diversos servidores podem ser obtidos, dando o código "0" de preferência 
na primeira chamada a "t_mail_to_node", e fornecendo o código obtido anteriormente
para cada nova chamada, até obter um ponteiro NULO com o código de erro NOENT (veja abaixo).

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Esta função não faz parte da XTI, e portanto seu uso deve ser evitado
se for desejada portabilidade para outras implementações.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, é retornado um ponteiro para o nome simbólico de um nó;
em caso de erro  é retornado um ponteiro NULO.
Se não houver nenhum servidor com código de preferência igual ou maior ao dado,
é retornado o código de erro NOENT.

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que não é fornecido o endereço do nó; para isto é necessário usar
posteriormente a função "t_node_to_addr".

.sp
A função devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
a função ser chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.br
.wo "(xti): "
t_addr
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"Domain Names - Concepts and Facilities", RFC 1034,
P. Mockapetris, November 1987.

.sp
"Domain Names - Implementation and Specification", RFC 1035,
P. Mockapetris, November 1987.

.sp
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
