.bp
.he 'FTP (cmd)'TROPIX: Manual de Referência'FTP (cmd)'
.fo 'Atualizado em 03.03.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "ftp -"
programa de cópia remota de arquivos da INTERNET
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ftp [-vn] <nó_remoto>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "ftp" é a interface do usuário para o protocolo padrão de
cópias de arquivos da INTERNET. O programa permite a cópia de arquivos
de/para um computador remoto de uma rêde.

.sp
Inicialmente, "ftp" obtém uma conexão com o servidor "ftp" do
<nó_remoto> e em seguida executa a seqüência de "login",
pedindo a conta/senha do usuário no <nó_remoto> (veja a opção "-n").
Finalmente, "ftp" passa o controle para o interpretador de comandos,
aguardando instruções do usuário.
Para indicar que está aguardando comandos, "ftp" imprime uma linha
contendo os caracteres "ftp> ".

.sp
As opções do programa são:

.in +3
.ip -v
Verboso: imprime as estatísticas sobre as transferências de dados,
além de outras informações.

.ip -n
Não executa automaticamente a seqüência de "login", passando diretamente o
contrôle para o interpretador de comandos. Repare no entanto, que a maioria
dos comandos só podem ser executados com um usuário em sessão. Para entrar
em sessão mais tarde, deve ser usado o comando "user".

.ep
.in -3

.sp
O nome do <nó_remoto> e/ou o nome do usuário podem ser pré-definidos no
ambiente (veja "environ" (fmt)) através da variável "FTP",
cujo valor deve ter a forma:

.sp
.(l
	node=<nó_remoto>,user=<nome_do_usuário>
.)l

.in
.sp
.b 'PARÂMETROS DE TRANSFERÊNCIA DE DADOS'
.in 5
O protocolo FTP define uma série de parâmetros que definem como a
transferência dos dados de um arquivo será realizada.

.sp
O tipo de representação dados pode ser ASCII, EBCDIC, IMAGEM ou LOCAL.
Esta implementação suporta apenas os tipos ASCII e IMAGEM.
O tipo ASCII é também chamado de ISO porque são conservados os 8 bits.

.sp
O contrôle de formato pode ser NON-PRINT, TELNET ou ASA.
Esta implementação suporta apenas o formato NON-PRINT.

.sp
A estrutura de dados pode ser FILE, RECORD ou PAGE.
Esta implementação suporta apenas a estrutura FILE.

.sp
O modo de transmissão de dados pode ser STREAM, BLOCK ou COMPRESSED.
Esta implementação suporta apenas o modo STREAM.

.sp
A conexão de dados é normalmente aberta no modo "ativo", isto é, com
o servidor executando o "connect". Isto pode ser alterado através do comando
interno "passive" (veja abaixo).

.in
.sp
.b COMANDOS
.in 5
O programa "ftp" reconhece os seguintes comandos:

.in +3
.ip " 1." 4
? [-s] [<comando>]
.sp
Como "help", abaixo.

.ip " 2." 4
!<comando> [<argumento> ...]
.sp
Executa o <comando> com os respectivos <argumento>s no nó local.
Isto vale inclusive para o comando "cd".

.ip " 3." 4
ascii
.sp
Altera o tipo da representação de dados para ISO (ASCII).

.ip " 4." 4
bin
.sp
Altera o tipo da representação de dados para IMAGEM.

.ip " 5." 4
binary
.sp
Altera o tipo da representação de dados para IMAGEM.

.ip " 6." 4
bye
.sp
Encerra o programa. Um EOF (normalmente <^D>) tem o mesmo efeito.

.ip " 7." 4
cd [<diretório_remoto>]
.sp
Altera o diretório corrente do nó remoto para <diretório_remoto>.
Se o <diretório_remoto> não for dado, será usado o diretório "HOME"
do usuário remoto.

.ip " 8." 4
cdup
.sp
Altera o diretório corrente do nó remoto para o pai do diretório
corrente atual.

.ip " 9." 4
cmp <arquivo_local> [<arquivo_remoto>]
.sp
Compara um arquivo do nó local com um arquivo do nó remoto. Se o nome
do <arquivo_remoto> não for dado, será igual ao nome do <arquivo_local>.

.ip 10. 4
delete <arquivo_remoto>
.sp
Remove o <arquivo_remoto>.

.ip 11. 4
dir [<arquivo_remoto> [<arquivo_local]]
.sp
É equivalente ao comando "ls", abaixo.

.ip 12. 4
exit
.sp
É equivalente ao comando "quit", abaixo.

.(t
.ip 13. 4
get <arquivo_remoto> [<arquivo_local>]
.sp
Copia um arquivo do nó remoto para o nó local. Se o nome do
<arquivo_local> não for dado, será igual ao nome do <arquivo_remoto>.
Se o nome do <arquivo_local> for "-", o conteúdo do <arquivo_remoto>
será impresso na saída padrão.
.)t

.ip 14. 4
help [-s] [<comando>]
.sp
Imprime uma descrição do significado do <comando>. Se o nome do <comando>
não for dado, imprime uma lista dos comandos disponíveis. Se for dada a
opção "-s" serão fornecidas as mesmas informações, mas obtidas do
servidor.

.ip 15. 4
image
.sp
Altera o tipo da representação de dados para IMAGEM.

.ip 16. 4
iso
.sp
Altera o tipo da representação de dados para ISO (ASCII).

.ip 17. 4
lc [<diretório_remoto>]
.sp
Imprime o conteúdo (no estilo de "lc") do <diretório_remoto>.
Se este não for dado, será considerado o diretório corrente remoto.
Este comando não é padrão do protocolo FTP, e não pode ser usado com
outros servidores.

.ip 18. 4
ls [<arquivo_remoto> [<arquivo_local]]
.sp
Imprime informações (no estilo de "ls -l") do <arquivo_remoto>.
O <arquivo_remoto> pode também ser um diretório, caso em que serão
impressas informações sobre o conteúdo do diretório remoto.
Se o <arquivo_remoto> não for dado (ou for igual a "."), será considerado
o diretório corrente remoto.
Se o <arquivo_local> for dado, as informações são postas neste arquivo;
em caso contrário são impressas na saída padrão.


.ip 19. 4
mget [-f] [<diretório>]
.sp
Copia os arquivos regulares contidos no <diretório> do nó remoto
para o nó local.
Os arquivos locais serão colocados no <diretório> (de mesmo nome) local,
que já deverá existir.
Se o nome do <diretório> não for dado, será considerado o diretório corrente
(remoto e local).
Se a opção "-f" não for dada, será pedida a confirmação da cópia de cada
arquivo.

.(t
.ip 20. 4
mkdir <diretório_remoto>
.sp
Cria o <diretório_remoto>.
.)t

.ip 21. 4
mput [-f] [<diretório>]
.sp
Copia os arquivos regulares contidos no <diretório> do nó local
para o nó remoto.
Os arquivos locais serão colocados no <diretório> (de mesmo nome) remoto,
que já deverá existir.
Se o nome do <diretório> não for dado, será considerado o diretório corrente
(local e remoto ).
Se a opção "-f" não for dada, será pedida a confirmação da cópia de cada
arquivo.

.ip 22. 4
mv <nome_antigo> <nome_novo>
.sp
Muda o nome do arquivo remoto <nome_antigo> para <nome_novo>.

.ip 23. 4
nlst [<diretório_remoto>]
.sp
Imprime os nomes dos arquivos regulares contidos no <diretório_remoto>.
Se o <diretório_remoto> não for dado, será considerado
o diretório corrente remoto.
Este comando é útil para verificar quais os arquivos que um comando "mget"
irá copiar.

.ip 24. 4
noop
.sp
Não realiza nenhuma operação.
Este comando é útil para verificar se a conexão ainda está
ativa e estimar o tempo de resposta do servidor.

.ip 25. 4
passive
.sp
Alterna entre os modos "ativo" e "passivo" da abertura da conexão
de dados. No modo "ativo", é o servidor que realiza o "connect",
enquanto que no modo "passivo" é o cliente.

.ip 26. 4
port [-sn] [<número_do_port>]
.sp
Pelo protocolo FTP, a conexão de dados deve usar normalmente o "port"
de número 20, podendo ser mudado através de um comando "PORT".
Além disto, uma vez mudado este número,
este novo "port" deverá ser utilizado pelo servidor em todas as operações
seguintes (até a recepção de um novo comando "PORT").

.sp
No entanto, há servidores FTP que não seguem estas convenções.
A função do comando "port" é o de permitir o uso destes servidores.

.sp
Se for dada a opção "-s", será executado um comando "PORT" para cada
operação que utiliza a conexão de dados.
Por outro lado, se for dada a opção "-n", será executado um comando
"PORT" apenas se o número do "port" for diferente do utilizado
na operação anterior.

.sp
Se for dado um <número_do_port> diferente de zero, este será utilizado
a partir das operações seguintes. Se ele for igual a zero, será obtido
um número de "port" temporário do sistema operacional, e que será
utilizado a partir das operações seguintes.
Se o  número de "port" dado não puder ser usado, também será obtido
um número de "port" temporário do sistema operacional.
Se o <número_do_port> não for dado, continuará a ser usado o "port"
anteriormente utilizado.

.sp
Inicialmente, "ftp" utiliza o "port" 20 (padrão) para a conexão de dados e
executa um comando "PORT" para cada operação que usa a conexão de dados.

.ip 27. 4
put <arquivo_local> [<arquivo_remoto>]
.sp
Copia um arquivo do nó local para o nó remoto. Se o nome do
<arquivo_remoto> não for dado, será igual ao nome do <arquivo_local>.

.ip 28. 4
pwd
.sp
Imprime o nome do diretório corrente remoto.

.ip 29. 4
quit
.sp
Encerra o programa. Um EOF (normalmente "^D") tem o mesmo efeito.

.ip 30. 4
quote <nome> [<argumento_1> [<argumento_2>] ]
.sp
Envia para o servidor um comando consistindo do <nome> e
possivelmente um ou dois <argumento>s (exatamente como dado(s)).
É esperada uma (e apenas uma) resposta do servidor.
A chegada de outras respostas pode ser verificada através do comando
"reset".

.sp
O comando "quote" é útil para a utilização de comandos disponíveis
em um outro servidor, que no entanto esta implementação "ftp" não suporta.
Uma outra possibilidade é o uso de comandos que ainda
irão ser definidos futuramente no protocolo FTP
(e que estarão disponíveis em um futuro servidor).

.sp
Repare que com "quote" só podemos utilizar comandos que não utilizam a
conexão de dados.

.ip 31. 4
rename <nome_antigo> <nome_novo>
.sp
Muda o nome do arquivo remoto <nome_antigo> para <nome_novo>.

.ip 32. 4
reset
.sp
Lê uma resposta do servidor (se disponível).
Isto é útil para que o cliente e o servidor fiquem novamente em
fase, caso o servidor (violando o protocolo) emita resposta(s)
a mais.

.ip 33. 4
rm <arquivo_remoto>
.sp
Remove o <arquivo_remoto>.

.ip 34. 4
rmdir <diretório_remoto>
.sp
Remove o <diretório_remoto>.

.ip 35. 4
status
.sp
Fornece informações sobre o cliente e o servidor.

.ip 36. 4
system
.sp
Obtém o tipo de sistema operacional do servidor.

.ip 37. 4
type [ascii|a|image|i|binary|bin|b]
.sp
Altera o tipo da representação de dados.
Os nomes válidos são "ascii" e "a" para o tipo ISO (ASCII) e
"image", "i", "binary" e "bin" para o tipo IMAGEM.
Se não for dado nenhum nome, será impresso o tipo corrente.

.ip 38. 4
user [<nome_do_usuário>]
.sp
Inicia a seqüência de "login". Se o <nome_do_usuário> não for
dado, ele é pedido, assim como a senha e conta (caso necessários).
A não ser que seja dada a opção "-n" (do programa), esta seqüência
é iniciada automaticamente quando "ftp" é executado.

.in -3
.ep

.in
.sp
.b "INTERRUPÇÃO DE COMANDOS E DE CÓPIA DE ARQUIVOS"
.in 5
Esta implementação do protocolo FTP ainda não possibilita a interrupção
de uma cópia de arquivos em andamento. Se for usada a tecla de interrupção
(normalmente <^C>), será impressa uma mensagem indicando isto, mas a
cópia prosseguirá até o final.

.sp
O único uso significativo da tecla de interrupção é nos
comandos "mget" e "mput", que serão interrompidos ao final da
cópia do arquivo em andamento.

.in
.sp
.b "COMANDOS ACEITOS/NÃO ACEITOS PELO SERVIDOR"
.in 5
O servidor reconhece e processa os seguintes comandos:

.sp
.(l
	USER	PASS,	PASV,	ACCT,	CWD,	CDUP,	QUIT,
	PORT,	TYPE,	STRU,	MODE,	RETR,	STOR,	RNFR,
	RNTO,	DELE,	RMD,	MKD,	PWD,	LIST,	LC,
	NLST,	SYST,	STAT,	HELP,	NOOP
.)l

.sp
.(t
O servidor reconhece, mas não processa os seguintes comandos:

.sp
.(l
	SMNT,	REIN,	STOU,	APPE,	ALLO,	REST,
	ABOR,	SITE
.)l
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
tcmpto
.br
.wo "(fmt): "
environ
.br

.in
.sp
.b
REFERÊNCIAS
.r
.(l
Postel, J. & Reynolds J.:
"FILE TRANSFER PROTOCOL (FTP), RFC 959",
Outubro de 1985.
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
