.bp
.he 'RLOGIN (cmd)'TROPIX: Manual de Referência'RLOGIN (cmd)'
.fo 'Atualizado em 15.08.97'Versão 3.0.2'Pag. %'

.b NOME
.in 5
.wo "rlogin -"
entra em sessão em um nó remoto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
rlogin [-iv] [-l <usuário_remoto>] <nó_remoto>

rlogin [-iv] [<usuário_remoto>@]<nó_remoto>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "rlogin" estabelece uma sessão no <nó_remoto> da rêde de
comunicações.

.sp
Uma vez tendo completada a conexão e entrado em sessão com o <nó_remoto>,
"rlogin" lê caracteres do teclado, enviado-os para o <nó_remoto>
e lê caracteres recebidos do <nó_remoto>, escrevendo-os na tela do terminal.

.sp
O eco é realizado no <nó_remoto> (isto é, pelo servidor), de tal
modo que a sessão remota é transparente (a não ser pela menor velocidade).

.sp
O tipo do terminal remoto é o mesmo do que o local
(como dado na variável "TERM" do ambiente).

.sp
As opções do comando são:

.in +3
.ip -i
Não converte o código na recepção: normalmente, é efetuada a conversão
dos códigos recebidos através da rêde para o código apropriado do
terminal local (por exemplo ISO para ABICOMP). Com a opção
"-i" esta conversão não é efetuada; isto é útil quando o código
recebido através de rêde já é o adequado para o terminal.  

.ip -v
Verboso: Diversas informações são impressas, entre as quais os nomes
do usuário local e remoto.

.ip -l
Define o nome do usuário remoto (pode também ser dado no formato
com o caractere "@"): se não for dado, o nome do usuário remoto
é o mesmo do que o nome do usuário local.
.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Já é processado o protocolo de cópia do tamanho da janela.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
telnet
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"BSD Rlogin", B. Kanter, Request for Comments 1282.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
