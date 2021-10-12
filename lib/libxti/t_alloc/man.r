.bp
.he 'T_ALLOC (xti)'TROPIX: Manual de Referência'T_ALLOC (xti)'
.fo 'Escrito em 03.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_alloc -"
aloca uma estrutura de dados da biblioteca
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

void *t_alloc (int fd, T_ALLOC_ENUM struct_type, int fields);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_alloc" aloca memória dinamicamente para as diversas
estruturas de dados necessárias pelos argumentos das várias funções
da biblioteca "libxti". Esta função aloca memória para a estrutura
especificada e também para as áreas referenciadas por ela.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | fd		 |     x      |     /      |
	    | struct_type	 |     x      |     /      |
	    | fields		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "struct_type" especifica qual a estrutura a alocar
e deve ter um dos seguintes valores:

.(l

		T_BIND_STR		Estrutura "T_BIND"
		T_CALL_STR		Estrutura "T_CALL"
		T_OPTMGMT_STR		Estrutura "T_OPTMGMT"
		T_DIS_STR		Estrutura "T_DISCON"
		T_UNITDATA_STR		Estrutura "T_UNITDATA"
		T_UDERROR_STR		Estrutura "T_UDERR"
		T_INFO_STR		Estrutura "T_INFO"
.)l

.sp
onde cada uma destas estruturas pode ser subsequentemente usada
como argumento para uma ou mais funções da biblioteca "libxti".

.sp
Cada uma das estruturas acima (exceto "T_INFO") contém ao menos um
membro do tipo "NETBUF" (veja <xti.h>). Para cada membro deste tipo,
o usuário pode especificar que a sua área ("buf") seja
alocada também. O tamanho alocado para esta área é baseado na
informação obtida no argumento "info" retornado por "t_open"
ou "t_getinfo".

.sp
Os membros relevantes do argumento "info" (veja "t_getinfo" (xti))
são descritos na lista abaixo. O argumento "fields" determina quais
as áreas a alocar, onde o seu valor deve ser um outório de um ou mais dos
seguintes indicadores:

.in +3
.ip T_ADDR 9
O campo "addr" das estruturas "T_BIND", "T_CALL", "T_UNITDATA" ou
"T_UDERR" (tamanho obtido de "info->addr").

.ip T_OPT 9
O campo "opt" das estruturas "T_OPTMGMT", "T_CALL", "T_UNITDATA" ou
"T_UDERR" (tamanho obtido de "info->options").

.ip T_UDATA 9
O campo "udata" das estruturas "T_DISCON", "T_CALL" ou "T_UNITDATA"
(para T_CALL_STR, o tamanho é o valor máximo entre "info->connect"
e "info->discon"; para "T_DIS_STR", o tamanho é o valor de "info->discon";
para "T_UNITDATA_STR", o tamanho é o valor de "info->tsdu").

.ip T_ALL 9
Todos os campos relevantes da estrutura.

.ep
.in -3

.sp
Para cada campo especificado em "fields", "t_alloc" irá alocar memória
para a área associada ao campo, inicializar o campo "len" para ZERO
e inicializar adequadamente o ponteiro "buf" e o campo "maxlen".
Como o comprimento da área alocada é baseado no mesmo tamanho
retornado para o usuário por uma chamada a "t_open" ou "t_getinfo",
"fd" deve se referir ao "endpoint" de transporte através da qual
a estrutura alocada será utilizada.

.sp
Se a informação de tamanho associado a algum campo for -1 ou -2
(veja "t_getinfo"), "t_alloc" não terá meios de determinar o tamanho
da área a alocar, e falhará, retornando com [EINVAL] em "t_errno".
Para todos os campos não especificados em "fields", "buf" receberá
um ponteiro NULO, e "maxlen" o valor ZERO.

.sp
O uso de "t_alloc" para alocar as estruturas ajuda a manter
a compatibilidade dos programas dos usuários com as futuras versões
das funções da interface de transporte ("libxti").

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
A função NÃO ESTÁ IMPLEMENTADA na presente versão.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve um ponteiro para a estrutura alocada.
Em caso contrário, devolve um ponteiro NULO
e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_free, t_getinfo, t_open
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
.b ESTADO
.in 5
Efetivo.

.in
