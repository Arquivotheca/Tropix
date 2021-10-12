.bp
.he 'MKNOD (cmd)'TROPIX: Manual de Referência'MKNOD (cmd)'
.fo 'Atualizado em 30.05.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "mknod -"
cria um arquivo especial
.br

.in
.sp
.b SINTAXE
.in 5
.(l
/etc/mknod <nome> <tipo> <major> <minor>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mknod" cria o arquivo especial <nome>.

.sp
O argumento <tipo> dá o tipo do arquivo:
"b" para arquivo especial de blocos (estruturado) ou
"c" para arquivo especial de caracteres (não-estruturado).

.sp
Os argumentos <major> e <minor> dão a identificação numérica
do arquivo especial.

.in
.sp
.b OBSERVAÇÃO
.in 5
Este comando é reservado para o administrador do sistema;
ele poderá obter as identificações numéricas dos arquivos especiais
em determinadas tabelas do sistema operacional.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkfifo
.br
.wo "(sys): "
mknod
.br

.in
.sp
.b EXEMPLO
.in 11
/etc/mknod /dev/tty006 c 4 6
.sp
.in 5
cria o arquivo "/dev/tty006" como um dispositivo especial de 
caracteres com <major> 4 e <minor> 6.


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
