.bp
.he 'INIT (cmd)'TROPIX: Manual de Modificações'INIT (cmd)'
.fo 'Atualizado em 17.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "init -"
inicialização da operação do sistema
.br

.in
.sp 2
.ip "01.12.87  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "06.11.88  2.1.0" 20
Foi alterado o nome do arquivo "/etc/profile" para "/etc/globalenv",
e agora as suas linhas são testadas para verificar se possuem '='.

.sp
Agora, a escrita da data é feita após
a mensagem do nome
do "sh".

.ip "23.10.89  2.3.0" 20
Foi criada a idéia do "PRINT_CHILD_STATUS".

.ip "01.10.91  2.3.1" 20
Foi tornada automática a obtenção do nome do sistema através de "uname".

.ip "21.10.91  2.3.2" 20
Agora é colocado a variável "SYS" no ambiente.

.ip "16.11.92  3.0.0" 20
Agora só ativa as linhas com estado == '*'.

.ip "16.02.97  3.0.1" 20
Trocado "boot" por "root" na busca da senha.

.ip "08.05.97  3.0.2" 20
Uma pequena revisão.

.ip "20.06.97  3.0.3" 20
Acertando o modo de "utmp" e "mtab".

.ip "31.01.99  3.1.6" 20
O modo mono-usuário agora entra como superusuário.

.ip "08.12.99  3.3.3" 20
Obtendo o nome do "sh" do SCB.

.ip "17.07.04  4.6.0" 20
Usando as funções de "initent" para acessar "/etc/initab".

