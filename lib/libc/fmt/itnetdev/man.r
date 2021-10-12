.bp
.he 'ITNETDEV (fmt)'TROPIX: Manual de Referência'ITNETDEV (fmt)'
.fo 'Atualizado em 30.08.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "itnetdev -"
tabela de dispositivos/roteamento da INTERNET
.br

.in
.sp
.b DESCRIÇÃO
.in 5
A tabela "itnetdev" é utilizada durante o processo de inicialização
da INTERNET para obter informações sobre os endereços IP
(veja t_intro (xti)), dispositivos usados para entrada/saída de datagramas
(linhas seriais e "ethernet") e o roteamento (isto é, como decidir
para qual dispositivo deve ser enviado cada datagrama).

.sp
A tabela reside no arquivo "/etc/itnetdev", e pode ser editada
diretamente pelo administrador do sistema através de "ed" (cmd)
ou "vi" (cmd).

.sp
A configuração deste arquivo é dependente de como o computador será
conectado à INTERNET; há 3 modos básicos:

.in +3
.ip 1. 3
A uma rede INSTITUCIONAL: o computador está situado
em uma firma ou universidade, etc ...;
a ligação é normalmente através de "ethernet"
(embora também possa ser feita através de linha
serial usando protocolos SLIP ou PPP).
Neste caso, o nome do computador/domínio, endereço IP, máscara da
rede e o endereço do servidor de nomes do DNS são dados
pelo gerente da rede ou pelo protocolo DHCP (veja abaixo).

.ip 2. 3
A uma rede DOMÉSTICA: os computadores estão situados na casa do usuário;
a ligação entre eles pode ser através de "ethernet"
ou linha serial usando protocolos SLIP ou PPP.
Neste caso, o nome do computador/domínio e o seu endereço IP
podem ser livremente escolhidos pelo usuário;
para os endereços IP é aconselhável utilizar endereços da forma
"192.168.0.xxx" (isto é, a rede "192.168.0.0"),
pois são endereços reservados para uso local,
evitando a colisão com endereços de algum computador presente na
rede mundial INTERNET; não é necessário o uso de um servidor de nomes;
os endereços dos vários computadores devem ser incluídos no
arquivo "/etc/itnetdev".

.ip 3. 3
A uma rede de linha (telefônica) discada (através de "modem"),
usando protocolo PPP e utilizando os serviços de um provedor
(veja "ppp" (cmd)).
Neste caso, o nome do computador/domínio
podem ser livremente escolhidos pelo usuário, mas de tal modo
a evitar colisões com redes já existentes. 
O endereço IP é automaticamente atribuído no momento da ligação,
e o endereço IP do servidor de nomes do DNS é dado pelo provedor.

.sp
Repare que podemos ter simultaneamente um acesso de linha discada
e um dos dois casos anteriores; se for uma rede institucional, use
os dados fornecidos pelo gerente da rede, conforme acima.


.bc	/*************************************/
Deve-se, no entanto, tomar cuidado com a palavra chave "default"
(ver abaixo).
.ec	/*************************************/

.ep
.in -3

.sp
O arquivo "/etc/itnetdev" consiste de 4 partes.
Acompanhe no arquivo protótipo já presente na distribuição do
TROPIX (os diversos campos são separados por um ou mais
brancos ou caracteres <ht>; linhas começando por "#" são comentários):

.in +3
.ip 1. 3
Uma linha definindo o nome do próprio computador, juntamente com
o domínio. Este endereço IP é sempre "127.0.0.1"
(que é usado quando o computador quer referir a si próprio).
Exemplo:

.sp
.nf
		127.0.0.1 meu_computador.meu_domínio.com.br
.fi

.sp
Ao alterar o nome do "meu_computador", altere também o campo
"nodename" da estrutura "scb" (veja "edscb" (cmd)).

.ip 2. 3
Linhas dando endereços IP de computadores previamente escolhidos.
Esta parte só é necessária para redes domésticas, embora também possa
ser incluída nos outros dois casos (para que não sejam necessárias
consultas a um servidor de nomes). Exemplo:

.sp
.nf
		192.168.0.3 outro_computador.meu_domínio.com.br
.fi

.ip 3. 3
Linhas definindo os dispositivos associados a cada classe de endereços IP
(ou seja, roteamentos).
Estas linhas NÃO são necessárias se o computador estiver
conectado apenas a uma rede de linha discada (veja "ppp" (cmd)).

.sp
No caso de "ethernet" doméstico, são necessárias linhas do tipo

.sp
.nf
		/dev/ed0		192.168.0.2
		netmask=255.255.255.0
.fi

.sp
onde "/dev/ed0" corresponde ao dispositivo "ethernet".
O valor "192.168.0.2" é o endereço IP deste computador
(através deste dispositivo), e "255.255.255.0"
é a máscara da rede doméstica, o que irá definir
a rede "192.168.0.0".

.sp
Estes dados são usados na decisão de roteamento.
Se a operação "E BINÁRIO" do endereço IP destino com a máscara
der como resultado o endereço da rede, então
o datagrama será enviado através do dispositivo "/dev/ed0".

.sp
Se a "ethernet" for institucional, há duas possibilidades:
na primeira, estes valores são dados pelo gerente da rede e na
segunda é utilizado o protocolo DHCP (que obtém automaticamente
estes valores).

.sp
Na primeira hipótese (valores dados pelo gerente da rede),
as linhas podem (por exemplo) ter a forma

.sp
.nf
		/dev/ed0		146.164.16.220
		netmask=255.255.255.192, default
.fi

.sp
onde "/dev/ed0" corresponde ao dispositivo "ethernet".
O valor "146.164.16.220" é o endereço IP deste computador
(através deste dispositivo), e "255.255.255.192"
é a máscara da rede, o que define a rede "146.164.16.192"
(através de um "E BINÁRIO").

.sp
A palavra chave "default" ao final da linha indica que um roteador
deve ser usado caso o endereço IP não seja da rede "146.164.16.192".
O roteador tem (quase sempre) o primeiro endereço da rede, no caso
"146.164.16.193".

.sp
No segundo caso (protocolo DHCP),
as linhas podem (por exemplo) ter a forma

.sp
.nf
		/dev/ed0		dhcp
		netmask=0.0.0.0, default
.fi

.sp
Repare que "default" finaliza a tabela de roteamento, isto é,
ela indica que todos os datagramas ainda não enviadas serão dirigidas
para o último dispositivo.

.bc	/*************************************/
.sp
Se for usada (adicionalmente) uma linha discada ("modem"),
a tabela de roteamento é modificada automaticamente no momento do
estabelecimento da conexão: se a tabela ainda NÃO tiver uma entrada
"default", a linha discada será utilizada como "default"; caso contrário,
ela será posta em penúltimo lugar. Isto significa que uma linha discada
tem prioridade sobre uma entrada "default".
.ec	/*************************************/

.bc	/*************************************/
Em particular, se foi usado o "default",
não poderá ser usado linha discada ("modem").
.ec	/*************************************/

.sp
No caso de uma linha serial PPP, estas linhas têm a forma

.sp
.nf
	      /dev/ppp002,-clocal,fifo=16,115.2,ppp=vj 192.168.0.2
	      netmask=255.255.255.0
.fi

.sp
onde "/dev/ppp002" corresponde à linha serial com protocolo PPP.
Lembre-se de que "ppp000" corresponde a COM1, "ppp002" a COM2, ...

.sp
O argumento "-clocal" deve ser dado caso o cabo físico contenha os
sinais de controle de fluxo por "hardware" (RTS/CTS); em caso contrário
deve ser posto "clocal". O argumento "fifo" determina o número de caracteres
transmitidos de cada vez; normalmente não precisa ser alterado.
A velocidade (no caso "115.2") é a taxa de transmissão (no caso
115200 baud, ou seja 115200 bits/segundo). Veja os outros valores
possíveis através de "stty" (cmd). O argumento "ppp=vj" deve ser dado
caso seja utilizada a compressão do cabeçalho TCP/IP.
Naturalmente, todos estes argumentos devem ser iguais nas duas extremidades
da ligação PPP.

.sp
No caso de uma linha serial SLIP, estas linhas têm a forma

.sp
.nf
		/dev/slip002,-clocal,fifo=16,115.2 192.168.0.2
		netmask=255.255.255.0
.fi

.sp
onde "/dev/slip002" corresponde à linha serial com protocolo SLIP.
Lembre-se de que "slip000" corresponde a
COM1, "slip002" a COM2, ...

.sp
Os argumentos "-clocal", "fifo" e "115.2" são análogos ao caso PPP,
acima.

.sp
Se for usada (adicionalmente) uma linha discada ("modem"),
a tabela de roteamento é modificada automaticamente no momento do
estabelecimento da conexão: se a tabela ainda NÃO tiver uma entrada
"default", a linha discada será utilizada como "default"; caso contrário,
ela será posta em penúltimo lugar. Isto significa que uma linha discada
tem prioridade sobre uma entrada "default".

.ip 4. 3
Uma (ou mais) linha(s) contendo os endereços IP dos servidores de nomes DNS.
Um servidor de nomes executa a tradução de nomes simbólicos tais
como "rocinha.nce.ufrj.br" em endereços IP (no caso "146.164.8.2").

.sp
Uma linha tem a forma:

.sp
.nf
		name_server 200.17.63.121 146.164.10.2
.fi

.sp
onde podem ser dados vários endereços. Será utilizado o servidor
que oferecer resposta mais rápida.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
internet, ppp, vi, ed, edscb
.br
.wo "(xti): "
t_intro
.br
.br
.wo "(fmt): "
scb

.in
.sp
.(t
.b ARQUIVOS
.in 5
/etc/itnetdev
.)t

.in
.sp
.(t
.b EXEMPLO
.nf
	#
	#	Rêde doméstica (pode haver linha discada)
	#
	127.0.0.1		jupiter.tropix.com.br

	192.168.0.3		marte.tropix.com.br
	192.168.0.4		venus.tropix.com.br

	146.164.8.7		barra.nce.ufrj.br
	146.164.8.3		vidigal.nce.ufrj.br
	146.164.8.2		rocinha.nce.ufrj.br

	/dev/ed0				192.168.0.2
	netmask=255.255.255.0

	name_server		200.17.63.121	146.164.10.2
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
