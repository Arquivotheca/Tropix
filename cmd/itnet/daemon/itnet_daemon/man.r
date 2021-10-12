.bp
.he 'ITNET_DAEMON (cmd)'TROPIX: Manual de Referência'ITNET_DAEMON (cmd)'
.fo 'Atualizado em 14.11.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "itnet_daemon -"
processo gerenciador da internet
.br

.in
.sp
.b SINTAXE
.in 5
.(l
/usr/lib/itnet/itnet_daemon
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "itnet_daemon" é o processo gerenciador da internet.

.sp
Ele lê os arquivos de configuração "/etc/itnetdev" e "/etc/itnetserv",
e cria os seguintes processos:

.in +3
.ip 1. 3
"in_daemon" (que controla o recebimento de datagramas);

.ip 2. 3
"out_daemon" (que controla a saída de datagramas, incluindo as retransmissões);

.ip 3. 3
"name_s" (que envia pedidos de endereços IP para os servidores de nomes do
DNS);

.ip 4. 3
os diversos "serv_daemon" (que esperam pedidos de conexões/serviços
para os vários servidores).

.ep
.in -3

.sp
Normalmente, o programa "itnet_daemon" é iniciado durante a execução
do arquivo "/etc/rc", embora possa também ser executado manualmente
(principalmente para testes).

.sp
Para cancelar todos estes processos (isto é, a INTERNET), basta dar o
comando

.sp
.nf
		kill -s int pid
.fi

.sp
onde "pid" é o PID do processo "itnet_daemon" (obtido através do "ps").

.sp
Se for desejado incluir/excluir um servidor, NÃO é necessário
cancelar/reiniciar a internet. Basta editar o arquivo "/etc/itnetserv",
e executar o comando

.sp
.nf
		kill pid
.fi

.sp
onde "pid" é o mesmo descrito acima. Com isto, "itnet_daemon" relê o
arquivo "/etc/itnetserv", e ativa/desativa o servidor.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
editscb
.br
.wo "(xti): "
t_intro
.br
.wo "(fmt): "
itnetserv, itnetdev
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/itnetdev
 /etc/itnetserv
 /etc/rc

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
