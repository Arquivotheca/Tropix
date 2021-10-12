.bp
.he 'ITNETSERV (fmt)'TROPIX: Manual de Referência'ITNETSERV (fmt)'
.fo 'Atualizado em 30.08.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "itnetserv -"
tabela de servidores da INTERNET
.br

.in
.sp
.b DESCRIÇÃO
.in 5
A tabela "itnetserv" é utilizada durante o processo de inicialização
da INTERNET para obter informação
sobre quais servidores devem ser ativados (veja "internet" (cmd)).

.sp
A tabela reside no arquivo "/etc/itnetserv", e pode ser editada
diretamente pelo administrador do sistema através de "ed" (cmd)
ou "vi" (cmd). A tabela consiste de uma linha para cada servidor;
cada linha é composta de 5 campos separados por ":":

.sp
.nf
      *:protocolo:porta:conexões:/usr/lib/itnet/servidor_s [-opções]
.fi

.sp
O primeiro campo deve consistir de um único caractere, que
deve ser um asterisco ("*") para indicar um servidor ativo, e um branco
para inativo.

.sp
O segundo campo dá o nome do protocolo utilizado pelo servidor:
"tcp" ou "udp" (veja "t_intro" (xti)).

.sp
O terceiro campo dá o número da porta através da qual o servidor deve
esperar pedidos de conexão/serviços.

.sp
O quarto campo dá o número máximo de conexões de um mesmo cliente
concomitantemente aceitas pelo servidor. O valor NULO significa
número máximo ilimitado.

.sp
O quinto campo dá a linha de comando (com possíveis opções), que deve ser executada quando
vierem os pedidos de conexão/serviços (isto é, o código do servidor).
Repare que (normalmente) os servidores residem no diretório
"/usr/lib/itnet".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
internet, vi, ed
.br
.wo "(xti): "
t_intro
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/itnetserv

.in
.sp
.(t
.b EXEMPLO
.nf
	*:udp: 53:0:/usr/lib/itnet/name_s
	*:tcp: 21:0:/usr/lib/itnet/ftp_s
	*:tcp: 23:0:/usr/lib/itnet/telnet_s
	*:tcp: 37:0:/usr/lib/itnet/nettime_s
	 :tcp: 79:0:/usr/lib/itnet/finger_s
	*:tcp: 80:3:/usr/lib/itnet/www_s
	*:tcp:137:0:/usr/lib/itnet/tcmpto_s -p
	 :tcp:513:0:/usr/lib/itnet/rlogin_s
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
