.he 'MKDIR (cmd)'TROPIX: Manual de Referência'MKDIR (cmd)'
.fo 'Atualizado em 10.05.02'Versão 4.2.0'Pag. %'
.bp

.b NOME
.in 5
.wo "mkdir -"
cria diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mkdir [-f] <diretório> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mkdir" cria os <diretório>s dados.

.sp
As entradas "." (o próprio diretório) e ".." (o pai do diretório)
são criadas automaticamente.

.sp
A criação de um diretório requer a permissão de escrita no pai
do diretório que se deseja criar.

.sp
As opções do comando são:

.in +3
.ip -f
Cria os diretórios intermediários (se necessário).

.sp
Normalmente, "mkdir" espera que os diretórios intermediários
já existam (por exemplo, no comando "mkdir alpha/beta",
o comando espera que "alpha" já exista).

.sp
Com esta opção,
"mkdir" cria também estes diretórios intermediários faltando.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
mkdir
.br
.wo "(cmd): "
rm, rmdir
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
