.bp
.he 'NOHUP (cmd)'TROPIX: Manual de Referência'NOHUP (cmd)'
.fo 'Atualizado em 15.04.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "nohup -"
executa um comando imune ao sinal SIGHUP
.br

.in
.sp
.b SINTAXE
.in 5
.(l
nohup <comando> [<opções>] [<argumentos>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "nohup" executa o <comando> ignorando o sinal SIGHUP.
O objetivo é evitar o término do <comando> quando o usuário sai de sessão
(isto é, quando termina o "login" (cmd), "rlogin" (cmd) ou "telnet" (cmd)).

.sp
Se as saídas padrão (normal e de erro) não foram rediredionadas, elas são
adicionadas ao final do arquivo "nohup.out". Se este arquivo não puder
ser escrito no diretório corrente, a saída é posta no arquivo "$HOME/nohup.out".

.in
.sp
.b OBSERVAÇÕES
.in 5
No caso de:

.sp
.(l
	nohup <comando1> ; <comando2>
.)l

.sp
a ação de "nohup" aplica-se apenas ao <comando1>.

.sp
Para estes casos é recomendado criar um arquivo contendo a seqüência de comandos,
e executá-la através de "sh" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, login, rlogin, telnet
.br
.wo "(sys): "
signal
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in







.bc	/*************************************/
.bp
.he 'NICE (cmd)'TROPIX: Manual de Referência'NICE (cmd)'
.fo Éscrito em 30.7.87'Versão 1.0'Pag. %'

.b NOME
.in 5
.wo "nice -"
executa um programa com prioridade alterada
.br

.in
.sp
.b SINTAXE
.in 5
nice [-p [+]incr] comando [argumentos]

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "nice" executa o "comando" com a prioridade alterada,
de acordo com o argumento "incr" (que normalmente é considerado
como um número negativo, isto é, um decremento de prioridade). 
.sp
As prioridades vão de -20 (mínima) a +20 (máxima). A prioridade
normal de um programa executado diretamente (isto é, sem o comando
"nice") é 0.
.sp
Utilizando o "nice" sem o argumento "incr", "comando" será executado
com prioridade -10. Se for dado "incr", ele será utilizado como
prioridade para o "comando".
.sp
Para o caso de superusuários, podemos utilizar o prefixo "+"
ao incremento, e neste caso ele será considerado positivo.

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que tanto no comando "nice" como no "ps", utilizamos a idéia
intuitiva de utilizar números positivos (ou maiores) para prioridades
melhores (que melhoram a escalação do processo).
Isto não é o caso da chamada ao sistema "nice" cuja convenção
é a inversa.
.sp
Através do comando "ps" podemos monitorar a prioridade do processo
em questão.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, ps
.br
.wo "(sys): "
nice
.br

.in
.sp
.b EXEMPLOS
.in 7
.sp
.nf
	          nice -p 5 sroff arq.r >arq &

     formata o arquivo "arg.r" em "background" com prioridade -5.
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
.ec	/*************************************/
