.bp
.he 'EXPORTS (fmt)'TROPIX: Manual de Referência'EXPORTS (fmt)'
.fo 'Atualizado em 06.03.06'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "exports -"
tabela de diretórios exportados do NFS
.br

.in
.sp
.b DESCRIÇÃO
.in 5
A tabela "exports" é utilizada durante o processo de inicialização
da INTERNET para obter informações sobre os diretórios "exportados",
isto é, os diretórios que o servidor NFS deve disponibilizar para
os clientes NFS de outros computadores.

.sp
A tabela reside no arquivo "/etc/exports", e pode ser editada
diretamente pelo administrador do sistema através de "ed" (cmd)
ou "vi" (cmd).

.sp
Após a inicialização do sistema, os diretórios exportados podem ser
alterados através de "exportfs" (cmd).

.sp
As linhas deste arquivo tem o formato do exemplo abaixo:

.sp
.nf
		/src venus(rw,async,no_root_squash)
.fi

.sp
Cada linha define um par (diretório, computador cliente). Esta linha
autoriza o computador cliente "venus" a montar o diretório "/src".
Para permitir a montagem do mesmo diretório por outro cliente, será necessária outra linha.

.sp
O nome do cliente pode conter os caracteres "*" e "?" com o mesmo significado
da "sh". Por exemplo, "*.nce.ufrj.br" aceitará todos os computadores do domínio "nce.ufrj.br".

.sp
Linhas em branco ou começando por "#" serão ignoradas.

.sp
O nome do cliente pode ser seguido de uma série de opções entre parenteses.
Estas opções podem ter os seguintes valores:

.in +3
.ip secure 15
Só aceita datagramas NFS com a porta abaixo de 1024 (não implementado).

.ip insecure 15
Aceita datagramas NFS com qualquer porta (opção "default").

.ip ro 15
Monta o diretório apenas para leituras (opção "default").

.ip rw 15
Monta o diretório para leituras e escritas.

.ip sync 15
As escritas dos blocos dos arquivos será feita imediatamente, e só
após a conclusão desta escrita será enviada a confirmação para o cliente (não implementado).

.ip async 15
A confirmação para o cliente é enviado imediatamente, mas
as escritas dos blocos dos arquivos será feita apenas de acordo 
com o algorítmo do CACHE de blocos do servidor (opção "default").

.ip hide 15
As subárvores dos sistemas de arquivos exportados que correspondem a um
outro dispositivo (isto é, a um outro sistema de arquivos montado em algum ponto
do sistema de arquivos exportado) são "invisíveis" para o cliente
(opção "default").

.sp
Se alguma subárvore deste tipo deve ser exportada, será necessário
um outro ponto de montagem.

.ip nohide 15
As subárvores são "visíveis" (veja acima, não implementado).
Esta opção não é implementada, pois irá implicar em números de
nós-índice duplicados na mesma montagem.

.ip root_squash 15
Este é um item de segurança que impede a um superusuário de um cliente
possuir acessos de superusuário neste servidor. Esta opção mapeia
o UID 0 de superusuário do cliente em -2 no servidor (opção "default").

.sp
Lembre-se que normalmente os UIDs dos outros usuários NÃO é alterada (veja abaixo).

.ip no_root_squash 15
Não altera o UID do superusuário (veja acima).
O superusuário de um cliente irá possuir acessos de superusuário neste servidor.

.ip all_squash 15
Este é um item de segurança mais radical, que mapeia
o UID de todos os usuários do cliente em -2 no servidor.

.ip anonuid=uid 15
Mapeia o UID de todos os usuários do cliente em "uid" no servidor.
Esta opção é útil para clientes com sistemas operacionais que não possuem o conceito
de UID.

.ip anongid=gid 15
Mapeia o GID de todos os usuários do cliente em "gid" no servidor.
Esta opção é útil para clientes com sistemas operacionais que não possuem o conceito
de GID.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
internet, exportfs, sh, vi, ed
.br
.wo "(fmt): "
nfs
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/exports

.bc	/*************************************/
.in
.sp
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
.ec	/*************************************/

.in
.sp
.b ESTADO
.in 5
Efetivo.
