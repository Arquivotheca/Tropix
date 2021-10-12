.bp
.he 'ENVIRON (fmt)'TROPIX: Manual de Referência'ENVIRON (fmt)'
.fo 'Atualizado em 27.01.99'Versão 3.1.6'Pag. %'

.b NOME
.in 5
.wo "environ -"
ambiente do usuário
.br

.in
.sp
.b DESCRIÇÃO
.in 5
Quando um processo é iniciado, ele recebe um vetor de ponteiros
para cadeias chamado de "ambiente" (ou "environment"),
construído pela chamada ao sistema "exec" (sys).
Por convenção, estas cadeias têm a forma "nome=valor".

.sp
Embora cada usuário possa definir seus "nomes" particulares,
existe um conjunto de "nomes" globais,
criado pelos utilitários "init" (cmd)
e "login" (cmd),
ao qual (normalmente) todos os utilitários têm acesso.
.sp

A lista dos "nomes" globais é a seguinte:

.in +3
.ip PATH 8
Contém a sequência de diretórios pesquisados pelas funções
"execvp" e "execlp" (libc) e o utilitário "sh" (cmd)
para a busca dos arquivos que contêm os utilitários.
Os diretórios são separados por ":".
O valor inicial de "PATH", atribuído pelo utilitário "login" 
é "PATH=.:/bin:/usr/bin".

.ip HOME 8
Contém o nome do diretório inicial do usuário, atribuído por
"login" a partir do arquivo de contas e senhas "passwd" (fmt).

.ip TERM 8
Contém o tipo do terminal, atribuído por "init", a partir do arquivo
"/etc/initab".
Esta informação pode ser utilizada pelos utilitários
(como por ex. "vi" (cmd)), para obter as características
peculiares de cada terminal.

.ip TZMIN 8
Esta variável NÃO está mais no ambiente; o fuso horário agora
é obtido através da chamada ao sistema "gettzmin" (sys).

.ip USER 8
Contém o nome do usuário, como dado ao entrar em sessão.

.ip SHELL 8
Contém o nome do utilitário utilizado como interpretador
de comandos pelo usuário.

.in -3
.ep
.in 5
.sp
Nomes adicionais podem ser colocados no ambiente através
de "putenv" (libc) e "exec" (sys), ou então através
dos comandos "set" e "export" do "sh" (cmd). 
Não é recomendável utilizar "nomes" que entrem em conflito
com os nomes das variáveis do "sh", tais como "PS1", "PS2", "IFS".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
init, login, sh, vi
.br
.wo "(sys): "
exec, gettzmin
.br
.wo "(libc):"
getenv, putenv
.br
.wo "(fmt): "
passwd
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/passwd
 /etc/initab
 /etc/globalenv

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
