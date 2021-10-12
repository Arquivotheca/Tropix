.bp 1
.he 'CMD (fmt)'TROPIX: Manual de Referência'CMD (fmt)'
.fo 'Atualizado em 28.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "cmd -"
sintaxe da utilização de comandos

.in
.sp
.b SINTAXE
.in 5
<nome> [-<opção> ...] [<arg> ...]
.sp
.nf
   onde:
.fi

.ip <nome>: 9
nome do comando.

.ip <opção>: 9
sempre precedida de '-'.
Uma <opção> pode ser vir acompanhada de um argumento
(por exemplo "-f new.makefile") ou não
(por exemplo "-v").
Estes argumentos das opções não devem ser confundidos
com os argumentos dos comandos.

.ip <arg>: 9
argumento do comando. Estes argumentos não têm sintaxe definida, e
variam de comando para comando.

.ep
.in
.sp
.b DESCRIÇÃO
.in 5
Abaixo é dada a sintaxe da utilização dos comandos. 
Recomendamos que todos os comandos sigam esta sintaxe para 
a homogeneidade do sistema TROPIX.
Na construção de novos comandos, a função "getopt" (libc)
pode (e deve) ser utilizada para a verificação da sintaxe.

.in +3
.ip 1. 3
As opções devem constar de apenas um caractere.

.ip 2. 3
Todas as opções devem ser precedidas de '-'.

.ip 3. 3
Opções sem argumentos podem ser agrupadas após um único '-'.

.ip 4. 3
O primeiro argumento de uma opção deve vir separado da opção
por brancos.

.ip 5. 3
Argumentos de opções não podem ser opcionais.

.ip 6. 3
Grupos de argumentos de uma opção devem vir separados por
virgulas, ou brancos e neste caso, entre aspas
(por ex. -p 114,abdc,alpha ou -p "114 abcd alpha").

.ip 7. 3
A cadeia "--" pode ser usada para indicar o final das opções.

.ip 8. 3
O caractere '-' precedido e seguido de brancos deve apenas
ser utilizado com o significado de entrada ou saída padrão.

.in -3
.ep
.in
.sp
.b
OBSERVAÇÃO
.r
.in 5
No TROPIX procura-se respeitar ao máximo estas regras definidas
pela AT&T. Ironicamente, o sistema onde estas regras são muito
pouco respeitadas é no próprio UNIX da AT&T.
.br

.(t
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
getopt
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
