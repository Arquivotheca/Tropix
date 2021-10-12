.bp
.he 'SU (cmd)'TROPIX: Manual de Referência'SU (cmd)'
.fo 'Atualizado em 11.07.97'Versão 3.0.2'Pag. %'

.b NOME
.in 5
.wo "su -"
muda de conta temporariamente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
su [-e] [<conta nova>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "su" muda temporariamente de conta (identificação do usuário)
para a "<conta nova>".
No caso de a <conta nova> não ser dada, assume-se "root", ou
seja, "superusuário".

.sp
O comando "su" pede a senha necessária (a não ser que o usuário de "su"
seja um superusuário) e se esta for dada corretamente, troca para
a conta especificada, executando o seu interpretador de comandos
(normalmente "sh" (cmd)).

.sp
Normalmente, nem o "ambiente" nem o diretório corrente são modificados
(veja a opção "-e").

.sp
O usuário permanece nesta nova conta até que o "sh" termine
(com "exit" ou EOF, normalmente <^D>), ou então seja executado outro "su".

.sp
As opções do comando são:

.in +3
.ip -e
Utiliza o ambiente da <conta nova>: o diretório corrente é trocado
para o diretório "HOME" da <conta nova>, e é executado o seu ".profile".
O novo ambiente será um outório dos ambientes antigo e novo, onde
as variáveis em comum terão os valores do ambiente novo.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, sh
.br
.wo "(fmt): "
environ, passwd
.br

.in
.sp
.b ARQUIVOS
.in 5
.(l
/etc/passwd		arquivo de contas e senhas
$HOME/.profile		"profile" do usuário
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
