.bp
.he 'MESG (cmd)'TROPIX: Manual de Referência'MESG (cmd)'
.fo 'Atualizado em 23.05.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "mesg -"
habilita ou desabilita a recepção de mensagens
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mesg [-nNsSyY]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mesg" habilita ou desabilita o terminal do usuário a receber
mensagens de outros usuários (por exemplo através de "write" ou "to" (cmd)).   

.sp
Sem opções, o comando imprime o estado corrente do terminal.
 
.sp
As opções do comando são:

.in +3
.ip -n
Desabilita a recepção (pode também ser dado "-N").

.ip -s
Habilita a recepção (pode também ser dado "-S", "-y" ou "-Y").

.ep
.in -3

.in
.sp
.b
CÓDIGO DE RETORNO
.r
.in 5
O comando retorna 0 se o terminal está habilitado, 1 se está desabilitado,
e 2 para o caso de erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
to, write
.br

.in
.sp
.b ARQUIVOS
.in 5
 /dev/console
 /dev/tty*

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
