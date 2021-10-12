.bp
.he 'TELNET (cmd)'TROPIX: Manual de Referência'TELNET (cmd)'
.fo 'Atualizado em 31.08.98'Versão 3.0.3'Pag. %'

.b NOME
.in 5
.wo "telnet -"
interface com um nó remoto usando o protocolo TELNET
.br

.in
.sp
.b SINTAXE
.in 5
.(l
telnet [-eliv] <nó_remoto> [<porta_remota>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "telnet" é a interface do usuário (cliente) para a
comunicação com um nó remoto da rêde usando o protocolo TELNET.

.sp
Uma vez tendo completada a conexão com o <nó_remoto>, "telnet"
lê caracteres do teclado, enviado-os para o <nó_remoto>
e lê caracteres recebidos do <nó_remoto>, escrevendo-os na tela do terminal.

.sp
Normalmente (isto é, sem as opções "-e" e "-l"), "telnet" opera
no modo "caracter a caracter com eco remoto" (desde que o servidor
do <nó_remoto> concorde em ecoar os caracteres):
cada caractere teclado é imediatamente
enviado ao <nó_remoto> e o eco é realizado pelo servidor.
Este modo é o que permite o melhor contrôle
sobre o(s) processo(s) em execução no <nó_remoto>, mas é que mais
sobrecarrega a rêde de comunicações.

.sp
Se a <porta_remota> for dada, usa esta porta ao invés da porta
padrão do "telnet" (que é 23). Neste caso, automaticamente entra
no modo "linha a linha com eco local", e não processa as seqüências
de contrôle das opções TELNET.

.sp
As opções do comando são:

.in +3
.ip -e
Modo "caracter a caracter com eco local":
com esta opção cada caractere teclado é imediatamente
enviado ao <nó_remoto>, mas o eco é realizado localmente por "telnet".
Este modo sobrecarrega menos a rêde (pois os ecos não a atravessam);
por outro lado, podem haver dificuldades na utilização de editores de
textos tais como o "vi".

.ip -l
Modo "linha a linha com eco local":
com esta opção, os caracteres são ecoados localmente por "telnet"
e somente são enviadas linhas completas para o <nó_remoto>.
Este modo é o que menos sobrecarrega a rêde
(pois os ecos não a atravessam e os caracteres são compactados em linhas);
por outro lado, podem haver dificuldades na utilização de editores de
textos tais como o "vi" e com os caracteres de controle tais como
<^C>, etc ...

.ip -i
Não converte o código na recepção: normalmente, é efetuada a conversão
dos códigos recebidos através da rêde para o código apropriado do
terminal local (por exemplo ISO para ABICOMP). Com a opção
"-i" esta conversão não é efetuada; isto é útil quando o código
recebido através de rêde já é o adequado para o terminal.  

.ip -v
Verboso: Diversas informações são impressas, entre as quais as negociações
entre o cliente e o servidor.

.ep
.in -3

.sp
O programa "telnet" (cliente) pede ao servidor para que o
servidor ecoe (ou não) os caracteres (de acordo com as opções dadas).

.in
.sp
.b OBSERVAÇÕES
.in 5
Tanto o cliente "telnet" como o servidor nunca enviam os caracteres
de controle "go ahead".

.sp
Somente parte do protocolo TELNET está implementado. Por exemplo,
os caracteres de controle tais como "intr" (normalmente <^C>,
ver "stty" (cmd) ou "termio" (fmt)),
são enviados sem conversões,
quando (pelo protocolo TELNET) deveriam ser convertidos nas
seqüências IAC ... (por exemplo, <^C> em IAC IP).

.sp
Das inúmeras opções TELNET, somente as opções ECHO (1), SUPRESS GO AHEAD (3),
TERMINAL TYPE (24) e WINDOW SIZE (31) são reconhecidas e processadas.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
stty, rlogin
.br
.wo "(fmt): "
termio
.br

.in
.sp
.b
REFERÊNCIAS
.r
.(l
Postel, J. & Reynolds J.:
"TELNET PROTOCOL SPECIFICATION, RFC 854",
Maio de 1983.
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
