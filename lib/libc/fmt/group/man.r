.bp
.he 'GROUP (fmt)'TROPIX: Manual de Referência'GROUP (fmt)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "group -"
formato do arquivo de grupos
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O arquivo "/etc/group" contém informações sobre os
grupos do sistema. Este arquivo
usualmente é utilizado por utilitários para obter o nome simbólico do grupo
a partir do valor numérico.

.sp
O arquivo pode ser editado diretamente pelo administrador do sistema
através de "ed" (cmd) ou "vi" (cmd). O arquivo consiste de uma linha
para cada grupo; cada linha contém os seguintes campos
separados por ":":

.sp 2
.nf
		grupo:senha:gid:membros
.fi

.sp 2
O "grupo" é o nome simbólico do grupo.
Pode ter no máximo 14 caracteres.

.sp
A "senha" é a senha encriptada de acesso ao grupo. No momento
não é utilizada.

.sp
O "gid" é o valor numérico do grupo.

.sp
O campo "membros" contém os nomes simbólicos dos membros
do grupo, isto é, os nomes dos usuários (contas) que pertencem
a este grupo, separados por vírgulas.
No momento, este campo não é utilizado.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ed, vi
.br
.wo "(libc):"
pwent, grent
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/group
 /usr/include/group.h

.in
.sp
.b EXEMPLO
.nf
	sys::0:
	lsi::10:
	tropix::11:
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
