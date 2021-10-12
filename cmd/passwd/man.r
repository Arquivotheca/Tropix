.bp
.he 'PASSWD (cmd)'TROPIX: Manual de Referência'PASSWD (cmd)'
.fo 'Atualizado em 03.06.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "passwd -"
altera a senha do usuário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
passwd [<conta>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "passwd" instala ou altera uma senha associada ao usuário <conta>.
A senha controla o acesso de um usuário ao sistema (veja "login" (cmd)).

.sp
Usuários regulares (isto é, que não são "superusuários", veja "intro" (sys))
podem instalar/alterar apenas a própria senha, caso em que a <conta>
não precisa ser dada.

.sp
Por questões de segurança, as senhas devem satisfazer as seguintes
regras de formação:

.in +5
.ip 1. 3
Cada senha deve ser composta de caracteres do código ISO exceto <nl> e <cr>.
O caractere "kill" (normalmente <^U>) também deve ser evitado
(veja "getpass" (libc)). Apenas são considerados os primeiros 14 caracteres.

.ip 2. 3
Caso a senha contenha letras, algarismos e caracteres especiais,
ela deve ter no mínimo 6 caracteres.

.ip 3. 3
Caso a senha contenha apenas letras e algarismos,
ela deve ter no mínimo 7 caracteres.

.ip 4. 3
Caso a senha contenha apenas letras, ela deve ter no mínimo 8 caracteres.

.ip 5. 3
Caso a senha contenha apenas algarismos, ela deve ter no mínimo 11 caracteres.

.ep
.in -5

.sp
Antes de alterar uma senha, o comando "passwd" pede e confere a senha antiga.
A seguir, é pedida a senha nova duas vezes; em caso de divergência
entre as duas versões, ou se a senha não segue as regras
dadas acima, a senha nova é pedida novamente (até um máximo de 4 vezes).

.sp
Usuários regulares (mas não superusuários) podem possuir uma senha nula
(isto é, podem não necessitar de uma senha para entrar em sessão) teclando
apenas <nl> ou <cr> para as duas versões da senha nova.

.in
.sp
.b OBSERVAÇÕES
.in 5
O comando "passwd" não dispensa os superusuários das
regras de formação das senhas.

.sp
O TROPIX não utiliza a técnica de envelhecimento de senhas
por ter sido considerada inócua para a segurança do sistema.

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login
.br
.wo "(sys): "
intro
.br
.wo "(libc):"
crypt, getpass
.br
.wo "(fmt): "
passwd
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
/etc/passwd

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
