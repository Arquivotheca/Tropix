.bp
.he 'CRYPT (man)'TROPIX: Manual de Referência'CRYPT (man)'
.fo 'Atualizado em 20.04.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "crypt -"
codifica/decodifica arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
crypt [-c] [<entrada> [<saída>]]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "crypt" lê o arquivo <entrada>, codifica-o/decodifica-o
de acordo com certa transformação,
e coloca a versão transformada no arquivo <saída>.

.sp
Se <saída> não for dada, a versão transformada é escrita na saída padrão.
Se além disto, a <entrada> não for dada, o arquivo original
é lido da entrada padrão.

.sp
A transformação é determinada por uma <chave> e uma <complexidade>.
A chave é sempre pedida pelo terminal. A <complexidade> é normalmente
48, a não ser que seja dada a opção "-c", quando então o seu valor
é também pedido pelo terminal.

.sp
A transformação que codifica um certo arquivo é a mesma que o irá
decodificar posteriormente; a seqüência de comandos

.sp
.(l
		crypt original transformado
		crypt transformado
.)l
.sp
irá escrever o arquivo "original" no terminal (desde naturalmente,
que a mesma <chave> seja dada para os dois comandos).

.in
.sp
.b OBSERVAÇÕES
.in 5
Quanto maior a <complexidade>, maior será o tempo necessário para
a transformação, o que irá dificultar a decodificação por pessoas
não autorizadas.

.sp
Para maior segurança, não utilize <chave>s com menos de 5 caracteres.

.sp
Se dois ou mais arquivos codificados com a mesma chave forem
concatenados, e este resultado for decodificado, apenas o primeiro
arquivo será decodificado corretamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
getpass
.br

.in
.sp
.b
REFERÊNCIA
.r
.in 5
Reeds, J.A. & Weinberger, P.J., "File Security and the
Unix System Crypt Command", AT&T Bell Laboratories
Technical Journal, Vol. 63, No. 8, Out. 1984.

.in
.sp
.b ARQUIVOS
.in 5
/dev/tty

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in
