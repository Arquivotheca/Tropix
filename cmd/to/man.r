.bp
.he 'TO (cmd)'TROPIX: Manual de Referência'TO (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "to -"
escreve uma linha de mensagem para um usuário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
to <usuário> <mensagem>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "to" envia a <mensagem> curta para o <usuário>.

.sp
Se o <usuário> estiver em sessão em vários terminais,
a <mensagem> será enviada para todos eles. 

.sp
Se o nome do <usuário> for "all",
a <mensagem> será enviada para todos os usuários em sessão,
em todos os terminais.

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "to" envia apenas mensagens de uma linha.
Para enviar mensagens maiores, deve-se utilizar o comando "write" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
write, mail
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/utmp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
