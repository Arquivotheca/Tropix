.bp
.he 'EXPORTFS (cmd)'TROPIX: Manual de Referência'EXPORTFS (cmd)'
.fo 'Atualizado em 07.03.06'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "exportfs -"
lista/atualiza a tabela "exports" do NFS
.br

.in
.sp
.b SINTAXE
.in 5
.(l
exportfs [-eduf]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "exportfs" gerencia a tabela de sistemas de arquivos exportados
pelo NFS. O formato desta tabela pode ser visto em "exports" (fmt).

.sp
A tabela é contida no arquivo "/etc/exports", de onde é lida durante a inicialização
do sistema operacional, e copiada para o núcleo. Esta cópia da tabela no núcleo é a que
efetivamente irá decidir quais são os sistemas de arquivos exportados.

.sp
Através do comando "exportfs", podemos alterar dinamicamente a lista dos sistemas exportados.
Além disto, o comando lista as duas tabelas e indica as diferenças entre elas.

.sp
As opções do comando são:

.in +3
.ip --
Sem opções, é listada a tabela "exports" do núcleo
(a tabela efetiva no momento).

.ip -e
Lista a tabela "exports" do arquivo "/etc/exports".

.ip -d
Indica as diferenças entre as duas tabelas.
Linhas adicionadas em "/etc/exports" são indicadas com um prefixo "+";
linhas removidas de "/etc/exports" são indicadas com um prefixo "-" e
linhas modificadas são indicadas com um prefixo "!".

.ip -u
Atualiza a tabela "exports" do núcleo, a partir de "/etc/exports",
isto é, torna "/etc/exports" a tabela efetiva dinamicamente.

.ip -f
Normalmente, é pedida uma confirmação da atualização da tabela "exports"
do núcleo. Com esta opção, esta confirmação é dispensada.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(fmt): "
nfs, exports
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/exports

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
