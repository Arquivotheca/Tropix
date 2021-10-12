.bp
.he 'WHO (cmd)'TROPIX: Manual de Referência'WHO (cmd)'
.fo 'Atualizado em 14.07.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "who -"
informa quem está no sistema
.br

.in
.sp
.b SINTAXE
.in 5
.(l
who [-i] [<arquivo>]
who am I
who am i
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "who", em sua primeira forma, imprime uma linha para cada usuário
corrente do sistema, contendo: o nome do usuário,
nome do seu terminal (linha de comunicações), data/hora quando entrou
em sessão, estado do terminal e local/telefone onde ele está localizado.
Se for uma sessão remota, ao invés do local/telefone é indicado o
nome do nó remoto entre parenteses.

.sp
O estado de um terminal indica se ele está habilitado a receber mensagens
de outros usuários ou não (veja "to", "write", "mesg" (cmd)).
Um "+" indica que está habilitado e um "-" indica que não está
(um superusuário pode enviar mensagens para qualquer terminal,
habilitado ou não).

.sp
Na forma "who am I" (ou "who am i"), imprime apenas uma linha,
referente ao próprio usuário que está utilizando o comando.

.sp
Se for dado um <arquivo>, ao invés de imprimir informações sobre os
usuários correntes (obtidas do arquivo "/etc/utmp"), o comando "who" imprime as
informações obtidas deste <arquivo> dado. O formato do <arquivo>
deve ser o mesmo de "/etc/utmp" - pode ser dado (por exemplo)
"/usr/adm/wtmp", que contém um histórico das entradas/saídas em/de sessão.

.sp
As opções do comando são:

.in +3
.ip -i
Imprime também o tempo inativo de cada terminal.
Este tempo contém o número de horas e minutos transcorridos desde a
última atividade no terminal. Um ponto (".") indica que houve
atividade durante o último minuto, e portanto é "corrente".
Se não houve atividade nas últimas 24 horas, isto é indicado por "OLD".
Esta opção pode ser usada apenas por superusuários.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, to, write, mesg, rlogin, telnet
.br

.in
.sp
.b ARQUIVOS
.in 5
.(l
/etc/utmp
/etc/initab
/usr/adm/wtmp
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
