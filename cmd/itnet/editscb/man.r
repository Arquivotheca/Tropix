.bp
.he 'EDITSCB (cmd)'TROPIX: Manual de Referência'EDITSCB (cmd)'
.fo 'Atualizado em 14.08.98'Versão 3.1.0'Pag. %'

.b NOME
.in 5
.wo "editscb -"
edita/imprime o bloco de controle da INTERNET
.br

.in
.sp
.b SINTAXE
.in 5
.(l
editscb
editscb -e [-v] [<porta>]
editscb -t [-v]
editscb -f [-v] <dev> [<sz>]
editscb -d [-v] [+-][<arg>]
editscb -p [-v] <arg>=<valor>
.)l

.bc	/*************************************/
editscb [-etv] [-f <dev> [<sz>]] [-d [+-][<arg>]]
						[-p <arg>=<valor>]
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "editscb" gerencia diversos parâmetros da INTERNET. 

.sp
Sem opções, "editscb" imprime inicialmente uma linha com os
seguinte parâmetros gerais (veja "t_intro"):

.in +3
.ip 1.
ITBLOCKs: O número de áreas de entrada/saída
em uso/alocadas para datagramas.

.ip 2.
RAW EPs: O número de estruturas ("endpoints") em uso/alocadas
para o modo-de-acesso-interno.

.ip 3.
UDP EPs: O número de estruturas em uso/alocadas para o modo-sem-conexão.

.ip 4.
TCP EPs: O número de estruturas em uso/alocadas para o modo-com-conexão.

.ep
.in -3

.sp
Em seguida "editscb" imprime os seguintes parâmetros do modo-com-conexão (TCP):

.in +3
.ip 1.
WND_SZ: O tamanho total da janela:
Número de bytes permitidos para a recepção de dados em uma conexão.

.ip 2.
GOOD_WND: O "bom" tamanho da janela:
Limite de bytes para permitir o início de transmissão em uma conexão.

.ip 3.
ALPHA: O fator de amortecimento do algoritmo de retransmissão TCP em \%
(Veja o RFC 793, pg. 41, para detalhes sobre este algoritmo).

.ip 4.
BETA: O fator de variância de atraso do algoritmo de retransmissão TCP
(Veja o RFC 793, pg. 41, para detalhes sobre este algoritmo).

.ip 5.
SRTT: O tempo amortecido de viagem de ida e volta inicial (em segundos)
do algoritmo de retransmissão TCP
(Veja o RFC 793, pg. 41, para detalhes sobre este algoritmo).

.ip 6.
N_TRANS: O número máximo de transmissões dos datagramas TCP.

.ip 7.
WAIT: O tempo máximo (em segundos) de espera para conexões TCP
ainda não completadas.
 
.ip 8.
SILENCE: O tempo máximo (em segundos) de manutenção de conexões TCP sem
transmissão/recepção de datagramas.

.ip 9.
MAX_SGSZ: O tamanho máximo (em bytes) do segmento TCP permitido nas negociações.

.ep
.in -3

.in
.sp
.b DEPURAÇÃO/CONTROLE
.in 5
O sistema operacional TROPIX permite que a atividade dos módulos da
INTERNET sejam monitorados/controlados (veja a opção "-d").

.sp
Os argumentos <arg> que alteram a depuração/controle são:

.in +3
.ip 'wi:' 4
Grava a entrada de datagramas no arquivo "itnet.debug".

.ip 'wo:' 4
Grava a saída de datagramas no arquivo "itnet.debug".

.ip 'li:' 4
Relata a entrada de datagramas.

.ip 'lo:' 4
Relata a saída   de datagramas.

.ip 'rt:' 4
Relata retransmissões.

.ip 'oq:' 4
Relata remoções da fila de retransmissão.

.ip 'ce:' 4
Causa erros propositais de "checksum" na recepção de datagramas.

.ip 'er:' 4
Relata erros.

.ip 'in:' 4
Lista algumas infomações adicionais.

.ip 'bs:' 4
Relata segmentos recusados.

.ip 'lw:' 4
Relata tamanhos das janelas.

.ip 'tt:' 4
Relata cálculos do SRTT.

.ip 'gw:' 4
Executa a função de GATEWAY.

.ip 'pi:' 4
Usa a otimização para conexões TCP internas.

.ep
.in -3

.in
.sp
.b 'OPÇÕES DE "EDITSCB"'
.in 5
As opções do comando são:

.in +3
.ip -e
Imprime as estruturas ("endpoints") TCP em uso. Para cada conexão,
são dados os endereços IP ("Internet Protocol"), portas,
o estado da conexão, os números de seqüência,
os tamanhos das janelas, os tamanhos das filas de entrada/saída,
e outras variáveis da estrutura TCP_EP (Veja o arquivo 
"/usr/include/sys/itnet.h" e o RFC 793).

.sp
Se for dada a <porta> opcional, serão impressos somente os
dados referentes à esta porta local.

.ip -t
Imprime 3 tabelas: a tabela de endereços IP conhecidos,
a tabela de roteamento e a tabela de endereços dos dispositivos "ethernet".

.sp
A tabela de endereços contém uma linha para cada nó conhecido da rêde,
contendo o seu endereço IP, alguns indicadores e o nome do nó.
Os indicadores são:
A coluna "C" contém "C" para o nome de um
computador, "A" para um nome "alias" de um computador e "M" para
um domínio de correio eletrônico.
A coluna "E" (erros) contém "N" para um nome inexistente, "S" para um êrro
do servidor de nomes e <sp> se não houver erros.
A coluna "I" contém o índice do servidor de nomes utilizado para obter
este endereço; corresponde à lista dada no final do arquivo "/etc/itnetdev".
A coluna "PRE" contém o valor de "preferência" de um servidor de correio.

.sp
A tabela de roteamento indica para qual dispositivo de saída
o datagrama deve ser enviado (baseado no seu endereço IP destino).
O datagrama é enviado para o dispositivo associado à primeira linha que
satisfizer: ((endereço & MÁSCARA) == REDE).
O campo LOCAL dá o endereço IP fonte a ser utilizado no datagrama. 
O campo GATEWAY dá o endereço IP de um roteador, caso a rede destino não
esteja conectada diretamente a este computador.
O campo DISPOSITIVO dá o nome simbólico do dispositivo que deve enviar o
datagrama.

.sp
A tabela de endereços dos dispositivos "ethernet" fornece o endereço físico
do dispositivo "ethernet" (48 bits) associado ao endereço IP. 

.ip -v
Verboso.

.ip -f
Consulta/altera o tamanho do FIFO de saída da linha serial <dev>.
Esta opção só é pertinente se a linha serial for o modêlo NS16550
(ou equivalentes), e <dev> for um dispositivo SLIP ou PPP.
Se <sz> for dado (de 1 a 16), altera o valor para <sz>.
Se <sz> não for dado, imprime o valor corrente.

.ip -d
Depuração/controle: Esta opção permite consultar/alterar a
depuração/controle da INTERNET (veja os argumentos acima).

.sp
Com a opção "-d -" são impressos os estados atuais dos argumentos
(asteriscos no início da linha indicam que o argumento está ligado);
com a opção "-d <arg>" ou "-d +<arg>" é ligado o argumento <arg>;
com a opção "-d -<arg>" é desligado o argumento <arg>.

.ip -p
Altera o parâmetros.
Podem apenas ser alterados os parâmetros do modo-com-conexão (TCP):
WND_SZ, GOOD_WND, ALPHA, BETA, SRTT,
N_TRANS, WAIT, SILENCE e MAX_SGSZ.
O parâmetro ALPHA deve ser dado como uma percentagem; os demais
deve ser dados com o próprio valor; vários parâmetros pode ser dados
separados por vírgulas.

.sp
Repare que os valores de ITBLOCKs, RAW EPs, UDP EPs e TCP EPs são modificados
por "edscb" e NÃO por este comando "editscb".

.sp
Consulte "scb" (fmt), parâmetros "nitblock", "nrawep", "nudpep", "ntcpep", 
"ndns", "nroute" e "nether".

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que NÃO há arquivo de configuração para os parâmetros TCP.
Se for desejado usar (sempre) um valor não padrão, é necessário
incluir um linha executando "editscb" no arquivo "/etc/rc".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
edscb, prscb
.br
.wo "(xti): "
t_intro
.br
.wo "(fmt): "
scb
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
Postel, J., "Transmission Control Protocol", RFC 793,
USC/Information Sciences Institute, September 1981.

.in
.sp
.b ARQUIVOS
.in 5
/etc/itnetdev
 /etc/rc

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
