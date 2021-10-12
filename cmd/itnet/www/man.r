.bp
.he 'WWW (cmd)'TROPIX: Manual de Referência'WWW (cmd)'
.fo 'Atualizado em 07.05.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
Servidor e cliente HTTP
.sp
.wo "www_s -"
servidor
.br
.wo "www   -"
pequeno cliente para testes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
/usr/lib/itnet/www_s

www [-pgv] [-r <intervalo>] [-n <nó_remoto>] [<página>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O servidor "www_s" aceita pedidos de serviços conforme o protocolo HTTP.
No momento está processando apenas o comando "GET", mas já com possibilidade
da retomada de uma transmissão do ponto em que foi interrompida.

.sp
Para utilizar este servidor, deve-se criar uma conta "www"
(veja "edusr" (cmd)), onde o diretório é usado como a raiz da árvore
contendo todos os arquivos que podem ser pedidos através do protocolo;
estes arquivos deve pertencer (naturalmente) à conta "www".

.sp
Ao receber o pedido de um arquivo, "www_s" acrescenta o prefixo dado pelo
diretório acima; se este nome for o de um diretório, é acrescentado o
sufixo "index.html". NÃO são permitidos nomes contendo "..".

.sp
Certifique-se de que há a linha referente a este servidor no arquivo
"/etc/itnetserv" (veja "itnetserv" (fmt)).

.sp 3
O cliente "www" para testes acessa o <nó_remoto>, e tenta obter a
<página> dada.

.sp
Se o <nó_remoto> não for dado, "www" procura uma
cadeia da forma "TNET=node=<nó_remoto>" no ambiente (veja "tnet" (cmd)).

.sp
Se a <página> não for dada, usa o valor "index.html".

.sp
As opções do comando são:

.in +3
.ip --
Normalmente, "www" escreve na tela as linhas HTTP recebidas, e o número
de linhas da <página>.

.ip -p
Imprime a <página> recebida na tela.

.ip -g
Salva a <página> recebida no diretório corrente.

.ip -v
Verboso.

.ip -r
Dá o <intervalo> no formato começo-final (comforme o protocolo HTTP 1/1).

.ip -r
Fornece o <nó_remoto> (ao invés de usar o do TNET do ambiente).

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
edusr, tnet
.br
.wo "(fmt): "
itnetserv
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/itnetserv

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
