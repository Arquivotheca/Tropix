.bp
.he 'DF (cmd)'TROPIX: Manual de Referência'DF (cmd)'
.fo 'Atualizado em 10.09.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "df -"
informa sobre o espaço disponível de sistemas de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
df [-kbi] [<nome> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "df" dá informações sobre o espaço total, ocupado e livre 
de sistemas de arquivos (montados ou não).
Para sistemas desmontados, "df" somente fornece dados para os
tipos T1 e V7.

.sp
De acordo com as opções (ver abaixo), "df" dá informações sobre
a região do sistema de arquivos destinada a blocos de arquivos ou
sobre a região destinada a nós-índice.

.sp
Cada sistema de arquivos é dado através de um <nome>, que pode ser

.in +3
.ip 1. 3
um dispositivo: o arquivo especial de blocos ou de caracteres que contém
o sistema de arquivos (por exemplo, "/dev/rwe2");

.ip 2. 3
o diretório de montagem do sistema de arquivos (por exemplo, "/u");

.ip 3. 3
um outro arquivo qualquer (não especial) contido no sistema de arquivos
montado.

.ep
.in -3

.sp
As opções do comando são:

.in +3
.ip --
Sem opções, são dados o espaço total, ocupado e livre em MB (além da
taxa de ocupação em \%) da região do sistema de arquivos destinada
a blocos de arquivos.

.ip -k
Como acima, em KB.

.ip -b
Como acima, em blocos de 512 bytes.

.ip -i
São dados o número total, ocupado e livre de nós-índice (além da
taxa de ocupação em \%) do sistema de arquivos.

.ep
.in -3

.sp
Se não for dado nenhum <nome>, o comando informa sobre todos os sistemas
de arquivos montados, incluindo a raiz do sistema.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mount
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/mtab

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
