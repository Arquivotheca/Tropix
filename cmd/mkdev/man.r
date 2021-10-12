.bp
.he 'MKDEV (cmd)'TROPIX: Manual de Referência'MKDEV (cmd)'
.fo 'Atualizado em 30.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "mkdev -"
cria/atualiza os dispositivos de "/dev"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mkdev [-wufasv]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mkdev" cria e/ou atualiza os dispositivos de "/dev"
(ou "/aroot/dev", veja a opção "-a")
para que "/dev" fique consistente com a tabela de partições e
a tabela de dispositivos do núcleo do TROPIX.

.sp
Os dispositivos envolvidos
nestas operações são todos os dispositivos especiais estruturados
(de blocos) e não-estruturados (de caracteres) associados a discos rígidos,
discos removíveis (com as suas respectivas partições), além
de todos os outros dispositivos não-estruturados
tais como linhas seriais, telas de modo texto, pseudo terminais,
dispositivos "ethernet", etc ...

.bc	/*************************************/
Em outras palavras, são os dispositivos com os nomes
"/dev/hd*", "/dev/rhd*", "/dev/sd*" e "/dev/rsd*".
.ec	/*************************************/

.sp
Além disto, são atualizados os dispositivos "/dev/root" e "/dev/swap".

.sp
Também são atualizados sinônimos de dispositivos (isto é, dispositivos
que são elos físicos de dispositivos da tabela de partições).
Assim sendo, é mais interessante criar sinônimos para
partições usando o comando "ln" ao invés de criar novos dispositivos
através do comando "mknod".

.sp
O comando também verifica se há dispositivos "espúrios", ou seja,
dispositivos presentes em "/dev", mas não mais presentes
na tabela de partições nem na tabela de dispositivos do núcleo do TROPIX.

.sp
Normalmente, "mkdev" apenas indica as inconsistências, sem alterar "/dev".

.sp
As opções do comando são:

.in +3
.ip -w
Modifica "/dev", criando/atualizando/removendo os dispositivos em "/dev"
para que reflitam corretamente a tabela de partições/tabela de dispositivos.

.ip -u
Sinônimo de "-w".

.ip -f
O comando normalmente pede permissão para cada modificação em "/dev"
a realizar. Com esta opção, o usuário dá permissão a "mkdev" a realizar
todas as modificações necessárias sem outras consultas.

.ip -a
Cria e/ou atualiza os dispositivos de "/aroot/dev" ao invés de "/dev".

.ip -s
Modo silencioso. Não escreve as inconsistências encontradas.
Implica "-f".

.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Este comando somente pode ser executado pelo superusuário.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
prdisktb, ln, mknod
.br
.wo "(fmt): "
parttb
.br

.in
.sp
.(t
.b ARQUIVOS
.in 5
/dev/*
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
