.bp
.he 'FINGER (cmd)'TROPIX: Manual de Referência'FINGER (cmd)'
.fo 'Atualizado em 17.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "finger -"
obtém informações sobre usuários remotos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
finger [-v] [-u <usuário_remoto>] <nó_remoto>
finger [-v] [<usuário_remoto>@]<nó_remoto>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "finger" fornece algumas informações sobre usuários de nós
remotos.

.sp
Se o <usuário_remoto> não for dado, imprime os seguintes dados sobre
todos os usuários remotos em sessão:
nome (da conta) do usuário, terminal/vídeo, data/hora em que entrou em sessão,
e o nome do local do nó.
Repare que este formato é semelhante ao do comando "who" (cmd).

.bc	/*************************************/
.sp
Para usuários em sessão, fornece o nome (da conta) do usuário,
o terminal/vídeo, a data/hora em que entrou em sessão e o nome do local
do nó, em um formato semelhante ao comando "who" (cmd).
.ec	/*************************************/

.sp
As opções do comando são:

.in +3
.ip -u
Com esta opção, somente são dadas a informações acima para o
<usuário_remoto> dado.

.sp
Além disto, são dados o nome completo do <usuário_remoto>,
o seu diretório "home", o seu interpretador de comandos (veja "edusr" (cmd)),
a data/hora da sua última entrada em sessão, a data/hora da última
recepção de correio eletrônico e o seu "plano" (conteúdo do arquivo
"$HOME/.plan").

.sp
Esta opção pode ser dada também na forma com o caractere "@" (veja acima).

.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
who, edusr
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"Name/Finger", K. Harrenstein, Request for Comments 742.

.in
.sp
.b ARQUIVOS
.in 5
/etc/passwd
 /usr/mail/*
 /var/adm/lastlog
 /var/adm/utmp
 /var/adm/wtmp
 $HOME/.plan

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
