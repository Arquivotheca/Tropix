.bp
.he 'PPP (cmd)'TROPIX: Manual de Referência'PPP (cmd)'
.fo 'Escrito em 14.04.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.sp
.wo "ppp -"
cria uma comunicação PPP com um nó remoto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ppp [-l <linha>] [-t <texto>] [-p <conta>:<senha>]
.)l

.bc	/*************************************/
ppp [-l <linha>] [-s <velocidade>] [-t <texto>] [-f <n>] [-j]

Opções:	-l: Define a <linha> de comunicações a utilizar
	    (se não dada, é utilizada a linha "/dev/pppline")
	-t: Escreve o <texto> de inicialização do modem
	    (contendo o número do telefone)
	-p: Usa a <conta:senha> dada

Obs.:	Para desconectar use "ps -k" ou "kill ..."
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ppp" cria uma conexão PPP ("Point to Point Protocol")
através de uma linha de comunicações.

.sp
Para criar uma conexão PPP através do comando "ppp" são necessários
os seguintes recursos/conhecimentos:

.in +3
.ip 1.
Uma conta em um provedor que permita o acesso através de linha discada
(por exemplo, o NCE/UFRJ).

.ip 2.
O endereço IP do servidor de nomes (DNS) do provedor
(por exemplo, 146.164.10.2 no caso do NCE).

.sp
Este endereço deve ser incluído na linha "name_server"
do arquivo "/etc/itnetdev" (veja itnetdev (fmt)). Este endereço deve
ser dado antes de inicializar a rede (por exemplo, através de
um "boot").

.ip 3.
O número de telefone a discar (por exemplo, 598-3240 no caso do NCE).

.ip 4.
Configurar o modem. Para isto é necessário saber o endereço ("port")
do modem e o seu IRQ (número da interrupção). Estes dois dados podem
ser alterados apenas nesta execução do TROPIX (durante o "boot") ou
(um pouco mais) definitivamente através do comando "edscb" (cmd).

.sp
Para facilitar, é útil dar um "link" (isto é, criar um sinônimo) do 
dispositivo do modem ("/dev/ppp00?") para o nome "default"
("/dev/pppline"); veja a opção "-l".

.ip 5.
Saber os caracteres de inicialização/controle do modem. Normalmente
basta uma cadeia do tipo "atpd5983240" (para discar o NCE), mas dependendo
da marca/modelo do modem, outros caracteres de controle podem ser necessários.
Consulte o manual do seu modem.

.sp
Por exemplo, se for desejado teclar no modo de tom (ao invés de pulso
como acima), basta trocar o "p" por "t".
Se você tiver um "modem" "US Robotics", talvez seja útil
acrescentar os caracteres "s6=5", que aumentam o tempo de espera pelo
sinal de discar para 5 segundos
(considerando que os telefones cariocas (muitas vêzes)
demoram para dar o sinal de discar).
Com estas duas modificações, teríamos a cadeia "ats6=5td5983240".

.sp
Esta cadeia pode ser dada através da opção "-t" (ver abaixo), ou ser teclada
manualmente.

.bc	/*************************************/
Com os telefones brasileiros, que (muitas vêzes) demoram para dar o sinal de discar,
é útil acrescentar os caracteres "s6=5"

.ip 6.
Saber a velocidade do modem. Se não for usada a opção "-s" (ver abaixo),
o modem irá tentar negociar a velocidade de 9600 baud (a não ser que ele
seja mais lento do que isto). Normalmente, o mais simples é utilizar
a opção "-s 38.4", pois neste caso o modem irá negociar a maior velocidade
comum (isto no atual estado da tecnologia, que são modems de até 33600
baud).
.ec	/*************************************/

.ep
.in -3

.sp
.(t
Após a discagem, quando o modem do provedor atender, inicia-se a fase de
autenticação, onde negocia-se o nome da conta e da senha. Em seguida, assim que
o provedor enviar o primeiro pacote PPP, o programa "ppp" inicia
automaticamente a negociação PPP, após a qual volta o "prompt" do
"sh" (cmd). Neste ponto, fica um processo "ppp" em "background",
e a conexão recem criada já pode ser usada.
.)t

.sp
Repare na linha "Entrando em modo PPP ...". Nela são dadas a velocidade
da conexão, e o número do processo ("pid") que será usado
mais tarde para terminar a conexão.

.sp
Nesta versão de "ppp" já é processado o protocolo PAP ("Password
Authentication Protocol").

.sp
Se não for conseguida a conexão na primeira tentativa, o comando "ppp"
executa mais 4 tentativas, após as quais a execução é encerrada.
Durante esta fase, o comando pode ser cancelado teclando-se os
caracteres "~.".

.sp
Utilize o comando "editscb" (cmd) com a opção "-t" para verificar 
se o dispositivo "ppp" foi incluído na tabela de roteamento.
Para testar a disponibilidade de nós da rede, use o comando "ping" (cmd).

.sp
Para encerrar a conexão, basta cancelar o processo "ppp" (que ficou em
"background"). Para isto usamos o comando "kill" (cmd) para terminar
o processo cujo número foi dado no momento do estabelecimento da conexão
("pid"), veja acima. Se você não guardou este número, 
podemos usar o comando "ps" (cmd) com o argumento "-k".
Uma outra alternativa é de usar "ps" apenas para saber o "pid" do processo
"ppp" e em seguida usar o comando "kill".

.sp
As opções do comando são:

.in +3
.ip -l
Define a <linha> de comunicações a utilizar.
Se esta opção não for dada, é utilizada a linha "/dev/pppline".
Repare que "/dev/ppp000" corresponde a COM1 (do MS-DOS),
"/dev/ppp001" corresponde a COM2,
"/dev/ppp002" corresponde a COM3, e assim por adiante.

.bc	/*************************************/
ppp [-l <linha>] [-s <velocidade>] [-t <texto>] [-f <n>] [-j]

.ip -s
Define a <velocidade> da linha.
Uma lista completa de velocidades é dada em "stty" (cmd).
Se esta opção não for dada, a velocidade da linha será estabelecida
em 115200 baud, e posteriormente o modem irá negociar a maior velocidade
possível com o modem do provedor.
Esta opção só precisa ser usada se for desejado utilizar velocidades
mais baixas do que o máximo acima mencionado.
.ec	/*************************************/

.ip -t
Escreve o <texto> dado na linha. Este <texto> é normalmente usado
para inicializar um modem e discar o número de telefone desejado. 

.bc	/*************************************/
.ip -f
Define o número máximo <n> de caracteres a escrever no FIFO
de saída da linha serial em cada interrupção.
Os valores válidos vão de 1 a 16 (mas dependem do tipo da linha serial).
O objetivo desta opção é a de reduzir a velocidade de saída da
linha serial, e normalmente NÃO deve ser usada.

.ip -j
Normalmente, é utilizada a compressão de "Van Jacobson" nos cabeçalhos
TCP/IP, que reduz o tamanho típico de 40 bytes para (até) 3 bytes. 
Com esta opção, esta compressão não é utilizada,
e normalmente (a opção) NÃO deve ser usada.
.ec	/*************************************/

.ip -p
Fornece a conta/senha para a conexão PPP. Normalmente, para a "conta" é
usado o próprio nome do usuário dado no "login" (cmd), e que está em geral disponível
através da variável "USER" do ambiente (veja "environ" (fmt)). Através desta
opção podemos dar um outro nome de conta.

.sp
A senha correspondente é pedida no início da execução de "ppp". Podemos também
adicioná-la à opção "-p" da linha de comando (veja a observação, abaixo),
o que por um lado é cômodo, por outro lado torna a senha menos secreta.

.sp
Na forma "-p <conta>:<senha>" são dados a conta e senha,
em "-p <conta>" é dada apenas a conta, e em "-p :<senha> é dada apenas a senha.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Uma forma cômoda de utilizar o programa "ppp" é o de construir
arquivos contendo já uma linha de comando para um provedor.
Veja como exemplo o arquivo "/usr/pub/bin/pppnce".

.sp
Durante a conexão PPP, é adicionada uma entrada para o dispositivo
"ppp" na tabela de roteamento. Esta entrada terá sempre prioridade
sobre entradas "default" (veja "itnetdev").

.bc	/*************************************/
Lembre-se de que o roteamento para o dispositivo "ppp" NÃO será
possível se for usado o roteamento "default" (veja "itnetdev").
.ec	/*************************************/

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, ps, kill, stty, edscb, editscb, ping
.br
.wo "(fmt): "
itnetdev, environ
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
W. Simpson, "The Point-to-Point Protocol (PPP)", RFC 1661,
Julho 1994.

.sp
V. Jacobson, "Compressing TCP/IP Headers for Low-Speed Serial
Links", RFC 1144, Fevereiro 1990.

.in
.sp
.b ARQUIVOS
.in 5
/dev/ppp00?
 /etc/itnetdev
 /usr/pub/bin/pppnce

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
