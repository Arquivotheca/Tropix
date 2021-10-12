.bp
.he 'PASSWD (fmt)'TROPIX: Manual de Referência'PASSWD (fmt)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "passwd -"
formato do arquivo de contas/senhas
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O arquivo "/etc/passwd" contém informações sobre os
usuários do sistema, utilizadas principalmente pelo utilitário
"login" (cmd) para verificar a permissão do acesso de um usuário ao
sistema.

.sp
O arquivo pode ser editado diretamente pelo administrador do sistema
através de "ed" (cmd) ou "vi" (cmd). O arquivo consiste de uma linha
para cada usuário; cada linha contém os seguintes campos
separados por ":":

.sp 2
.nf
	conta:senha:nome:uid:gid:cota:expir:tranca:dir:sh
.fi

.sp 2
A "conta" é o nome pelo qual o usuário é conhecido pelo
sistema. Pode ter no máximo 14 caracteres.

.sp
A "senha" é a senha encriptada, através da qual é conferida
a permissão de acesso do usuário no sistema. Este campo não
deve ser preenchido pelo administrador; ele deve colocar a
cadeia "-------------" (uma sequência de 13 menos), o que
significa "sem senha", e posteriormente utilizar o utilitário
"passwd" (cmd) para colocar uma senha. 

.sp
O "nome" é um campo utilizado como comentário; o sistema
não consulta este campo. Normalmente é utilizado para conter
o nome completo do usuário.

.sp
O "uid" e "gid" são os valores numéricos do usuário
e seu grupo, que irão ser utilizados na criação de arquivos,
verificação de permissão de acesso, etc....

.sp
Os campos "cota", "expir" e "tranca" são campos reservados
para controle do número máximo de blocos que o usuário
pode ter em disco, a data de expiração da conta e um
código de "tranca" que o utilitário "login" poderá verificar
para permitir ou não o acesso do usuário ao sistema
(não implementado no momento).

.sp
O campo "dir" contém o nome completo (caminho) do diretório
inicial do usuário (parâmetro "HOME" do ambiente (fmt)).

.sp
O campo "sh" é o nome completo do utilitário utilizado como
interpretador de comandos pelo usuário. Normalmente, este campo
está vazio, indicando o utilitário "/bin/sh" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, sh, ed, vi, passwd
.br
.wo "(libc):"
pwent, grent
.br
.wo "(fmt): "
environ
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/passwd
 /usr/include/pwd.h

.in
.sp
.b EXEMPLO
.nf
	ncd.josé:Y4LzoX0ZAE5r1:José Farias:104:35:0:0::/u/josé:
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
