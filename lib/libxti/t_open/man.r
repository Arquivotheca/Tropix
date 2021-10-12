.bp
.he 'T_OPEN (xti)'TROPIX: Manual de Referência'T_OPEN (xti)'
.fo 'Escrito em 06.01.93'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_open -"
cria um "endpoint" de transporte
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>
#include <fcntl.h>

int t_open (const char *name, int oflag, T_INFO *info);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_open" deve ser chamada como primeira etapa na inicialização
de um "endpoint" de transporte.
Um "endpoint" é uma estrutura interna do sistema de comunicações
na qual estão as informações necessárias para uma comunicação 
em andamento.
.sp

.(l
	    +----------------+------------+------------+
	    |   Argumentos   | Na chamada | No retorno |
	    +----------------+------------+------------+
	    | name           |     x      |     /      |
	    |		     |		  |	       |
	    | oflag          |     x      |     /      |
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
O argumento "name" indica o nome do dispositivo associado
ao protocolo do nível de transporte a ser utilizado na comunicação
(veja abaixo).

.sp
O argumento "oflag" deve ser construído a partir do indicador O_RDWR,
associado ou não, com O_NDELAY (ou O_NONBLOCK) - veja "open" (sys).

.sp
Em caso de sucesso, "t_open" retorna um descritor de arquivos
que identifica o "endpoint" (usualmente mencionado como "fd").
Este descritor deve ser usado em todas as chamadas subseqüentes
a funções para se referir a este "endpoint".

.sp
Se "info" for um ponteiro não NULO, "t_open" também preenche os diversos
membros da estrutura "T_INFO", contendo várias características
"default" do protocolo de transporte (veja "t_getinfo" (xti)). 

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Nesta implementação, que utiliza os protocolos INTERNET, os nomes
dos dispositivos devem ser "/dev/itntcp" para o protocolo TCP
(modo-com-conexão), "/dev/itnudp" para o protocolo UDP
(modo-sem-conexão) e "/dev/itnraw" para o protocolo definido pelo usuário
(modo-de-acesso-interno). Este último só pode ser usado pelo superusuário.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve um descritor de arquivos válido.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open
.br
.wo "(xti): "
t_getinfo
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
