.bp
.he 'TEST (cmd)'TROPIX: Manual de Referência'TEST (cmd)'
.fo 'Atualizado em 24.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "test -"
testa propriedades de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
test [-o] [-{rwxfdcblpugkms} <arquivo>] ... [-t <fd>] ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "test" testa diversas propriedades de arquivos,
com a finalidade principal de fornecer um código de retorno
para o comando de programação "if" do "sh" (cmd).
 
.sp
Se a expressão (composta do e-ório ou ou-tório das diversas
propriedades testadas) for verdadeira, "test" retorna "0"
(o que é interpretado como verdadeiro pelo "if" do "sh");
no caso contrário retorna "1" (o que é interpretado como falso).

.sp
As diversas permissões de acesso a arquivos são testadas utilizando-se
os UID/GID efetivos (ver "intro" (sys)). Se não for dado nenhum
teste de arquivo, o retorno de "test" é falso.

.sp
Se o <arquivo> for um elo simbólico, a propriedade será testada
no alvo referenciado, exceto na opção "-l" (ver abaixo).

.sp
As opções do comando são:

.in +5
.ip -o 14
Normalmente, é realizado o e-ório dos diversos testes dos arquivos.
Com esta opção, é feito um ou-tório.
Se usada, esta opção deve ser a primeira.

.ip '-r <arquivo>' 14
Verdadeiro se o <arquivo> existe, e puder ser lido.

.ip '-w <arquivo>' 14
Verdadeiro se o <arquivo> existe, e puder ser escrito.

.ip '-x <arquivo>' 14
Verdadeiro se o <arquivo> existe, e puder ser executado.

.ip '-f <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for regular.

.ip '-d <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for um diretório.

.ip '-c <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for especial de caracteres
(não-estruturado).

.ip '-b <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for especial de blocos
(estruturado).

.ip '-l <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for um elo simbólico.

.ip '-p <arquivo>' 14
Verdadeiro se o <arquivo> existe, e for um FIFO.

.ip '-u <arquivo>' 14
Verdadeiro se o <arquivo> existe, e o bite "set-user-ID" estiver ligado.

.ip '-g <arquivo>' 14
Verdadeiro se o <arquivo> existe, e o bite "set-group-ID" estiver ligado.

.ip '-k <arquivo>' 14
Verdadeiro se o <arquivo> existe, e o bite para guardar
o texto no "swap" estiver ligado.

.ip '-m <arquivo>' 14
Verdadeiro se o <arquivo> existe, e o bite para guardar
o texto na memória estiver ligado.

.ip '-s <arquivo>' 14
Verdadeiro se o <arquivo> existe, e o seu tamanho for maior do que zero.

.ip '-t <fd>' 14
Verdadeiro se o arquivo cujo descritor for <fd> estiver aberto e for um terminal.

.ep
.in -5

.in
.sp
.b OBSERVAÇÃO
.in 5
O teste "-x" apenas consulta os bites correspondentes do modo do arquivo,
não garantindo que o referido arquivo possa realmente ser executado.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(sys): "
intro
.br

.in
.sp
.b EXEMPLOS
.in 5
O comando
.sp
.nf
		test -f alpha -d dir
.fi

.sp
retorna verdadeiro se "alpha" for um arquivo regular E "dir" um diretório. 

.sp
Por outro lado, o comando
.sp
.nf
		test -o -f alpha -d dir
.fi

.sp
retorna verdadeiro se "alpha" for um arquivo regular OU "dir" um diretório. 

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
