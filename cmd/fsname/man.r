.bp
.he 'FSNAME (cmd)'TROPIX: Manual de Referência'FSNAME (cmd)'
.fo 'Atualizado em 10.09.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "fsname -"
consulta/atualiza nomes de sistemas de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
fsname   [-t]  <dispositivo> ...
fsname -s [-f] <dispositivo> <nome> [<volume>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Cada sistema de arquivos T1 e V7 contém 2 nomes: o nome do próprio sistema de
arquivos ("nome"), e o nome do volume que o contém ("volume").
O comando "fsname" permite consultar e/ou atualizar
os nomes/volumes de sistemas de arquivos.
Estes nomes são também manipulados por outros comandos,
tais como "mkfs", "fsck" e "mount" (cmd).

.sp
A função do programa depende da opção dada:

.in +3
.ip -t
Imprime um tabela contendo os tipos (T1 ou V2), nomes e volumes
dos <dispositivo>s dados. Esta opção é assumida, se não for dada nenhuma opção.

.sp
Se "fsname" não conseguir identificar o sistema de arquivo, imprime
apenas um ponto (".") para o tipo.
Para <dispositivo>s montados, são reconhecidos outros tipos de sistemas
de arquivos. 

.ip -s
Altera o "nome" e (opcionalmente) o "volume" do sistema de arquivos
residente em <dispositivo> para <nome> e <volume>.

.bc	/*************************************/
Além disto, o "número mágico", que valida um sistema
de arquivos é verificado, e o usuário tem chance de corrigí-lo,
caso esteja errado.
.ec	/*************************************/

.sp
Se para o <nome> e/ou <volume> forem dados a cadeia "-", será atribuído
um valor padrão: para o "nome" a cadeia vazia, e para o "volume" o nome
do dispositivo contido na tabela de partições.

.sp
Se para o <nome> e/ou <volume> forem dados a cadeia ".", ou ainda se não
forem dados, os seus valores não serão alterados.

.ip -f
Normalmente, é pedida uma confirmação do usuário para alterar
o nome/volume do sistema de arquivos. Com esta opção, a confirmação
é dispensada.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Este comando é reservado para o administrador
do sistema, e não deve ser utilizado em sistemas de arquivos
montados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkfs, fsck, mount
.br
.wo "(sys): "
ustat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
