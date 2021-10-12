.bp
.he 'POP3 (cmd)'TROPIX: Manual de Referência'POP3 (cmd)'
.fo 'Atualizado em 01.01.03'Versão 4.4.0'Pag. %'

.b NOME
.in 5
.sp
.wo "pop3 -"
acessa uma caixa postal remota
.br

.in
.sp
.b SINTAXE
.in 5
.(l
pop3 [-av] [<usuário_remoto>@][<nó_remoto>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "pop3" permite o acesso a uma caixa postal remota, através
do protocolo POP3 (Post Office Protocol, Version 3).

.sp
Após a conexão com o <nó_remoto>, o programa "pop3" verifica se há
cartas para o <usuário_remoto> na caixa postal remota, que ainda não estejam
presentes na caixa postal local (isto é, cartas "novas").
Se houverem cartas "novas" o usuário é consultado para decidir se deseja copiá-las
para a caixa postal local.

.sp
Para cada carta "nova", o usuário é consultado para saber se deseja copiá-la para
a caixa postal local, e em seguida, remove-la da caixa postal remota.
As respostas seguem as convenções da função "askyesno" (libc) no modo "de força",
isto é, letras minúsculas decidem ações sobre uma carta em particular e
letras maiúsculas decidem ações sobre todas as cartas seguintes. As ações
das cópias e remoções são independentes.

.sp
As cartas, uma vez copiadas para a caixa postal local, podem ser lidas através
do programa "mail" (cmd).

.sp
As opções do comando são:

.in +3
.ip -a
Normalmente, "pop3" apenas oferece para a remoção as cartas novas da caixa
postal remota. Com a opção "-a", todas as cartas da caixa postal remota
são oferecidas para remoção. Também neste caso são seguidas as
convenções da função "askyesno" (libc) no modo "de força".

.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b 'ARGUMENTOS NO AMBIENTE'
.in 5
Para simplificar o uso do programa, o nome do <usuário_remoto> e/ou do <nó_remoto>
podem ser dados no ambiente (fmt).
A cadeia do ambiente deve ter uma das formas

.sp
.nf
		POP3=<usuário_remoto>@<nó_remoto>
		POP3=<nó_remoto>
.fi

.sp
O nome do <nó_remoto> é procurado seguindo a ordem: 
argumento da linha de execução do programa,
ambiente (cadeia "POP3").

.sp
O nome do <usuário_remoto> é procurado seguindo a ordem: 
argumento da linha de execução do programa,
ambiente (cadeia "POP3").

.sp
O nome do <usuário_local> é procurado seguindo a ordem: 
ambiente (cadeia "USER"), 
nome dado ao entrar na conta (conforme o programa "who" (cmd)),
nome obtido através da identificação numérica do usuário (UID).

.in
.sp
.(t
.b OBSERVAÇÕES
.in 5
O programa "pop3" espera que o servidor do <nó_remoto> processe
o comando "UIDL", para permitir a identificação das cartas.
Se isto não ocorrer, ele supõe que tôdas as cartas remotas
são novas. Neste caso, uma estrategia aconselhável é a de sempre copiar
tôdas as cartas para a caixa postal local, removendo as cartas
da caixa postal remota.
.)t

.sp
O servidor "pop3" processa os seguintes comandos:
"HELP", "USER", "PASS", "QUIT", "NOOP", "LIST", "UIDL",
"STAT", "TOP", "LAST", "RETR", "DELE" e "RSET".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mail, who
.br
.wo "(libc):"
askyesno
.br
.wo "(fmt): "
ambiente
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
 J. Myers & M. Rose: "Post Office Protocol - version 3", RFC 1725, Novembro de 1994.

.in
.sp
.b ARQUIVOS
.in 5
 /var/spool/mail/$USER/l.*.t
 /var/spool/mail/$USER/status

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in




.bc	/*************************************/
.bp
.he 'POP3 (cmd)'TROPIX: Manual de Referência'POP3 (cmd)'
.fo 'Atualizado em 21.08.98'Versão 1.0.0'Pag. %'

.b NOME
.in 5
.wo "pop3_s -"
envia as cartas do correio eletrônico segundo o protocolo POP3
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O servidor pop3_s permite o processamento remoto de correspondência,
que é manipulada segundo o protocolo POP3, tornando simples o
acesso remoto através de programas de correio eletrônico que executem
em estações de trabalho não UNIX.

.sp
As opções do comando são:

.ip 'HELP [<cadeia>]' 24
Fornece informações sobre os comandos.

.ip 'USER <nome do usuário>' 24
Especifica o nome do usuário.

.ip 'PASS <senha>' 24
Fornece a senha.

.ip QUIT 24
Encerra o serviço do servidor.

.ip NOOP 24
Apenas responde +OK

.ip 'LIST [<num.mensagem>]' 24
Fornece tamanho de uma mensagem (ou de todas).

.ip STAT 24
Fornece número de mensagens e tamanho total.

.ip 'TOP  <num.mensagem> [<num.linhas>]' 24
Fornece o cabecalho e as primeiras linhas de uma mensagem.

.ip LAST 24
Informa o número da última mensagem processada.

.ip 'RETR <nome arquivo>' 24
Obtém o conteúdo de uma mensagem.

.ip 'DELE <Num.mensagem>' 24
Remove uma mensagem.

.ip RSET 24
Desapaga todas as mensagens.

.ep
.in -3

.in
.sp
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mail
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
Rose, M: "Post Office Protocol - version 3", RFC 1225, Maio de 1991.
.)t

.in
.sp
.(t
.b ARQUIVOS
.in 5
/usr/spool/mail/$USER/l.*.a
.br
/usr/spool/mail/$USER/status
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
.ec	/*************************************/
