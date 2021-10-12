.bp 1
.he 'CHGRP (cmd)'TROPIX: Manual de Referência'CHGRP (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "chgrp -"
troca a identificação do grupo (GID) de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
chgrp [-N] <grupo> [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "chgrp" troca a identificação do grupo 
de um ou mais arquivos para o novo <grupo> dado.

.sp
O <grupo> pode ser dado na forma numérica
(conforme as convenções da linguagem "C", ver "strtol" (libc")),
ou na forma do nome de um grupo, encontrado no arquivo "/etc/group".

.sp
Se for utilizada a opção "-N", ou se NÃO forem dados nomes de arquivos,
estes serão lidos da entrada padrão.


.in
.sp
.b
OBSERVAÇÕES
.r
.in 5
Somente o dono do arquivo (ou o superusuário)
pode trocar a identificação do grupo deste arquivo.

.sp
Se a troca é realizada por um usuário que não é
superusuário, os bites S_ISUID ("set user ID") e S_ISGID ("set group ID")
do modo do arquivo são zerados (ver "chmod" (sys)). 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chown, chmod
.br
.wo "(sys): "
chown, chmod
.br
.wo "(libc):"
strtol
.br
.wo "(fmt): "
group, passwd
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/group
 /etc/passwd

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
