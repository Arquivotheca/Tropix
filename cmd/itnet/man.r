.bp
.he 'INTERNET (cmd)'TROPIX: Manual de Referência'INTERNET (cmd)'
.fo 'Atualizado em 27.08.99'Versão 3.2.2'Pag. %'

.b NOME
.in 5
.wo "internet -"
como preparar o TROPIX para acessar a INTERNET
.br

.in
.sp
.b DESCRIÇÃO
.in 5
Após a instalação do TROPIX, a INTERNET já deve estar ativa no modo interno.
Para verificar, use o comando "editscb -t" (cmd), e certifique-se
de que há uma linha terminada com "<local>" (abaixo de "DISPOSITIVO").

.sp
Um outro modo de testar é executar o comando "ping" (cmd):

.sp
.nf
		ping meu_computador
.fi

.sp
O nome "meu_computador" é provisório; ele deve ser substituído por
um nome adequado no arquivo "/etc/itnetdev" (veja abaixo). 

.sp
O modo interno é necessário (entre outros) para o sistema gráfico
X-Window.

.sp
A INTERNET (naturalmente) também pode (e deve) ser usada externamente através de
vários dispositivos de entrada/saída.
Abaixo damos os passos necessários para configurar a INTERNET:

.in +3
.ip 1. 3
Acertar as portas/IRQs dos dispositivos usados pela internet: linhas
seriais e/ou "ethernet". Veja "scb" (fmt) e "edscb" (cmd).
Certifique-se de que eles são reconhecidos durante a carga do TROPIX.

.ip 2. 3
Editar o arquivo "/etc/itnetserv".
Este arquivo (muito provavelmente) já está pronto, e não necessita de
modificações. Veja "itnetserv" (fmt).

.ip 3. 3
Editar o arquivo "/etc/itnetdev".
Veja "itnetdev" (fmt).

.ip 4. 3
Uma vez com estes dois arquivos prontos, podemos verificar se a INTERNET
já pode ser iniciada corretamente, recarregando o sistema operacional.
Durante a passagem do modo "mono-usuário" para "multi-usuário", é
executado o programa

.sp
.nf
		/usr/lib/itnet/itnet_daemon 
.fi

.sp
através do arquivo "/etc/rc" (veja "rc" (fmt) e "itnet_daemon" (cmd)).
Observe se é impressa alguma mensagem de erro. Se houver alguma
mensagem, corrija o arquivo que ocasionou o erro, e reexecute o comando
acima (diretamente, NÃO é necessário recarregar o sistema).

.sp
No caso de não ser impressa nenhuma mensagem de erro, provavelmente
os diversos processos da INTERNET foram iniciados corretamente.
Verifique através do comando "ps -x" (veja "ps" (cmd)) se este foi o caso
(devem estar presentes processos com os nomes "itnet_daemon",
"in_daemon", "out_daemon" e vários "serv_daemon").


.(t
.ip 5. 3
Faça um teste com

.sp
.nf
		ping meu_computador
.fi
.)t

.sp
e com

.sp
.nf
		rlogin meu_computador
.fi

.sp
onde "meu_computador" é o nome dado ao computador no início do
arquivo "/etc/itnetdev".

.ip 6. 3
Para a utilização de linha discada (protocolo PPP), veja "ppp" (cmd).

.ep
.in -3

.sp 2
Uma vez com a internet operacional,
o comando "editscb" pode ser usado para monitorar o seu funcionamento.
De particular importância é
a opção "-t", através da qual são impressas as tabelas
de endereços IP conhecidos,
de roteamento e de endereços "ethernet"
(veja "editscb" (cmd)).
Verificar se a tabela de roteamento está correta.

.sp
Alguns dos servidores escrevem informações no arquivo "/var/adm/itnetlog"
(caso ele já exista); para consultar este arquivo use "show" ou "tail" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):"
edscb, ps, itnet_daemon, ppp, ping, rlogin, editscb,
.wo "				    show, tail"
.br
.wo "(fmt): "
scb, itnetserv, itnetdev, rc
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/itnetdev
 /etc/itnetserv
 /usr/lib/itnet/itnet_daemon 
 /etc/rc
 /var/adm/itnetlog

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
