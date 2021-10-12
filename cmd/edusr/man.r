.bp
.he 'EDUSR (cmd)'TROPIX: Manual de Referência'EDUSR (cmd)'
.fo 'Atualizado em 15.01.99'Versão 3.1.1'Pag. %'

.b NOME
.in 5
.wo "edusr -"
gerencia as contas/grupos de usuários 
.br

.in
.sp
.b SINTAXE
.in 5
.(l
edusr
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "edusr" é uma interface para o gerenciamento das contas
e grupos de usuários do sistema TROPIX.
Ele gerencia o arquivos de contas/senhas "/etc/passwd",
o arquivo de grupos "/etc/group",
cria/remove os diretórios contendo os arquivos
dos usuários (diretório "home"),
e a suas caixas postais.

.sp
No TROPIX (assim como em outros sistemas operacionais de filosofia
UNIX), cada usuário do sistema é identificado por um inteiro
não negativo chamado
UID. Além disto, ele pertence a um certo grupo de usuários,
que (também) é identificado por um inteiro não negativo chamado GID.

.sp
Estes dois números definem as permissões que o usuário possui para
acessar certos recursos do sistema (tais como ler/escrever um
certo arquivo ou executar um certo programa).
Se o UID de um usuário for 0, ele é considerado um "superusuário",
e tem permissão para acessar todos os recursos do sistema;
em "intro" (sys) são dados mais detalhes.

.sp
O programa "edusr" supõe que os arquivos de contas/senhas e
o arquivo de grupos estejam (respectivamente) ordenados crescentemente
pelo UID e GID.

.sp
O programa "edusr" possui um interpretador de comandos, que le e
processa instruções do usuário.
Para indicar que está aguardando comandos, "edusr" imprime
uma linha contendo os caracteres
.sp
.nf
		"edusr [n]> "
.fi
.sp
onde "n" é o número
do comando no histórico (ver os comandos "h", "eh" e "xh" abaixo). 

.sp
Através do interpretador de comandos podemos efetuar diversas operações,
tais como obter uma lista dos usuários, criar novos usuários, remover usuários,
etc ... A lista completa é dada abaixo.

.in
.sp
.b "INFORMAÇÕES DO ARQUIVO DE CONTAS"
.in 5
O arquivo de contas/senhas "/etc/passwd" contém uma linha para cada usuário
do sistema. Cada uma destas linhas contém 10 campos separados
pelo caractere ":", contendo diversas informações.
Todos estas informações podem ser listadas através do comando "pu"
(veja abaixo).

.sp
Elas são (acompanhe no arquivo "/usr/include/pwd.h"):

.in +3
.ip " 1." 4
Nome do Usuário: Pode ter no máximo 14 caracteres.

.ip " 2." 4
Senha encriptada: Esta senha é consultada pelo programa "login" sempre
que um usuário entra em sessão. O comando "edusr" não processa estas senhas;
para um usuário regular a linha é criada sem senha e
para um superusuário a linha é criada com a senha do "root".
Em ambos os casos, as senhas devem ser modificadas posteriormente
através do programa "passwd" (cmd).

.ip " 3." 4
Comentário: Contém normalmente o nome completo do usuário.

.ip " 4." 4
Identificação do usuário (UID).

.ip " 5." 4
Identificação do grupo do usuário (GID).

.ip " 6." 4
Cota: O número máximo de blocos (de 512 bytes) que o usuário
pode alocar no disco.
Se este valor for NULO, o usuário não tem limite de blocos.
Na presente versão do sistema, este controle não está implementado.

.ip " 7." 4
Data de expiração: Indica a data após a qual não é mais permitido
ao usuário entrar em sessão no sistema.
Se este valor for NULO, o usuário não tem data de expiração.

.ip " 8." 4
Tranca: Informação utilizada pelo controle de cotas.

.ip " 9." 4
Diretório "home": Diretório corrente do usuário após entrar em sessão.
É neste diretório que o usuário deve criar o seus arquivos.
Como ele pode também criar subdiretórios, o diretório "home"
é na verdade a raiz da árvore contendo todos os arquivos do usuário.

.ip "10." 4
Interpretador de comandos: Programa que é executado ao entrar em
sessão. Normalmente é um interpretador de comandos (tal como "/bin/sh" (cmd)).

.ep
.in -3

.in
.sp
.b "VALORES CORRENTES"
.in 5
Para facilitar a criação de contas, o programa "edusr" armazena os
uma série de "valores correntes":

.in +3
.ip " 1." 4
O grupo corrente: O grupo a que pertencerá o novo usuário.
Pode ser consultado/alterado através do comando "g".

.ip " 2." 4
A cota corrente: O limite de cotas que será atribuído ao novo usuário.
Pode ser consultado/alterado através do comando "c".

.ip " 3." 4
A data de expiração corrente: A data de expiração que será atribuída ao novo usuário.
Pode ser consultado/alterado através do comando "e".

.ip " 4." 4
O diretório "home" corrente: O diretório que será atribuído ao novo usuário.
Pode ser consultado/alterado através do comando "home".

.ip " 5." 4
O interpretador de comandos corrente: O interpretador que será atribuído
ao novo usuário. Pode ser consultado/alterado através do comando "shell".

.ep
.in -3

.sp
Através do comando "set" podemos consultar todos os valores correntes
simultaneamente.

.sp
Além dos valores descritos acima, "edusr" também consulta o arquivo
"/usr/pub/lib/profile.sample", que é usado para criar o arquivo
".profile" dos usuários.
Este arquivo naturalmente pode ser modificado conforme as necessidades.

.in
.sp
.(t
.b COMANDOS
.in 5
O programa "edusr" reconhece e processa os seguintes comandos:

.bc	/*************************************/
.in +3
.ec	/*************************************/
.ip " 1." 4
?
.sp
Como "help", abaixo.
.)t

.ip " 2." 4
!<comando> [<argumento> ...]
.sp
Executa o <comando> com os respectivos <argumento>s através do "sh".
Isto vale inclusive para o comando "cd".
Todo o contexto de "edusr" é conservado.

.ip " 3." 4
c [<cota>]
.sp
Como "cota", abaixo.

.ip " 4." 4
cota [<cota>]
.sp
Altera a cota corrente de blocos por usuário.
Se a <cota> não for dada, imprime a "cota" corrente.
Uma cota de 0 blocos significa a ausência do limite de cotas.

.ip " 5." 4
data [<expir>]
.sp
Como "expir", abaixo.
 
.ip " 6." 4
e [<expir>]
.sp
Como "expir", abaixo.

.ip " 7." 4
eh [<n>]
.sp
Edita (e opcionalmente executa) o comando de número <n> do histórico.
Se <n> for omitido, edita o último comando presente no histórico.
A edição é realizada pela função "edit" (libc).
Após a edição, o usuário é consultado se deseja realmente executar
o comando editado.

.(t
.ip " 8." 4
exit
.sp
Como "quit", abaixo.
.)t

.ip " 9." 4
expir [<expir>]
.sp
Altera a data de expiração da conta de usuário.
Se <expir> não for dada, é impressa a data de expiração corrente.
O formato de <expir> deve ser "dd.mm.aa", ou "0" significando
a ausência de data de expiração.

.ip "10." 4
g [<grupo>]
.sp
Como "mkgrp", abaixo.

.ip "11." 4
h
.sp
Imprime os comandos do histórico. Nesta versão, o histórico armazena
os últimos 20 comandos executados.

.ip "12." 4
help
.sp
Imprime uma lista de todos os comandos disponíveis.
Para obter um resumo de um <comando>, use "<comando> -H" ou "<comando> ?".

.ip "13." 4
home [<diretório>]
.sp
Altera o diretório "home".
Se o <diretório> não for dado, imprime o "home" corrente.

.ip "14." 4
mg [<grupo>]
.sp
Como "mkgrp", abaixo.

.ip "15." 4
mkgrp [<grupo>]
.sp
Altera o grupo corrente ou cria um novo grupo.
Se o <grupo> não for dado, imprime o grupo corrente.
Se o <grupo> dado ainda não existir, ele é criado, e passa a ser
o grupo corrente.

.ip "16." 4
mkusr [-s] <usuário> [<comentário>]
.sp
Cria o novo <usuário> com os valores correntes.
O <comentário>, se dado, engloba todos os argumentos após <usuário>.
.sp
Sem a opção "-s", é criado um usuário regular sem senha, cujo UID
é escolhido automaticamente pelo programa "edusr".
Com a opção "-s", é criado um superusuário com a senha do "root".
.sp
NÃO esqueça de atribuir uma senha posteriormente através do utilitário
"passwd" (cmd).

.ip "17." 4
mu [-s] <usuário> [<comentário>]
.sp
Como "mkusr", acima.

.(t
.ip "18." 4
pg [<grupo>]
.sp
Como "prgrp", abaixo.
.)t

.ip "19." 4
prgrp [<grupo>]
.sp
Imprime o <grupo>. Se <grupo> não for dado, imprime todos os grupos.

.ip "20." 4
prusr [-l] [-g <grupo>] [<usuário>]
.sp
Imprime informações sobre o <usuário>. Se o <usuário> não for dado,
imprime informações sobre todos os usuários. Se for dada a opção
"-g", somente imprime informações sobre os usuários do <grupo> dado.

.sp
Sem a opção "-l", imprime o nome do usuário, seu UID, o nome (e GID) do grupo
a que pertence, um asterisco se NÃO contiver senha e o comentário
(normalmente o nome completo do usuário).

.sp
Com a opção "-l", imprime o nome do usuário, seu UID, o nome do interpretador
de comandos e o diretório "home".

.sp
Com a opção "-l" dada duas vezes, imprime o nome do usuário, seu UID,
a cota, um "L" se a conta estiver "trancada", a data de expiração e um
"E" caso a conta esteja expirada.
O caractere "." indica a ausência de cota/data de expiração.
 
.ip "21." 4
pu [-l] [-g <grupo>] [<usuário>]
.sp
Como "prusr", acima.

.ip "22." 4
q
.sp
Como "quit", abaixo.

.ip "23." 4
quit
.sp
Termina a execução de "edusr".
Um EOF (normalmente <^D>) tem o mesmo efeito.

.ip "24." 4
quota [<cota>]
.sp
Como "cota", acima.

.(t
.ip "25." 4
rg <grupo>
.sp
Como "rmgrp", abaixo.
.)t

.ip "26." 4
rmgrp <grupo>
.sp
Remove o <grupo>, juntamente com os seus usuários e arquivos.
Este comando deve ser utilizado com cuidado, para evitar a remoção
acidental de arquivos de usuários.

.ip "27." 4
rmusr <usuário>
.sp
Remove o <usuário>, juntamente com os seus arquivos e caixa postal.
Este comando deve ser utilizado com cuidado, para evitar a remoção
acidental de arquivos e/ou cartas das caixas postais dos usuários.

.ip "28." 4
ru <usuário>
.sp
Como "rmusr", acima.

.ip "29." 4
set
.sp
Imprime os valores correntes.

.ip "30." 4
shell [<shell>]
.sp
Altera o interpretador de comandos ("shell") corrente.
Se <shell> não for dado, imprime o interpretador corrente.

.ip "31." 4
u [-s] <usuário> [<comentário>]
.sp
Como "mkusr", acima.

.ip "32." 4
xh [<n>]
.sp
Excuta o comando de número <n> do histórico.
Se <n> for omitido, executa o último comando presente no histórico.

.ep
.bc	/*************************************/
.in -3
.ec	/*************************************/

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys):  "
intro
.br
.wo "(libc): "
edit
.br
.wo "(cmd):  "
passwd, sh
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
/etc/passwd
 /etc/group
 /home/...
 /var/spool/mail/...
 /usr/include/pwd.h
 /usr/pub/lib/profile.sample

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
