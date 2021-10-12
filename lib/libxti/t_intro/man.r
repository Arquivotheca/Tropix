.bp
.he 'INTRO (xti)'TROPIX: Manual de Referência'INTRO (xti)'
.fo 'Escrito em 04.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "intro -"
introdução aos conceitos da interface XTI

.in
.sp 2
.b INTRODUÇÃO
.in 5
A especificação XTI ("X/Open Transport Interface") define uma interface
de serviço de transporte que tenta ser independente do veículo
específico do transporte (isto é, do sistema de transporte e do
protocolo nele usado).
A interface é definida através de um conjunto de funções para a
linguagem de programação "C" (a biblioteca "xti", ou "libxti").

.sp
XTI engloba um grande conjunto de funções e facilidades que pode
variar de importância e/ou utilidade. Uma aplicação será portátil
através de vários sistemas com implementações diversas da XTI, se
uma (ou ambas) das seguintes afirmativas for verdadeira:

.in +3

.ip a. 3
A aplicação foi programada para modificar seu comportamento de
acordo com o subconjunto das funções e facilidades disponíveis
em cada uma das implementações nas quais a aplicação deve funcionar.

.ip b. 3
A aplicação só utiliza as funções e facilidades básicas, que
garantidamente estão presentes em todas as implementações.

.ep
.in -3

.sp
Aos poucos, nesta introdução, serão indicadas quais as funções
e facilidades suportadas nesta implementação.

.in
.sp 2
.(t
.b 'MODELO DE PROGRAMAÇÃO'
.in 5

.sp
.(l
	+---------------+			+---------------+
	|		|			|		|
	|   Aplicação	| 			|   Aplicação	|
	|		|			|		|
	+-------+-------+			+-------+-------+
		|					|
		|					|
	+-------+-------+			+-------+-------+
	|		|			|		|
	|  Sistema de	| 			|  Sistema de	|
	|		+--->>--( REDE )---<<---+		|
	|  Transporte	|			|  Transporte	|
	|		|			|		|
	+---------------+			+---------------+
.)l
.)t

.sp
O modelo de programação abstrato que será utilizado em toda esta
introdução (e nos demais manuais de referência das funções da "libxti")
contém apenas dois níveis: o nível de usuário (ou programa de aplicação),
que utiliza os serviços da XTI, e o (assim chamado) sistema de transporte,
que executa todos os serviços relacionados à rede de comunicações.
A comunicação entre este dois níveis se dá exclusivamente através das
funções da "libxti".

.sp
Em uma implementação real, o sistema de transporte é na
verdade um sistema complexo composto de vários níveis, que no entanto
não precisam ser considerados aqui
(pois são transparentes para o usuário da rede).

.in
.sp 2
.b 'CODIFICAÇÃO DAS CHAVES DOS ARGUMENTOS DAS FUNÇÕES'
.in 5
Cada manual de referência de uma função da "libxti"
contém uma tabela que resume os conteúdos dos argumentos de entrada
e saída da função.
Os significados das chaves são as seguintes:

.in +3
.ip ' x ' 5
O valor do argumento é significativo
(se for um argumento de entrada
ele deve ter um valor atribuído antes da chamada, e se for um argumento
de saída ele contém um valor que pode ser consultado após a chamada).

.ip (x) 5
O conteúdo do objeto apontado pelo argumento é significativo.

.ip ' ? ' 5
O valor do argumento é significativo, mas o argumento é opcional.

.ip (?) 5
O conteúdo do objeto apontado pelo argumento é opcional.

.ip ' / ' 5
O valor do argumento é irrelevante.

.ip ' = ' 5
O valor do argumento (após a chamada) não é modificado.


.in -3
.ep

.in
.sp 2
.b 'COMPATIBILIDADE ENTRE OS PROTOCOLOS ISO E UDP/TCP'
.in 5
A XTI foi originalmente definida em função do protocolo ISO,
e posteriormente estendida para os protocolos da INTERNET (TCP e UDP).
Esta implementação dispõe (na presente versão)
apenas dos protocolos TCP, UDP e o modo-de-acesso-interno.

.sp
O modo-de-acesso-interno ("raw") permite o uso de protocolos
próprios definidos a nível de usuário, que o sistema de transporte
encapsula com um cabeçalho IP. O objetivo deste modo é o de 
possibilitar a definição e teste de novos protocolos
(que possivelmente mais tarde serão incorporados ao sistema
de transporte), e também o uso do protocolo ICMP.
Este modo não faz parte da XTI, e portanto seu uso deve ser evitado
se for desejada portabilidade para outras implementações.

.sp
O restante desta seção utiliza conceitos que irão ser definidos
apenas mais adiante, e em um primeira leitura, o leitor pode
passar para o início da seção seguinte.

.sp
Embora tornar o protocolo utilizado "invisível" para o usuário
seja um dos objetivos da XTI, isto não se efetivou integralmente,
principalmente em virtude dos indicadores "T_MORE" e "T_EXPEDITED"
e da desconexão ordenada.

.sp
Os indicadores "T_MORE" e "T_EXPEDITED" originam-se do protocolo ISO,
e embora o protocolo TCP tenha conceitos semelhantes nos indicadores
PUSH e URGENT, eles não são intercambiáveis.

.sp
Nesta implementação, foram introduzidos os indicadores "T_PUSH" e "T_URGENT",
e os 4 indicadores têm os seguintes efeitos:

.in +3
.ip T_MORE 13
No protocolo ISO, este indicador é utilizado para assinalar
segmentos/datagramas incompletos. Nesta implementação, este
indicador é sempre ignorado, exceto na função "t_rcvudata",
para indicar que o datagrama não foi recebido integralmente.

.ip T_EXPEDITED 13
No protocolo ISO, este indicador é utilizado para assinalar
segmentos transmitidos/recebidos com dados de prioridade mais alta
(e são processados em um outro fluxo de dados).
Nesta implementação, este indicador é sempre ignorado.
É possível fazer com que este indicador
tenha o efeito de URGENT (veja abaixo) com as linhas

.sp
.(l
			#undef	T_EXPEDITED
			#define	T_EXPEDITED	T_URGENT
.)l

.sp
após a inclusão de <xti.h>.

.ip T_URGENT 13
Este indicador é semelhante a EXPEDITED, isto é, indica a
transmissão/recepção de dados de prioridade mais alta.
No entanto, o protocolo TCP dispõe de apenas um fluxo de dados.

.sp
Neste manual de referência e nos manuais de todas as funções da "libxti",
o termo "urgente" e "expedito" é usado como sinônimo.
Do mesmo modo, os eventos "T_EXDATA" e "T_GOEXDATA" e
referem-se a dados "urgentes". 

.ip T_PUSH 13
Por questões de eficiência, normalmente um segmento TCP só é
transmitido quando completa um determinado tamanho "ideal".
Em certas situações, no entanto, é necessário transmitir o segmento
mesmo estando incompleto. Para isto, utilizamos este indicador
na função "t_snd", ou ainda, podemos utilizar a função "t_push"
(que no entanto não é padrão da "libxti").

.ep
.in -3

.sp
Outra diferença entre os protocolos é a presença
da desconexão ordenada (funções "t_sndrel" e "t_rcvrel")
no protocolo TCP, que efetua uma desconexão mais elegante
do que a desconexão abortiva do protocolo ISO.

.sp
É claro que se forem usados os indicadores "T_PUSH" e "T_URGENT",
a desconexão ordenada ou a função "t_push", fica comprometida a
portabilidade para outra implementação da XTI.

.in
.sp 2
.(t
.b '"ENDPOINTS" DE TRANSPORTE'
.in 5
Um "endpoint" de transporte é uma estrutura de dados interna do sistema
de transporte que contém as informações necessárias para
o controle de uma comunicação em andamento.
.)t

.sp
A função "t_open" aloca um "endpoint" para o usuário, recebendo como
primeiro argumento o nome de um identificador de protocolo
(como por exemplo "/dev/itntcp" para o protocolo TCP, "/dev/itnudp"
para o protocolo UDP e "/dev/itnraw" para o modo-de-acesso-interno),
e devolve um descritor de arquivos ("fd").
Este descritor de arquivos "fd" deve ser usado como argumento
em todas as chamadas subseqüêntes a funções da "libxti" para
se referir a este "endpoint".

.sp
Para estar ativo, um "endpoint" deve ter um endereço de protocolo
associado, o que é feito através da função "t_bind".

.sp
Repare que não existe uma associação direta dos processos ao
sistema de transporte. Deste modo, vários outros processos que podem
obter o "fd" (ou um "fd" equivalente) através das funções "fork", "thread",
"dup" e "fcntl" (sys), podem acessar o mesmo "endpoint",
e portanto o mesmo caminho de comunicações.

.in
.sp 2
.b 'ENDEREÇOS DE PROTOCOLO'
.in 5
Em uma rede de computadores, para que um usuário possa determinar com qual
nó da rede (computador) deseja estabelecer uma comunicação, são utilizados
os endereços de protocolo.
A interface XTI não define o formato destes endereços.

.sp
Nesta implementação, um endereço de protocolo consiste da estrutura
"INADDR", com 3 membros (veja o arquivo de inclusão <xti.h>):
O endereço INTERNET do nó na rede ("a_addr"), a porta de serviço no nó
("a_port") e a família de protocolos ("a_family", não usado nesta versão).
No modo-de-acesso-interno, não existe o conceito de porta de serviço,
e o membro "a_port" contém o número do protocolo (através do "#define"
"a_proto").

.sp
Normalmente, em cada nó da rede, são executados
diversos (assim chamados) servidores, cada um deles oferecendo
um determinado serviço. A porta de serviço determina com qual destes
servidores a comunicação deve ser estabelecida.

.in
.sp 2
.b 'ASSOCIAÇÃO DE UM PROCESSO A UM "ENDPOINT"'
.in 5
Um processo pode alocar simultaneamente vários "endpoints". No entanto,
no modo síncrono, o processo deve executar seqüencialmente as
diversas ações do sistema de transporte nos "endpoints" associados.

.sp
É importante lembrar que o sistema de transporte trata todos os
usuários de um "endpoint" como se fosse um único usuário.
Assim, se diversos processos compartilharem o mesmo
"fd" (através de "fork", "thread", "dup" ou "fcntl"), eles devem
sincronizar as suas ações para que não executem uma função que
seja inapropriada num certo estado de um "endpoint".

.sp
As funções "t_getstate" e "t_sync" retornam o estado corrente do
sistema de transporte, possibilitando deste modo o usuário
a verificar este estado antes de tomar novas ações.

.sp
Um processo pode esperar por um pedido de conexão (função "t_listen")
em um "fd" e aceitar a conexão em um outro "fd", para o qual foi
associado um endereço com o argumento "qlen" igual a zero
(veja a função "t_bind"). Isto facilita a confecção de servidores
que aguardam pedidos de conexão em uma dada porta. O servidor
aceita a conexão em um novo "endpoint" e cria um processo filho
("fork" ou "thread") para executar o serviço, sem bloquear a recepção de outros
pedidos de conexão no "endpoint" original.

.in
.sp 2
.(t
.b 'MODOS DE SERVIÇO'
.in 5
A interface de serviço de transporte suporta três modos de serviço:
o modo-com-conexão, o modo-sem-conexão e o modo-de-acesso-interno.
Um "endpoint" não pode suportar mais de um modo simultaneamente.
.)t

.sp
O modo-com-conexão oferece um serviço orientado a circuito, e possibilita
a transferência de dados de uma maneira confiável e ordenada.
Este serviço possibilita a negociação de parâmetros e opções que
governam a transferência de dados.
Ele provê um mecanismo de identificação que evita a perda de tempo
gerada pela transmissão e resolução de endereços durante a fase de
transferência de dados.
Além disto, ele provê um contexto no qual unidades sucessivas de dados
transferidas entre usuários estão relacionadas logicamente.
Este tipo de serviço é atrativo para aplicações que necessitam de
interações relativamente longas e orientadas a um fluxo de dados.
Este é o serviço oferecido pelo protocolo TCP.

.sp
Em contraste, o modo-sem-conexão é orientado a mensagem e suporta
transferências em unidades auto-contidas sem nenhuma relação
lógica necessária entre as unidades. Estas unidades são chamadas
de "datagramas". Este serviço necessita de uma convenção entre os
usuários envolvidos, que determina as características dos dados
a serem transmitidos. Este serviço não suporta nenhuma negociação
de parâmetros e/ou opções.
Toda a informação necessária para a entrega de um datagrama
(por ex. o endereço do destinatário) é fornecida ao sistema de
transporte, juntamente com os dados a serem transmitidos, em
um pedido único de serviço, que não tem nenhuma relação
com nenhum outro pedido (anterior ou posterior).
Este tipo de serviço é atrativo para aplicações que envolvem
interações de pedido/resposta de curta duração, exibem um
alto nível de redundância, são reconfiguráveis dinamicamente
ou não necessitam de entrega de dados garantida nem sequencial.
Este é o serviço oferecido pelo protocolo UDP.

.sp
O modo-de-acesso-interno ("raw") interage com um nível mais básico do
sistema de transporte, e assim como o modo-sem-conexão,
ele é orientado à transferência de datagramas.
Ele permite o uso de protocolos próprios definidos a nível de aplicação,
e o objetivo é o de possibilitar a definição e teste de novos protocolos
(que possivelmente mais tarde serão incorporados ao sistema de transporte).
Todos os detalhes do novo protocolo são de responsabilidade da aplicação;
o sistema de transporte apenas adiciona/retira ao/do datagrama um cabeçalho IP
e o transmite/recebe.


.bc	/*************************************/
Na transmissão, o datagrama já deve conter (além dos dados
propriamente ditos), o cabeçalho do protocolo que o usuário deseja 
utilizar, com todos os campos devidamente preenchidos
(inclusive o "checksum", se houver).
Na recepção, o usuário recebe o datagrama completo com o cabeçalho
do protocolo e é sua responsabilidade processar todos os campos.
(o sistema de transporte

Neste modo, não existe o conceito de "porta de serviço", e portanto
o datagrama não pode ser enviado para um determinado processo
(a aplicação é que deve decidir se o datagrama recebido
destina-se a ela). 
Na recepção, pode ser indicado o número de um protocolo, 
.ec	/*************************************/

.in
.sp 2
.b 'TRATAMENTO DE ERROS'
.in 5
Cada chamada a uma função da biblioteca "libxti" pode retornar
com um código de erro de um repertório de um ou mais códigos.
Retornos com erro são indicados pelo valor -1.
Uma variável externa, "t_errno", que está definida no arquivo
de inclusão <xti.h>, contém o número específico do erro ocorrido.
Este valor é atribuído quando ocorre um erro, mas ele não é removido
por uma chamada com sucesso a uma função da biblioteca, de tal modo
que ele só deve ser consultado quando um erro foi detetado pelo código de
retorno. Uma função de diagnóstico, "t_error", pode ser usada
para imprimir uma mensagem de erro.
O estado de um "endpoint" pode mudar em caso de erro.

.sp
Nesta implementação, os erros da interface de transporte
estão unificados com os erros do sistema operacional, de tal
modo que a variável "t_errno" é idêntica à variável "errno",
e a função "t_error" é idêntica à função "perror" (libc).
Isto já está definido no arquivo <xti.h>, e portanto é transparente
para o usuário. Em conseqüencia, o erro [TSYSERR] nunca ocorre.

.in
.sp 2
.b 'MODOS DE EXECUÇÃO SÍNCRONO E ASSÍNCRONO'
.in 5
A interface de serviço de transporte é inerentemente assíncrona;
vários eventos podem ocorrer independentemente das ações do
usuário do sistema de transporte. Por exemplo, um usuário pode estar
enviando dados através de uma conexão de transporte quando chega uma
indicação assíncrona de desconexão. O usuário deve ser informado
(de algum modo) que a conexão foi desfeita.

.sp
A interface de serviço de transporte suporta dois modos para tratar
eventos assíncronos: o modo síncrono e o modo assíncrono. No modo
síncrono de operação, as primitivas de transporte esperam por
determinados eventos antes de retornarem ao usuário. Enquanto
espera, o usuário não pode executar outras tarefas. Por exemplo,
uma função que tenta receber dados no modo síncrono só irá retornar
o controle para o usuário quando houver dados disponíveis.
O modo síncrono é o modo normal ("default") de execução. É um
modo útil para processos de usuários que querem esperar pela
ocorrência de eventos, ou para processos de usuários que mantêm
apenas uma conexão de transporte. Note que, se ocorrer um sinal,
chamadas bloqueadas são interrompidas e retornam com -1 (a
variável "t_errno" recebe o valor [EINTR], ver <errno.h>).
Neste caso a chamada à função não tem efeito.

.sp
Por outro lado, o modo assíncrono provê um mecanismo para
notificar um usuário da ocorrência de um evento sem força-lo a
esperar pelo evento. A possibilidade de manusear eventos de um
modo assíncrono é uma capacidade desejável para uma interface de
transporte. Isto possibilita a usuários efetuarem trabalhos úteis
enquanto esperam por um evento em particular. Por exemplo, uma
função que tenta receber dados no modo assíncrono vai retornar
o controle imediatamente para o usuário, se nenhum dado está
disponível. O usuário pode então testar periodicamente por dados
até que eles cheguem. O modo assíncrono é adequado para aplicações que
têm longas esperas entre eventos e têm outras tarefas que podem
ser executadas neste meio tempo, ou então para aplicações que
processam múltiplas conexões concorrentemente.

.sp
Os dois modos de execução não são oferecidos através de interfaces
distintas ou dois conjuntos de funções. Ao invés disto, as funções
que processam eventos de entrada têm dois modos de operação:
síncrono e assíncrono. O modo desejado é especificado através do
indicador O_NONBLOCK, que pode ser dado quando o "endpoint" é criado
(função "t_open"), ou antes de utilizar qualquer função específica
através da chamada ao sistema "fcntl" (sys). O efeito deste indicador
é local para este processo, e está descrito no manual de referência
de cada função.

.sp
A interface de serviço de transporte define 9 eventos assíncronos
para cobrir os serviços no modo-com-conexão,
modo-sem-conexão e modo-de-acesso-interno.
Eles são representados como indicadores distintos em uma máscara de
bits, usando os seguintes nomes simbólicos: T_LISTEN, T_CONNECT,
T_DATA, T_EXDATA, T_DISCONNECT, T_ORDREL, T_UDERR, T_GODATA, T_GOEXDATA.
Eles são descritos mais adiante, na seção GERÊNCIA DE EVENTOS.

.sp
Mesmo que um processo chame as funções no modo síncrono, ele necessita
de reconhecer certos eventos assíncronos e tomar determinadas providências,
caso necessário. Isto é efetuado através do erro especial [TLOOK], que
é retornado por uma função quando ocorre um evento assíncrono.
A função "t_look" deve ser então utilizada para identificar o
evento específico que ocorreu.

.sp
Outro modo de notificar um processo de que um evento assíncrono ocorreu
é através do teste periódico por um dos eventos assíncronos listados
acima.
Neste modo, o processo realiza trabalho útil, e periodicamente
testa se ocorreu algum evento. Esta facilidade é obtida utilizando-se
o indicador O_NONBLOCK.

.in
.sp 2
.b 'EVENTOS E A FUNÇÃO "t_look"'
.in 5
Todos os eventos que ocorrem em um "endpoint" são armazenados pela
XTI. Este eventos são obtidos (um de cada vez) através da função "t_look".
Se ocorreram múltiplos eventos, a ordem na qual "t_look" fornece
os eventos é dependente da implementação, e a aplicação não deve
depender de uma ordem específica.
Um evento fica armazenado em um "endpoint" até que seja tratado.
Cada evento tem uma função correspondente que o trata e o retira
do "endpoint". Dois eventos, T_GODATA e T_GOEXDATA são também
retirados do "endpoint" já pela própria chamada a "t_look".

.sp
.(t
A seguinte tabela resume estas idéias:

.sp
.(l
	+---------------+---------------+-----------------------+
	|    Evento	|    Removido  	|  Função que o trata	|
	| 		| por "t_look"?	|			|
	+---------------+---------------+-----------------------+
	| T_LISTEN	|	Não	|  t_listen		|
	| T_CONNECT	|	Não	|  t_rcvconnect		|
	| T_DATA	|	Não	|  t_rcv[udata|raw]	|
	| T_EXDATA	|	Não	|  t_rcv		|
	| T_DISCONNECT	|	Não	|  t_rcvdis		|
	| T_UDERR	|	Não	|  t_rcvuderr		|
	| T_ORDREL	|	Não	|  t_rcvrel		|
	| T_GODATA	|	Sim	|  t_snd[udata|raw]	|
	| T_GOEXDATA	|	Sim	|  t_snd		|
	+---------------+---------------+-----------------------+
.)l
.)t

.in
.sp 2
.b 'GERÊNCIA DE EVENTOS'
.in 5
Cada chamada a uma função da interface XTI trata de um "endpoint"
de cada vez. Não é possível esperar simultaneamente por diversos
eventos de diferentes fontes, particularmente de diversas conexões.
A interface XTI possui um serviço de gerência de eventos, que
possibilita um processo a ser notificado dos seguintes eventos:

.in +3

.ip * 2
T_LISTEN: Um pedido de conexão de um usuário remoto foi recebido
pelo sistema de transporte (somente no modo-com-conexão);
este evento pode ocorrer apenas quando já foi atribuído um endereço válido
ao "endpoint" (função "t_bind") e ainda não foi estabelecida uma
conexão de transporte.

.ip * 2
T_CONNECT: Uma resposta de conexão foi recebida pelo sistema de transporte;
ocorre apenas após um "t_connect" (somente no modo-com-conexão).

.ip * 2
T_DATA: Dados normais foram recebidos pelo sistema de transporte.

.ip * 2
T_EXDATA: Dados urgentes (expeditos) foram recebidos pelo sistema de transporte.

.ip * 2
T_DISCONNECT: Um pedido de desconexão abortiva foi recebido pelo sistema de
transporte; pode ser reportado pelas duas funções de transferências
de dados e pelas funções "t_accept" e "t_snddis"
(somente no modo-com-conexão).

.ip * 2
T_ORDREL: Um pedido de desconexão ordenada foi recebido pelo sistema de
transporte (somente no modo-com-conexão com desconexão ordenada).

.ip * 2
T_UDERR: Foi detetado um erro na transmissão anterior de um datagrama.
Pode ser notificado pelas funções "t_rcvudata" e "t_unbind"
(somente no modo-sem-conexão).

.ip * 2
T_GODATA: As restrições de fluxo para a transmissão de dados normais foram
suspensas. Dados normais podem ser transmitidos novamente.

.ip * 2
T_GOEXDATA: As restrições de fluxo para a transmissão de dados urgentes foram
suspensas. Dados urgentes podem ser transmitidos novamente.

.in -3
.ep

.in
.sp 2
.b 'VISÃO GERAL DO MODO-COM-CONEXÃO'
.in 5
O serviço de transporte do modo-com-conexão consiste de 4 fases de
comunicação:

.in +3

.ip * 2
Inicialização/de-inicialização.

.ip * 2
Estabelecimento de uma conexão.

.ip * 2
Transferência de dados.

.ip * 2
Desconexão.

.in -3
.ep

.sp
Com o objetivo de estabelecer uma conexão de transporte, uma
aplicação do usuário deve:

.in +3

.ip 1. 3
Fornecer um identificador de transporte do modo desejado
ao sistema de transporte (usando "t_open"); isto cria
um "endpoint" de transporte através do qual o usuário pode
requisitar serviços ao sistema de transporte.

.ip 2. 3
Associar um endereço ao "endpoint" (usando "t_bind").

.ip 3. 3
Utilizar as funções de conexão apropriadas ("t_connect" ou
"t_listen" e "t_accept") para estabelecer uma conexão de transporte.
O conjunto de funções depende de o usuário ser um cliente ou servidor.

.ip 4. 3
Uma vez com a conexão estabelecida, podemos intercambiar dados
normais e expeditos (urgentes).
O protocolo ISO prevê o conceito de dados "expeditos", e o protocolo
TCP o conceito de dados "urgentes". Embora as idéias sejam
similares, significando dados "de alta prioridade", o protocolo
ISO supõe a existência de dois canais de comunicacão
independentes, um para dados normais e outro para expeditos.
Com isto, no protocolo ISO, dados expeditos podem chegar ao
destino e serem processados por ele antes de dados normais, que
possivelmente foram enviados anteriormente.
O protocolo TCP contém apenas um canal de comunicação, e os dados
urgentes são enviados do mesmo modo que os normais, apenas com
uma indicação de urgente, para que a aplicação no destino
saiba que se tratam de dados "de alta prioridade", e tome as
devidas providências.
Como esta implementação oferece apenas o protocolo TCP, utilizaremos
o termo expedito como sendo urgente.

.ip 5. 3
A conexão pode ser desfeita a qualquer momento, usando as
funções de desconexão. Em seguida o usuário pode de-inicializar
o "endpoint" de transporte fechando o descritor de arquivos devolvido
por "t_open" (e portanto liberando os recursos para uso futuro),
ou especificar um novo endereço (após o antigo ter sido
liberado através de "t_unbind"), ou ainda utilizar o mesmo endereço
e estabelecer uma nova conexão de transporte.

.in -3
.ep

.in
.sp 2
.b 'FASE DE INICIALIZAÇÃO/DE-INICIALIZAÇÃO'
.in 5
Um resumo das funções que efetuam a inicialização/de-inicialização
é dado abaixo. Todas estas funções realizam apenas operações locais;
nenhuma informação é transmitida através da rede de comunicações.


.in +3

.ip 't_open ()' 15
Esta função cria um "endpoint" de transporte e retorna informações
específicas de protocolo associadas ao "endpoint". Também retorna
um descritor de arquivos que serve de identificador local para o
"endpoint".

.ip 't_bind ()' 15
Esta função associa um endereço de protocolo a um "endpoint",
e deste modo ativando o "endpoint". Ela também pode pedir para iniciar
a aceitação de indicações de conexão.

.ip 't_optmgmt ()' 15
Esta função possibilita o usuário a obter ou negociar opções do
protocolo com o sistema de transporte.

.ip 't_unbind ()' 15
Esta função incapacita um "endpoint" de tal modo que mais nenhum
pedido destinado a este "endpoint" seja aceito pelo sistema de transporte.

.ip 't_close ()' 15
Esta função informa ao sistema de transporte que o usuário terminou
suas operações com o "endpoint", e libera todos os recursos locais
associados a este "endpoint".

.in -3
.ep

.sp
As funções seguintes são também operações de gerência locais,
mas podem ser executadas durante qualquer fase da comunicação:

.in +3

.ip 't_getinfo ()' 15
Esta função retorna informações específicas de protocolo associadas
ao "endpoint".

.ip 't_getstate ()' 15
Esta função retorna o estado corrente do "endpoint".

.ip 't_sync ()' 15
Esta função sincroniza as estruturas de dados gerenciadas pela biblioteca
de transporte com o sistema de transporte.

.ip 't_alloc ()' 15
(OPCIONAL, NÃO DISPONÍVEL) Esta função aloca memória para a estrutura
de dados especificada.

.ip 't_free ()' 15
(OPCIONAL, NÃO DISPONÍVEL) Esta função libera a memória da estrutura
de dados que foi alocada por "t_alloc".

.ip 't_error ()' 15
Esta função imprime uma mensagem descrevendo o último erro encontrado
durante uma chamada a uma função do sistema de tansporte.

.ip 't_look ()' 15
Esta função retorna o evento corrente associado com um "endpoint".

.in -3
.ep

.in
.sp 2
.(t
.b 'VISÃO GERAL DO ESTABELECIMENTO DE UMA CONEXÃO'
.in 5
Esta fase possibilita dois usuários da rede de comunicações a estabelecerem
uma conexão de transporte entre si. Neste cenário, um dos usuários
é considerado ativo e inicia a conversação (cliente), enquanto o outro
usuário é passivo e espera por um pedido de conexão (servidor).
.)t

.sp
No modo-com-conexão:

.in +3

.ip * 2
O usuário deve inicialmente criar um "endpoint".

.ip * 2
Em seguida, deve ser atribuído um endereço para o "endpoint".

.ip * 2
Um "endpoint" pode ser associado com uma, e apenas uma,
conexão de transporte estabelecida.

.ip * 2
É possível usar um "endpoint" para receber pedidos de conexão
e colocá-los em uma fila. Este tipo de operação é declarado na
chamada a "t_bind", com o argumento "qlen" com um valor maior do que 0.

.ip * 2
A função "t_listen" é usada para verificar se há algum pedido de
conexão na fila. Se há algum, retorna detalhes da indicação da
conexão, e um número de seqüência local que identifica univocamente
este pedido de conexão. No modo assíncrono, se a fila estiver vazia,
retorna com um valor negativo e a variável "t_errno" com o valor
[TNODATA]. O número de pedidos de conexão ainda não processados na
fila é limitado pelo valor de "qlen" dado por ocasião da chamada
à função "t_bind".

.ip * 2
Se o "endpoint" tiver mais do que um pedido de conexão na fila, o usuário
deve tirar da fila todos os pedidos de conexão (assim como os de
desconexão), antes de aceitar ou rejeitar algum deles ou todos.

.ip * 2
Ao aceitar uma conexão, o servidor pode utilizar o mesmo "endpoint"
(o utilizado pela função "t_listen") ou um outro.

.sp
Se o mesmo "endpoint" for utilizado, ele não pode mais ser usado
para receber (e enfileirar) novos pedidos de conexão.
O endereço do protocolo estará "ocupado" enquanto o "endpoint" estiver
ativo. Nenhum outro "endpoint" pode receber o mesmo endereço enquanto
o "endpoint" original estiver na fase de transferência de dados
ou na fase de desconexão (isto é, até que seja dado um "t_unbind").

.sp
Se for utilizado um "endpoint" diferente, o "endpoint" utilizado pela
função "t_listen" pode continuar a receber e enfileirar pedidos de
conexão.

.bc	/*************************************/
.ip * 2
Se o usuário chamar a função "t_connect" em um "endpoint" no qual
tinha sido chamado "t_listen", este "endpoint" não pode mais ser
usado para receber e enfileirar pedidos de conexão.
.ec	/*************************************/

.in -3
.ep

.sp
.(t
As funções que efetuam estas operações de conexão são as seguintes:

.in +3

.ip 't_connect ()' 17
Esta função pede uma conexão a um usuário de transporte em um destino
específico e espera pela resposta do usuário remoto. Esta função
pode ser efetuada no modo síncrono ou assíncrono. No modo síncrono,
a função espera pela resposta do usuário remoto antes de retornar o
controle ao usuário local. No modo assíncrono, a função inicia o
estabelecimento da conexão, mas retorna o controle ao usuário local
antes de uma resposta chegar.
.)t

.ip 't_rcvconnect ()' 17
Esta função possibilita a um usuário ativo (cliente) a determinar
o estado de um pedido de conexão enviado anteriormente. Se o pedido
foi aceito, a fase de estabelecimento de conexão estará completa no
retorno desta função. Esta função é utilizada em conjunto com
"t_connect" para estabelecer uma conexão no modo assíncrono.

.ip 't_listen ()' 17
Esta função possibilita um usuário passivo (servidor) a receber
pedidos de conexão de outros usuários (clientes).

.ip 't_accept ()' 17
Esta função é chamada por um usuário passivo (servidor) para aceitar
uma determinada conexão após o recebimento de um pedido.

.in -3
.ep

.in
.sp 2
.b 'VISÃO GERAL DA TRANSFERÊNCIA DE DADOS'
.in 5
Uma vez que uma conexão de transporte tenha sido estabelecida entre
dois usuários, dados podem ser transferidos em um e outro sentidos
através da conexão, em um modo "full duplex". Para efetuar estas
transferências de dados no modo-com-conexão são utilizadas as funções:

.in +3

.ip 't_snd ()' 17
Esta função possibilita usuários de transporte a enviarem dados normais
ou urgentes através de uma conexão.

.ip 't_rcv ()' 17
Esta função possibilita usuários de transporte a receberem dados normais
ou urgentes através de uma conexão.

.in -3
.ep

.sp
Nesta implementação podemos também usar as chamadas ao sistema "read" e
"write" (sys) para transferir dados. Lembre-se no entanto, de que isto
irá prejudicar a portabilidade da aplicação.
Veja detalhes nas funções "t_snd" e "t_rcv".

.sp
Na fase de transferência de dados, a ocorrência do evento T_DISCONNECT
implica em um retorno com falha das funções "t_snd" ou "t_rcv" com
"t_errno" com o valor [TLOOK]. O usuário deve então chamar "t_look" para
obter mais detalhes.

.in
.sp 2
.b 'RECEBENDO DADOS'
.in 5
Se houver dados (normais ou urgentes) disponíveis, então uma chamada
a "t_rcv" retorna dados imediatamente. Se a conexão de transporte não
existir mais, então a chamada retorna imediatamente com uma indicação
de falha. Se não houver dados disponíveis imediatamente, e a conexão
existe, então o resultado de uma chamada a "t_rcv" depende do modo:

.in +3

.ip * 2
Modo assíncrono: a chamada retorna imediatamente, indicando falha.
O usuário tem de verificar a chegada de dados, ou por repetidas chamadas
a "t_rcv" ou utilizando a função "t_look".

.ip * 2
Modo síncrono: a chamada é bloqueada até que dados estejam disponíveis,
seja recebida uma indicação de desconexão ou chegue um sinal.
O usuário pode usar a função "t_look" para determinar se há dados
disponíveis, ao invés de chamar a função "t_rcv", que pode
bloquear o processo.

.sp
Nesta implementação podemos também usar a função "t_nread"
para verificar se há dados disponíveis. Lembre-se no entanto, de que isto
irá prejudicar a portabilidade da aplicação.

.in -3
.ep

.in
.sp 2
.b 'ENVIANDO DADOS'
.in 5

Se os dados podem ser aceitos imediatamente pelo sistema de transporte,
então eles são aceitos e a chamada retorna o número de bytes aceitos.
Se os dados não podem ser aceitos em virtude de uma condição de falha
permanente, então a chamada retorna com uma indicação de falha. Se os
dados não podem ser aceitos imediatamente em virtude de condições
transientes (por ex. falta de "buffers" ou controle de fluxo), então
o resultado de uma chamada a "t_snd" depende do modo de execução:

.in +3

.ip * 2
Modo assíncrono: a chamada retorna imediatamente, indicando falha.
Se a falha foi causada por restrições de fluxo de dados, então é
possível que somente uma parte dos dados seja aceita pelo sistema de
transporte. Neste caso, "t_snd" retornará um valor de bytes menor
do que o pedido para ser transmitido. Neste caso, o usuário pode
chamar "t_snd" novamente para transmitir os dados restantes, ou
então esperar a liberação da restrição de fluxo através de "t_look",
e então transmitir os dados.

.ip * 2
Modo síncrono: a chamada é bloqueada até que uma das seguintes
condições seja verdadeira:

.in +3
.ip - 2
As restrições de controle de fluxo sejam removidas, e o sistema de
transporte esteja apto a aceitar todos os dados a transmitir.
Neste caso, "t_snd" retorna com sucesso.

.ip - 2
É recebida uma indicação de desconexão. Neste caso, "t_snd" retorna
com falha, com "t_errno" contendo o valor [TLOOK]. O usuário deve
então chamar "t_look", que indicará o evento T_DISCONNECT.
Os dados que estavam sendo transmitidos serão perdidos.

.ip - 2
Ocorre um problema interno. Neste caso, a função "t_snd" retorna
com falha, com "t_errno" contendo o código do erro.
Os dados que estavam sendo transmitidos serão perdidos.

.in -3
.ep

.in -3
.ep

.sp
Pelo padrão XTI, dados normais e urgentes (expeditos) constituem
dois fluxos de dados distintos. Como isto no entanto, é imcompatível
com o protocolo TCP, nesta implementação, os dados normais e urgentes
utilizam apenas um fluxo de dados.

.sp
O padrão XTI prevê também dois modos de enviar dados, o orientado
a registro e o orientado a uma seqüência de bytes. A diferença entre
os dois é a de que o modo orientado a registro preserva as fronteiras
entre as mensagens, o que não ocorre no modo orientado a uma seqüência de
bytes. Um usuário pode determinar este modo diretamente no "t_open",
ou posteriormente através de "t_getinfo". O campo "tsdu" fornece
o tamanho máximo do registro. Se for igual a zero, é oferecido o modo
orientado a uma seqüência de bytes, o que é o caso desta implementação.
Para maiores detalhes, veja "t_getinfo".

.in
.sp 2
.b 'VISÃO GERAL DA DESCONEXÃO'
.in 5
O protocolo TCP suporta a desconexão ordenada e a desconexão abortiva.

.sp
Uma desconexão abortiva pode ser invocada durante
a fase de estabelecimento da conexão, ou durante
a fase de transferência de dados.
Durante a fase de estabelecimento da conexão, um usuário pode usar
a desconexão abortiva para rejeitar um pedido de conexão.
Durante a fase de transferência de dados, qualquer um dos dois
usuário pode abortar a conexão a qualquer momento.
A desconexão abortiva não é negociada pelos usuários e ela tem
efeito imediato. O usuário do outro lado da conexão é notificado
quando a conexão é abortada. O sistema de transporte pode também
iniciar uma desconexão abortiva, caso em que ambos usuário são
informados que a conexão não exite mais. Não há garantia da
entrega de dados uma vez que uma desconexão abortiva tenha sido
iniciada.

.sp
Qualquer que seja o estado de uma conexão de transporte, os
seus usuários são informados, assim que possível, de uma
falha através de um evento de desconexão ou um retorno com
falha de uma chamada a "t_snd" ou "t_rcv". Se o usuário deseja
prevenir a perda de dados pela notificação do usuário remoto
de uma desconexão iminente, é de sua responsabilidade utilizar um
mecanismo de nível mais alto. Por exemplo, o usuário pode enviar
dados específicos (urgentes) e esperar pela resposta antes de
iniciar uma desconexão abortiva.

.sp
A desconexão ordenada, que pode ser iniciada apenas na
fase de transferência de dados, possibilita a dois usuários
a terminarem "graciosamente" uma conexão. O procedimento
da desconexão ordenada evita a perda de dados que pode ocorrer
durante uma desconexão abortiva.

.sp
As funções que efetuam a desconexão são:

.in +3

.ip 't_snddis ()' 15
Esta função pode ser chamada por qualquer um dos usuários para
iniciar uma desconexão abortiva. Pode também ser usada para rejeitar
um pedido de conexão durante a fase de estabelecimento da conexão.

.ip 't_rcvdis ()' 15
Esta função identifica a razão da desconexão abortiva, onde a
conexão é liberada pelo sistema de transporte ou outro usuário.

.ip 't_sndrel ()' 15
Esta função pode ser chamada por qualquer um dos usuários para
iniciar uma desconexão ordenada. A conexão permance intacta até
que ambos os usuários chamem esta função e "t_rcvrel".

.ip 't_rcvrel ()' 15
Esta função deve ser chamada quando um usuário é informado de um
pedido de uma desconexão ordenada, como um modo de informar o
sistema de transporte de que o usuário está ciente das ações do
usuário remoto.

.in -3
.ep

.in
.sp 2
.b 'VISÃO GERAL DO MODO-SEM-CONEXÃO'
.in 5
O serviço de transporte do modo-sem-conexão consiste de duas fases
de comunicação: Inicialização/de-inicialização e transferência de dados.
Com a finalidade de efetuar uma transferência de dados no modo-sem-conexão,
um usuário deve:

.in +3
.ip 1. 3
Obter um "endpoint" apropriado para este modo, usando a função "t_open".

.ip 2. 3
Associar a este "endpoint" um endereço, usando a função "t_bind".

.ip 3. 3
O usuário pode então enviar e/ou receber dados utilizando as
funções "t_sndudata" e "t_rcvudata".

.ip 4. 3
Uma vez terminada a fase de transferência de dados, o usuário
pode ou fechar diretamente o descritor de arquivos retornado por
"t_open" (usado "t_close"), ou então iniciar uma nova transferência
de dados utilizando um novo endereço (usando "t_unbind" e
"t_bind").

.in -3
.ep

.in
.sp 2
.b 'FASE DE INICIALIZAÇÃO/DE-INICIALIZAÇÃO'
.in 5
As funções que efetuam a inicialização/de-inicialização são as
mesmas das usadas no serviço de transporte do modo-com-conexão
(veja acima).

.in
.sp 2
.b 'VISÃO GERAL DA TRANSFERÊNCIA DE DADOS'
.in 5
Uma vez que o "endpoint" tenha sido ativado, o usuário pode
enviar e receber dados através do "endpoint", através das seguintes funções:

.in +3
.ip 't_sndudata ()' 15
Esta função possibilita um usuário a enviar uma unidade completa de
dados (datagrama) para o endereço especificado.

.ip 't_sndudata ()' 15
Esta função possibilita um usuário a receber unidades completas de
dados (datagramas) de outros usuários.

.ip 't_rcvuderr ()' 15
Esta função possibilita um usuário a receber informações de erro
referentes a um datagrama enviado anteriormente.

.in -3
.ep

.sp
Os únicos eventos que podem ser reportados ao usuário são
T_UDERR, T_DATA e T_GODATA. Dados urgentes (expeditos) não
podem ser usados no serviço de transporte do modo-sem-conexão.

.in
.sp 2
.b 'RECEBENDO DADOS'
.in 5
Se houver dados disponíveis (um datagrama), então uma chamada
a "t_rcvudata" retorna imediatamente indicando o número de bytes
recebidos. Se não houver dados disponíveis imediatamente, então
o resultado de uma chamada a "t_rcvudata" depende do modo:

.in +3

.ip * 2
Modo assíncrono: a chamada retorna imediatamente, indicando falha.
O usuário tem de verificar a chegada de dados, ou por repetidas chamadas
a "t_rcvudata" ou utilizando a função "t_look".

.ip * 2
Modo síncrono: a chamada é bloqueada até que dados estejam disponíveis,
seja detetado um erro ou chegue um sinal.
O usuário pode usar a função "t_look" para determinar se há dados
disponíveis, ao invés de chamar a função "t_rcvudata", que pode
bloquear o processo.

.sp
Nesta implementação podemos também usar a função "t_nread"
para verificar se há dados disponíveis. Lembre-se no entanto, de que isto
irá prejudicar a portabilidade da aplicação.

.in -3
.ep

.in
.sp 2
.b 'ENVIANDO DADOS'
.in 5

.in +3

.ip * 2
Modo assíncrono: O sistema de transporte pode não ter a capacidade de
enviar o datagrama imediatamente, por restrições de controle de fluxo.
Neste caso, a função "t_sndudata" retorna com falha, contendo o erro
[TFLOW] em "t_errno". Neste caso, o usário pode tentar novamente mais
tarde, ou utilizar a função "t_look" para verificar se a restrição de
controle de fluxo foi removida.

.ip * 2
Modo síncrono: Com o objetivo de manter algum controle de fluxo,
a função "t_sndudata" retorna somente quando for possível enviar
novamente um datagrama.
Um processo que envia dados no modo síncrono pode ficar
bloqueado durante algum tempo.

.in -3
.ep

.sp
Se a função "t_sndudata" for chamada antes de o usuário destinatário
ativar o seu "endpoint" de transporte, o datagrama pode ser perdido.

.in
.sp 2
.b 'VISÃO GERAL DO MODO-DE-ACESSO-INTERNO'
.in 5
O serviço de transporte do modo-de-acesso-interno consiste de duas fases
de comunicação: Inicialização/de-inicialização e transferência de dados.
Com a finalidade de efetuar uma transferência de dados no
modo-de-acesso-interno, um usuário deve:

.in +3
.ip 1. 3
Obter um "endpoint" apropriado para este modo, usando a função "t_open".

.ip 2. 3
Associar a este "endpoint" um endereço e/ou número de protocolo,
usando a função "t_bind".

.ip 3. 3
O usuário pode então enviar e/ou receber datagramas utilizando as
funções "t_sndraw" e "t_rcvraw".

.ip 4. 3
Uma vez terminada a fase de transferência de dados, o usuário
pode ou fechar diretamente o descritor de arquivos retornado por
"t_open" (usado "t_close"), ou então iniciar uma nova transferência
de dados utilizando um novo endereço/número de protocolo
(usando "t_unbind" e "t_bind").

.in -3
.ep

.in
.sp 2
.b 'FASE DE INICIALIZAÇÃO/DE-INICIALIZAÇÃO'
.in 5
As funções que efetuam a inicialização/de-inicialização são as
mesmas das usadas no serviço de transporte do modo-com-conexão
(veja acima).

.in
.sp 2
.b 'VISÃO GERAL DA TRANSFERÊNCIA DE DADOS'
.in 5
Uma vez que o "endpoint" tenha sido alocado e ativado, o usuário pode
enviar e receber dados através do "endpoint", através das seguintes funções:

.in +3
.ip 't_sndraw ()' 15
Esta função possibilita um usuário a enviar um datagrama (contendo
o cabeçalho do protocolo e dados) para o endereço especificado.

.ip 't_rcvraw ()' 15
Esta função possibilita um usuário a receber um datagrama (contendo
o cabeçalho do protocolo e dados) de outros nós da rede.

.in -3
.ep

.sp
Os únicos eventos que podem ser reportados ao usuário são
T_DATA e T_GODATA. Dados urgentes (expeditos) não
podem ser usados no serviço de transporte do modo-de-acesso-interno.

.in
.sp 2
.(t
.b 'RECEBENDO DADOS'
.in 5
Se houver dados disponíveis (um datagrama), então uma chamada
a "t_rcvraw" retorna imediatamente indicando o número de bytes
recebidos. Se não houver dados disponíveis imediatamente, então
o resultado de uma chamada a "t_rcvraw" depende do modo:
.)t

.in +3

.ip * 2
Modo assíncrono: a chamada retorna imediatamente, indicando falha.
O usuário tem de verificar a chegada de dados, ou por repetidas chamadas
a "t_rcvraw" ou utilizando a função "t_look".

.ip * 2
Modo síncrono: a chamada é bloqueada até que dados estejam disponíveis,
seja detetado um erro ou chegue um sinal.
O usuário pode usar a função "t_look" para determinar se há dados
disponíveis, ao invés de chamar a função "t_rcvraw", que pode
bloquear o processo.

.sp
Nesta implementação podemos também usar a função "t_nread"
para verificar se há dados disponíveis. Lembre-se no entanto, de que isto
irá prejudicar a portabilidade da aplicação.

.in -3
.ep

.in
.sp 2
.b 'ENVIANDO DADOS'
.in 5

.in +3

.ip * 2
Modo assíncrono: O sistema de transporte pode não ter a capacidade de
enviar o datagrama imediatamente, por restrições de controle de fluxo.
Neste caso, a função "t_sndraw" retorna com falha, contendo o erro
[TFLOW] em "t_errno". Neste caso, o usário pode tentar novamente mais
tarde, ou utilizar a função "t_look" para verificar se a restrição de
controle de fluxo foi removida.

.ip * 2
Modo síncrono: Com o objetivo de manter algum controle de fluxo,
a função "t_sndraw" retorna somente quando for possível enviar
novamente um datagrama. Um processo que envia dados no modo
síncrono pode ficar bloqueado durante algum tempo.

.in -3
.ep

.in
.sp 2
.b 'ESTADOS DA INTERFACE DE TRANSPORTE'
.in 5

A interface de serviço de transporte XTI gerencia um "endpoint" de
transporte usando um máximo de 8 estados: T_UNINIT, T_UNBND, T_IDLE,
T_OUTCON, T_INCON, T_DATAXFER, T_INREL e T_OUTREL.

.sp
Os estados T_OUTCON, T_INCON e T_DATAXFER são significativos somente
no modo-com-conexão.

.sp
Os estados T_INREL e T_OUTREL são significativos somente
no modo-com-conexão, se a função de desconexão ordenada for usada.

.sp
.(t
A tabela abaixo descreve todos os estados como vistos pelo usuário
do sistema de transporte:

.sp
.(l
	+---------------+-------------------------------+
	|    Estado	|	   Descrição		|
	+---------------+-------------------------------+
	| T_UNINIT	| Não inicializado - estado	|
	|		| inicial e final da interface	|
	+---------------+-------------------------------+
	| T_UNBND	| Inicializado, mas sem endereço|
	+---------------+-------------------------------+
	| T_IDLE	| Inicializado e com endereço,	|
	|		| mas sem conexão		|
	+---------------+-------------------------------+
	| T_OUTCON	| Aguardando resposta de um	|
	|		| pedido de conexão (cliente)	|
	+---------------+-------------------------------+
	| T_INCON	| Aguardando um	pedido de	|
	|		| conexão (servidor)		|
	+---------------+-------------------------------+
	| T_DATAXFER	| Transferindo dados		|
	+---------------+-------------------------------+
	| T_OUTREL	| Aguardando resposta de um	|
	|		| pedido de desconexão ordenada	|
	+---------------+-------------------------------+
	| T_INREL	| Aguardando um	pedido de	|
	|		| desconexão ordenada		|
	+---------------+-------------------------------+
.)l
.)t

.in
.sp 2
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
fork, thread, dup, fcntl
.br
.wo "(libc):"
perror
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
