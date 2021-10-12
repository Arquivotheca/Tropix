.bp
.he 'WRITE (cmd)'TROPIX: Manual de Referência'WRITE (cmd)'
.fo 'Atualizado em 14.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "write -"
escreve mensagens para um outro usuário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
write <usuário> [<tty>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "write" copia linhas do seu terminal para o terminal do
<usuário>, com a finalidade de estabelecer uma comunicação entre dois usuários.

.sp
Ao iniciar, "write" envia a mensagem

.sp
.nf
	MENSAGEM de "seu nome" ("seu terminal"):
.fi

.sp
Se a conexão é completada com sucesso, é escrita a mensagem

.sp
.nf
	Conectado com o usuário "<usuário>" no terminal "<tty>"
.fi

.sp
no seu terminal, além de soar a campainha,
para indicar que as mensagens digitadas estão sendo enviadas.

.sp
Neste ponto, o <usuário> deve utilizar o comando "write" para estabelecer
uma comunicação no sentido inverso.

.sp
A comunicação continua até que seja lido um EOF (normalmente <^D>,
ver "stty" (cmd)), seja enviado um sinal de interrupção
("SIGINT", normalmente <^C>, ver "stty"),
ou o <usuário> execute o comando "mesg -n" (ver "mesg" (cmd)).
Neste ponto, "write" envia a mensagem

.sp
.nf
	FIM da mensagem de "seu nome" ("seu terminal")
.fi

.sp
Se o <usuário> está em sessão em mais de um terminal, pode ser usado
o argumento <tty> para indicar para qual dos terminais devem ser enviadas
as mensagens.  Se isto não for feito, as mensagens serão enviadas para
a primeira entrada do <usuário> no arquivo "/etc/utmp" (conforme pode
ser visto através de "who" (cmd)) .

.sp
O comando "write" obedece às permissões atribuídas pelo comando "mesg"
(cmd), inclusive para superusuários.

.sp
Se o primeiro caractere da linha de uma mensagem for "!", a linha
será interpretada como sendo uma linha de comando, e executada através
do "sh" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mesg, stty, mail, who
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
