.bp
.he 'T_GETINFO (xti)'TROPIX: Manual de Referência'T_GETINFO (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_getinfo -"
obtém informações do serviço de transporte
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_getinfo (int fd, T_INFO *info);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_getinfo" retorna as características correntes do protocolo
de transporte associado ao descritor de arquivo "fd". As informações
obtidas são as mesmas do que as obtidas através da função "t_open".
A função "t_getinfo" permite o usuário a obter estas informações
durante qualquer fase da comunicação.

.sp
.(l
	    +----------------+------------+------------+
	    |   Argumentos   | Na chamada | No retorno |
	    +----------------+------------+------------+
	    | fd             |     x      |     /      |
	    |		     |		  |	       |
	    | info->addr     |     /      |     x      |
	    | info->options  |     /      |     x      |
	    | info->tsdu     |     /      |     x      |
	    | info->etsdu    |     /      |     x      |
	    | info->connect  |     /      |     x      |
	    | info->discon   |     /      |     x      |
	    | info->servtype |     /      |     x      |
	    +----------------+------------+------------+
.)l

.sp
O ponteiro "info" deve apontar para uma estrutura "T_INFO", cujos
seguintes membros são preenchidos pela função "t_getinfo":

.sp
.(t
.(l
	long	addr;		/* Tamanho máximo do endereço */
	long 	options;	/* Tamanho máximo das opções */
	long	tsdu;		/* Tamanho máximo da mensagem */
				/* normal */
	long	etsdu;		/* Tamanho máximo da mensagem */
				/* urgente */
	long	connect;	/* Tamanho máximo de dados para */
				/* primitivas de conexão */
	long	discon;		/* Tamanho máximo de dados para */
				/* primitivas de desconexão */
	long	servtype;	/* Tipos de serviço disponíveis */
.)l
.)t

.sp 2
Os conteúdos destes membros têm os seguintes significados:

.in +3
.ip addr 10
Um valor maior ou igual a zero indica o tamanho máximo do endereço do
protocolo de transporte;
um valor igual -1 especifica que não há limite para o tamanho do endereço;
um valor igual a -2 especifica que o usuário não tem acesso aos endereços.

.ip options 10
Um valor maior ou igual a zero indica o tamanho máximo das opções
específicas do protocolo utilizado;
um valor igual -1 especifica que não há limite para o tamanho das opções;
um valor igual a -2 especifica que o sistema de transporte
não suporta estas opções.

.ip tsdu 10
Um valor maior que zero indica que o sistema de transporte suporta
o modo de enviar dados normais orientado a registro
(preservando as fronteiras entre as mensagens)
e o valor dado é o tamanho máximo do registro;
Um valor igual a -1 indica que não há limite para o tamanho máximo do registro;
um valor igual a zero indica que o sistema de transporte suporta
apenas o modo de enviar dados normais orientado a uma seqüência de bytes
(sem preservar as fronteiras entre as mensagens);
um valor igual a -2 indica que o sistema de transporte
não realiza transferência de dados normais.

.ip etsdu 10
Como acima (tsdu), para dados urgentes.

.ip connect 10
Um valor maior ou igual a zero indica a quantidade máxima de dados
que pode ser enviada durante a fase de estabelecimento da conexão;
um valor igual -1 especifica que não há limite para a quantidade destes dados;
um valor igual a -2 especifica que o sistema de transporte
não realiza transferência de dados durante a fase de estabelecimento
da conexão.

.ip discon 10
Um valor maior ou igual a zero indica a quantidade máxima de dados
que pode ser enviada durante a fase de desconexão (funções "t_snddis" e
"t_rcvdis" apenas);
um valor igual -1 especifica que não há limite para a quantidade destes dados;
um valor igual a -2 especifica que o sistema de transporte
não realiza transferência de dados durante a fase de desconexão.

.ip servtype 10
Este campo especifica os modos de comunicação disponíveis no sistema
de transporte. São eles:

.in +10
.ip T_COTS	12
Modo-com-conexão, mas sem desconexão ordenada.

.ip T_COTS_ORD 12
Modo-com-conexão e com desconexão ordenada.

.ip T_CLTS	12
Modo-sem-conexão.

.ep
.in -10

.ep
.in -3

.sp 2
Se um usuário deseja obter independência de protocolo, os tamanhos
descritos acima devem ser consultados para determinar o tamanho das
áreas que irão conter as informações.
Uma alternativa é a utilização da função "t_alloc" para alocar estas
áreas.
Não é preciso lembrar que, se o usuário exceder algum dos tamanhos
permitidos, ocorrerá um erro.
Os valores dos diversos membros
descritos acima podem mudar como um resultado da negociação de opções.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
No modo-de-acesso-interno, o campo "servtype" recebe o valor 0.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função preenche a estrutura e devolve o valor 0.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_intro, t_open, t_alloc
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
