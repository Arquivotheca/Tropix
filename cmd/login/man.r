.bp 1
.he 'LOGIN (cmd)'TROPIX: Manual de Referência'LOGIN (cmd)'
.fo 'escrito em 22.05.97'Versão 3.0.3'Pag. %'

.b NOME
.in 5
login - entra em sessão

.in
.sp
.b SINTAXE
.in 5
login

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "login" é utilizado no inicio de cada sessão no
terminal/video/janela,
permitindo a identificação do usuário.

.sp
O comando pede o nome do usuário após "LOGIN: ", e caso 
a conta do usuário tenha senha, esta é pedida, tomando-se
o cuidado de evitar o echo dos caracteres no terminal.
.bc	/*************************************/
No caso do usuário ser superusuário, o comando pede que
a senha seja dada novamente.
.ec	/*************************************/

.sp
Após um "login" com sucesso,  diversos arquivos de contabilidade
são atualizados, é indicada a existência de correspondência
para o usuário, e é impressa a "mensagem do dia", caso esta exista.

.sp
São tambem atualizadas as identificações do usuário, o diretório
corrente, e o ambiente.

.sp
Para sair de sessão, basta teclar um
caractere "eof" (normalmente <^D>, ver "stty" (cmd)), ou
o comando "login", em ambos casos transferindo o controle para o
"init", que irá invocar novamente um "login".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
(cmd): init, mail, passwd.

.in
.sp
.b ARQUIVOS
.nf
     /etc/utmp           Contabilidade
     /usr/adm/wtmp       Contabilidade
     /usr/adm/lastlog	 Contabilidade
     /usr/mail/*         Correspondência
     /etc/motd           Mensagem do Dia
     /etc/passwd	 Contas & Senhas
     /etc/initab         Tabela de Terminais 
     /bin/sh		 Interpretador de comandos
     /dev/video*	 Videos
     /dev/tty*		 Terminais
     /dev/pty*		 Pseudo-terminais
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
